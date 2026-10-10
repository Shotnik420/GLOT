#include "pid.h"
#include "main.h" // Wymagane, żeby pid.c widziało hspi1 i piny zdefiniowane w CubeMX

// Podpinamy SPI z main.c, żeby funkcje MPU mogły z niego korzystać
extern SPI_HandleTypeDef hspi1;

// --- FUNKCJE PID ---

void PID_Init(PIDControls *pid, float p, float i, float d, float iLim, float sLim) {
    pid->kp = p;
    pid->ki = i;
    pid->kd = d;

    pid->itg = 0.0f;
    pid->prevErr = 0.0f;

    pid->iLimit = iLim;
    pid->servoLimit = sLim;
}

// Zmieniono z void na float, bo funkcja zwraca wartość!
float PID_Compute(PIDControls *pid, float desired, float actual, float dt) {
    float err = desired - actual;

    // Bierzemy proporcje do błędu (P)
    float p_out = pid->kp * err; // Poprawiono 'error' na 'err'

    // Sumujemy błąd w czasie (I)
    pid->itg += err * dt; // Poprawiono 'error' na 'err'

    // Blokada przed zbytnim wychyleniem itg
    if(pid->itg > pid->iLimit) {
        pid->itg = pid->iLimit;
    }
    else if(pid->itg < -pid->iLimit) {
        pid->itg = -pid->iLimit; // Dodano brakujący minus!
    }

    // Proporcjowanie itg
    float i_out = pid->itg * pid->ki;

    // Zmiana błędu w czasie (D)
    float dtv = (err - pid->prevErr) / dt;
    float d_out = dtv * pid->kd;

    pid->prevErr = err;

    float total_out = p_out + i_out + d_out;

    // Blokada przed zbytnim wychyleniem SERWA
    if(total_out > pid->servoLimit) {
        total_out = pid->servoLimit; // Poprawiono 'serwoLimit'
    }
    else if(total_out < -pid->servoLimit) {
        total_out = -pid->servoLimit; // Dodano brakujący minus!
    }

    return total_out;
}

// --- FUNKCJE MPU9250 (GY-91) ---

void MPU9250_WriteReg(uint8_t reg, uint8_t data) {
    uint8_t txData[2];
    txData[0] = reg & 0x7F; // Zerowy bit MSB -> ZAPIS
    txData[1] = data;

    // Upewnij się, że w CubeMX pin nazywa się dokładnie GY91CS_Pin i jest na odpowiednim porcie
    HAL_GPIO_WritePin(GPIOA, GY91CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, txData, 2, 10);
    HAL_GPIO_WritePin(GPIOA, GY91CS_Pin, GPIO_PIN_SET);
}

void MPU9250_ReadRegs(uint8_t reg, uint8_t *data, uint8_t len) {
    uint8_t txData = reg | 0x80; // Ustawiony bit MSB -> ODCZYT

    HAL_GPIO_WritePin(GPIOA, GY91CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &txData, 1, 10);
    HAL_SPI_Receive(&hspi1, data, len, 10);
    HAL_GPIO_WritePin(GPIOA, GY91CS_Pin, GPIO_PIN_SET);
}

void MPU9250_Init(void) {
    // Wybudzenie czujnika (PWR_MGMT_1)
    MPU9250_WriteReg(0x6B, 0x00);
    HAL_Delay(100);

    // Filtr dolnoprzepustowy DLPF (CONFIG). Wartość 0x03 to filtr ok. 41Hz. (odcina drgania silnika)
    MPU9250_WriteReg(0x1A, 0x03);

    // Konfiguracja Żyroskopu na ±500 stopni na sekundę (GYRO_CONFIG)
    MPU9250_WriteReg(0x1B, 0x08);

    // Konfiguracja Akcelerometru na ±4g (ACCEL_CONFIG)
    MPU9250_WriteReg(0x1C, 0x08);
}

void MPU9250_Read(IMU_Data_t *imu) {
    uint8_t raw_data[14];

    // Odczyt 14 bajtów zaczynając od rejestru 0x3B (ACCEL_XOUT_H)
    MPU9250_ReadRegs(0x3B, raw_data, 14);

    // Składanie 16-bitowych wartości
    int16_t ax = (raw_data[0] << 8) | raw_data[1];
    int16_t ay = (raw_data[2] << 8) | raw_data[3];
    int16_t az = (raw_data[4] << 8) | raw_data[5];

    int16_t gx = (raw_data[8] << 8) | raw_data[9];
    int16_t gy = (raw_data[10] << 8) | raw_data[11];
    int16_t gz = (raw_data[12] << 8) | raw_data[13];

    // Przeliczenie na wartości fizyczne:
    // Akcelerometr dla ±4g ma czułość 8192 LSB/g
    imu->accel_x = (float)ax / 8192.0f;
    imu->accel_y = (float)ay / 8192.0f;
    imu->accel_z = (float)az / 8192.0f;

    // Żyroskop dla ±500 dps ma czułość 65.5 LSB/dps
    imu->gyro_x = (float)gx / 65.5f;
    imu->gyro_y = (float)gy / 65.5f;
    imu->gyro_z = (float)gz / 65.5f;
}
