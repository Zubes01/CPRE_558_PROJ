################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccs1230/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./Timer.obj" \
"./adc.obj" \
"./cliff.obj" \
"./hello_task.obj" \
"./lab6_template.obj" \
"./lcd.obj" \
"./main.obj" \
"./movement.obj" \
"./open_interface.obj" \
"./ping_template.obj" \
"./resetSimulation.obj" \
"./scan.obj" \
"./servo.obj" \
"./startup_ccs.obj" \
"./uart-interrupt.obj" \
"./uart.obj" \
"./drivers/rtos_hw_drivers.obj" \
"./eat_shit_and_DIE/croutine.obj" \
"./eat_shit_and_DIE/event_groups.obj" \
"./eat_shit_and_DIE/heap_4.obj" \
"./eat_shit_and_DIE/list.obj" \
"./eat_shit_and_DIE/port.obj" \
"./eat_shit_and_DIE/portasm.obj" \
"./eat_shit_and_DIE/queue.obj" \
"./eat_shit_and_DIE/stream_buffer.obj" \
"./eat_shit_and_DIE/tasks.obj" \
"./eat_shit_and_DIE/timers.obj" \
"./utils/uartstdio.obj" \
"./utils/ustdlib.obj" \
"../hello_freertos_ccs.cmd" \
$(GEN_CMDS__FLAG) \
-llibc.a \
-l"C:/ti/TivaWare_C_Series-2.2.0.295/driverlib/ccs/Debug/driverlib.lib" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include drivers/subdir_vars.mk
-include eat_shit_and_DIE/subdir_vars.mk
-include utils/subdir_vars.mk
-include subdir_rules.mk
-include drivers/subdir_rules.mk
-include eat_shit_and_DIE/subdir_rules.mk
-include utils/subdir_rules.mk
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
558_scheduler.out \

EXE_OUTPUTS__QUOTED += \
"558_scheduler.out" \


# All Target
all: 558_scheduler.out

# Tool invocations
558_scheduler.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: ARM Linker'
	"C:/ti/ccs1230/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -Ooff --define=ccs="ccs" --define=TARGET_IS_TM4C129_RA1 --define=PART_TM4C123GH6PM -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --ual -z -m"558_scheduler.map" --heap_size=4096 --stack_size=4096 -i"C:/ti/ccs1230/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/lib" -i"C:/ti/ccs1230/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/include" --reread_libs --diag_wrap=off --display_error_number --warn_sections --xml_link_info="558_scheduler_linkInfo.xml" --rom_model -o "558_scheduler.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)
	-$(RM) "Timer.obj" "adc.obj" "cliff.obj" "hello_task.obj" "lab6_template.obj" "lcd.obj" "main.obj" "movement.obj" "open_interface.obj" "ping_template.obj" "resetSimulation.obj" "scan.obj" "servo.obj" "startup_ccs.obj" "uart-interrupt.obj" "uart.obj" "drivers\rtos_hw_drivers.obj" "eat_shit_and_DIE\croutine.obj" "eat_shit_and_DIE\event_groups.obj" "eat_shit_and_DIE\heap_4.obj" "eat_shit_and_DIE\list.obj" "eat_shit_and_DIE\port.obj" "eat_shit_and_DIE\portasm.obj" "eat_shit_and_DIE\queue.obj" "eat_shit_and_DIE\stream_buffer.obj" "eat_shit_and_DIE\tasks.obj" "eat_shit_and_DIE\timers.obj" "utils\uartstdio.obj" "utils\ustdlib.obj" 
	-$(RM) "Timer.d" "adc.d" "cliff.d" "hello_task.d" "lab6_template.d" "lcd.d" "main.d" "movement.d" "open_interface.d" "ping_template.d" "resetSimulation.d" "scan.d" "servo.d" "startup_ccs.d" "uart-interrupt.d" "uart.d" "drivers\rtos_hw_drivers.d" "eat_shit_and_DIE\croutine.d" "eat_shit_and_DIE\event_groups.d" "eat_shit_and_DIE\heap_4.d" "eat_shit_and_DIE\list.d" "eat_shit_and_DIE\port.d" "eat_shit_and_DIE\queue.d" "eat_shit_and_DIE\stream_buffer.d" "eat_shit_and_DIE\tasks.d" "eat_shit_and_DIE\timers.d" "utils\uartstdio.d" "utils\ustdlib.d" 
	-$(RM) "eat_shit_and_DIE\portasm.d" 
	-@echo 'Finished clean'
	-@echo ' '

post-build:
	-"C:/ti/ccs1230/ccs/utils/tiobj2bin/tiobj2bin" "558_scheduler.out" "558_scheduler.bin" "C:/ti/ccs1230/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/bin/armofd" "C:/ti/ccs1230/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/bin/armhex" "C:/ti/ccs1230/ccs/utils/tiobj2bin/mkhex4bin"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

