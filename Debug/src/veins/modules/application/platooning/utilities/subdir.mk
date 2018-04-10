################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/veins/modules/application/platooning/utilities/BasePositionHelper.cc \
../src/veins/modules/application/platooning/utilities/PositionHelper.cc 

CC_DEPS += \
./src/veins/modules/application/platooning/utilities/BasePositionHelper.d \
./src/veins/modules/application/platooning/utilities/PositionHelper.d 

OBJS += \
./src/veins/modules/application/platooning/utilities/BasePositionHelper.o \
./src/veins/modules/application/platooning/utilities/PositionHelper.o 


# Each subdirectory must supply rules for building sources it contributes
src/veins/modules/application/platooning/utilities/%.o: ../src/veins/modules/application/platooning/utilities/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


