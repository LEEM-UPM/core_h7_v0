#include "can_shared_data.h"
#include <string.h>

can_rx_data_t g_can_shared;
static TX_MUTEX _mutex;

UINT can_shared_data_init(void) {
  memset(&g_can_shared, 0, sizeof(g_can_shared));
  return tx_mutex_create(&_mutex, "CAN shared mutex", TX_INHERIT);
}

UINT can_shared_data_lock(void) {
  return tx_mutex_get(&_mutex, CAN_SHARED_MUTEX_TIMEOUT_TICKS);
}

void can_shared_data_unlock(void) { tx_mutex_put(&_mutex); }