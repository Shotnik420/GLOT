#include "pid.h"

void PID_Init(PID_Controller *pid, float kp, float ki, float kd, float out_min, float out_max, float i_limit) {
    pid->Kp = kp;
    pid->Ki = ki;
    pid->Kd = kd;

    pid->out_min = out_min;
    pid->out_max = out_max;
    pid->integral_limit = i_limit;

    PID_Reset(pid);
}

float PID_Compute(PID_Controller *pid, float setpoint, float measured, float dt) {
    float error = setpoint - measured;

    //To bo dzieliło przez zero z jakiegoś powodu
    if (dt <= 0.0f) {
        dt = 0.001f;
    }

    // PROPORCJONALNY
    float P_out = pid->Kp * error;

    // 3. CALKUJĄCY
    pid->integral_sum += error * dt;

    // gdy stery są na maxa wychylone, a samolot nadal nie może osiągnąć zadanego kąta
    if (pid->integral_sum > pid->integral_limit) {
        pid->integral_sum = pid->integral_limit;
    } else if (pid->integral_sum < -pid->integral_limit) {
        pid->integral_sum = -pid->integral_limit;
    }

    float I_out = pid->Ki * pid->integral_sum;

    // ROZNICZKUJACY
    float derivative = (error - pid->prev_error) / dt;
    float D_out = pid->Kd * derivative;

    //Zapis błędu
    pid->prev_error = error;

    //SOOMA
    float output = P_out + I_out + D_out;

    //Barierki by nie zadało więcej jak może serwo.
    if (output > pid->out_max) {
        output = pid->out_max;
    } else if (output < pid->out_min) {
        output = pid->out_min;
    }

    return output;
}

void PID_Reset(PID_Controller *pid) {
    pid->integral_sum = 0.0f;
    pid->prev_error = 0.0f;
}
