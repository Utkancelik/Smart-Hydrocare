#include "main.h"
#include "STM32F407_I2C_LCD16x02_Driver.h"
#include "string.h"
#include "stdlib.h"
#include <stdio.h>


/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;


void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_UART4_Init(void);
static void MX_I2C2_Init(void);
static void MX_UART5_Init(void);

uint8_t moisture_data[4] = {0,0,0,0};
uint8_t water_level_rxBuffer[1] = {0};


int main(void)
{
  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_UART4_Init();
  MX_I2C2_Init();
  MX_UART5_Init();
	LCD_Init();
	LCD_Init_1();
	
  while (1)
  {
		HAL_UART_Receive (&huart5,  water_level_rxBuffer, 1, 100);
		HAL_Delay(1000);
		
		if(water_level_rxBuffer[0] ==0)
		{
			LCD_Send_Cmd_1(LCD_SET_ROW1_COL1);
			LCD_Send_String_1("Water level");
			LCD_Send_Cmd_1(LCD_SET_ROW2_COL1);
			LCD_Send_String_1("%80-100");

		}
		
		if(water_level_rxBuffer[0] ==1)
		{
			LCD_Send_Cmd_1(LCD_SET_ROW1_COL1);
			LCD_Send_String_1("Water level");
			LCD_Send_Cmd_1(LCD_SET_ROW2_COL1);
			LCD_Send_String_1("%60-80");
		}
		
		if(water_level_rxBuffer[0] ==2)
		{
			LCD_Send_Cmd_1(LCD_SET_ROW1_COL1);
			LCD_Send_String_1("Water level");
			LCD_Send_Cmd_1(LCD_SET_ROW2_COL1);
			LCD_Send_String_1("%40-60");			
		}
		
		if(water_level_rxBuffer[0] ==3)
		{
			LCD_Send_Cmd_1(LCD_SET_ROW1_COL1);
			LCD_Send_String_1("Water level");
			LCD_Send_Cmd_1(LCD_SET_ROW2_COL1);
			LCD_Send_String_1("%20-40");
		}
		
		if(water_level_rxBuffer[0] ==4)
		{
			LCD_Send_Cmd_1(LCD_SET_ROW1_COL1);
			LCD_Send_String_1("Water level LOW!");		
			LCD_Send_Cmd_1(LCD_SET_ROW2_COL1);
			LCD_Send_String_1("%0-20");			
		}
		
		 // Start ADC Conversion
    HAL_ADC_Start(&hadc1);
    // Poll ADC1 Peripheral & TimeOut = 1mSec
    HAL_ADC_PollForConversion(&hadc1, 1);
		
		int sensorValue = HAL_ADC_GetValue(&hadc1);
		
		// Convert ADC value to humidity percentage (adjust according to my sensor)
    float humidityPercentage = (float)sensorValue / 4000.0 * 100.0;
		
		humidityPercentage = 100 - humidityPercentage;
		if(humidityPercentage < 0)
		{
			humidityPercentage = 0 - humidityPercentage;
		}
		
		if(humidityPercentage < 35)
		{
			moisture_data[0] = 1;
			HAL_UART_Transmit (&huart4, moisture_data, sizeof (moisture_data), 10);
			char sensor_humidity[20];
			sprintf(sensor_humidity, "Moisture: %.2f%%", humidityPercentage);
			
			LCD_Send_Cmd(LCD_SET_ROW1_COL1);
			LCD_Send_String(sensor_humidity);
			LCD_Send_Cmd(LCD_SET_ROW2_COL1);
			LCD_Send_String("Motor:ON ");
			HAL_Delay(1000);
		}

    else
		{
			moisture_data[0] = 0;
			HAL_UART_Transmit (&huart4, moisture_data, sizeof (moisture_data), 10);
			char sensor_humidity[20];
			sprintf(sensor_humidity, "Moisture: %.2f%%", humidityPercentage);
			
			LCD_Send_Cmd(LCD_SET_ROW1_COL1);
			LCD_Send_String(sensor_humidity);
			LCD_Send_Cmd(LCD_SET_ROW2_COL1);
			LCD_Send_String("Motor:OFF");
			HAL_Delay(1000);
		}
			
			LCD_Send_Cmd(LCD_CLEAR_DISPLAY);
			LCD_Send_Cmd_1(LCD_CLEAR_DISPLAY); 

  }
}


void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 88;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }


  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}


static void MX_ADC1_Init(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

static void MX_I2C1_Init(void)
{

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
}


static void MX_I2C2_Init(void)
{
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }

}


static void MX_UART4_Init(void)
{
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
}


static void MX_UART5_Init(void)
{
  huart5.Instance = UART5;
  huart5.Init.BaudRate = 115200;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }

}


static void MX_GPIO_Init(void)
{
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
}


void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
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
