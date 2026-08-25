/*
 * comms.c
 *
 *  Created on: 2 sie 2026
 *      Author: Szoot
 */


#include "comms.h"
#include "usbd_cdc_if.h" // Wymagane dla USB
#include <string.h>

// --- ZMIENNE ZEWNĘTRZNE (Z MAIN.C) ---
extern SPI_HandleTypeDef hspi1;

// --- DEFINICJE PRYWATNE DLA NRF24 ---
#define R_REGISTER    0x00
#define W_REGISTER    0x20
#define R_RX_PAYLOAD  0x61

bool isDebugging = true;

// --- FUNKCJE PRYWATNE (WIDOCZNE TYLKO TUTAJ) ---
	uint8_t nRF24_ReadReg(uint8_t reg) {
    uint8_t tx = R_REGISTER | (reg & 0x1F);
    uint8_t rx;
    HAL_GPIO_WritePin(NRF_CSN_GPIO_Port, NRF_CSN_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &tx, 1, 100);
    HAL_SPI_Receive(&hspi1, &rx, 1, 100);
    HAL_GPIO_WritePin(NRF_CSN_GPIO_Port, NRF_CSN_Pin, GPIO_PIN_SET);
    return rx;
}

static void nRF24_WriteReg(uint8_t reg, uint8_t data) {
    uint8_t buf[2];
    buf[0] = W_REGISTER | (reg & 0x1F);
    buf[1] = data;
    HAL_GPIO_WritePin(NRF_CSN_GPIO_Port, NRF_CSN_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, buf, 2, 100);
    HAL_GPIO_WritePin(NRF_CSN_GPIO_Port, NRF_CSN_Pin, GPIO_PIN_SET);
}

static void nRF24_WriteRegMulti(uint8_t reg, uint8_t *data, uint8_t size) {
    uint8_t cmd = W_REGISTER | (reg & 0x1F);
    HAL_GPIO_WritePin(NRF_CSN_GPIO_Port, NRF_CSN_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &cmd, 1, 100);
    HAL_SPI_Transmit(&hspi1, data, size, 100);
    HAL_GPIO_WritePin(NRF_CSN_GPIO_Port, NRF_CSN_Pin, GPIO_PIN_SET);
}

// --- FUNKCJE PUBLICZNE (Zadeklarowane w comms.h) ---

void nRF24_InitRX(void) {
    HAL_GPIO_WritePin(NRF_CE_GPIO_Port, NRF_CE_Pin, GPIO_PIN_RESET);
    nRF24_WriteReg(0x01, 0x01); // EN_AA: Auto-Ack
    nRF24_WriteReg(0x02, 0x01); // EN_RXADDR: Pipe 0
    nRF24_WriteReg(0x03, 0x03); // SETUP_AW: 5 bytes
    nRF24_WriteReg(0x05, 115);  // RF_CH: 115
    nRF24_WriteReg(0x06, 0x26); // RF_SETUP: 250 kbps, 0 dBm
    nRF24_WriteReg(0x11, sizeof(DanePada));
    uint8_t address[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
    nRF24_WriteRegMulti(0x0A, address, 5);
    nRF24_WriteReg(0x00, 0x0F); // RX Mode, Power UP
    HAL_Delay(2);
    HAL_GPIO_WritePin(NRF_CE_GPIO_Port, NRF_CE_Pin, GPIO_PIN_SET);
}

uint8_t nRF24_DataReady(void) {
    uint8_t status = nRF24_ReadReg(0x07);
    if ((status != 0xFF) && (status & (1 << 6))) {
        nRF24_WriteReg(0x07, (1 << 6)); // Skasowanie flagi przerwania
        return 1;
    }
    return 0;
}

// Od razu wpisujemy dane do przekazanej struktury
void nRF24_ReadPayload(DanePada *dane) {
    uint8_t cmd = R_RX_PAYLOAD;
    HAL_GPIO_WritePin(NRF_CSN_GPIO_Port, NRF_CSN_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &cmd, 1, 100);
    HAL_SPI_Receive(&hspi1, (uint8_t*)dane, sizeof(DanePada), 100); // Czytamy prosto do struktury
    HAL_GPIO_WritePin(NRF_CSN_GPIO_Port, NRF_CSN_Pin, GPIO_PIN_SET);

    cmd = 0xE2; // FLUSH_RX
    HAL_GPIO_WritePin(NRF_CSN_GPIO_Port, NRF_CSN_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &cmd, 1, 100);
    HAL_GPIO_WritePin(NRF_CSN_GPIO_Port, NRF_CSN_Pin, GPIO_PIN_SET);
}

void USB_Print(char *msg) {
	if(isDebugging) return;
    uint32_t start_time = HAL_GetTick();
    while (CDC_Transmit_FS((uint8_t*)msg, strlen(msg)) == USBD_BUSY) {
        if (HAL_GetTick() - start_time > 5) break;
        HAL_Delay(1);
    }
}
