################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/wit_c_sdk/user_dev_conf.c \
../User/wit_c_sdk/wit_c_sdk.c 

OBJS += \
./User/wit_c_sdk/user_dev_conf.o \
./User/wit_c_sdk/wit_c_sdk.o 

C_DEPS += \
./User/wit_c_sdk/user_dev_conf.d \
./User/wit_c_sdk/wit_c_sdk.d 


# Each subdirectory must supply rules for building sources it contributes
User/wit_c_sdk/%.o User/wit_c_sdk/%.su: ../User/wit_c_sdk/%.c User/wit_c_sdk/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-User-2f-wit_c_sdk

clean-User-2f-wit_c_sdk:
	-$(RM) ./User/wit_c_sdk/user_dev_conf.d ./User/wit_c_sdk/user_dev_conf.o ./User/wit_c_sdk/user_dev_conf.su ./User/wit_c_sdk/wit_c_sdk.d ./User/wit_c_sdk/wit_c_sdk.o ./User/wit_c_sdk/wit_c_sdk.su

.PHONY: clean-User-2f-wit_c_sdk

