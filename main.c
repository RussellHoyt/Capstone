/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "usb_host.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx_hal_tim.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_ADC1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC2_Init(void);
static void MX_TIM6_Init(void);
void MX_USB_HOST_Process(void);

/* USER CODE BEGIN PFP */
uint8_t Rx_data[11];
uint32_t test = 5;

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	HAL_UART_Receive_IT (&huart2, Rx_data, 11);

}


void Set_Pin_Output (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void Set_Pin_Input (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void BUS_delay (uint32_t us)
{
    __HAL_TIM_SET_COUNTER(&htim6,0);
    while ((__HAL_TIM_GET_COUNTER(&htim6)) < us);

}

uint8_t BUS_Start (void)
{
    uint8_t Response = 0;
    Set_Pin_Output(GPIOA, GPIO_PIN_4);

    HAL_GPIO_WritePin (GPIOA, GPIO_PIN_4, 0);

    BUS_delay(480);

    Set_Pin_Input(GPIOA, GPIO_PIN_4);

    BUS_delay(80);

    if (!(HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_4))) Response = 1;
    else Response = -1;

    BUS_delay(400);

    return Response;
}

void BUS_Write (uint8_t data)
{
    Set_Pin_Output(GPIOA, GPIO_PIN_4);

    for (int i=0; i<8; i++)
    {

        if ((data & (1<<i))!=0) {
            // write 1

            Set_Pin_Output(GPIOA,GPIO_PIN_4);
            HAL_GPIO_WritePin (GPIOA, GPIO_PIN_4, 0);
            BUS_delay(1);

            Set_Pin_Input(GPIOA, GPIO_PIN_4);
            BUS_delay(60);
        }

        else {
            // write 0

            Set_Pin_Output(GPIOA, GPIO_PIN_4);
            HAL_GPIO_WritePin (GPIOA, GPIO_PIN_4, 0);
            BUS_delay(60);

            Set_Pin_Input(GPIOA, GPIO_PIN_4);
        }
    }
}

uint8_t BUS_Read (void)
{
    uint8_t value=0;
    Set_Pin_Input(GPIOA, GPIO_PIN_4);

    for (int i=0;i<8;i++)
    {
    	Set_Pin_Output(GPIOA,GPIO_PIN_4);

        HAL_GPIO_WritePin (GPIOA, GPIO_PIN_4, 0);
        BUS_delay(2);

        Set_Pin_Input(GPIOA, GPIO_PIN_4);
        if (HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_4))
        {
            value |= 1<<i;  // read = 1
        }
        BUS_delay(60);
    }
    return value;
}


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
  HAL_TIM_Base_Start(&htim6);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_USB_HOST_Init();
  MX_ADC1_Init();
  MX_USART2_UART_Init();
  MX_ADC2_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */




  HAL_UART_Receive_IT (&huart2, Rx_data, 11);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	  unsigned int analog_value_water_level;
	  //unsigned int i;
	  signed int pH_Level_ADC;
	  double pH_Level;
	  unsigned int tds;
	  uint8_t temp1, temp2;
	  uint8_t Reading = 0;
	  uint16_t comTemp;
	  double temperature_reading = 0;
	  char msg[30];
	  unsigned int senseAuto;
	  unsigned int senseMan;
	  unsigned int pump1;
	  unsigned int pump2;
	  unsigned int pump3;
	  unsigned int pump4;
	  unsigned int pump5;
	  unsigned int pump6;
	  unsigned int pump7;
	  unsigned int pump8;

	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_RESET);

	  while (1)
	  {
		//set all values
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET); // Pump 1
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET); // Pump 2
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET); // Pump 3
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET); // Pump 4
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET); // Pump 5
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_SET); // Pump 6
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET); // Pump 7



		if (Rx_data[0] == 'S'){
			senseAuto = 0;
			senseMan = 0;
			pump1 = 0;
			pump2 = 0;
			pump3 = 0;
			pump4 = 0;
			pump5 = 0;
			pump6 = 0;
			pump7 = 0;
			pump8 = 0;


		}
		else {

			senseAuto = Rx_data[0];
			senseMan = Rx_data [1];
			pump1 = Rx_data[2];
			pump2 = Rx_data[3];
			pump3 = Rx_data[4];
			pump4 = Rx_data[5];
			pump5 = Rx_data[6];
			pump5 = Rx_data[7];
			pump6 = Rx_data[8];
			pump7 = Rx_data[9];
			pump8 = Rx_data[10];
		}


		HAL_ADC_Start(&hadc2);
		HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY);

		analog_value_water_level = HAL_ADC_GetValue(&hadc2);

	    HAL_ADC_Stop(&hadc2);
	    //senseAuto = '1';
	    if (senseAuto == '1') {
	    	// Fill sensor tank
	    	while(analog_value_water_level < 1400){
	    		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
	    		// Might need to use different ADC for water level
	    		HAL_ADC_Start(&hadc2);
	    		HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY);

	    		analog_value_water_level = HAL_ADC_GetValue(&hadc2);

	    		HAL_ADC_Stop(&hadc2);

	    		sprintf(msg, "%u, %s", analog_value_water_level, Rx_data);
	    			    			    	// Send out all data to user
	    		HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
	    		HAL_Delay(250);

	    	}
	    	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);

	    	// do sense
	    	// Sense all data

//	    	Reading = BUS_Start();
//
//	    	HAL_Delay (1);
//
//	    	BUS_Write (0xCC);
//	    	BUS_Write (0x44);
//
//	    	HAL_Delay (800);
//
//	    	Reading = BUS_Start();
//
//	    	HAL_Delay(1);
//
//	    	BUS_Write (0xCC);  // skip ROM
//	    	BUS_Write (0xBE);  // Read Scratch-pad
//
//	    	temp1 = BUS_Read();
//	    	temp2 = BUS_Read();
//	    	comTemp = ((temp2<<8)|temp1);
//
//	    	temperature_reading = (float)comTemp/16;

	    	HAL_ADC_Start(&hadc1);
//	    	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
//
//	    	temperature_reading = HAL_ADC_GetValue(&hadc1);



	    	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);

	    	tds = HAL_ADC_GetValue(&hadc1);

	    	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);

	    	pH_Level_ADC = HAL_ADC_GetValue(&hadc1);
	    	pH_Level = -1 *(((pH_Level_ADC / 1024.0) * 14)-14); //cast to float

	    	HAL_ADC_Stop(&hadc1);
	    	// send data
	    	// Add more data from other sensors to this ****
	    	sprintf(msg, "%u, %.3f, %.3f, %s", tds, temperature_reading, pH_Level, Rx_data);
	    	// Send out all data to user
	    	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);


//	    	// check data with given values
			  if(pH_Level > 6.5){
				  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_9); //Pump 2
				  HAL_Delay(3750);
				  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
			  }
			  if(pH_Level < 5.5){
				  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9); //Pump 3
				  HAL_Delay(3750);
				  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
			  }


	    	  if(tds < 100){
	    		  //put ~1mL of each solution
	    		  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8); //Pump 4
	    		  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7); //Pump 5
	    		  HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_11); //Pump 6
	    		  HAL_Delay(1875);
	    		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	    		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
	    		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_SET);

	    	  }

			// Add more data points for all sensors to automate pumps
	    }
	    else if (senseMan == '1') {
	    	// Fill sensor tank
	    	while(analog_value_water_level < 1400){
	    		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
	    		// Might need to use different ADC for water level
	    		HAL_ADC_Start(&hadc2);
	    		HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY);

	    		analog_value_water_level = HAL_ADC_GetValue(&hadc2);

	    		HAL_ADC_Stop(&hadc2);

	    		sprintf(msg, "%u, %s", analog_value_water_level, Rx_data);
	    			    			    	// Send out all data to user
	    		HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
	    		HAL_Delay(250);

	    	}
	    	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);

	    	// do sense
	    	// Sense all data

//	    	Reading = BUS_Start();
//
//	    	HAL_Delay (1);
//
//	    	BUS_Write (0xCC);
//	    	BUS_Write (0x44);
//
//	    	HAL_Delay (800);
//
//	    	Reading = BUS_Start();
//
//	    	HAL_Delay(1);
//
//	    	BUS_Write (0xCC);  // skip ROM
//	    	BUS_Write (0xBE);  // Read Scratch-pad
//
//	    	temp1 = BUS_Read();
//	    	temp2 = BUS_Read();
//	    	comTemp = ((temp2<<8)|temp1);
//
//	    	temperature_reading = (float)comTemp/16;

	    	HAL_ADC_Start(&hadc1);
//	    	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
//
//	    	temperature_reading = HAL_ADC_GetValue(&hadc1);



	    	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);

	    	tds = HAL_ADC_GetValue(&hadc1);

	    	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);

	    	pH_Level_ADC = HAL_ADC_GetValue(&hadc1);
	    	pH_Level = -1 *(((pH_Level_ADC / 1024.0) * 14)-14); //cast to float

	    	HAL_ADC_Stop(&hadc1);
	    	// send data
	    	// Add more data from other sensors to this ****
	    	sprintf(msg, "%u, %.3f, %.3f, %s", tds, temperature_reading, pH_Level, Rx_data);
	    	// Send out all data to user
	    	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);


	    }

//
//	    HAL_ADC_Start(&hadc1);
//	    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
//
//	    analog_value_water_level = HAL_ADC_GetValue(&hadc1);
//
//	    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
//	    temperature_reading = HAL_ADC_GetValue(&hadc1);
//
//	    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
//	    tds = HAL_ADC_GetValue(&hadc1);
//
//	    HAL_ADC_Stop(&hadc1);
//
//
//	    sprintf(msg, "%u, %u, %u, %s", analog_value_water_level, temperature_reading, tds, Rx_data);
//	    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
//		//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);

		  if(pump2 == '1'){
		  	  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_9); //Pump 2
		  	  HAL_Delay(1875);
		  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);

		  }
		  if(pump3 == '1'){
		      HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9); //Pump 3
		  	  HAL_Delay(1875);
		  	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);

		  }
		  if(pump4 == '1'){
		  	 HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8); //Pump 4
		  	 HAL_Delay(1875);
		  	 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);

		  }
		  if(pump5 == '1'){
			 HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7); //Pump 5
			 HAL_Delay(1875);
			 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
		  }
		  if(pump6 == '1'){
		  	 HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_11); //Pump 6
		  	 HAL_Delay(1875);
		  	 HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_SET);

		  }
		  if(pump7 == '1'){
		  	 HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_10); //Pump 7
		  	 HAL_Delay(1875);
		  	 HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET);

		  }
		  if(pump8 == '1'){
			 HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_9); //Pump Main pump
		  }




		  memset(msg, 0, 30);
		  memset(Rx_data, 0, 11);

		  HAL_Delay(1000);
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_10B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 2;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_14;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_15;
  sConfig.Rank = 2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc2.Init.Resolution = ADC_RESOLUTION_12B;
  hadc2.Init.ScanConvMode = DISABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion = 1;
  hadc2.Init.DMAContinuousRequests = DISABLE;
  hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */

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
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
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
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */



  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 0;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 65535;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */
  __TIM6_CLK_ENABLE();

  HAL_TIM_Base_Init(&htim6);
  HAL_TIM_Base_Start(&htim6);
  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CS_I2C_SPI_GPIO_Port, CS_I2C_SPI_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(OTG_FS_PowerSwitchOn_GPIO_Port, OTG_FS_PowerSwitchOn_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|LD4_Pin
                          |LD3_Pin|LD5_Pin|LD6_Pin|Audio_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10, GPIO_PIN_RESET);

  /*Configure GPIO pin : CS_I2C_SPI_Pin */
  GPIO_InitStruct.Pin = CS_I2C_SPI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CS_I2C_SPI_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : OTG_FS_PowerSwitchOn_Pin PC7 PC8 PC9 */
  GPIO_InitStruct.Pin = OTG_FS_PowerSwitchOn_Pin|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PDM_OUT_Pin */
  GPIO_InitStruct.Pin = PDM_OUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(PDM_OUT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BOOT1_Pin */
  GPIO_InitStruct.Pin = BOOT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BOOT1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : CLK_IN_Pin */
  GPIO_InitStruct.Pin = CLK_IN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(CLK_IN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PD9 PD10 PD11 LD4_Pin
                           LD3_Pin LD5_Pin LD6_Pin Audio_RST_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|LD4_Pin
                          |LD3_Pin|LD5_Pin|LD6_Pin|Audio_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PA8 PA9 PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : I2S3_SCK_Pin I2S3_SD_Pin */
  GPIO_InitStruct.Pin = I2S3_SCK_Pin|I2S3_SD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : OTG_FS_OverCurrent_Pin */
  GPIO_InitStruct.Pin = OTG_FS_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(OTG_FS_OverCurrent_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : MEMS_INT2_Pin */
  GPIO_InitStruct.Pin = MEMS_INT2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(MEMS_INT2_GPIO_Port, &GPIO_InitStruct);

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

#ifdef  USE_FULL_ASSERT
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
