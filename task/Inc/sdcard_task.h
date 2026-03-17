#pragma once

#include <stdint.h>
#include "ff.h"
#include "fatfs.h"
#include "platform_generic.h"

// Configuration
#define MAX_FILE_NUMBER 50
#define SHARED_BUFFER_SIZE 2048

// External variables (defined in main.c or elsewhere)
extern volatile uint8_t cycle_next_flag;
extern volatile uint8_t cycle_prev_flag;
extern volatile uint8_t read_file_flag;
extern volatile uint8_t file_content_updated;
extern char shared_file_buffer[SHARED_BUFFER_SIZE];
extern osMutexId_t fileMutexHandle;

// Function declarations
void SDCard_Init(void);
uint8_t SDCard_ScanDirectory(const char *path);
void SDCard_CycleNext(void);
void SDCard_CyclePrev(void);
void SDCard_ReadCurrentFile(void);
void SDCardTaskLoop(void);
