/*
 * Configuración de la librería USB Device de ST para el core
 * (third_party/stm32_usb_device_library).
 *
 * Basado en Projects/STM32H743I-EVAL/Applications/USB_Device/CDC_Standalone
 * del paquete STM32Cube H7, adaptado a USB_OTG_HS con PHY interno de full
 * speed (PA11/PA12) inicializado por MX_USB_OTG_HS_PCD_Init().
 */

#ifndef __USBD_CONF_H
#define __USBD_CONF_H

#include "stm32h7xx_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define USBD_MAX_NUM_INTERFACES     1U
#define USBD_MAX_NUM_CONFIGURATION  1U
#define USBD_MAX_STR_DESC_SIZ       64U
#define USBD_SELF_POWERED           1U
#define USBD_DEBUG_LEVEL            0U

/* Memoria estática: la clase CDC pide un único bloque */
#define USBD_malloc  (void *)USBD_static_malloc
#define USBD_free    USBD_static_free
#define USBD_memset  memset
#define USBD_memcpy  memcpy
#define USBD_Delay   HAL_Delay

#define USBD_UsrLog(...)
#define USBD_ErrLog(...)
#define USBD_DbgLog(...)

void *USBD_static_malloc(uint32_t size);
void USBD_static_free(void *p);

#endif /* __USBD_CONF_H */
