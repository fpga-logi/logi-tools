################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/chan\ fat\ fs/diskio.c \
../src/chan\ fat\ fs/ff.c 

OBJS += \
./src/chan\ fat\ fs/diskio.o \
./src/chan\ fat\ fs/ff.o 

C_DEPS += \
./src/chan\ fat\ fs/diskio.d \
./src/chan\ fat\ fs/ff.d 


# Each subdirectory must supply rules for building sources it contributes
src/chan\ fat\ fs/diskio.o: ../src/chan\ fat\ fs/diskio.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__USE_CMSIS=CMSISv1p30_LPC13xx -DDEBUG -D__CODE_RED -D__REDLIB__ -I"D:\Dropbox\Prj\Valent\LOGI-FAMILY\SRC\Mark1\MCU\Mark1_DragDrop_Config\Mark1_DragDrop_Config\src\chan fat fs" -I"D:\Dropbox\Prj\Valent\LOGI-FAMILY\SRC\Mark1\MCU\Mark1_DragDrop_Config\Mark1_DragDrop_Config\src\chan fat fs\option" -I"D:\Dropbox\Prj\Valent\LOGI-FAMILY\SRC\Mark1\MCU\Mark1_DragDrop_Config\Mark1_DragDrop_Config\src" -I"D:\Dropbox\Prj\Valent\LOGI-FAMILY\SRC\Mark1\MCU\Mark1_DragDrop_Config\Mark1_DragDrop_Config\inc" -I"D:\Dropbox\Prj\Valent\LOGI-FAMILY\SRC\Mark1\MCU\Mark1_DragDrop_Config\CMSISv1p30_LPC13xx\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -mcpu=cortex-m3 -mthumb -MMD -MP -MF"src/chan fat fs/diskio.d" -MT"src/chan\ fat\ fs/diskio.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/chan\ fat\ fs/ff.o: ../src/chan\ fat\ fs/ff.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__USE_CMSIS=CMSISv1p30_LPC13xx -DDEBUG -D__CODE_RED -D__REDLIB__ -I"D:\Dropbox\Prj\Valent\LOGI-FAMILY\SRC\Mark1\MCU\Mark1_DragDrop_Config\Mark1_DragDrop_Config\src\chan fat fs" -I"D:\Dropbox\Prj\Valent\LOGI-FAMILY\SRC\Mark1\MCU\Mark1_DragDrop_Config\Mark1_DragDrop_Config\src\chan fat fs\option" -I"D:\Dropbox\Prj\Valent\LOGI-FAMILY\SRC\Mark1\MCU\Mark1_DragDrop_Config\Mark1_DragDrop_Config\src" -I"D:\Dropbox\Prj\Valent\LOGI-FAMILY\SRC\Mark1\MCU\Mark1_DragDrop_Config\Mark1_DragDrop_Config\inc" -I"D:\Dropbox\Prj\Valent\LOGI-FAMILY\SRC\Mark1\MCU\Mark1_DragDrop_Config\CMSISv1p30_LPC13xx\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -mcpu=cortex-m3 -mthumb -MMD -MP -MF"src/chan fat fs/ff.d" -MT"src/chan\ fat\ fs/ff.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


