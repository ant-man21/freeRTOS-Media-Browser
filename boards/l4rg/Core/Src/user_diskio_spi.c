#include "ff_gen_drv.h"
#include "user_diskio_spi.h"
#include "main.h"
#include <stdio.h>

extern UART_HandleTypeDef huart2;  // Add this line
extern SPI_HandleTypeDef hspi1;
extern osMutexId_t spiMutexHandle;
// SD card responses
#define SD_RESPONSE_NO_ERROR      0x00
#define SD_IN_IDLE_STATE          0x01
#define SD_START_TOKEN            0xFE

static uint8_t SD_SendCmd(uint8_t cmd, uint32_t arg, uint8_t crc);
static void SD_Select(void);
static void SD_Deselect(void);
static uint8_t SD_WaitReady(void);
static uint8_t SD_ReadSingleBlock(uint32_t sector, uint8_t *buffer);

static volatile DSTATUS Stat = STA_NOINIT;

DSTATUS USER_SPI_initialize(BYTE pdrv)
{
    uint8_t response;
    uint16_t retry;

    printf("USER_SPI_initialize: Starting...\r\n");

    SD_Deselect();

    // Send 80+ dummy clocks
    uint8_t dummy = 0xFF;
    osMutexAcquire(spiMutexHandle, osWaitForever);
    for(int i = 0; i < 10; i++) {
        HAL_SPI_Transmit(&hspi1, &dummy, 1, 100);
    }
    osMutexRelease(spiMutexHandle);

//    printf("Sending CMD0...\r\n");
    // CMD0: GO_IDLE_STATE
    retry = 0;
    do {
        response = SD_SendCmd(0, 0, 0x95);
        retry++;
    } while((response != 0x01) && (retry < 200));


    if(retry >= 200) {
    	printf("CMD0 failed!\r\n");
        return STA_NOINIT;
    }

//    printf("Sending CMD8...\r\n");
    // CMD8: SEND_IF_COND (check voltage)
    response = SD_SendCmd(8, 0x1AA, 0x87);

    if(response == 0x01) {
        // Read 4-byte response
        uint8_t ocr[4];
        osMutexAcquire(spiMutexHandle, osWaitForever);
        for(int i = 0; i < 4; i++) {
            HAL_SPI_TransmitReceive(&hspi1, &dummy, &ocr[i], 1, 100);
        }
        osMutexRelease(spiMutexHandle);
//        printf("CMD8 OCR: %02X %02X %02X %02X\r\n", ocr[0], ocr[1], ocr[2], ocr[3]);
    }

    SD_Deselect();

//    printf("Sending ACMD41...\r\n");
    // ACMD41: SD_SEND_OP_COND
    retry = 0;
    do {

        // Send CMD55 (prefix for any ACMD)
        SD_Select();

//        printf("  Sending CMD55 packet...\r\n");
        uint8_t cmdPacket55[6] = {0x40 | 55, 0x00, 0x00, 0x00, 0x00, 0xFF};
        osMutexAcquire(spiMutexHandle, osWaitForever);
        HAL_SPI_Transmit(&hspi1, cmdPacket55, 6, 100);
        // Get CMD55 response
        uint8_t cmd55_resp = 0xFF;
        uint8_t dummy = 0xFF;
        for(int i = 0; i < 10; i++) {
            HAL_SPI_TransmitReceive(&hspi1, &dummy, &cmd55_resp, 1, 100);
            if(cmd55_resp != 0xFF) break;
        }
        osMutexRelease(spiMutexHandle);
        SD_Deselect();

        HAL_Delay(1);

//        printf("  Selecting SD card again...\r\n");
        SD_Select();

//        printf("  Sending ACMD41 packet...\r\n");
        uint8_t cmdPacket41[6] = {0x40 | 41, 0x40, 0x00, 0x00, 0x00, 0xFF};
        osMutexAcquire(spiMutexHandle, osWaitForever);
        HAL_SPI_Transmit(&hspi1, cmdPacket41, 6, 100);
        osMutexRelease(spiMutexHandle);
        // Get ACMD41 response
        response = 0xFF;
        osMutexAcquire(spiMutexHandle, osWaitForever);
        for(int i = 0; i < 10; i++) {
            HAL_SPI_TransmitReceive(&hspi1, &dummy, &response, 1, 100);
            if(response != 0xFF) break;
        }
        osMutexRelease(spiMutexHandle);
//        printf("  ACMD41 response: 0x%02X\r\n", response);

        SD_Deselect();


        retry++;
        if(retry >= 400) {
        	printf("ACMD41 timeout!\r\n");
            break;
        }

        HAL_Delay(10);
    } while(response != 0x00);

//    printf("ACMD41 loop exited: final response: 0x%02X (retry: %d)\r\n", response, retry);


    if(retry >= 400) {
    	printf("ACMD41 failed!\r\n");
        return STA_NOINIT;
    }

    printf("SD Card initialized successfully!\r\n");
    Stat &= ~STA_NOINIT;
    return Stat;
}


DSTATUS USER_SPI_status(BYTE pdrv)
{
    return Stat;
}

DRESULT USER_SPI_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
    for(UINT i = 0; i < count; i++) {
        if(SD_ReadSingleBlock(sector + i, buff + (i * 512)) != 0) {
            return RES_ERROR;
        }
    }
    return RES_OK;
}

DRESULT USER_SPI_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
    return RES_WRPRT;  // Write protected for now
}

DRESULT USER_SPI_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    DRESULT res = RES_ERROR;

    if(Stat & STA_NOINIT) return RES_NOTRDY;

    switch(cmd) {
        case CTRL_SYNC:
            SD_Select();
            if(SD_WaitReady() == 0x00)
                res = RES_OK;
            SD_Deselect();
            break;

        case GET_SECTOR_COUNT:
            *(DWORD*)buff = 1024000;  // Dummy value for now
            res = RES_OK;
            break;

        case GET_BLOCK_SIZE:
            *(WORD*)buff = 512;
            res = RES_OK;
            break;

        default:
            res = RES_PARERR;
    }

    return res;
}

// Helper functions
static void SD_Select(void)
{
    HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_RESET);
    HAL_Delay(1);
}

static void SD_Deselect(void)
{
    HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_SET);
    HAL_Delay(1);
}

static uint8_t SD_WaitReady(void)
{
    uint8_t response;
    uint8_t dummy = 0xFF;
    uint16_t retry = 0;

    osMutexAcquire(spiMutexHandle, osWaitForever);
    do {
        HAL_SPI_TransmitReceive(&hspi1, &dummy, &response, 1, 100);
        retry++;
        if(retry > 5000) return 0xFF;
    } while(response != 0xFF);
    osMutexRelease(spiMutexHandle);
    return 0x00;
}

static uint8_t SD_SendCmd(uint8_t cmd, uint32_t arg, uint8_t crc)
{
    uint8_t response;
    uint8_t cmdPacket[6];
    uint8_t dummy = 0xFF;

    SD_Select();
    SD_WaitReady();

    cmdPacket[0] = 0x40 | cmd;
    cmdPacket[1] = (arg >> 24);
    cmdPacket[2] = (arg >> 16);
    cmdPacket[3] = (arg >> 8);
    cmdPacket[4] = arg;
    cmdPacket[5] = crc;

    osMutexAcquire(spiMutexHandle, osWaitForever);
    HAL_SPI_Transmit(&hspi1, cmdPacket, 6, 100);

    // Wait for response
    for(int i = 0; i < 10; i++) {
        HAL_SPI_TransmitReceive(&hspi1, &dummy, &response, 1, 100);
        if(response != 0xFF) break;
    }
    osMutexRelease(spiMutexHandle);

    return response;
}

static uint8_t SD_ReadSingleBlock(uint32_t sector, uint8_t *buffer)
{
    uint8_t response;
    uint8_t dummy = 0xFF;

    SD_Select();

    response = SD_SendCmd(17, sector, 0xFF);  // CMD17: READ_SINGLE_BLOCK

    if(response != 0x00) {
        SD_Deselect();
        printf("  CMD17 failed!\r\n");
        return 1;
    }

    // Wait for start token
    uint16_t retry = 0;
    osMutexAcquire(spiMutexHandle, osWaitForever);
    do {
        HAL_SPI_TransmitReceive(&hspi1, &dummy, &response, 1, 100);
        retry++;
        if(retry > 50000) {
            SD_Deselect();
            return 2;
        }
    } while(response != SD_START_TOKEN);

    // Read 512 bytes
    for(int i = 0; i < 512; i++) {
        HAL_SPI_TransmitReceive(&hspi1, &dummy, &buffer[i], 1, 100);
    }

    // Read CRC (2 bytes, ignore)
    HAL_SPI_TransmitReceive(&hspi1, &dummy, &response, 1, 100);
    HAL_SPI_TransmitReceive(&hspi1, &dummy, &response, 1, 100);
    osMutexRelease(spiMutexHandle);
    SD_Deselect();
    return 0;
}
