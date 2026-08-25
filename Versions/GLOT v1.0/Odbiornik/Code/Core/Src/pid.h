#ifndef PID_H
#define PID_H

#include <stdint.h>

typedef struct {
    // Współczynniki wzmocnienia
    float Kp;
    float Ki;
    float Kd;

    // Pamięć regulatora
    float integral_sum;
    float prev_error;

    // Limity
    float out_min;
    float out_max;
    float integral_limit;

} PID_Controller;

/* Prototypy funkcji */

/**
 * @brief Inicjalizuje strukture PID zadanymi wartościami
 */
void PID_Init(PID_Controller *pid, float kp, float ki, float kd, float out_min, float out_max, float i_limit);

/**
 * @brief Oblicza nową wartość wyjściową z regulatora PID
 * @param pid Wskaźnik na konkretny regulator
 * @param setpoint Wartość docelowa (np. z aparatury)
 * @param measured Wartość aktualna (np. z żyroskopu MPU9250)
 * @param dt Czas, jaki upłynął od ostatniego wywołania (w sekundach)
 * @return Wyliczona korekta, którą trzeba dodać/odjąć od serwa
 */
float PID_Compute(PID_Controller *pid, float setpoint, float measured, float dt);

/**
 * @brief Resetuje pamięć regulatora (np. przy wyłączeniu silnika/zmianie trybu)
 */
void PID_Reset(PID_Controller *pid);

#endif /* PID_H */
