################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/veins/base/modules/BaseApplLayer.cc \
../src/veins/base/modules/BaseBattery.cc \
../src/veins/base/modules/BaseLayer.cc \
../src/veins/base/modules/BaseMacLayer.cc \
../src/veins/base/modules/BaseMobility.cc \
../src/veins/base/modules/BaseModule.cc \
../src/veins/base/modules/BaseWorldUtility.cc \
../src/veins/base/modules/BatteryAccess.cc 

CC_DEPS += \
./src/veins/base/modules/BaseApplLayer.d \
./src/veins/base/modules/BaseBattery.d \
./src/veins/base/modules/BaseLayer.d \
./src/veins/base/modules/BaseMacLayer.d \
./src/veins/base/modules/BaseMobility.d \
./src/veins/base/modules/BaseModule.d \
./src/veins/base/modules/BaseWorldUtility.d \
./src/veins/base/modules/BatteryAccess.d 

OBJS += \
./src/veins/base/modules/BaseApplLayer.o \
./src/veins/base/modules/BaseBattery.o \
./src/veins/base/modules/BaseLayer.o \
./src/veins/base/modules/BaseMacLayer.o \
./src/veins/base/modules/BaseMobility.o \
./src/veins/base/modules/BaseModule.o \
./src/veins/base/modules/BaseWorldUtility.o \
./src/veins/base/modules/BatteryAccess.o 


# Each subdirectory must supply rules for building sources it contributes
src/veins/base/modules/%.o: ../src/veins/base/modules/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


