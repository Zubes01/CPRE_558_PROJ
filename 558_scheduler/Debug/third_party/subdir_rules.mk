################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
third_party/%.obj: ../third_party/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs1230/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -Ooff --include_path="C:/ti/ccs1230/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/include" --include_path="C:/ti/TivaWare_C_Series-2.2.0.295/examples/boards/ek-tm4c123gxl" --include_path="C:/ti/TivaWare_C_Series-2.2.0.295" --include_path="U:/cpre558ProjWorkspace/558_scheduler" --include_path="U:/cpre558ProjWorkspace/558_scheduler/eat_shit_and_DIE" --define=ccs="ccs" --define=TARGET_IS_TM4C129_RA1 --define=PART_TM4C123GH6PM -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --ual --preproc_with_compile --preproc_dependency="third_party/$(basename $(<F)).d_raw" --obj_directory="third_party" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


