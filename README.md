# core_h7_v0

Ordenador de vuelo (STM32H723).

## Estructura

| Carpeta / archivo | Qué es |
|-------------------|--------|
| `Core/` | Código generado por CubeMX (solo se toca dentro de `USER CODE`) |
| `App/flight_software.c` | `main()`: arranque y bucle principal |
| `App/sys_utilities.c` | Estado del resto de placas (`sys_state`), wake up y reparto de las tramas de CAN1 |
| `App/sys_power.c` | Placa de potencia: continuidad, encendido de pirotécnicos y raíles |
| `App/sys_avionics.c` | Placa de aviónica: calibración de IMUs y barómetros |
| `App/sys_rf.c` | Placa de RF: manda bytes para transmitir por radio (`sys_rf_transmit`) |
| `App/sensor_board.c` | Telemetría de la placa de sensores (CAN2) |
| `App/debug_console.c` | Consola de debug por USB (puerto COM virtual): banner al arrancar e informe cada segundo |
| `App/storage.c` | Archivos en las dos microSD (FileX standalone): montaje, leer/escribir/borrar, prueba al arrancar |
| `App/sounds.c` | Melodías con el zumbador (Bella Ciao al arrancar) |
| `App/led_show.c` | Animaciones en los 6 LEDs RGB (secuencia de arranque) |
| `Drivers/Buzzer/buzzer.c` | Zumbador en PE4 (TIM15_CH1N): tono de frecuencia variable |
| `Drivers/Leds/leds.c` | Los 6 LEDs RGB en cadena (WL-ICLED, tipo WS2812) en PD12: TIM4_CH1 + DMA |
| `Drivers/Sd_card/` | Acceso por bloques a SD1 (SDMMC1) y SD2 (SDMMC2) con DMA, y driver de FileX |
| `Drivers/Debug_out/debug_out.c` | Salida de texto de debug con buffer circular (no bloquea) sobre el USB |
| `Drivers/Usb_cdc/` | Puerto serie virtual USB (CDC) en USB_OTG_HS full speed, PA11/PA12, reloj HSI48 + CRS |
| `third_party/stm32_usb_device_library/` | Librería USB Device de ST (núcleo + clase CDC), copiada de STM32Cube H7 1.12.1 |
| `Drivers/Can_driver/can_driver.c` | Driver de FDCAN1 y FDCAN2: filtros, envío y recepción con ring buffer |
| `config/` | Configuración de ThreadX/FileX (FileX en modo standalone) |
| `bsp/` | Driver de SD para FileX con ThreadX (solo la PoC `poc/usd_filex_dma`, no se compila) |
| `third_party/` | ThreadX, FileX y `can_protocol` (submodule con el ICD del bus CAN1) |
| `poc/` | Pruebas de concepto (no se compilan por defecto) |

Cada placa nueva que se gestione desde el core va en su propio archivo
`App/sys_<placa>.c`, igual que `sys_power.c`.
