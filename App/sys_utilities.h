/* En este archivo se iran añadiendo las funciones de alto nivel
encargadas de la gestion del resto de placas, toma de decisiones y un largo
etcetera */

#ifndef SYS_UTILITIES_H
#define SYS_UTILITIES_H

#include <stdbool.h>
#include <stdint.h>

#include "can_driver.h"

/* Bits de sys_state.pyro_continuity (ICD, 6.5) */
#define SYS_PYRO_1A (1U << 0)
#define SYS_PYRO_1B (1U << 1)
#define SYS_PYRO_2A (1U << 2)
#define SYS_PYRO_2B (1U << 3)

/* Valor de un campo de sys_power_rails cuya lectura no es válida (ICD, 6.7) */
#define SYS_POWER_RAILS_INVALID 0xFFFFU

/* Tensiones y corrientes de los raíles de power (último POWER_RAILS) */
typedef struct {
  uint16_t current_3v3_ma;
  uint16_t current_5v_ma;
  uint16_t current_11v_ma;
  uint16_t voltage_3v3_mv;
  uint16_t voltage_5v_mv;
  uint16_t voltage_11v_mv;
} sys_power_rails;

/* Estado de la calibración de aviónica */
typedef enum {
  SYS_CALIBRATION_IDLE = 0, /* no se ha pedido nunca */
  SYS_CALIBRATION_WAITING,  /* 0x131 enviado, esperando 0x231 */
  SYS_CALIBRATION_DONE,     /* resultados en avionics_calibration */
  SYS_CALIBRATION_TIMEOUT,  /* SYS_CALIBRATION_TIMEOUT_MS sin respuesta */
} sys_calibration_status;

#define SYS_CALIBRATION_TIMEOUT_MS 5000U

/* Resultados de la última calibración de aviónica (ICD, 6.9), ya en
 * unidades físicas. NaN = ese valor no se pudo calibrar. */
typedef struct {
  float imu1_accel_mps2[3]; /* offset aplicado en aviónica, X Y Z */
  float imu1_gyro_radps[3];
  float imu2_accel_mps2[3];
  float imu2_gyro_radps[3];
  float baro1_kpa;          /* presión en la altura base */
  float baro2_kpa;
} sys_avionics_calibration;

/*
 * Componentes con los que ha arrancado una placa (INIT_STATUS, ICD 6.11).
 * supported marca los bits que significan algo en esa placa: un bit a 0 en
 * ok solo es un fallo si está a 1 en supported.
 */
typedef struct {
  bool valid; /* false hasta recibir el primer INIT_STATUS */
  uint32_t ok;
  uint32_t supported;
  uint32_t rx_ms; /* HAL_GetTick() al recibirlo */
} sys_init_status;

/* Bits de la placa de aviónica (ICD 6.11) */
#define SYS_AVIONICS_INIT_BARO1 (1U << 0)
#define SYS_AVIONICS_INIT_BARO2 (1U << 1)
#define SYS_AVIONICS_INIT_IMU1 (1U << 2)
#define SYS_AVIONICS_INIT_IMU2 (1U << 3)
#define SYS_AVIONICS_INIT_MAG (1U << 4)
#define SYS_AVIONICS_INIT_GPS (1U << 5)
#define SYS_AVIONICS_INIT_VECTORNAV (1U << 6)

typedef struct {
  /* Placas que han respondido al wake up */
  bool power;
  bool avionics;
  bool rf;

  /* Wake ups reenviados tras el arranque (sys_update) a las placas que
   * faltan: una placa que se reinicie o arranque tarde vuelve a contestar */
  uint32_t wake_up_retries;

  /* Continuidad de los pirotécnicos de power (último PYRO_CONTINUITY) */
  bool pyro_continuity_valid;     /* false hasta la primera respuesta */
  uint8_t pyro_continuity;        /* SYS_PYRO_*: 1 = hay continuidad */
  uint32_t pyro_continuity_rx_ms; /* HAL_GetTick() al recibirla */

  /* Raíles de power. Cada campo puede valer SYS_POWER_RAILS_INVALID. */
  bool power_rails_valid;         /* false hasta la primera respuesta */
  sys_power_rails power_rails;
  uint32_t power_rails_rx_ms;     /* HAL_GetTick() al recibirla */

  /* Componentes que ha arrancado cada placa (INIT_STATUS, ICD 6.11) */
  sys_init_status power_init;
  sys_init_status avionics_init;
  sys_init_status rf_init;

  /* Paquetes mandados a la RF para transmitir (CMD_RF_TRANSMIT, ICD 6.10) */
  uint32_t rf_packets_sent;
  uint32_t rf_packets_failed;

  /* Calibración de aviónica */
  sys_calibration_status avionics_calibration_status;
  sys_avionics_calibration avionics_calibration;
  uint32_t avionics_calibration_tx_ms; /* HAL_GetTick() al enviar el 0x131 */
} sys_state;

void sys_init(sys_state *state);

/* Tareas periódicas con el resto de placas. Llamar en cada vuelta del bucle
 * principal. */
void sys_update(sys_state *state);

/* Trata una trama recibida por CAN1 (can_driver_receive(CAN_PORT_1, ...)) */
void sys_process_can1_frame(sys_state *state, const can_rx_frame_t *frame);

/*
 * Ordena a power encender un pirotécnico (CMD_PYRO_FIRE, ICD 6.3). Power lo
 * mantiene encendido 1 s y lo apaga solo; no hay confirmación.
 * Devuelve false si power no respondió al wake up (no atendería la orden) o
 * si la trama no se pudo encolar. true solo indica que se ha encolado.
 */
/* Bytes máximos por paquete de radio (longitud máxima de datos del ICD) */
#define SYS_RF_PAYLOAD_MAX 32U

/*
 * Manda a la RF poner esos bytes en el aire (CMD_RF_TRANSMIT, ICD 6.10). La
 * RF los transmite tal cual, sin interpretarlos. Devuelve false si la RF no
 * respondió al wake up, si len es 0 o mayor que SYS_RF_PAYLOAD_MAX, o si la
 * trama no se pudo encolar. true solo indica que se ha encolado.
 */
bool sys_rf_transmit(sys_state *state, const uint8_t *data, uint8_t len);

bool sys_pyro_fire_1a(const sys_state *state);
bool sys_pyro_fire_1b(const sys_state *state);
bool sys_pyro_fire_2a(const sys_state *state);
bool sys_pyro_fire_2b(const sys_state *state);

/*
 * Pide a aviónica que calibre IMUs y barómetros (CMD_AVIONICS_CALIBRATE, ICD
 * 6.8) con el cohete inclinado tilt_deg grados. SOLO EN TIERRA: es
 * responsabilidad del core no enviarlo en vuelo.
 *
 * No bloquea: el resultado llega a state->avionics_calibration y
 * state->avionics_calibration_status pasa a DONE, o a TIMEOUT si no hay
 * respuesta en SYS_CALIBRATION_TIMEOUT_MS (aviónica tarda ~2 s).
 *
 * Devuelve false si aviónica no respondió al wake up, si ya hay una
 * calibración en curso o si la trama no se pudo encolar.
 */
bool sys_avionics_calibrate(sys_state *state, float tilt_deg);

#endif /* SYS_UTILITIES_H */
