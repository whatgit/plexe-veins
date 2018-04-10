################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/veins/modules/application/platooning/traffic/PlatoonsAdvancedTrafficManager.cc \
../src/veins/modules/application/platooning/traffic/PlatoonsPlusHumanTraffic.cc \
../src/veins/modules/application/platooning/traffic/PlatoonsTrafficManager.cc 

CC_DEPS += \
./src/veins/modules/application/platooning/traffic/PlatoonsAdvancedTrafficManager.d \
./src/veins/modules/application/platooning/traffic/PlatoonsPlusHumanTraffic.d \
./src/veins/modules/application/platooning/traffic/PlatoonsTrafficManager.d 

OBJS += \
./src/veins/modules/application/platooning/traffic/PlatoonsAdvancedTrafficManager.o \
./src/veins/modules/application/platooning/traffic/PlatoonsPlusHumanTraffic.o \
./src/veins/modules/application/platooning/traffic/PlatoonsTrafficManager.o 


# Each subdirectory must supply rules for building sources it contributes
src/veins/modules/application/platooning/traffic/%.o: ../src/veins/modules/application/platooning/traffic/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


