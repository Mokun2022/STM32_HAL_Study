################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Bsp/bsp_beep/bsp_beep.c 

OBJS += \
./Bsp/bsp_beep/bsp_beep.o 

C_DEPS += \
./Bsp/bsp_beep/bsp_beep.d 


# Each subdirectory must supply rules for building sources it contributes
Bsp/bsp_beep/%.o Bsp/bsp_beep/%.su Bsp/bsp_beep/%.cyclo: ../Bsp/bsp_beep/%.c Bsp/bsp_beep/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -Werror -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Bsp-2f-bsp_beep

clean-Bsp-2f-bsp_beep:
	-$(RM) ./Bsp/bsp_beep/bsp_beep.cyclo ./Bsp/bsp_beep/bsp_beep.d ./Bsp/bsp_beep/bsp_beep.o ./Bsp/bsp_beep/bsp_beep.su

.PHONY: clean-Bsp-2f-bsp_beep
