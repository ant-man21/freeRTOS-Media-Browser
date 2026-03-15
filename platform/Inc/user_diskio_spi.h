#ifndef USER_DISKIO_SPI_H
#define USER_DISKIO_SPI_H

#include "ff_gen_drv.h"

/* Function prototypes */
DSTATUS USER_SPI_initialize(BYTE pdrv);
DSTATUS USER_SPI_status(BYTE pdrv);
DRESULT USER_SPI_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count);
DRESULT USER_SPI_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count);
DRESULT USER_SPI_ioctl(BYTE pdrv, BYTE cmd, void *buff);

#endif /* USER_DISKIO_SPI_H */
