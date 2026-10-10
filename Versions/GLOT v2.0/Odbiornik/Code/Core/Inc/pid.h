#ifndef PID_H
#define PID_H

#include <stdint.h>

// --- STRUKTURA DLA PID ---
typedef struct {
    float kp;
    float ki;
    float kd;

    float itg;
    float prevErr;

    float iLimit;
    float servoLimit;
} PIDControls;

// --- STRUKTURA DLA MPU9250 ---
typedef struct {
    float accel_x;
    float accel_y;
    float accel_z;
    float gyro_x;
    float gyro_y;
    float gyro_z;
} IMU_Data_t;


// --- DEKLARACJE FUNKCJI ---

void PID_Init(PIDControls *pid, float p, float i, float d, float iLim, float sLim);
float PID_Compute(PIDControls *pid, float desired, float actual, float dt);

void MPU9250_WriteReg(uint8_t reg, uint8_t data);
void MPU9250_ReadRegs(uint8_t reg, uint8_t *data, uint8_t len);
void MPU9250_Init(void);
void MPU9250_Read(IMU_Data_t *imu);

#endif // PID_H
