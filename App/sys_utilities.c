#include "sys_utilities.h"

#include "can_driver.h"
#include "can_protocol/can_ids.h"
#include "can_protocol/can_pack.h"
#include "can_protocol/can_protocol_version.h"
#include "main.h"
#include "sys_avionics.h"
#include "sys_power.h"
#include "sys_rf.h"

/* Wake up: se repite a 10 Hz hasta que respondan todas o pasen 5 s */
#define SYS_WAKE_UP_PERIOD_MS 100U
#define SYS_WAKE_UP_TIMEOUT_MS 5000U

/* Pasado el arranque se sigue reenviando a 1 Hz mientras falte alguna placa */
#define SYS_WAKE_UP_RETRY_PERIOD_MS 1000U

/* Margen tras el wake up para recoger los INIT_STATUS de las placas */
#define SYS_INIT_STATUS_WAIT_MS 300U

static bool _all_awake(const sys_state *state);
static void _send_wake_up(void);
static void _wake_up_retry(sys_state *state);
static void _process_wake_up_ack(sys_state *state, const can_rx_frame_t *frame);
static bool _process_init_status(sys_state *state, const can_rx_frame_t *frame);

/*
 * Despierta al resto de placas por CAN1. Bloquea como máximo
 * SYS_WAKE_UP_TIMEOUT_MS. Al salir, cada campo de state indica si esa placa
 * ha respondido. Llamar después de can_driver_init().
 */
void sys_init(sys_state *state) {
  HAL_Delay(1000); /* esperar a que el resto de placas arranquen y configuren su CAN */
  state->power = false;
  state->avionics = false;
  state->rf = false;

  state->wake_up_retries = 0U;
  state->power_init.valid = false;
  state->avionics_init.valid = false;
  state->rf_init.valid = false;

  uint32_t start = HAL_GetTick();
  uint32_t last_tx = start - SYS_WAKE_UP_PERIOD_MS; /* el primero sale ya */

  while (!_all_awake(state) &&
         ((HAL_GetTick() - start) < SYS_WAKE_UP_TIMEOUT_MS)) {
    if ((HAL_GetTick() - last_tx) >= SYS_WAKE_UP_PERIOD_MS) {
      last_tx += SYS_WAKE_UP_PERIOD_MS;
      /* Si falla (nadie da ACK y la cola TX se llena) se reintenta en el
       * siguiente periodo */
      _send_wake_up();
    }

    can_rx_frame_t frame;
    while (can_driver_receive(CAN_PORT_1, &frame)) {
      /* Las placas mandan su INIT_STATUS nada más responder al wake up, así
       * que llega mientras se sigue esperando a las demás: hay que recogerlo
       * aquí o se pierde. */
      if (!_process_init_status(state, &frame)) {
        _process_wake_up_ack(state, &frame);
      }
    }
  }

  /* Las placas mandan su INIT_STATUS (ICD 6.11) justo después de responder
   * al wake up: se recogen aquí para que el informe de arranque ya los
   * tenga, en vez de aparecer un segundo más tarde. */
  const uint32_t init_status_deadline = HAL_GetTick() + SYS_INIT_STATUS_WAIT_MS;
  while ((int32_t)(HAL_GetTick() - init_status_deadline) < 0) {
    can_rx_frame_t frame;
    while (can_driver_receive(CAN_PORT_1, &frame)) {
      sys_process_can1_frame(state, &frame);
    }
  }

  sys_power_init(state);
  sys_avionics_init(state);
  sys_rf_init(state);
}

void sys_update(sys_state *state) {
  _wake_up_retry(state);
  sys_power_update(state);
  sys_avionics_update(state);
}

/*
 * INIT_STATUS (ICD 6.11): con qué componentes ha arrancado cada placa. Lo
 * envían solas tras atender el wake up, así que también llega si el core se
 * reinicia. Devuelve true si la trama era esta.
 */
static bool _process_init_status(sys_state *state,
                                 const can_rx_frame_t *frame) {
  if ((CAN_ID_GET_CLASS(frame->id) != CAN_CLASS_RESPONSE) ||
      (CAN_ID_GET_MSG(frame->id) != CAN_MSG_INIT_STATUS)) {
    return false;
  }
  if (frame->len < 8U) {
    return true; /* es INIT_STATUS, pero está mal formada */
  }

  sys_init_status *status;
  switch (CAN_ID_GET_NODE(frame->id)) {
  case CAN_NODE_POWER:
    status = &state->power_init;
    break;
  case CAN_NODE_AVIONICS:
    status = &state->avionics_init;
    break;
  case CAN_NODE_RF:
    status = &state->rf_init;
    break;
  default:
    return true;
  }

  status->ok = can_get_u32_le(&frame->data[0]);
  status->supported = can_get_u32_le(&frame->data[4]);
  status->rx_ms = HAL_GetTick();
  status->valid = true;
  return true;
}

/*
 * Tramas del bus común. Cada placa trata las suyas en su archivo (sys_power.c,
 * sys_avionics.c); los campos se leen con can_get_*() de can_pack.h.
 */
void sys_process_can1_frame(sys_state *state, const can_rx_frame_t *frame) {
  if (_process_init_status(state, frame) ||
      sys_power_process_frame(state, frame) ||
      sys_avionics_process_frame(state, frame)) {
    return;
  }

  /* Un WAKE_UP_ACK tardío también marca la placa como despierta */
  _process_wake_up_ack(state, frame);
  /* TODO: resto de mensajes según se definan en el ICD */
}

/* CMD_WAKE_UP (0x100) con la versión del protocolo (ICD, 6.1) */
static void _send_wake_up(void) {
  uint8_t wake_up[2];
  can_put_u16_le(&wake_up[0], CAN_PROTOCOL_VERSION);
  (void)can_driver_send(CAN_PORT_1, CAN_ID_CMD_WAKE_UP, wake_up,
                        sizeof(wake_up));
}

/*
 * Reenvía el wake up a 1 Hz mientras falte alguna placa por responder. Las
 * placas contestan a todos los wake ups, no solo al primero, así que una que
 * se reinicie o arranque tarde se recupera sola; las que ya respondieron
 * vuelven a contestar y su ACK simplemente las deja igual.
 */
static void _wake_up_retry(sys_state *state) {
  static uint32_t last_ms;

  if (_all_awake(state)) {
    return;
  }

  const uint32_t now = HAL_GetTick();
  if ((now - last_ms) < SYS_WAKE_UP_RETRY_PERIOD_MS) {
    return;
  }
  last_ms = now;

  state->wake_up_retries++;
  _send_wake_up();
}

static bool _all_awake(const sys_state *state) {
  return state->power && state->avionics && state->rf;
}

/* Cualquier otra trama que llegue durante el arranque se descarta */
static void _process_wake_up_ack(sys_state *state, const can_rx_frame_t *frame) {
  if ((CAN_ID_GET_CLASS(frame->id) != CAN_CLASS_RESPONSE) ||
      (CAN_ID_GET_MSG(frame->id) != CAN_MSG_WAKE_UP_ACK)) {
    return;
  }

  switch (CAN_ID_GET_NODE(frame->id)) {
  case CAN_NODE_POWER:
    state->power = true;
    break;
  case CAN_NODE_AVIONICS:
    state->avionics = true;
    break;
  case CAN_NODE_RF:
    state->rf = true;
    break;
  default:
    break;
  }
}
