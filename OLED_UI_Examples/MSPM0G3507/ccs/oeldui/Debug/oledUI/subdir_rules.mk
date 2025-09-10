################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
oledUI/%.o: ../oledUI/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"D:/TI/CCS/ccs/tools/compiler/ti-cgt-armllvm_4.0.3.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O1 -I"E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3507/ccs/oeldui/middle" -I"E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3507/ccs/oeldui/hardware" -I"E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3507/ccs/oeldui/app" -I"E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3507/ccs/oeldui/oledUI" -I"E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3507/ccs/oeldui" -I"E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3507/ccs/oeldui/Debug" -I"D:/TI/CCS/mspm0_sdk_2_05_01_00/source/third_party/CMSIS/Core/Include" -I"D:/TI/CCS/mspm0_sdk_2_05_01_00/source" -gdwarf-3 -MMD -MP -MF"oledUI/$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


