#include "sys_rf.h"

#include "can_driver.h"
#include "can_protocol/can_ids.h"
#include "main.h"

void sys_rf_init(sys_state *state) {
  state->rf_packets_sent = 0U;
  state->rf_packets_failed = 0U;
}

bool sys_rf_transmit(sys_state *state, const uint8_t *data, uint8_t len) {
  if ((data == NULL) || (len == 0U) || (len > SYS_RF_PAYLOAD_MAX)) {
    return false;
  }

  /* Si la RF no respondió al wake up no atendería la orden: no se envía */
  if (!state->rf) {
    state->rf_packets_failed++;
    return false;
  }

  if (!can_driver_send(CAN_PORT_1, CAN_ID_CMD_RF_TRANSMIT, data, len)) {
    state->rf_packets_failed++;
    return false;
  }

  state->rf_packets_sent++;
  return true;
}
