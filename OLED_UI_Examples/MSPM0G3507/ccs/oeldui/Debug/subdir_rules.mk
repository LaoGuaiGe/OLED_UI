################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"D:/TI/CCS/ccs/tools/compiler/ti-cgt-armllvm_4.0.3.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O1 -I"E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3507/ccs/oeldui/middle" -I"E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3507/ccs/oeldui/hardware" -I"E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3507/ccs/oeldui/app" -I"E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3507/ccs/oeldui/oledUI" -I"E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3507/ccs/oeldui" -I"E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3507/ccs/oeldui/Debug" -I"D:/TI/CCS/mspm0_sdk_2_05_01_00/source/third_party/CMSIS/Core/Include" -I"D:/TI/CCS/mspm0_sdk_2_05_01_00/source" -gdwarf-3 -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-1920213839: ../empty.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"D:/TI/CCS/ccs/utils/sysconfig_1.24.0/sysconfig_cli.bat" --script "E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3507/ccs/oeldui/empty.syscfg" -o "." -s "D:/TI/CCS/mspm0_sdk_2_05_01_00/.metadata/product.json" --compiler ticlang
	@echo 'Finished building: "$<"'
	@echo ' '

device_linker.cmd: build-1920213839 ../empty.syscfg
device.opt: build-1920213839
device.cmd.genlibs: build-1920213839
ti_msp_dl_config.c: build-1920213839
ti_msp_dl_config.h: build-1920213839
Event.dot: build-1920213839

%.o: ./%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"D:/TI/CCS/ccs/tools/compiler/ti-cgt-armllvm_4.0.3.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O1 -I"E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3507/ccs/oeldui/middle" -I"E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3507/ccs/oeldui/hardware" -I"E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3507/ccs/oeldui/app" -I"E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3507/ccs/oeldui/oledUI" -I"E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3507/ccs/oeldui" -I"E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3507/ccs/oeldui/Debug" -I"D:/TI/CCS/mspm0_sdk_2_05_01_00/source/third_party/CMSIS/Core/Include" -I"D:/TI/CCS/mspm0_sdk_2_05_01_00/source" -gdwarf-3 -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

startup_mspm0g350x_ticlang.o: D:/TI/CCS/mspm0_sdk_2_05_01_00/source/ti/devices/msp/m0p/startup_system_files/ticlang/startup_mspm0g350x_ticlang.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"D:/TI/CCS/ccs/tools/compiler/ti-cgt-armllvm_4.0.3.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O1 -I"E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3507/ccs/oeldui/middle" -I"E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3507/ccs/oeldui/hardware" -I"E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3507/ccs/oeldui/app" -I"E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3507/ccs/oeldui/oledUI" -I"E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3507/ccs/oeldui" -I"E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3507/ccs/oeldui/Debug" -I"D:/TI/CCS/mspm0_sdk_2_05_01_00/source/third_party/CMSIS/Core/Include" -I"D:/TI/CCS/mspm0_sdk_2_05_01_00/source" -gdwarf-3 -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


