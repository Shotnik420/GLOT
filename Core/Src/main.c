/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "nRF24.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "usb_device.h"
#include "comms.h"
#include "controls.h"
#include "pid.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define R_REGISTER    0x00
#define REG_CONFIG    0x00
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim11;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM11_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3; // Dodany TIM3 dla Elevator i Rudder
extern I2C_HandleTypeDef hi2c1; // Zakładam, że tak nazywa się zmienna I2C
#define MPU_ADDR (0x68 << 1)    // Adres 0xD0

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_TIM11_Init();
  MX_USB_DEVICE_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
          HAL_GPIO_WritePin(NRF_CSN_GPIO_Port, NRF_CSN_Pin, GPIO_PIN_SET);
          HAL_GPIO_WritePin(NRF_CE_GPIO_Port, NRF_CE_Pin, GPIO_PIN_RESET);
          char buf[64];
            USB_Print("\r\n--- ROZPOCZYNAM SKANOWANIE I2C ---\r\n");
            int devices_found = 0;

            for(uint16_t i = 1; i < 128; i++) {
                //Bit w lewo
                if(HAL_I2C_IsDeviceReady(&hi2c1, (i << 1), 2, 10) == HAL_OK) {
                    sprintf(buf, "Znaleziono urzadzenie pod adresem: 0x%02X\r\n", i);
                    USB_Print(buf);
                    devices_found++;
                }
            }

            if(devices_found == 0) {
                USB_Print("Brak urzadzen I2C! (Sprawdz zasilanie i kable SDA/SCL)\r\n");
            } else {
                USB_Print("Skanowanie zakonczone.\r\n");
            }
          PID_Controller pid_roll;
          PID_Controller pid_pitch;
          PID_Init(&pid_roll,  15.0f, 0.0f, 0.1f, -1000.0f, 1000.0f, 100.0f);
          PID_Init(&pid_pitch, 15.0f, 0.0f, 0.1f, -1000.0f, 1000.0f, 100.0f);
          HAL_Delay(1000);

          HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1); // THROTTLE
          HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // FLAPL
          HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); // FLAPR
          HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3); // ELEVATOR
          HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4); // RUDDER

          // 0. USTAWIENIE POZYCJI STARTOWYCH (ŚRODEK) DLA WSZYSTKICH SERW
          __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, cfgFlapL.center);
          __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, cfgFlapR.center);
          __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, cfgElev.center);
          __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, cfgRudder.center);

          USB_Print("\r\n=== START SYSTEMU ===\r\n");
          USB_Print("Uzbrajam ESC (Czekam 4 sekundy)...\r\n");


          USB_Print("\r\nSkanowanie I2C...\r\n");

          // Sprawdzamy, czy urządzenie pod tym adresem odpowiada
          if (HAL_I2C_IsDeviceReady(&hi2c1, MPU_ADDR, 2, 100) == HAL_OK) {
              USB_Print("SUKCES: Znaleziono zyroskop MPU9250 (GY-91)!\r\n");

              // Opcjonalnie: Odczyt rejestru WHO_AM_I (adres 0x75)
              uint8_t who_am_i = 0;
              HAL_I2C_Mem_Read(&hi2c1, MPU_ADDR, 0x75, 1, &who_am_i, 1, 100);

              char buf[64];
              sprintf(buf, "WHO_AM_I: 0x%02X (Powinno byc 0x71 lub 0x73)\r\n", who_am_i);
              USB_Print(buf);

              // WYBUDZENIE MPU9250 Z UŚPIENIA (zapis 0x00 do rejestru PWR_MGMT_1 - 0x6B)
              uint8_t pwr_mgmt = 0x00;
              HAL_I2C_Mem_Write(&hi2c1, MPU_ADDR, 0x6B, 1, &pwr_mgmt, 1, 100);
              USB_Print("MPU9250 pomyslnie wybudzony!\r\n");

          } else {
              USB_Print("BLAD: Nie wykryto zyroskopu. Sprawdz kable!\r\n");
          }
          // 1. UZBROJENIE ESC
          __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 900);
          HAL_Delay(4000);

          // 2. TEST SILNIKA
          USB_Print("TEST SILNIKA - UWAGA!\r\n");
          __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 1150);
          HAL_Delay(1500);
          __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 900);
          HAL_Delay(1000);

          // 3. TEST POWIERZCHNI STEROWYCH
          USB_Print("Test powierzchni sterowych...\r\n");
          Test_Servo(&htim2, TIM_CHANNEL_2, &cfgFlapL);
          Test_Servo(&htim2, TIM_CHANNEL_3, &cfgFlapR);
          Test_Servo(&htim3, TIM_CHANNEL_3, &cfgElev);
          Test_Servo(&htim3, TIM_CHANNEL_4, &cfgRudder);

          USB_Print("Kalibracja zakonczona. Inicjalizuje NRF24...\r\n");

          // 4. START RADIA
          nRF24_InitRX();

          uint8_t reg_status = nRF24_ReadReg(0x07);
          uint8_t reg_setup  = nRF24_ReadReg(0x06);
          uint8_t reg_ch     = nRF24_ReadReg(0x05);

          char usbBuf[128];
          sprintf(usbBuf, "Radio STATUS: 0x%02X | SETUP: 0x%02X | KANAL: %d\r\n", reg_status, reg_setup, reg_ch);
          USB_Print(usbBuf);

          if(reg_status == 0x00 || reg_status == 0xFF) {
              USB_Print("!!! BLAD KOMUNIKACJI SPI Z RADIEM !!!\r\n");
          } else {
              USB_Print("Radio gotowe. Nasluchuje...\r\n");
          }

          HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
          // ZMIENNE GLOBALNE W PĘTLI
            float target_roll = 0.0f;
            float target_pitch = 0.0f;
            float manual_rudder = 0.0f;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
            DanePada daneOdebrane;
            static uint32_t lastReceiveTime = 0;
            static uint32_t lastMpuReadTime = 0; // Dodano dla czasomierza MPU

            // Wartości brzegowe dla pada w Bluepad32
            const int16_t PAD_MIN = -512;
            const int16_t PAD_MAX = 511;
             static uint32_t lastPrintTime = 0;
            while (1)
              {

                  //1. Odbiór i zmiana kątów docelowych (Setpoint)

                  if (nRF24_DataReady()) {
                      nRF24_ReadPayload(&daneOdebrane);
                      lastReceiveTime = HAL_GetTick();

                      daneOdebrane.lx = apply_deadzone(daneOdebrane.lx, 100);
                      daneOdebrane.ly = apply_deadzone(daneOdebrane.ly, 100);
                      daneOdebrane.rx = apply_deadzone(daneOdebrane.rx, 100);
                      daneOdebrane.ry = apply_deadzone(daneOdebrane.ry, 100);

                      // MAPOWANIE DRĄŻKÓW NA KĄTY DO TARGETU
                      target_roll = (float)map_val(daneOdebrane.ly, PAD_MIN, PAD_MAX, -45, 45);
                      target_pitch = (float)map_val(daneOdebrane.ry, PAD_MIN, PAD_MAX, -45, 45);

                      // Rudder zostawiamy jako procent wychylenia (bez PID)
                      manual_rudder = daneOdebrane.lx;

                      // SILNIK (Gaz)
                      if (daneOdebrane.silnik == 1) {
                          uint32_t pwmThr = map_val(daneOdebrane.tr, 0, 1023, 900, 2000);
                          __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, pwmThr);
                      } else {
                          __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 900);
                      }
                  }

                  // =========================================================================
                  // 2. FAILSAFE
                  if (HAL_GetTick() - lastReceiveTime > 500) {
                      __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 900); // Wyłącz silnik

                      // Wymuszamy lot ślizgowy w kółko
                      target_roll = 0.0f;   // Skrzydła poziomo (niech PID o to dba)
                      target_pitch = 10.0f; // Lekko zadarty nos do szybowania (niech PID to trzyma)

                      uint32_t failRudder = cfgRudder.center + ((cfgRudder.max - cfgRudder.center) / 4);
                      __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, failRudder);
                  }

                  // =========================================================================
                  // 3. ODCZYT GY-91
                  if (HAL_GetTick() - lastMpuReadTime > 20) {
                      float dt = (float)(HAL_GetTick() - lastMpuReadTime) / 1000.0f;
                      lastMpuReadTime = HAL_GetTick();

                      uint8_t mpu_data[14];
                      if (HAL_I2C_Mem_Read(&hi2c1, MPU_ADDR, 0x3B, 1, mpu_data, 14, 100) == HAL_OK) {

                          // Rozkodowanie akcelerometru
                          float accel_x = (float)((int16_t)((mpu_data[0] << 8) | mpu_data[1]));
                          float accel_y = (float)((int16_t)((mpu_data[2] << 8) | mpu_data[3]));
                          float accel_z = (float)((int16_t)((mpu_data[4] << 8) | mpu_data[5]));

                          // Wyliczanie aktualnego kąta
                          float angle_roll = atan2f(accel_y, accel_z) * (180.0f / 3.14159265f);
                          float angle_pitch = atan2f(-accel_x, sqrtf(accel_y*accel_y + accel_z*accel_z)) * (180.0f / 3.14159265f);

                          // OBLICZANIE PID
                          float pid_out_roll = PID_Compute(&pid_roll, target_roll, angle_roll, dt);
                          float pid_out_pitch = PID_Compute(&pid_pitch, target_pitch, angle_pitch, dt);

                          // -----------------------------------------------------------
                          // MIKSER: Łączenie środka serwa z korektą PID
                          // -----------------------------------------------------------

                          // Lotki (Roll). Jedna lotka musi iść w górę, druga w dół, więc jedna ma "+", druga "-"!
                          // UWAGA: Zależnie od tego jak zamontujesz serwa, być może będziesz musiał zamienić znak.
                          int32_t out_flapL = cfgFlapL.center + (int32_t)pid_out_roll;
                          int32_t out_flapR = cfgFlapR.center + (int32_t)pid_out_roll;

                          // Ster wysokości (Pitch)
                          int32_t out_elev = cfgElev.center + (int32_t)pid_out_pitch;

                          // Ster kierunku (Rudder) - w 100% manualny
                          int32_t out_rudder = map_val(manual_rudder, PAD_MIN, PAD_MAX, cfgRudder.min, cfgRudder.max);

                          // SATURACJA
                          if(out_flapL > cfgFlapL.max) out_flapL = cfgFlapL.max;
                          if(out_flapL < cfgFlapL.min) out_flapL = cfgFlapL.min;

                          if(out_flapR > cfgFlapR.max) out_flapR = cfgFlapR.max;
                          if(out_flapR < cfgFlapR.min) out_flapR = cfgFlapR.min;

                          if(out_elev > cfgElev.max) out_elev = cfgElev.max;
                          if(out_elev < cfgElev.min) out_elev = cfgElev.min;

                          // WYSYŁANIE SYGNAŁÓW DO TIMERÓW
                          __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, out_flapL);
                          __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, out_flapR);
                          __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, out_elev);
                          __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, out_rudder);


                          if (HAL_GetTick() - lastPrintTime > 500) {
                                            lastPrintTime = HAL_GetTick();
                                            char buf[128];
                                            sprintf(buf, "ROLL: %5.1f* -> Serwo L: %4ld, R: %4ld | PITCH: %5.1f* -> Serwo Elev: %4ld\r\n",
                                                    angle_roll, out_flapL, out_flapR, angle_pitch, out_elev);
                                            USB_Print(buf);
                                        }
                      }
                  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 83;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 19999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 83;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 19999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM11 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM11_Init(void)
{

  /* USER CODE BEGIN TIM11_Init 0 */

  /* USER CODE END TIM11_Init 0 */

  /* USER CODE BEGIN TIM11_Init 1 */

  /* USER CODE END TIM11_Init 1 */
  htim11.Instance = TIM11;
  htim11.Init.Prescaler = 83;
  htim11.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim11.Init.Period = 65535;
  htim11.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim11.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim11) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM11_Init 2 */

  /* USER CODE END TIM11_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(NRF_CE_GPIO_Port, NRF_CE_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(NRF_CSN_GPIO_Port, NRF_CSN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : NRF_CE_Pin NRF_CSN_Pin */
  GPIO_InitStruct.Pin = NRF_CE_Pin|NRF_CSN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
