/*
 * Descriptores USB del puerto serie virtual del core. Basado en el ejemplo
 * CDC_Standalone de STM32Cube H7.
 *
 * VID/PID de ST para puerto serie virtual (0x0483/0x5740): Windows 10/11,
 * Linux y macOS lo reconocen sin instalar drivers.
 */

#include "usbd_desc.h"

#include "usbd_conf.h"
#include "usbd_core.h"

#define USBD_VID                 0x0483U
#define USBD_PID                 0x5740U
#define USBD_LANGID_STRING       0x409U
#define USBD_MANUFACTURER_STRING "LEEM UPM"
#define USBD_PRODUCT_STRING      "LEEM Core H7 (debug)"
#define USBD_CONFIGURATION_STRING "VCP Config"
#define USBD_INTERFACE_STRING    "VCP Interface"

static uint8_t *DeviceDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
static uint8_t *LangIDStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
static uint8_t *ManufacturerStrDescriptor(USBD_SpeedTypeDef speed,
                                          uint16_t *length);
static uint8_t *ProductStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
static uint8_t *SerialStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
static uint8_t *ConfigStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
static uint8_t *InterfaceStrDescriptor(USBD_SpeedTypeDef speed,
                                       uint16_t *length);

USBD_DescriptorsTypeDef VCP_Desc = {
    DeviceDescriptor,     LangIDStrDescriptor,    ManufacturerStrDescriptor,
    ProductStrDescriptor, SerialStrDescriptor,    ConfigStrDescriptor,
    InterfaceStrDescriptor,
};

static uint8_t s_device_desc[USB_LEN_DEV_DESC] = {
    0x12,                       /* bLength */
    USB_DESC_TYPE_DEVICE,       /* bDescriptorType */
    0x00, 0x02,                 /* bcdUSB 2.00 */
    0x02,                       /* bDeviceClass: CDC */
    0x02,                       /* bDeviceSubClass */
    0x00,                       /* bDeviceProtocol */
    USB_MAX_EP0_SIZE,           /* bMaxPacketSize */
    LOBYTE(USBD_VID), HIBYTE(USBD_VID),
    LOBYTE(USBD_PID), HIBYTE(USBD_PID),
    0x00, 0x02,                 /* bcdDevice 2.00 */
    USBD_IDX_MFC_STR,           /* iManufacturer */
    USBD_IDX_PRODUCT_STR,       /* iProduct */
    USBD_IDX_SERIAL_STR,        /* iSerialNumber */
    USBD_MAX_NUM_CONFIGURATION, /* bNumConfigurations */
};

static uint8_t s_langid_desc[USB_LEN_LANGID_STR_DESC] = {
    USB_LEN_LANGID_STR_DESC, USB_DESC_TYPE_STRING,
    LOBYTE(USBD_LANGID_STRING), HIBYTE(USBD_LANGID_STRING),
};

static uint8_t s_serial_desc[USB_SIZ_STRING_SERIAL] = {
    USB_SIZ_STRING_SERIAL, USB_DESC_TYPE_STRING,
};

static uint8_t s_str_desc[USBD_MAX_STR_DESC_SIZ];

/* Valor hexadecimal -> caracteres UTF-16 */
static void IntToUnicode(uint32_t value, uint8_t *pbuf, uint8_t len) {
  for (uint8_t idx = 0U; idx < len; idx++) {
    uint8_t nibble = (uint8_t)(value >> 28);
    pbuf[2U * idx] = (nibble < 0xAU) ? (uint8_t)(nibble + '0')
                                     : (uint8_t)(nibble + 'A' - 10U);
    pbuf[(2U * idx) + 1U] = 0U;
    value <<= 4;
  }
}

static uint8_t *DeviceDescriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
  (void)speed;
  *length = sizeof(s_device_desc);
  return s_device_desc;
}

static uint8_t *LangIDStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
  (void)speed;
  *length = sizeof(s_langid_desc);
  return s_langid_desc;
}

static uint8_t *ManufacturerStrDescriptor(USBD_SpeedTypeDef speed,
                                          uint16_t *length) {
  (void)speed;
  USBD_GetString((uint8_t *)USBD_MANUFACTURER_STRING, s_str_desc, length);
  return s_str_desc;
}

static uint8_t *ProductStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
  (void)speed;
  USBD_GetString((uint8_t *)USBD_PRODUCT_STRING, s_str_desc, length);
  return s_str_desc;
}

/* Número de serie a partir del identificador único del micro */
static uint8_t *SerialStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
  (void)speed;
  const uint32_t uid0 = *(uint32_t *)(UID_BASE);
  const uint32_t uid1 = *(uint32_t *)(UID_BASE + 4U);
  const uint32_t uid2 = *(uint32_t *)(UID_BASE + 8U);

  IntToUnicode(uid0 + uid2, &s_serial_desc[2], 8U);
  IntToUnicode(uid1, &s_serial_desc[18], 4U);

  *length = USB_SIZ_STRING_SERIAL;
  return s_serial_desc;
}

static uint8_t *ConfigStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
  (void)speed;
  USBD_GetString((uint8_t *)USBD_CONFIGURATION_STRING, s_str_desc, length);
  return s_str_desc;
}

static uint8_t *InterfaceStrDescriptor(USBD_SpeedTypeDef speed,
                                       uint16_t *length) {
  (void)speed;
  USBD_GetString((uint8_t *)USBD_INTERFACE_STRING, s_str_desc, length);
  return s_str_desc;
}
