#ifndef COMMS_H
#define COMMS_H

#include "main.h"
#include <stdint.h>
#include <stdbool.h>
// --- STRUKTURY DANYCH ---
// Przenosimy strukturę tutaj, bo main.c będzie jej używał do odbierania danych
typedef struct {
    int16_t lx, ly, rx, ry, tl, tr;
    uint8_t silnik;
} DanePada;

// --- PROTOTYPY FUNKCJI PUBLICZNYCH ---
extern bool isDebugging;
// Funkcje NRF24
void nRF24_InitRX(void);
uint8_t nRF24_DataReady(void);
void nRF24_ReadPayload(DanePada *dane); // Zmiana! Od razu przyjmuje strukturę
uint8_t nRF24_ReadReg(uint8_t reg);

// Funkcje USB
void USB_Print(char *msg);

#endif /* COMMS_H */
