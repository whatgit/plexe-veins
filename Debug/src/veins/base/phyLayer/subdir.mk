################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/veins/base/phyLayer/Antenna.cc \
../src/veins/base/phyLayer/BaseDecider.cc \
../src/veins/base/phyLayer/BasePhyLayer.cc \
../src/veins/base/phyLayer/ChannelInfo.cc \
../src/veins/base/phyLayer/ChannelState.cc \
../src/veins/base/phyLayer/Decider.cc \
../src/veins/base/phyLayer/MappingBase.cc \
../src/veins/base/phyLayer/MappingUtils.cc \
../src/veins/base/phyLayer/PhyUtils.cc \
../src/veins/base/phyLayer/Signal.cc 

CC_DEPS += \
./src/veins/base/phyLayer/Antenna.d \
./src/veins/base/phyLayer/BaseDecider.d \
./src/veins/base/phyLayer/BasePhyLayer.d \
./src/veins/base/phyLayer/ChannelInfo.d \
./src/veins/base/phyLayer/ChannelState.d \
./src/veins/base/phyLayer/Decider.d \
./src/veins/base/phyLayer/MappingBase.d \
./src/veins/base/phyLayer/MappingUtils.d \
./src/veins/base/phyLayer/PhyUtils.d \
./src/veins/base/phyLayer/Signal.d 

OBJS += \
./src/veins/base/phyLayer/Antenna.o \
./src/veins/base/phyLayer/BaseDecider.o \
./src/veins/base/phyLayer/BasePhyLayer.o \
./src/veins/base/phyLayer/ChannelInfo.o \
./src/veins/base/phyLayer/ChannelState.o \
./src/veins/base/phyLayer/Decider.o \
./src/veins/base/phyLayer/MappingBase.o \
./src/veins/base/phyLayer/MappingUtils.o \
./src/veins/base/phyLayer/PhyUtils.o \
./src/veins/base/phyLayer/Signal.o 


# Each subdirectory must supply rules for building sources it contributes
src/veins/base/phyLayer/%.o: ../src/veins/base/phyLayer/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


