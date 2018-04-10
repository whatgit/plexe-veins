################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/veins/modules/phy/Decider80211p.cc \
../src/veins/modules/phy/NistErrorRate.cc \
../src/veins/modules/phy/PhyLayer80211p.cc \
../src/veins/modules/phy/SNRThresholdDecider.cc \
../src/veins/modules/phy/SampledAntenna1D.cc 

CC_DEPS += \
./src/veins/modules/phy/Decider80211p.d \
./src/veins/modules/phy/NistErrorRate.d \
./src/veins/modules/phy/PhyLayer80211p.d \
./src/veins/modules/phy/SNRThresholdDecider.d \
./src/veins/modules/phy/SampledAntenna1D.d 

OBJS += \
./src/veins/modules/phy/Decider80211p.o \
./src/veins/modules/phy/NistErrorRate.o \
./src/veins/modules/phy/PhyLayer80211p.o \
./src/veins/modules/phy/SNRThresholdDecider.o \
./src/veins/modules/phy/SampledAntenna1D.o 


# Each subdirectory must supply rules for building sources it contributes
src/veins/modules/phy/%.o: ../src/veins/modules/phy/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


