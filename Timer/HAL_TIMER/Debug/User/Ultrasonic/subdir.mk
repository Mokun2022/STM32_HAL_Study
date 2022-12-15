################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/Ultrasonic/ultrasonic.c 

OBJS += \
./User/Ultrasonic/ultrasonic.o 

C_DEPS += \
./User/Ultrasonic/ultrasonic.d 


# Each subdirectory must supply rules for building sources it contributes
User/Ultrasonic/%.o User/Ultrasonic/%.su: ../User/Ultrasonic/%.c User/Ultrasonic/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-User-2f-Ultrasonic

clean-User-2f-Ultrasonic:
	-$(RM) ./User/Ultrasonic/ultrasonic.d ./User/Ultrasonic/ultrasonic.o ./User/Ultrasonic/ultrasonic.su

.PHONY: clean-User-2f-Ultrasonic

