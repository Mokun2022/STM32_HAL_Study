################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/uart/fifo.c \
../User/uart/uart.c 

OBJS += \
./User/uart/fifo.o \
./User/uart/uart.o 

C_DEPS += \
./User/uart/fifo.d \
./User/uart/uart.d 


# Each subdirectory must supply rules for building sources it contributes
User/uart/%.o User/uart/%.su User/uart/%.cyclo: ../User/uart/%.c User/uart/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-User-2f-uart

clean-User-2f-uart:
	-$(RM) ./User/uart/fifo.cyclo ./User/uart/fifo.d ./User/uart/fifo.o ./User/uart/fifo.su ./User/uart/uart.cyclo ./User/uart/uart.d ./User/uart/uart.o ./User/uart/uart.su

.PHONY: clean-User-2f-uart

