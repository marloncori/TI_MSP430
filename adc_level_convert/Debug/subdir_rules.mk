################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
lcd.obj: ../lcd.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccs740/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/bin/cl430" -vmsp --use_hw_mpy=none --include_path="C:/ti/ccs740/ccsv7/ccs_base/msp430/include" --include_path="C:/Users/NUC/workspace_v7/adc_level_convert" --include_path="C:/ti/ccs740/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/include" --advice:power=all --define=__MSP430G2452__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="lcd.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccs740/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/bin/cl430" -vmsp --use_hw_mpy=none --include_path="C:/ti/ccs740/ccsv7/ccs_base/msp430/include" --include_path="C:/Users/NUC/workspace_v7/adc_level_convert" --include_path="C:/ti/ccs740/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/include" --advice:power=all --define=__MSP430G2452__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="main.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


