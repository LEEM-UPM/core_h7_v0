/*
 * Puerto serie virtual (USB CDC) del core, por el conector USB (PA11 USB_N,
 * PA12 USB_P). En el PC aparece como un puerto COM; la velocidad que elija
 * el terminal da igual.
 *
 * Solo transmite: lo que llega del PC se descarta.
 */

#ifndef USB_CDC_H
#define USB_CDC_H

#include <stdbool.h>
#include <stdint.h>

/* Resultado de UsbCdc_Transmit() */
typedef enum {
  USB_CDC_TX_OK = 0,  /* transferencia iniciada */
  USB_CDC_TX_BUSY,    /* hay otra en curso: reintentar al terminar */
  USB_CDC_TX_OFFLINE, /* sin PC conectado / sin enumerar */
} UsbCdc_TxResult_t;

/* Llamado desde la interrupción del USB cuando termina una transmisión */
typedef void (*UsbCdc_TxDoneCallback_t)(void);

/* Arranca el dispositivo USB. Llamar una vez tras HAL_Init() y
 * SystemClock_Config(). */
void UsbCdc_Init(UsbCdc_TxDoneCallback_t tx_done);

/* Empieza a enviar `len` bytes. El buffer tiene que seguir válido hasta que
 * se llame a tx_done. Llamar con la interrupción del USB bloqueada o desde
 * tx_done. */
UsbCdc_TxResult_t UsbCdc_Transmit(uint8_t *data, uint16_t len);

/* true si el PC ha enumerado y configurado el dispositivo */
bool UsbCdc_IsConfigured(void);

#endif /* USB_CDC_H */
