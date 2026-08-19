#ifndef CONTROLS_H
#define CONTROLS_H

#include "main.h"

// --- STRUKTURY DANYCH ---
typedef struct {
    uint32_t center;
    uint32_t min;
    uint32_t max;
    char* name;
} ServoConfig;

// --- ZMIENNE GLOBALNE KONFIGURACYJNE ---
extern ServoConfig cfgFlapL;
extern ServoConfig cfgFlapR;
extern ServoConfig cfgElev;
extern ServoConfig cfgRudder;

// --- PROTOTYPY FUNKCJI ---
void Test_Servo(TIM_HandleTypeDef *htim, uint32_t channel, ServoConfig *cfg);
int16_t apply_deadzone(int16_t val, int16_t deadzone);
long map_val(long x, long in_min, long in_max, long out_min, long out_max);

#endif /* CONTROLS_H */
