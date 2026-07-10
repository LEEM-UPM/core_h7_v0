#include "gpio.h"
#include "main.h"
#include "stm32h723xx.h"
#include "fdcan.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>

FDCAN_TxHeaderTypeDef TxHeader; 
uint8_t TxData[8];
FDCAN_RxHeaderTypeDef RxHeader; 
uint8_t RxData[8];
volatile uint8_t message_received = 0;

void uart_log(char* str);
static void MX_FDCAN_User_Config(void);
extern void SystemClock_Config(void);
extern void PeriphCommonClock_Config(void);
static void log_uart(const char* msg);

int main(void){
    HAL_Init();
    SystemClock_Config();
    PeriphCommonClock_Config();
    MX_UART5_Init();
    MX_FDCAN1_Init();
    MX_FDCAN2_Init();
    MX_FDCAN_User_Config();

    // inicializar datos de prueba 
    TxData[0] = 0xAA;
    TxData[1] = 0xBB;
    TxData[2] = 0xCC; 
    TxData[3] = 0xDD;

    while(1){
        if(HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, TxData) != HAL_OK){
            log_uart("Error al solicitar trnamision desde fdcan1\n");
        } else {
            log_uart("Mensage enviado\n");
        }
        HAL_Delay(500);

        if(message_received){
            message_received = 0;
            log_uart("Mensage recibido con exito en fdcan2\n");
        }
    }
}

static void MX_FDCAN_User_Config(void){

    /*1. Configurar filtro fdcan2 para recepcion*/
    FDCAN_FilterTypeDef sFilterConfig;
    sFilterConfig.IdType = FDCAN_STANDARD_ID;
    sFilterConfig.FilterIndex = 0;
    sFilterConfig.FilterType = FDCAN_FILTER_RANGE;
    sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    sFilterConfig.FilterID1 = 0x111;
    sFilterConfig.FilterID2 = 0x7FF;

    if(HAL_FDCAN_ConfigFilter(&hfdcan2, &sFilterConfig) != HAL_OK){
        Error_Handler();
    }

    /*2. Arrancar comunicaciones FDCAN*/
    if(HAL_FDCAN_Start(&hfdcan1) != HAL_OK) Error_Handler();
    if(HAL_FDCAN_Start(&hfdcan2) != HAL_OK) Error_Handler();

    /*3. Activar notificaciones por interrupcion en fdcan2*/
    if(HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK){
        Error_Handler();
    }

    /*4. Preparar la cabecera de transmision FDCAN1 */
    TxHeader.Identifier = 0x321;
    TxHeader.IdType = FDCAN_STANDARD_ID;
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;
    TxHeader.DataLength = FDCAN_DLC_BYTES_4;
    TxHeader.ErrorStateIndicator = FDCAN_ESI_PASSIVE;
    TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
    TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
    TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    TxHeader.MessageMarker = 0;

}

void uart_log(char* str){
    HAL_UART_Transmit(&huart5, (uint8_t*)str,strlen(str),10);
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs){
    if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != 0){
        if(hfdcan->Instance = FDCAN2){
            /*extraer datos almacenados en la fifo (funcion a parte)*/
            if(HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK){
                message_received = 1;
            }
        }
    }
}

static void log_uart(const char *msg) {
  HAL_UART_Transmit(&huart5, (uint8_t *)msg, strlen(msg), 10);
}