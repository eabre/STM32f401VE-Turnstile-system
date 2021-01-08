/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "LCD.h"
#include "stdlib.h"
#include "time.h"
#include <stdio.h>
#include <string.h>
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
TIM_HandleTypeDef htim4;

/* USER CODE BEGIN PV */
char mesaj[16] = " "; //ilk satirdaki mesaj
char mesaj_2[16] = " "; //ikinci satirdaki mesaj
int selection = 0; //interrup secim sayaci
int decade=0; //10'luk sayicinin tutuldugu kisim
int counter_human=0;	//sayici
int temp_counter=0; //gecici sayicinin kaydedildigi yer
int temp_decade=0;	//gecici 10'luk sayicinin kaydedildigi yer
int temp=0;			//toplam sayi
int ms_bolum = 0; // ms_bolum = 1/milisecond => asagida acikladim
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM4_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void LED_Toggle(void)
{
	/*
	 * Bu fonksiyon timer-2 counter 100'e (ARR) ulastiginda gittigi TIM2 interruptinin
	 * icindeki fonksiyondur.
	 * 2 amac icin calisir;
	 * ilk amaci; kesmeden basilan buton ogrenildiginde ona uygun bir sekilde led'leri toggle eder
	 * ikinci olarak da delay icin gerekli olan degiskeni 100ms'de bir arttirir
	 */
	if(selection == 1)
	{
		HAL_GPIO_WritePin(Pause_LED_GPIO_Port, Pause_LED_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(Stop_LED_GPIO_Port, Stop_LED_Pin, GPIO_PIN_RESET);
		HAL_GPIO_TogglePin(Start_LED_GPIO_Port, Start_LED_Pin);
	}
	if(selection == 2)
	{
		HAL_GPIO_WritePin(Start_LED_GPIO_Port, Start_LED_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(Stop_LED_GPIO_Port, Stop_LED_Pin, GPIO_PIN_RESET);
		HAL_GPIO_TogglePin(Pause_LED_GPIO_Port, Pause_LED_Pin);
	}
	if(selection == 3)
	{
		HAL_GPIO_WritePin(Pause_LED_GPIO_Port, Pause_LED_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(Start_LED_GPIO_Port, Start_LED_Pin, GPIO_PIN_RESET);
		HAL_GPIO_TogglePin(Stop_LED_GPIO_Port, Stop_LED_Pin);
	}
	ms_bolum++;
}
void delay_ms(int x)
{
	/*100'le carpmamin nedeni; her 100ms de a bir arttigi icin
	 * ms_bolum = 1/(100 milisecond) diyebiliriz dolayisiyla 100*ms_bolum = 1 milisaniye olur
	 */
	while(100*ms_bolum < x)
	{
	}
	ms_bolum= 0;
}
void Random_Toggle(void)
{
	int x=100+(rand()%401); //100-500 ms arasi gecikme icin uretilen degisken
	HAL_GPIO_WritePin(Rand_Sig_GPIO_Port, Rand_Sig_Pin,GPIO_PIN_SET);
	delay_ms(x);
	HAL_GPIO_WritePin(Rand_Sig_GPIO_Port, Rand_Sig_Pin,GPIO_PIN_RESET);
	x=100+(rand()%401);
	delay_ms(x);
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	/*
	 * Burada butonlardan hangisi basildiysa sistem ona göre bir selection degeri atar
	 * ve lcd temizlemelerini saglar
	 */
	if(GPIO_Pin==Start_Button_Pin)
	{
		selection=1;
		temp=0;
		lcd_clear();
		delay_ms(20);
	}
	else if(GPIO_Pin==Pause_Button_Pin)
	{
		selection=2;
		lcd_clear();
		delay_ms(20);
	}
	else if(GPIO_Pin==Stop_Button_Pin)
	{
		selection=3;
		counter_human=0;
		decade=0;
		lcd_clear();
		delay_ms(20);
	}
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

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  lcd_init(_LCD_4BIT, _LCD_FONT_5x8, _LCD_2LINE); //LCD baslangic ayarlari
  HAL_TIM_Base_Start_IT(&htim4); //TIM2 interrupt sayaci baslatma
  delay_ms(200);
  lcd_clear();
  lcd_print(1, 1, "Baran Tanriverdi");
  lcd_print(2, 1, "Turnike Uygulama");
  delay_ms(500);
  lcd_clear();
  lcd_print(1, 1, "Baslata Basiniz");
  delay_ms(100);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  /*
	   * Burada EXTI kesmesinden elde edilen selection degerleri istenilen durumlar icin kullanilir
	   * selection = 1 ise, sistem sensor'u okuyarak sensorun SET ve RESET degerlerine gore sayiciyi arttirir ve 10'luk sayici ve normal sayiciyi yazdirir.
	   * selection = 2 ise, sistem bir okuma yapmaz sadece halihazirda olan degerleri yazdirir.
	   * selection = 3 ise, sistem toplam sayiyi yazdirir
	   */
	  if(selection ==1)
	  	{
	  		Random_Toggle();
	  		if(HAL_GPIO_ReadPin(Sensor_Reader_GPIO_Port, Sensor_Reader_Pin)!= GPIO_PIN_SET)
	  		{
	  			counter_human++;
	  			if(counter_human >= 5){
	  				decade++;
	  				counter_human=0;
	  			}
	  		}
	  		sprintf(mesaj, "Yolcu Sayisi: %d", decade);
	  		lcd_print(1, 1, mesaj);
	  		sprintf(mesaj_2, "Counter:%d", counter_human);
	  		temp_counter=counter_human;
	  		temp_decade= decade;
	  		temp= 5*temp_decade + temp_counter;
	  		lcd_print(2, 1, mesaj_2);
	  		delay_ms(100);
	  	}
	  	else if(selection ==2)
	  	{
	  		sprintf(mesaj, "Yolcu Sayisi: %d", decade);
	  		lcd_print(1, 1, mesaj);
	  		sprintf(mesaj_2, "Counter:%d", counter_human);
	  		lcd_print(2, 1, mesaj_2);
	  		delay_ms(100);
	  	}
	  	else if(selection ==3)
	  	{
	  		sprintf(mesaj, "Yolcu Sayisi:%d", temp);
	  		lcd_print(1, 1, mesaj);
	  		delay_ms(100);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 16000-1;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 100-1;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

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
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, LCD_D4_Pin|LCD_D5_Pin|LCD_D6_Pin|LCD_D7_Pin
                          |Rand_Sig_Pin|Start_LED_Pin|Pause_LED_Pin|Stop_LED_Pin
                          |LCD_RES_Pin|LCD_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LCD_D4_Pin LCD_D5_Pin LCD_D6_Pin LCD_D7_Pin
                           Rand_Sig_Pin Start_LED_Pin Pause_LED_Pin Stop_LED_Pin
                           LCD_RES_Pin LCD_EN_Pin */
  GPIO_InitStruct.Pin = LCD_D4_Pin|LCD_D5_Pin|LCD_D6_Pin|LCD_D7_Pin
                          |Rand_Sig_Pin|Start_LED_Pin|Pause_LED_Pin|Stop_LED_Pin
                          |LCD_RES_Pin|LCD_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : Start_Button_Pin Pause_Button_Pin Stop_Button_Pin */
  GPIO_InitStruct.Pin = Start_Button_Pin|Pause_Button_Pin|Stop_Button_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : Sensor_Reader_Pin */
  GPIO_InitStruct.Pin = Sensor_Reader_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Sensor_Reader_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
