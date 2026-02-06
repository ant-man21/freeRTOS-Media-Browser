// platform/src/esp32_spi.c
#include "platform_spi.h"
#include "freertos/semphr.h"
#include "driver/spi_master.h"

void SPI_Transmit(spi_handle_t hspi, uint8_t *data, uint16_t size) {
    spi_device_handle_t dev = (spi_device_handle_t)hspi;
    spi_transaction_t t = {0};
    t.length = size * 8;
    t.tx_buffer = data;
    spi_device_transmit(dev, &t);
}

void SPI_TransmitReceive(spi_handle_t hspi, uint8_t *tx, uint8_t *rx, uint16_t size) {
    spi_device_handle_t dev = (spi_device_handle_t)hspi;
    spi_transaction_t t = {0};
    t.length = size * 8;
    t.tx_buffer = tx;
    t.rx_buffer = rx;
    spi_device_transmit(dev, &t);
}

mutex_handle_t Mutex_Create(void) {
    return (mutex_handle_t)xSemaphoreCreateMutex();
}

void Mutex_Lock(mutex_handle_t mutex) {
    xSemaphoreTake((SemaphoreHandle_t)mutex, portMAX_DELAY);
}

void Mutex_Unlock(mutex_handle_t mutex) {
    xSemaphoreGive((SemaphoreHandle_t)mutex);
}

// !!! WORK IN PROGRESS !!! NO IDEA IF THIS EVEN WORKS