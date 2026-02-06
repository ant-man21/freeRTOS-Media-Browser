#include "user_diskio_spi.h"
#include "platform_generic.h"

extern spi_handle_t spiHandle;
extern mutex_handle_t spiMutex;

// SD card responses
#define SD_RESPONSE_NO_ERROR      0x00
#define SD_IN_IDLE_STATE          0x01
#define SD_START_TOKEN            0xFE

static uint8_t SD_SendCmd(uint8_t cmd, uint32_t arg, uint8_t crc);
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
    Mutex_Lock(spiMutex);
    for(int i = 0; i < 10; i++) {
        SPI_Transmit(spiHandle, &dummy, 1);
    }
    Mutex_Unlock(spiMutex);

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

    // CMD8: SEND_IF_COND (check voltage)
    response = SD_SendCmd(8, 0x1AA, 0x87);
    if(response == 0x01) {
        uint8_t ocr[4];
        Mutex_Lock(spiMutex);
        for(int i = 0; i < 4; i++) {
            SPI_TransmitReceive(spiHandle, &dummy, &ocr[i], 1);
        }
        Mutex_Unlock(spiMutex);
    }

    SD_Deselect();

    // ACMD41: SD_SEND_OP_COND
    retry = 0;
    do {
        SD_Select();

        uint8_t cmdPacket55[6] = {0x40 | 55, 0x00, 0x00, 0x00, 0x00, 0xFF};
        Mutex_Lock(spiMutex);
        SPI_Transmit(spiHandle, cmdPacket55, 6);
        uint8_t cmd55_resp = 0xFF;
        uint8_t dummy2 = 0xFF;
        for(int i = 0; i < 10; i++) {
            SPI_TransmitReceive(spiHandle, &dummy2, &cmd55_resp, 1);
            if(cmd55_resp != 0xFF) break;
        }
        Mutex_Unlock(spiMutex);
        SD_Deselect();

        HAL_Delay(1);
        SD_Select();

        uint8_t cmdPacket41[6] = {0x40 | 41, 0x40, 0x00, 0x00, 0x00, 0xFF};
        Mutex_Lock(spiMutex);
        SPI_Transmit(spiHandle, cmdPacket41, 6);
        Mutex_Unlock(spiMutex);

        response = 0xFF;
        Mutex_Lock(spiMutex);
        for(int i = 0; i < 10; i++) {
            SPI_TransmitReceive(spiHandle, &dummy, &response, 1);
            if(response != 0xFF) break;
        }
        Mutex_Unlock(spiMutex);
        SD_Deselect();

        retry++;
        if(retry >= 400) {
            printf("ACMD41 timeout!\r\n");
            break;
        }

        HAL_Delay(10);
    } while(response != 0x00);

    if(retry >= 400) {
        printf("ACMD41 failed!\r\n");
        return STA_NOINIT;
    }

    printf("SD Card initialized successfully!\r\n");
    Stat &= ~STA_NOINIT;
    return Stat;
}

DSTATUS USER_SPI_status(BYTE pdrv) { return Stat; }

DRESULT USER_SPI_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
    for(UINT i = 0; i < count; i++) {
        if(SD_ReadSingleBlock(sector + i, buff + (i * 512)) != 0) {
            return RES_ERROR;
        }
    }
    return RES_OK;
}

DRESULT USER_SPI_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count) { return RES_WRPRT; }

DRESULT USER_SPI_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    DRESULT res = RES_ERROR;

    if(Stat & STA_NOINIT) return RES_NOTRDY;

    switch(cmd) {
        case CTRL_SYNC:
            SD_Select();
            if(SD_WaitReady() == 0x00) res = RES_OK;
            SD_Deselect();
            break;
        case GET_SECTOR_COUNT:
            *(DWORD*)buff = 1024000;  // dummy
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

static uint8_t SD_WaitReady(void)
{
    uint8_t response;
    uint8_t dummy = 0xFF;
    uint16_t retry = 0;

    Mutex_Lock(spiMutex);
    do {
        SPI_TransmitReceive(spiHandle, &dummy, &response, 1);
        retry++;
        if(retry > 5000) return 0xFF;
    } while(response != 0xFF);
    Mutex_Unlock(spiMutex);
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

    Mutex_Lock(spiMutex);
    SPI_Transmit(spiHandle, cmdPacket, 6);

    for(int i = 0; i < 10; i++) {
        SPI_TransmitReceive(spiHandle, &dummy, &response, 1);
        if(response != 0xFF) break;
    }
    Mutex_Unlock(spiMutex);

    return response;
}

static uint8_t SD_ReadSingleBlock(uint32_t sector, uint8_t *buffer)
{
    uint8_t response;
    uint8_t dummy = 0xFF;

    SD_Select();

    response = SD_SendCmd(17, sector, 0xFF);
    if(response != 0x00) {
        SD_Deselect();
        printf("CMD17 failed!\r\n");
        return 1;
    }

    uint16_t retry = 0;
    Mutex_Lock(spiMutex);
    do {
        SPI_TransmitReceive(spiHandle, &dummy, &response, 1);
        retry++;
        if(retry > 50000) {
            SD_Deselect();
            return 2;
        }
    } while(response != SD_START_TOKEN);

    for(int i = 0; i < 512; i++) {
        SPI_TransmitReceive(spiHandle, &dummy, &buffer[i], 1);
    }

    // discard CRC
    SPI_TransmitReceive(spiHandle, &dummy, &response, 1);
    SPI_TransmitReceive(spiHandle, &dummy, &response, 1);
    Mutex_Unlock(spiMutex);
    SD_Deselect();
    return 0;
}
