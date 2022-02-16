################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"D:/Programs/TI/ccs/tools/compiler/ti-cgt-c2000_21.6.0.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla2 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu0 -Ooff --include_path="C:/Users/sopji/OneDrive - The University of Texas at Austin/Senior Design/BMS_Firmware" --include_path="C:/Users/sopji/OneDrive - The University of Texas at Austin/Senior Design/BMS_Firmware/device" --include_path="D:/Programs/TI/C2000Ware_4_00_00_00/driverlib/f28004x/driverlib" --include_path="D:/Programs/TI/C2000Ware_4_00_00_00/libraries/calibration/hrpwm/f28004x/include" --include_path="D:/Programs/TI/ccs/tools/compiler/ti-cgt-c2000_21.6.0.LTS/include" --define=DEBUG --define=CPU1 --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" --include_path="C:/Users/sopji/OneDrive - The University of Texas at Austin/Senior Design/BMS_Firmware/CPU1_RAM/syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


