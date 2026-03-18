// platform/Src/stm32_spi.c
#include "platform_generic.h"
#if defined(STM32L476xx)
  #include "stm32l4xx_hal.h"
#elif defined(STM32F411xE)
  #include "stm32f4xx_hal.h"
#endif
#include "cmsis_os.h"
#include "main.h" //maybe not ideal for portability

// STM32 HAL handles (CubeMX generated)
extern SPI_HandleTypeDef hspi1;
extern osMutexId_t spiMutexHandle;

// platform-agnostic handles
spi_handle_t spiHandle;
mutex_handle_t spiMutex;

void Platform_SPI_Init(void) {
    spiHandle = (spi_handle_t)&hspi1;
    spiMutex  = (mutex_handle_t)&spiMutexHandle; // store ADDRESS of handle
}

// SPI API (platform-agnostic)
void SPI_Transmit(spi_handle_t hspi, uint8_t *data, uint16_t size) {
    HAL_SPI_Transmit((SPI_HandleTypeDef*)hspi, data, size, 100);
}

void SPI_TransmitReceive(spi_handle_t hspi, uint8_t *tx, uint8_t *rx, uint16_t size) {
    HAL_SPI_TransmitReceive((SPI_HandleTypeDef*)hspi, tx, rx, size, 100);
}

// Mutex API
mutex_handle_t Mutex_Create(void) {
    return (mutex_handle_t)osMutexNew(NULL);
}

void Mutex_Lock(mutex_handle_t mutex) {
    osMutexAcquire(*(osMutexId_t*)mutex, osWaitForever);
}

void Mutex_Unlock(mutex_handle_t mutex) {
    osMutexRelease(*(osMutexId_t*)mutex);
}

// Board-level CS abstraction
void SD_Select(void) {
    HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_RESET);
    HAL_Delay(1);
}

void SD_Deselect(void) {
    HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_SET);
    HAL_Delay(1);
}

void Delay(uint32_t ms) {
    osDelay(ms);  // Change this line when porting
}
