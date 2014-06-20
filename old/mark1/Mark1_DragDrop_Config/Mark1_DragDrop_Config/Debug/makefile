################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/chan\ fat\ fs/option/subdir.mk
-include src/chan\ fat\ fs/subdir.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: Mark1_DragDrop_Config.axf

# Tool invocations
Mark1_DragDrop_Config.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"D:\Dropbox\Prj\Valent\LOGI-FAMILY\SRC\Mark1\MCU\Mark1_DragDrop_Config\CMSISv1p30_LPC13xx\Debug" -Xlinker --gc-sections -Xlinker -Map=Mark1_DragDrop_Config.map -mcpu=cortex-m3 -mthumb -T ../ld/usb_buffer.ld -o "Mark1_DragDrop_Config.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) Mark1_DragDrop_Config.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size Mark1_DragDrop_Config.axf; arm-none-eabi-objcopy -O binary Mark1_DragDrop_Config.axf Mark1_DragDrop_Config.bin; checksum Mark1_DragDrop_Config.bin;
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
