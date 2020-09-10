################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32L0xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
CGS\ -\ Orienteering\ Device.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
CGS\ -\ Orienteering\ Device.list \

OBJCOPY_BIN += \
CGS\ -\ Orienteering\ Device.bin \


# All Target
all: CGS\ -\ Orienteering\ Device.elf secondary-outputs

# Tool invocations
CGS\ -\ Orienteering\ Device.elf: $(OBJS) $(USER_OBJS) C:\Users\scott\Downloads\CGS\ -\ Orienteering\ Device\STM32L072KZTX_FLASH.ld
	arm-none-eabi-gcc -o "CGS - Orienteering Device.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m0plus -T"C:\Users\scott\Downloads\CGS - Orienteering Device\STM32L072KZTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="CGS - Orienteering Device.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

CGS\ -\ Orienteering\ Device.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "CGS - Orienteering Device.list"
	@echo 'Finished building: $@'
	@echo ' '

CGS\ -\ Orienteering\ Device.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "CGS - Orienteering Device.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
