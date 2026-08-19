#include "controls.h"
#include "comms.h"
#include <stdio.h>

// --- DEFINICJE ZMIENNYCH ---
ServoConfig cfgFlapL   = {1600, 1000, 2000, "Klapa Lewa (FlapL)"};
ServoConfig cfgFlapR   = {1300, 800, 1800, "Klapa Prawa (FlapR)"};
ServoConfig cfgElev    = {1200,  300,  2000, "Wysokosc (Elevator)"};
ServoConfig cfgRudder  = {1400, 1000, 2000, "Kierunek (Rudder)"};

// --- FUNKCJE ---

void Test_Servo(TIM_HandleTypeDef *htim, uint32_t channel, ServoConfig *cfg) {
    char infoBuf[64];
    sprintf(infoBuf, "--> Test: %s\r\n", cfg->name);
    USB_Print(infoBuf);

    __HAL_TIM_SET_COMPARE(htim, channel, cfg->center);
    HAL_Delay(500);
    __HAL_TIM_SET_COMPARE(htim, channel, cfg->min);
    HAL_Delay(600);
    __HAL_TIM_SET_COMPARE(htim, channel, cfg->max);
    HAL_Delay(600);
    __HAL_TIM_SET_COMPARE(htim, channel, cfg->center);
    HAL_Delay(500);
}

int16_t apply_deadzone(int16_t val, int16_t deadzone) {
    if (val > -deadzone && val < deadzone) {
        return 0;
    }
    return val;
}

long map_val(long x, long in_min, long in_max, long out_min, long out_max) {
    long out = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    long min_val = (out_min < out_max) ? out_min : out_max;
    long max_val = (out_min > out_max) ? out_min : out_max;

    if (out < min_val) return min_val;
    if (out > max_val) return max_val;
    return out;
}
