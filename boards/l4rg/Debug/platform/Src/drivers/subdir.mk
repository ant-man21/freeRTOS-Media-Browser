################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/Anthony/RTOS_media_browser/platform/src/drivers/ILI9341_GFX.c \
E:/Anthony/RTOS_media_browser/platform/src/drivers/ILI9341_STM32_Driver.c \
E:/Anthony/RTOS_media_browser/platform/src/drivers/user_diskio_spi.c 

OBJS += \
./platform/Src/drivers/ILI9341_GFX.o \
./platform/Src/drivers/ILI9341_STM32_Driver.o \
./platform/Src/drivers/user_diskio_spi.o 

C_DEPS += \
./platform/Src/drivers/ILI9341_GFX.d \
./platform/Src/drivers/ILI9341_STM32_Driver.d \
./platform/Src/drivers/user_diskio_spi.d 


# Each subdirectory must supply rules for building sources it contributes
platform/Src/drivers/ILI9341_GFX.o: E:/Anthony/RTOS_media_browser/platform/src/drivers/ILI9341_GFX.c platform/Src/drivers/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../FATFS/Target -I../FATFS/App -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/FatFs/src -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"E:/Anthony/RTOS_media_browser/platform/Inc" -IE:/Anthony/RTOS_media_browser/task/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
platform/Src/drivers/ILI9341_STM32_Driver.o: E:/Anthony/RTOS_media_browser/platform/src/drivers/ILI9341_STM32_Driver.c platform/Src/drivers/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../FATFS/Target -I../FATFS/App -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/FatFs/src -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"E:/Anthony/RTOS_media_browser/platform/Inc" -IE:/Anthony/RTOS_media_browser/task/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
platform/Src/drivers/user_diskio_spi.o: E:/Anthony/RTOS_media_browser/platform/src/drivers/user_diskio_spi.c platform/Src/drivers/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../FATFS/Target -I../FATFS/App -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/FatFs/src -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"E:/Anthony/RTOS_media_browser/platform/Inc" -IE:/Anthony/RTOS_media_browser/task/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-platform-2f-Src-2f-drivers

clean-platform-2f-Src-2f-drivers:
	-$(RM) ./platform/Src/drivers/ILI9341_GFX.cyclo ./platform/Src/drivers/ILI9341_GFX.d ./platform/Src/drivers/ILI9341_GFX.o ./platform/Src/drivers/ILI9341_GFX.su ./platform/Src/drivers/ILI9341_STM32_Driver.cyclo ./platform/Src/drivers/ILI9341_STM32_Driver.d ./platform/Src/drivers/ILI9341_STM32_Driver.o ./platform/Src/drivers/ILI9341_STM32_Driver.su ./platform/Src/drivers/user_diskio_spi.cyclo ./platform/Src/drivers/user_diskio_spi.d ./platform/Src/drivers/user_diskio_spi.o ./platform/Src/drivers/user_diskio_spi.su

.PHONY: clean-platform-2f-Src-2f-drivers

