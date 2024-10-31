################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Bsp/bsp_battery/bsp_battery.c 

OBJS += \
./Bsp/bsp_battery/bsp_battery.o 

C_DEPS += \
./Bsp/bsp_battery/bsp_battery.d 


# Each subdirectory must supply rules for building sources it contributes
Bsp/bsp_battery/%.o Bsp/bsp_battery/%.su Bsp/bsp_battery/%.cyclo: ../Bsp/bsp_battery/%.c Bsp/bsp_battery/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Bsp-2f-bsp_battery

clean-Bsp-2f-bsp_battery:
	-$(RM) ./Bsp/bsp_battery/bsp_battery.cyclo ./Bsp/bsp_battery/bsp_battery.d ./Bsp/bsp_battery/bsp_battery.o ./Bsp/bsp_battery/bsp_battery.su

.PHONY: clean-Bsp-2f-bsp_battery

