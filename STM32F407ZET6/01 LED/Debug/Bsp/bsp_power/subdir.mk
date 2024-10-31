################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Bsp/bsp_power/bsp_power.c 

OBJS += \
./Bsp/bsp_power/bsp_power.o 

C_DEPS += \
./Bsp/bsp_power/bsp_power.d 


# Each subdirectory must supply rules for building sources it contributes
Bsp/bsp_power/%.o Bsp/bsp_power/%.su: ../Bsp/bsp_power/%.c Bsp/bsp_power/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Bsp-2f-bsp_power

clean-Bsp-2f-bsp_power:
	-$(RM) ./Bsp/bsp_power/bsp_power.d ./Bsp/bsp_power/bsp_power.o ./Bsp/bsp_power/bsp_power.su

.PHONY: clean-Bsp-2f-bsp_power

