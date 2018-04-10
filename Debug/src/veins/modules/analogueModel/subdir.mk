################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/veins/modules/analogueModel/BreakpointPathlossModel.cc \
../src/veins/modules/analogueModel/GenericFading.cc \
../src/veins/modules/analogueModel/JakesFading.cc \
../src/veins/modules/analogueModel/LogNormalShadowing.cc \
../src/veins/modules/analogueModel/NakagamiFading.cc \
../src/veins/modules/analogueModel/PERModel.cc \
../src/veins/modules/analogueModel/SimpleObstacleShadowing.cc \
../src/veins/modules/analogueModel/SimplePathlossModel.cc \
../src/veins/modules/analogueModel/TwoRayInterferenceModel.cc 

CC_DEPS += \
./src/veins/modules/analogueModel/BreakpointPathlossModel.d \
./src/veins/modules/analogueModel/GenericFading.d \
./src/veins/modules/analogueModel/JakesFading.d \
./src/veins/modules/analogueModel/LogNormalShadowing.d \
./src/veins/modules/analogueModel/NakagamiFading.d \
./src/veins/modules/analogueModel/PERModel.d \
./src/veins/modules/analogueModel/SimpleObstacleShadowing.d \
./src/veins/modules/analogueModel/SimplePathlossModel.d \
./src/veins/modules/analogueModel/TwoRayInterferenceModel.d 

OBJS += \
./src/veins/modules/analogueModel/BreakpointPathlossModel.o \
./src/veins/modules/analogueModel/GenericFading.o \
./src/veins/modules/analogueModel/JakesFading.o \
./src/veins/modules/analogueModel/LogNormalShadowing.o \
./src/veins/modules/analogueModel/NakagamiFading.o \
./src/veins/modules/analogueModel/PERModel.o \
./src/veins/modules/analogueModel/SimpleObstacleShadowing.o \
./src/veins/modules/analogueModel/SimplePathlossModel.o \
./src/veins/modules/analogueModel/TwoRayInterferenceModel.o 


# Each subdirectory must supply rules for building sources it contributes
src/veins/modules/analogueModel/%.o: ../src/veins/modules/analogueModel/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


