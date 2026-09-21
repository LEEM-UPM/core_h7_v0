#include <stdio.h>

#include "buzzer.h"
#include "can_driver.h"
#include "can_protocol/can_ids.h"
#include "can_protocol/can_pack.h"
#include "debug_console.h"
#include "debug_out.h"
#include "fdcan.h"
#include "gpio.h"
#include "led_show.h"
#include "main.h"
#include "sensor_board.h"
#include "sounds.h"
#include "storage.h"
#include "sys_utilities.h"

extern void SystemClock_Config(void);
extern void PeriphCommonClock_Config(void);

/* Periodo del LED de actividad: parpadea mientras lleguen tramas válidas */
#define LED_ACTIVITY_PERIOD_MS 250U

/*
 * Trama de prueba de 1 byte por CAN2 a 50 Hz.
 * ID por encima de 0x101–0x105: en el arbitraje ganan siempre las tramas de
 * la placa de sensores, que no retransmite y las perdería.
 */
#define CAN2_TX_TEST_ID 0x200U
#define CAN2_TX_PERIOD_MS 20U         /* 50 Hz */
#define CAN2_TX_VALUE_PERIOD_MS 1000U /* alterna 0 <-> 255 cada segundo */

/*
 * Trama de prueba por CAN1 a 1 Hz: contador uint32 en la clase de debug.
 * Quitar cuando haya mensajes reales definidos en el ICD.
 */
#define CAN1_TX_TEST_ID CAN_ID(CAN_CLASS_DEBUG, CAN_NODE_CORE, 0x0U)
#define CAN1_TX_PERIOD_MS 1000U

/*
 * Prueba de radio: el core manda a la RF un texto para que lo transmita tal
 * cual (CMD_RF_TRANSMIT, ICD 6.10). Cada 5 s para no pasarse del ciclo de
 * trabajo permitido en 868 MHz. Quitar cuando haya telemetría real.
 */
#define RF_TX_TEST_PERIOD_MS 5000U

sys_state state;

int main(void) {
  HAL_Init();
  SystemClock_Config();
  PeriphCommonClock_Config();
  MX_GPIO_Init();
  MX_FDCAN1_Init();
  MX_FDCAN2_Init();

  /* Consola de debug por USB (puerto COM virtual). Lo que se escriba antes de
   * que el PC abra el puerto queda en el buffer. */
  DebugOut_Init();
  DebugConsole_Banner();

  /* Secuencia de LEDs y melodía de arranque, antes de despertar al resto de
   * placas */
  buzzer_init();
  led_show_startup();
  sounds_startup();

  DebugConsole_WakeUpStart();
  can_driver_init();

  sys_init(&state);
  DebugConsole_Startup(&state);

  /* Tarjetas SD: montaje y prueba de escritura/lectura (resultado en la
   * consola, sección TARJETAS SD). Bloquea mientras dura (~1 s por tarjeta). */
  storage_init();

  /* Calibración de aviónica al arrancar (en tierra), por ahora con el cohete
   * vertical: 0 grados. El resultado llega a state.avionics_calibration. */
  (void)sys_avionics_calibrate(&state, 0.0f);

  can_rx_frame_t frame;

  uint32_t valid_frames = 0U;
  uint32_t led_last_frames = 0U;
  uint32_t led_last_tick = HAL_GetTick();

  uint8_t can2_tx_value = 0U;
  uint32_t can2_tx_last_tick = HAL_GetTick();
  uint32_t can2_tx_value_last_tick = can2_tx_last_tick;

  uint32_t can1_tx_counter = 0U;
  uint32_t can1_tx_last_tick = HAL_GetTick();

  uint32_t rf_tx_counter = 0U;
  uint32_t rf_tx_last_tick = HAL_GetTick();

  while (1) {
    /* ---- Recepción ---- */
    while (can_driver_receive(CAN_PORT_1, &frame)) {
      sys_process_can1_frame(&state, &frame);
    }

    while (can_driver_receive(CAN_PORT_2, &frame)) {
      valid_frames += sensor_board_process_frame(frame.id, frame.data,
                                                 frame.len, frame.rx_tick_ms);
    }

    /* TODO: usar sensor_board_get_data() según lo que haya que hacer con los
     * datos (log, reenvío...) */

    /* ---- Gestión del resto de placas (continuidad de power a 1 Hz...) ---- */
    sys_update(&state);

    /* ---- Consola de debug por USB (informe cada segundo) ---- */
    DebugConsole_Update(&state);

    /* ---- Transmisión ---- */
    uint32_t now = HAL_GetTick();

    if ((now - can1_tx_last_tick) >= CAN1_TX_PERIOD_MS) {
      can1_tx_last_tick += CAN1_TX_PERIOD_MS;

      uint8_t buf[4];
      can_put_u32_le(&buf[0], can1_tx_counter++);
      can_driver_send(CAN_PORT_1, CAN1_TX_TEST_ID, buf, sizeof(buf));
    }

    if ((now - rf_tx_last_tick) >= RF_TX_TEST_PERIOD_MS) {
      rf_tx_last_tick += RF_TX_TEST_PERIOD_MS;

      char text[SYS_RF_PAYLOAD_MAX];
      const int n = snprintf(text, sizeof(text), "LEEM CORE #%lu",
                             (unsigned long)rf_tx_counter++);
      (void)sys_rf_transmit(&state, (const uint8_t *)text, (uint8_t)n);
    }

    if ((now - can2_tx_value_last_tick) >= CAN2_TX_VALUE_PERIOD_MS) {
      can2_tx_value_last_tick += CAN2_TX_VALUE_PERIOD_MS;
      can2_tx_value = (can2_tx_value == 0U) ? 255U : 0U;
    }

    if ((now - can2_tx_last_tick) >= CAN2_TX_PERIOD_MS) {
      can2_tx_last_tick += CAN2_TX_PERIOD_MS;
      can_driver_send(CAN_PORT_2, CAN2_TX_TEST_ID, &can2_tx_value, 1U);
    }

    /* ---- LED de actividad (placa de sensores) ---- */
    if ((now - led_last_tick) >= LED_ACTIVITY_PERIOD_MS) {
      led_last_tick = now;
      if (valid_frames != led_last_frames) {
        led_last_frames = valid_frames;
        HAL_GPIO_TogglePin(LED_1_GPIO_Port, LED_1_Pin);
      } else {
        HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_RESET);
      }
    }
  }
}
