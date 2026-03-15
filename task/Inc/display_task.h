#pragma once

#include <stdint.h>
#include <string.h>
#include "platform_generic.h"
#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"
#include "5x5_font.h"

// Configuration
#define SHARED_BUFFER_SIZE 2048
#define INFO_AREA_X 10
#define INFO_AREA_Y 30
#define MAX_LINE_CHARS 38  // max characters per line
#define MAX_TEXT_LENGTH 512

// External variables (defined in main.c or elsewhere)
extern volatile uint8_t file_content_updated;
extern char shared_file_buffer[SHARED_BUFFER_SIZE];
extern osMutexId_t fileMutexHandle;

// Function declarations
void Display_Init(uint16_t bg_color);
void DisplayTaskLoop(uint16_t bg_color, uint16_t text_color, char* local_buffer);
