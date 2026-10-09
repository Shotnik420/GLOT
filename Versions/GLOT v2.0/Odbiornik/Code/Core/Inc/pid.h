#ifndef PID_H
#define PID_H

#include "main.h"

typedef struct {
    float kp;
    float ki;
    float kd;

    float itg;
    float prevErr;

    float iLimit;
    float servoLimit;
} PIDControls;

void PID_Init(PIDControls *pid, float p, float i, float d, float iLim, float sLim);

float PID_Compute(PIDControls *pid, float setpoint, float measured, float dt);
#endif /* CONTROLS_H */
