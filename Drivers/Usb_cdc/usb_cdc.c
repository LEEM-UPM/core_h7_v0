/*
 * Puerto serie virtual (USB CDC) del core. La interfaz CDC está basada en
 * usbd_cdc_if_template.c de la librería USB Device de ST.
 */

#include "usb_cdc.h"

#include "main.h"
#include "usbd_cdc.h"
#include "usbd_core.h"
#include "usbd_desc.h"

#define USB_CDC_RX_BUFFER_SIZE CDC_DATA_FS_MAX_PACKET_SIZE

static USBD_HandleTypeDef s_usbd;
static uint8_t s_rx_buffer[USB_CDC_RX_BUFFER_SIZE];
static UsbCdc_TxDoneCallback_t s_tx_done = NULL;

/* Parámetros de línea que pide el terminal. En un puerto virtual no afectan
 * a nada, pero hay que devolverlos si el PC los pregunta. */
static uint8_t s_line_coding[7] = {0x00, 0xC2, 0x01, 0x00, /* 115200 */
                                   0x00,                   /* 1 stop bit */
                                   0x00,                   /* sin paridad */
                                   0x08};                  /* 8 bits */

/* ===========================================================================
 * Interfaz CDC (la llama la librería desde la interrupción del USB)
 * ======================================================================== */

static int8_t Cdc_Init(void) {
  USBD_CDC_SetRxBuffer(&s_usbd, s_rx_buffer);
  return USBD_OK;
}

static int8_t Cdc_DeInit(void) { return USBD_OK; }

static int8_t Cdc_Control(uint8_t cmd, uint8_t *pbuf, uint16_t length) {
  switch (cmd) {
  case CDC_SET_LINE_CODING:
    if (length >= sizeof(s_line_coding)) {
      memcpy(s_line_coding, pbuf, sizeof(s_line_coding));
    }
    break;
  case CDC_GET_LINE_CODING:
    if (length >= sizeof(s_line_coding)) {
      memcpy(pbuf, s_line_coding, sizeof(s_line_coding));
    }
    break;
  default:
    break;
  }
  return USBD_OK;
}

/* Lo que envía el PC se descarta; solo se vuelve a armar la recepción */
static int8_t Cdc_Receive(uint8_t *buf, uint32_t *len) {
  (void)buf;
  (void)len;
  USBD_CDC_SetRxBuffer(&s_usbd, s_rx_buffer);
  USBD_CDC_ReceivePacket(&s_usbd);
  return USBD_OK;
}

static int8_t Cdc_TransmitCplt(uint8_t *buf, uint32_t *len, uint8_t epnum) {
  (void)buf;
  (void)len;
  (void)epnum;
  if (s_tx_done != NULL) {
    s_tx_done();
  }
  return USBD_OK;
}

static USBD_CDC_ItfTypeDef s_cdc_fops = {
    Cdc_Init, Cdc_DeInit, Cdc_Control, Cdc_Receive, Cdc_TransmitCplt,
};

/* ===========================================================================
 * API
 * ======================================================================== */

void UsbCdc_Init(UsbCdc_TxDoneCallback_t tx_done) {
  s_tx_done = tx_done;

  if ((USBD_Init(&s_usbd, &VCP_Desc, 0) != USBD_OK) ||
      (USBD_RegisterClass(&s_usbd, USBD_CDC_CLASS) != USBD_OK) ||
      (USBD_CDC_RegisterInterface(&s_usbd, &s_cdc_fops) != USBD_OK) ||
      (USBD_Start(&s_usbd) != USBD_OK)) {
    Error_Handler();
  }
}

bool UsbCdc_IsConfigured(void) {
  return s_usbd.dev_state == USBD_STATE_CONFIGURED;
}

UsbCdc_TxResult_t UsbCdc_Transmit(uint8_t *data, uint16_t len) {
  if (!UsbCdc_IsConfigured()) {
    return USB_CDC_TX_OFFLINE;
  }

  const USBD_CDC_HandleTypeDef *hcdc =
      (USBD_CDC_HandleTypeDef *)s_usbd.pClassData;
  if ((hcdc == NULL) || (hcdc->TxState != 0U)) {
    return USB_CDC_TX_BUSY;
  }

  USBD_CDC_SetTxBuffer(&s_usbd, data, len);
  return (USBD_CDC_TransmitPacket(&s_usbd) == USBD_OK) ? USB_CDC_TX_OK
                                                       : USB_CDC_TX_BUSY;
}
