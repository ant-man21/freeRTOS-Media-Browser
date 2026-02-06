// platform/include/platform_spi.h
#pragma once
#include <stdio.h>
#include <stdint.h>

typedef void* spi_handle_t;
typedef void* mutex_handle_t;

void SPI_Transmit(spi_handle_t hspi, uint8_t *data, uint16_t size);
void SPI_TransmitReceive(spi_handle_t hspi, uint8_t *tx, uint8_t *rx, uint16_t size);

mutex_handle_t Mutex_Create(void);
void Mutex_Lock(mutex_handle_t mutex);
void Mutex_Unlock(mutex_handle_t mutex);
