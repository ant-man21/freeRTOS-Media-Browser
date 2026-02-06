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
#include "cmsis_os.h"
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"
#include "5x5_font.h"
#include <string.h>
#include "platform_generic.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define INFO_AREA_X 10
#define INFO_AREA_Y 30
#define MAX_LINE_CHARS 38  // max characters per line
#define MAX_TEXT_LENGTH 512
#define MAX_FILE_NUMBER 32
#define SHARED_BUFFER_SIZE 512
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for DisplayTask */
osThreadId_t DisplayTaskHandle;
const osThreadAttr_t DisplayTask_attributes = {
  .name = "DisplayTask",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for SDCardTask */
osThreadId_t SDCardTaskHandle;
const osThreadAttr_t SDCardTask_attributes = {
  .name = "SDCardTask",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for ButtonTask */
osThreadId_t ButtonTaskHandle;
const osThreadAttr_t ButtonTask_attributes = {
  .name = "ButtonTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for spiMutex */
osMutexId_t spiMutexHandle;
const osMutexAttr_t spiMutex_attributes = {
  .name = "spiMutex"
};
/* Definitions for fileMutex */
osMutexId_t fileMutexHandle;
const osMutexAttr_t fileMutex_attributes = {
  .name = "fileMutex"
};
/* USER CODE BEGIN PV */
volatile uint8_t cycle_next_flag = 0;
volatile uint8_t cycle_prev_flag = 0;
volatile uint8_t read_file_flag = 1;
char shared_file_buffer[SHARED_BUFFER_SIZE];
volatile uint8_t file_content_updated = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
void StartDefaultTask(void *argument);
void DisplayTaskEntry(void *argument);
void SDCardTaskEntry(void *argument);
void ButtonTaskEntry(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// Redirect printf to UART
int _write(int file, char *ptr, int len)
{
  HAL_UART_Transmit(&huart2, (uint8_t*)ptr, len, HAL_MAX_DELAY);
  return len;
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
  MX_USART2_UART_Init();
  MX_FATFS_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  Platform_SPI_Init();
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();
  /* Create the mutex(es) */
  /* creation of spiMutex */
  spiMutexHandle = osMutexNew(&spiMutex_attributes);

  /* creation of fileMutex */
  fileMutexHandle = osMutexNew(&fileMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of DisplayTask */
  DisplayTaskHandle = osThreadNew(DisplayTaskEntry, NULL, &DisplayTask_attributes);

  /* creation of SDCardTask */
  SDCardTaskHandle = osThreadNew(SDCardTaskEntry, NULL, &SDCardTask_attributes);

  /* creation of ButtonTask */
  ButtonTaskHandle = osThreadNew(ButtonTaskEntry, NULL, &ButtonTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
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
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
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
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, TFT_CS_Pin|TFT_DC_Pin|TFT_RESET_Pin|SD_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : FILE_FORWARD_Pin FILE_BACK_Pin */
  GPIO_InitStruct.Pin = FILE_FORWARD_Pin|FILE_BACK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : TFT_CS_Pin TFT_DC_Pin TFT_RESET_Pin SD_CS_Pin */
  GPIO_InitStruct.Pin = TFT_CS_Pin|TFT_DC_Pin|TFT_RESET_Pin|SD_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_DisplayTaskEntry */
/**
* @brief Function implementing the DisplayTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_DisplayTaskEntry */
void DisplayTaskEntry(void *argument)
{
  /* USER CODE BEGIN DisplayTaskEntry */

  uint16_t bg_color = BLACK;
  uint16_t text_color = CYAN;
  char local_buffer[SHARED_BUFFER_SIZE];

  printf("Display: Init starting\r\n");
  ILI9341_Init();
  printf("Display: Init complete\r\n");
  ILI9341_Fill_Screen(bg_color);
  /* Infinite loop */
  for(;;) {
	  if(file_content_updated) {
		  printf(">>> Display: Flag detected! <<<\r\n");  // DEBUG
          // Copy shared buffer to local buffer (thread-safe)
          osMutexAcquire(fileMutexHandle, osWaitForever);
          strncpy(local_buffer, shared_file_buffer, SHARED_BUFFER_SIZE - 1);
          printf("Display: Copied %d chars to local buffer\r\n", strlen(local_buffer));  // DEBUG
          local_buffer[SHARED_BUFFER_SIZE - 1] = '\0';
          file_content_updated = 0;  // Clear flag
          osMutexRelease(fileMutexHandle);

          // Clear display area
          ILI9341_Fill_Screen(bg_color);
		  // Draw header
		  ILI9341_Draw_Text("STM32 Display Test", 10, 10, YELLOW, 2, bg_color);
		  const char *p = local_buffer;   // start of string
		  uint8_t cursor_x = INFO_AREA_X;
		  uint8_t cursor_y = INFO_AREA_Y;
		  static char line_buf[MAX_LINE_CHARS + 1] = {0};
		  uint8_t line_idx = 0;

		  while(*p)  // loop until null terminator
		  {
			  char c = *p++;  // get current char, advance pointer

			  if(c == '\n' || line_idx >= MAX_LINE_CHARS)
			  {
				  // terminate current line
				  line_buf[line_idx] = '\0';
				  ILI9341_Draw_Text(line_buf, cursor_x, cursor_y, text_color, 1, bg_color);

				  // reset buffer
				  line_idx = 0;
				  cursor_y += 8; // 5x5 font + 1px spacing

				  if(c == '\n') continue; // skip storing newline
			  }

			  line_buf[line_idx++] = c;
		  }

		  // draw any remaining characters
		  if(line_idx > 0)
		  {
			  line_buf[line_idx] = '\0';
			  ILI9341_Draw_Text(line_buf, cursor_x, cursor_y, text_color, 1, bg_color);
		  }

		  // draw any remaining characters
		  if(line_idx > 0)
		  {
			  line_buf[line_idx] = '\0';
			  ILI9341_Draw_Text(line_buf, cursor_x, cursor_y, text_color, 1, bg_color);
		  }
		  printf("Display Updated\r\n");
	}
	//      UBaseType_t highWaterMark = uxTaskGetStackHighWaterMark(NULL); // NULL = current task
	//      printf("Free stack words left: %lu\n", highWaterMark);
    osDelay(100); // wait before refreshing
  }
  /* USER CODE END DisplayTaskEntry */
}

/* USER CODE BEGIN Header_SDCardTaskEntry */
/**
* @brief Function implementing the SDCardTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_SDCardTaskEntry */
void SDCardTaskEntry(void *argument)
{
  /* USER CODE BEGIN SDCardTaskEntry */
  FATFS FatFs;
  FIL File;
  FRESULT fres;
  UINT bytesRead;
  DSTATUS stat;
  DIR dir;
  FILINFO fno;
  static uint8_t fileIndex = 0;
  static char fileList[MAX_FILE_NUMBER][50];  // Store up to MAX_FILE_NUMBER filenames (47.3 format)
  static uint8_t fileCount = 0;

  osDelay(500);

  printf("SD Card Test Starting...\r\n");

  // Check disk status before mount
  stat = disk_initialize(0);
  printf("disk_initialize returned: %d (0=OK, 1=NOINIT, 2=NOTRDY, 4=PROTECT)\r\n", stat);

  if(stat != 0) {
      printf("Disk initialization failed!\r\n");
      while(1) { osDelay(1000); }
  }

  // Mount the SD card
  fres = f_mount(&FatFs, "", 1);
  if(fres != FR_OK) {
      printf("f_mount error: %d\r\n", fres);
      while(1) { osDelay(1000); }
  }
  printf("SD Card mounted successfully!\r\n");

  // Scan directory and build file list
  fres = f_opendir(&dir, "/sappy_notes");
  if(fres == FR_OK) {
      while(1) {
          fres = f_readdir(&dir, &fno);
          if(fres != FR_OK || fno.fname[0] == 0) break;  // Break on error or end

          // Skip directories and hidden files
          if(!(fno.fattrib & AM_DIR) && fno.fname[0] != '.') {
              if(fileCount < MAX_FILE_NUMBER) {
            	  sprintf(fileList[fileCount], "/sappy_notes/%s", fno.fname);
                  printf("Found file: %s\r\n", fileList[fileCount]);
                  fileCount++;
              }
          }
      }
      f_closedir(&dir);
      printf("Total files found: %d\r\n", fileCount);
  } else {
      printf("f_opendir error: %d\r\n", fres);
      while(1) { osDelay(1000); }
  }

  if(fileCount == 0) {
      printf("No files found on SD card!\r\n");
      while(1) { osDelay(1000); }
  }

  /* Infinite loop */
  for(;;)
  {
      // ========== CHECK BUTTON FLAGS ==========
      if(cycle_next_flag) {
          cycle_next_flag = 0;  // Clear flag
          fileIndex = (fileIndex + 1) % fileCount;  // Cycle forward
          printf("\n>>> NEXT: Cycling to file %d/%d <<<\r\n", fileIndex + 1, fileCount);
          read_file_flag = 1;
      }

      if(cycle_prev_flag) {
          cycle_prev_flag = 0;  // Clear flag
          // Cycle backward (wrap around)
          fileIndex = (fileIndex == 0) ? (fileCount - 1) : (fileIndex - 1);
          printf("\n>>> PREV: Cycling to file %d/%d <<<\r\n", fileIndex + 1, fileCount);
          read_file_flag = 1;
      }
      if(read_file_flag) {
          read_file_flag = 0;  // Clear flag immediately
          printf(">>> About to read file <<<\r\n");  // DEBUG
		  // ========== OPEN AND READ CURRENT FILE ==========
		  printf("\r\n--- Reading file %d/%d: %s ---\r\n",
				 fileIndex + 1, fileCount, fileList[fileIndex]);

		  fres = f_open(&File, fileList[fileIndex], FA_READ);
		  if(fres != FR_OK) {
			  printf("f_open error: %d\r\n", fres);
              osMutexAcquire(fileMutexHandle, osWaitForever);
              snprintf(shared_file_buffer, SHARED_BUFFER_SIZE,
                       "Error: Could not open file\n%s", fileList[fileIndex]);
              file_content_updated = 1;
              osMutexRelease(fileMutexHandle);
		  } else {
			  // Read the file
			  osMutexAcquire(fileMutexHandle, osWaitForever);
			  fres = f_read(&File, shared_file_buffer, SHARED_BUFFER_SIZE - 1, &bytesRead);
			  if(fres == FR_OK) {
				  shared_file_buffer[bytesRead] = '\0';  // Null-terminate
				  printf("File contents (%u bytes): %s\r\n", bytesRead, shared_file_buffer);
				  file_content_updated = 1;
			  } else {
				  printf("f_read error: %d\r\n", fres);
				  file_content_updated = 1;
			  }

			  // Close the file
			  osMutexRelease(fileMutexHandle);
			  f_close(&File);
		  }
      }
      osDelay(100);

      // ========== AUTO-PLAY (OPTIONAL) ==========
      // Comment out these lines if you only want button control
//      if(autoPlay) {
//          osDelay(5000);  // Wait 5 seconds
//          fileIndex = (fileIndex + 1) % fileCount;  // Auto-cycle
//          printf("\n>>> AUTO: Moving to next file <<<\r\n");
//      } else {
//          osDelay(100);  // Short delay for responsive button checking
//      }
  }
  /* USER CODE END SDCardTaskEntry */
}

/* USER CODE BEGIN Header_ButtonTaskEntry */
/**
* @brief Function implementing the ButtonTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ButtonTaskEntry */
void ButtonTaskEntry(void *argument)
{
  /* USER CODE BEGIN ButtonTaskEntry */
	  uint8_t btn_next_prev = GPIO_PIN_SET;
	  uint8_t btn_prev_prev = GPIO_PIN_SET;

	  printf("Button Task Started\r\n");
  /* Infinite loop */
	  for(;;)
		  {
		    // Read current button states
		    uint8_t btn_next_current = HAL_GPIO_ReadPin(FILE_FORWARD_GPIO_Port, FILE_FORWARD_Pin);
		    uint8_t btn_prev_current = HAL_GPIO_ReadPin(FILE_BACK_GPIO_Port, FILE_BACK_Pin);

		    // Detect NEXT button press (falling edge: HIGH -> LOW)
		    if(btn_next_prev == GPIO_PIN_SET && btn_next_current == GPIO_PIN_RESET) {
		      cycle_next_flag = 1;
		      printf("Next button pressed!\r\n");
		    }

		    // Detect PREV button press (falling edge: HIGH -> LOW)
		    if(btn_prev_prev == GPIO_PIN_SET && btn_prev_current == GPIO_PIN_RESET) {
		      cycle_prev_flag = 1;
		      printf("Prev button pressed!\r\n");
		    }

		    // Save current state for next iteration
		    btn_next_prev = btn_next_current;
		    btn_prev_prev = btn_prev_current;

		    // Delay 50ms (acts as debounce filter)
		    osDelay(50);
		  }
  /* USER CODE END ButtonTaskEntry */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
