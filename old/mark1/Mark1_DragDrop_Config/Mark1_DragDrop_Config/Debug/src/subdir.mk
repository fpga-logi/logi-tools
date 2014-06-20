################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/BoardSupport.c \
../src/ButtonDetect.c \
../src/FLASH.c \
../src/FPGA_Config.c \
../src/System.c \
../src/Timers.c \
../src/cr_startup_lpc13.c \
../src/i2cslave.c \
../src/main.c \
../src/msccallback.c \
../src/usbdesc.c 

OBJS += \
./src/BoardSupport.o \
./src/ButtonDetect.o \
./src/FLASH.o \
./src/FPGA_Config.o \
./src/System.o \
./src/Timers.o \
./src/cr_startup_lpc13.o \
./src/i2cslave.o \
./src/main.o \
./src/msccallback.o \
./src/usbdesc.o 

C_DEPS += \
./src/BoardSupport.d \
./src/ButtonDetect.d \
./src/FLASH.d \
./src/FPGA_Config.d \
./src/System.d \
./src/Timers.d \
./src/cr_startup_lpc13.d \
./src/i2cslave.d \
./src/main.d \
./src/msccallback.d \
./src/usbdesc.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__USE_CMSIS=CMSISv1p30_LPC13xx -DDEBUG -D__CODE_RED -D__REDLIB__ -I"D:\Dropbox\Prj\Valent\LOGI-FAMILY\SRC\Mark1\MCU\Mark1_DragDrop_Config\Mark1_DragDrop_Config\src\chan fat fs" -I"D:\Dropbox\Prj\Valent\LOGI-FAMILY\SRC\Mark1\MCU\Mark1_DragDrop_Config\Mark1_DragDrop_Config\src\chan fat fs\option" -I"D:\Dropbox\Prj\Valent\LOGI-FAMILY\SRC\Mark1\MCU\Mark1_DragDrop_Config\Mark1_DragDrop_Config\src" -I"D:\Dropbox\Prj\Valent\LOGI-FAMILY\SRC\Mark1\MCU\Mark1_DragDrop_Config\Mark1_DragDrop_Config\inc" -I"D:\Dropbox\Prj\Valent\LOGI-FAMILY\SRC\Mark1\MCU\Mark1_DragDrop_Config\CMSISv1p30_LPC13xx\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


