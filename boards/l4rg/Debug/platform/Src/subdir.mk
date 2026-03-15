################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/Anthony/RTOS_media_browser/platform/src/stm32_generic.c 

OBJS += \
./platform/Src/stm32_generic.o 

C_DEPS += \
./platform/Src/stm32_generic.d 


# Each subdirectory must supply rules for building sources it contributes
platform/Src/stm32_generic.o: E:/Anthony/RTOS_media_browser/platform/src/stm32_generic.c platform/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../FATFS/Target -I../FATFS/App -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/FatFs/src -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"E:/Anthony/RTOS_media_browser/platform/Inc" -IE:/Anthony/RTOS_media_browser/task/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-platform-2f-Src

clean-platform-2f-Src:
	-$(RM) ./platform/Src/stm32_generic.cyclo ./platform/Src/stm32_generic.d ./platform/Src/stm32_generic.o ./platform/Src/stm32_generic.su

.PHONY: clean-platform-2f-Src

