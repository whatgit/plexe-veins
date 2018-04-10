################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/veins/modules/mobility/LinearMobility.cc 

CC_DEPS += \
./src/veins/modules/mobility/LinearMobility.d 

OBJS += \
./src/veins/modules/mobility/LinearMobility.o 


# Each subdirectory must supply rules for building sources it contributes
src/veins/modules/mobility/%.o: ../src/veins/modules/mobility/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


