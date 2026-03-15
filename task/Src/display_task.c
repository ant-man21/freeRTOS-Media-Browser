#include<display_task.h>
void Display_Init(uint16_t bg_color) {
  printf("Display: Init starting\r\n");
  ILI9341_Init();
  printf("Display: Init complete\r\n");
  ILI9341_Fill_Screen(bg_color);
  return;
}
void DisplayTaskLoop(uint16_t bg_color,
		             uint16_t text_color,
		             char* local_buffer)
{
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
	  ILI9341_Draw_Text("RTOS Display Test", 10, 10, YELLOW, 2, bg_color);
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
	//      UBaseType_t highWaterMark = uxTaskGetStackHighWaterMark(NULL); // NULL = current task
	//      printf("Free stack words left: %lu\n", highWaterMark);
	osDelay(100); // wait before refreshing
	return;
}
