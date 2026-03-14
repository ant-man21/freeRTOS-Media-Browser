#include "sdcard_task.h"
#include "diskio.h"
#include <stdio.h>
#include <string.h>

// Private variables
static FATFS FatFs;
static uint8_t fileIndex = 0;
static char fileList[MAX_FILE_NUMBER][50];
static uint8_t fileCount = 0;

/**
 * @brief Initialize SD card and mount filesystem
 */
void SDCard_Init(void)
{
    DSTATUS stat;
    FRESULT fres;
    
    Delay(500);
    
    printf("SD Card Test Starting...\r\n");
    
    // Check disk status before mount
    stat = disk_initialize(0);
    printf("disk_initialize returned: %d (0=OK, 1=NOINIT, 2=NOTRDY, 4=PROTECT)\r\n", stat);

    if(stat != 0) {
        printf("Disk initialization failed!\r\n");
        while(1) { Delay(1000); }
    }
    
    // Mount the SD card
    fres = f_mount(&FatFs, "", 1);
    if(fres != FR_OK) {
        printf("f_mount error: %d\r\n", fres);
        while(1) { Delay(1000); }
    }
    printf("SD Card mounted successfully!\r\n");
}

/**
 * @brief Scan directory and build file list
 * @param path Directory path to scan
 * @return Number of files found (0 if error or no files)
 */
uint8_t SDCard_ScanDirectory(const char *path)
{
    FRESULT fres;
    DIR dir;
    FILINFO fno;
    
    fileCount = 0;
    fileIndex = 0;
    
    fres = f_opendir(&dir, path);
    if(fres == FR_OK) {
        while(1) {
            fres = f_readdir(&dir, &fno);
            if(fres != FR_OK || fno.fname[0] == 0) break;  // Break on error or end
            
            // Skip directories and hidden files
            if(!(fno.fattrib & AM_DIR) && fno.fname[0] != '.') {
                if(fileCount < MAX_FILE_NUMBER) {
                    snprintf(fileList[fileCount], sizeof(fileList[fileCount]), 
                             "%s/%s", path, fno.fname);
                    printf("Found file: %s\r\n", fileList[fileCount]);
                    fileCount++;
                }
            }
        }
        f_closedir(&dir);
        printf("Total files found: %d\r\n", fileCount);
    } else {
        printf("f_opendir error: %d\r\n", fres);
    }
    
    if(fileCount == 0) {
        printf("No files found on SD card!\r\n");
    }
    
    return fileCount;
}

/**
 * @brief Cycle to next file in list
 */
void SDCard_CycleNext(void)
{
    fileIndex = (fileIndex + 1) % fileCount;
    printf("\n>>> NEXT: Cycling to file %d/%d <<<\r\n", fileIndex + 1, fileCount);
}

/**
 * @brief Cycle to previous file in list
 */
void SDCard_CyclePrev(void)
{
    fileIndex = (fileIndex == 0) ? (fileCount - 1) : (fileIndex - 1);
    printf("\n>>> PREV: Cycling to file %d/%d <<<\r\n", fileIndex + 1, fileCount);
}

/**
 * @brief Read current file into shared buffer
 */
void SDCard_ReadCurrentFile(void)
{
    FIL File;
    FRESULT fres;
    UINT bytesRead;
    
    printf(">>> About to read file <<<\r\n");
    printf("\r\n--- Reading file %d/%d: %s ---\r\n",
           fileIndex + 1, fileCount, fileList[fileIndex]);
    
    fres = f_open(&File, fileList[fileIndex], FA_READ);
    if(fres != FR_OK) {
        printf("f_open error: %d\r\n", fres);
        Mutex_Lock(fileMutexHandle);
        snprintf(shared_file_buffer, SHARED_BUFFER_SIZE,
                 "Error: Could not open file\n%s", fileList[fileIndex]);
        file_content_updated = 1;
        Mutex_Unlock(fileMutexHandle);

    } else {
        // Read the file
    	Mutex_Lock(fileMutexHandle);
        fres = f_read(&File, shared_file_buffer, SHARED_BUFFER_SIZE - 1, &bytesRead);
        if(fres == FR_OK) {
            shared_file_buffer[bytesRead] = '\0';  // Null-terminate
            printf("File contents (%u bytes): %s\r\n", bytesRead, shared_file_buffer);
            file_content_updated = 1;
        } else {
            printf("f_read error: %d\r\n", fres);
            snprintf(shared_file_buffer, SHARED_BUFFER_SIZE, "Error reading file");
            file_content_updated = 1;
        }
        Mutex_Unlock(fileMutexHandle);
        
        // Close the file
        f_close(&File);
    }
}

/**
 * @brief Main task loop - handles button events and file reading
 */
void SDCardTaskLoop(void)
{
    for(;;)
    {
        // Check button flags
        if(cycle_next_flag) {
            cycle_next_flag = 0;
            SDCard_CycleNext();
            read_file_flag = 1;
        }
        
        if(cycle_prev_flag) {
            cycle_prev_flag = 0;
            SDCard_CyclePrev();
            read_file_flag = 1;
        }
        
        if(read_file_flag) {
            read_file_flag = 0;
            SDCard_ReadCurrentFile();
        }
        
        Delay(100);
    }
}
