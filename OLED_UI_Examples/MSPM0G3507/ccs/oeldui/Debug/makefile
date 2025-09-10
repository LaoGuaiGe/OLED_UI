################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := D:/TI/CCS/ccs/tools/compiler/ti-cgt-armllvm_4.0.3.LTS

GEN_OPTS__FLAG := @"./device.opt" 
GEN_CMDS__FLAG := -Wl,-l"./device_linker.cmd" 

ORDERED_OBJS += \
"./empty.o" \
"./ti_msp_dl_config.o" \
"./startup_mspm0g350x_ticlang.o" \
"./app/app_key_task.o" \
"./hardware/hw_key.o" \
"./middle/mid_button.o" \
"./middle/mid_debug_led.o" \
"./middle/mid_debug_uart.o" \
"./middle/mid_timer.o" \
"./oledUI/OLED.o" \
"./oledUI/OLED_Fonts.o" \
"./oledUI/OLED_UI.o" \
"./oledUI/OLED_UI_Driver.o" \
"./oledUI/OLED_UI_MenuData.o" \
"./oledUI/OLED_driver.o" \
$(GEN_CMDS__FLAG) \
-Wl,-ldevice.cmd.genlibs \
-Wl,-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include app/subdir_vars.mk
-include hardware/subdir_vars.mk
-include middle/subdir_vars.mk
-include oledUI/subdir_vars.mk
-include subdir_rules.mk
-include app/subdir_rules.mk
-include hardware/subdir_rules.mk
-include middle/subdir_rules.mk
-include oledUI/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
oeldui.out 

EXE_OUTPUTS__QUOTED += \
"oeldui.out" 


# All Target
all: $(OBJS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "oeldui.out"

# Tool invocations
oeldui.out: $(OBJS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: Arm Linker'
	"D:/TI/CCS/ccs/tools/compiler/ti-cgt-armllvm_4.0.3.LTS/bin/tiarmclang.exe" @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O1 -gdwarf-3 -Wl,-m"oeldui.map" -Wl,--heap_size=0x400 -Wl,--stack_size=0x400 -Wl,-i"D:/TI/CCS/mspm0_sdk_2_05_01_00/source" -Wl,-i"E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3507/ccs/oeldui" -Wl,-i"E:/github/OLED_UI/OLED_UI_Examples/MSPM0G3507/ccs/oeldui/Debug/syscfg" -Wl,-i"D:/TI/CCS/ccs/tools/compiler/ti-cgt-armllvm_4.0.3.LTS/lib" -Wl,--diag_wrap=off -Wl,--display_error_number -Wl,--warn_sections -Wl,--xml_link_info="oeldui_linkInfo.xml" -Wl,--rom_model -o "oeldui.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(GEN_MISC_FILES__QUOTED)$(GEN_FILES__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "empty.o" "ti_msp_dl_config.o" "startup_mspm0g350x_ticlang.o" "app\app_key_task.o" "hardware\hw_key.o" "middle\mid_button.o" "middle\mid_debug_led.o" "middle\mid_debug_uart.o" "middle\mid_timer.o" "oledUI\OLED.o" "oledUI\OLED_Fonts.o" "oledUI\OLED_UI.o" "oledUI\OLED_UI_Driver.o" "oledUI\OLED_UI_MenuData.o" "oledUI\OLED_driver.o" 
	-$(RM) "empty.d" "ti_msp_dl_config.d" "startup_mspm0g350x_ticlang.d" "app\app_key_task.d" "hardware\hw_key.d" "middle\mid_button.d" "middle\mid_debug_led.d" "middle\mid_debug_uart.d" "middle\mid_timer.d" "oledUI\OLED.d" "oledUI\OLED_Fonts.d" "oledUI\OLED_UI.d" "oledUI\OLED_UI_Driver.d" "oledUI\OLED_UI_MenuData.d" "oledUI\OLED_driver.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

