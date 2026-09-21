#include "sys_power.h"

#include "can_protocol/can_ids.h"
#include "can_protocol/can_pack.h"
#include "main.h"

/* Peticiones periódicas a power: continuidad de los pirotécnicos (ICD 6.4) y
 * tensiones/corrientes de los raíles (ICD 6.6) */
#define SYS_POWER_REQUEST_PERIOD_MS 1000U

/* Longitud de POWER_RAILS (ICD, 6.7) */
#define POWER_RAILS_LEN 12U

/* Byte de canal de CMD_PYRO_FIRE (ICD, 6.3) */
#define PYRO_CHANNEL_1A 0x01U
#define PYRO_CHANNEL_1B 0x02U
#define PYRO_CHANNEL_2A 0x03U
#define PYRO_CHANNEL_2B 0x04U

static uint32_t _last_request_ms;

static bool _pyro_fire(const sys_state *state, uint8_t channel);
static void _process_power_rails(sys_state *state, const can_rx_frame_t *frame);

void sys_power_init(sys_state *state) {
  state->pyro_continuity_valid = false;
  state->pyro_continuity = 0U;
  state->pyro_continuity_rx_ms = 0U;

  state->power_rails_valid = false;
  state->power_rails.current_3v3_ma = SYS_POWER_RAILS_INVALID;
  state->power_rails.current_5v_ma = SYS_POWER_RAILS_INVALID;
  state->power_rails.current_11v_ma = SYS_POWER_RAILS_INVALID;
  state->power_rails.voltage_3v3_mv = SYS_POWER_RAILS_INVALID;
  state->power_rails.voltage_5v_mv = SYS_POWER_RAILS_INVALID;
  state->power_rails.voltage_11v_mv = SYS_POWER_RAILS_INVALID;
  state->power_rails_rx_ms = 0U;

  /* Las primeras peticiones salen en la primera vuelta del bucle */
  _last_request_ms = HAL_GetTick() - SYS_POWER_REQUEST_PERIOD_MS;
}

void sys_power_update(sys_state *state) {
  uint32_t now = HAL_GetTick();

  /* Peticiones a 1 Hz. Si power no respondió al wake up sigue esperándolo y
   * no las atendería, así que no se envían. */
  if ((now - _last_request_ms) >= SYS_POWER_REQUEST_PERIOD_MS) {
    _last_request_ms += SYS_POWER_REQUEST_PERIOD_MS;
    if (state->power) {
      (void)can_driver_send(CAN_PORT_1, CAN_ID_CMD_PYRO_CONTINUITY, NULL, 0U);
      (void)can_driver_send(CAN_PORT_1, CAN_ID_CMD_POWER_RAILS, NULL, 0U);
    }
  }
}

bool sys_power_process_frame(sys_state *state, const can_rx_frame_t *frame) {
  switch (frame->id) {
  case CAN_ID_PYRO_CONTINUITY: /* 6.5: 1 byte, un bit por canal */
    if (frame->len >= 1U) {
      state->pyro_continuity =
          can_get_u8(&frame->data[0]) &
          (SYS_PYRO_1A | SYS_PYRO_1B | SYS_PYRO_2A | SYS_PYRO_2B);
      state->pyro_continuity_rx_ms = frame->rx_tick_ms;
      state->pyro_continuity_valid = true;
    }
    return true;

  case CAN_ID_POWER_RAILS: /* 6.7: 12 bytes, uint16 little-endian */
    _process_power_rails(state, frame);
    return true;

  default:
    return false;
  }
}

static void _process_power_rails(sys_state *state, const can_rx_frame_t *frame) {
  /* Una trama más corta se descarta entera: los campos quedarían descolocados */
  if (frame->len < POWER_RAILS_LEN) {
    return;
  }

  const uint8_t *d = frame->data;
  state->power_rails.current_3v3_ma = can_get_u16_le(&d[0]);
  state->power_rails.current_5v_ma = can_get_u16_le(&d[2]);
  state->power_rails.current_11v_ma = can_get_u16_le(&d[4]);
  state->power_rails.voltage_3v3_mv = can_get_u16_le(&d[6]);
  state->power_rails.voltage_5v_mv = can_get_u16_le(&d[8]);
  state->power_rails.voltage_11v_mv = can_get_u16_le(&d[10]);
  state->power_rails_rx_ms = frame->rx_tick_ms;
  state->power_rails_valid = true;
}

bool sys_pyro_fire_1a(const sys_state *state) {
  return _pyro_fire(state, PYRO_CHANNEL_1A);
}

bool sys_pyro_fire_1b(const sys_state *state) {
  return _pyro_fire(state, PYRO_CHANNEL_1B);
}

bool sys_pyro_fire_2a(const sys_state *state) {
  return _pyro_fire(state, PYRO_CHANNEL_2A);
}

bool sys_pyro_fire_2b(const sys_state *state) {
  return _pyro_fire(state, PYRO_CHANNEL_2B);
}

/* CMD_PYRO_FIRE: 1 byte con el canal. Un mensaje por canal. */
static bool _pyro_fire(const sys_state *state, uint8_t channel) {
  if (!state->power) {
    return false;
  }

  uint8_t data[1];
  can_put_u8(&data[0], channel);
  return can_driver_send(CAN_PORT_1, CAN_ID_CMD_PYRO_FIRE, data, sizeof(data));
}
