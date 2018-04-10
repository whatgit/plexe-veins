################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/veins/modules/application/platooning/scenarios/AccelerateAndBrakeScenario.cc \
../src/veins/modules/application/platooning/scenarios/BaseScenario.cc \
../src/veins/modules/application/platooning/scenarios/BrakingScenario.cc \
../src/veins/modules/application/platooning/scenarios/IntersectionScenario.cc \
../src/veins/modules/application/platooning/scenarios/ManualDrivingScenario.cc \
../src/veins/modules/application/platooning/scenarios/PlatoonMergingScenario.cc \
../src/veins/modules/application/platooning/scenarios/SinusoidalScenario.cc \
../src/veins/modules/application/platooning/scenarios/StandstillObstacleScenario.cc 

CC_DEPS += \
./src/veins/modules/application/platooning/scenarios/AccelerateAndBrakeScenario.d \
./src/veins/modules/application/platooning/scenarios/BaseScenario.d \
./src/veins/modules/application/platooning/scenarios/BrakingScenario.d \
./src/veins/modules/application/platooning/scenarios/IntersectionScenario.d \
./src/veins/modules/application/platooning/scenarios/ManualDrivingScenario.d \
./src/veins/modules/application/platooning/scenarios/PlatoonMergingScenario.d \
./src/veins/modules/application/platooning/scenarios/SinusoidalScenario.d \
./src/veins/modules/application/platooning/scenarios/StandstillObstacleScenario.d 

OBJS += \
./src/veins/modules/application/platooning/scenarios/AccelerateAndBrakeScenario.o \
./src/veins/modules/application/platooning/scenarios/BaseScenario.o \
./src/veins/modules/application/platooning/scenarios/BrakingScenario.o \
./src/veins/modules/application/platooning/scenarios/IntersectionScenario.o \
./src/veins/modules/application/platooning/scenarios/ManualDrivingScenario.o \
./src/veins/modules/application/platooning/scenarios/PlatoonMergingScenario.o \
./src/veins/modules/application/platooning/scenarios/SinusoidalScenario.o \
./src/veins/modules/application/platooning/scenarios/StandstillObstacleScenario.o 


# Each subdirectory must supply rules for building sources it contributes
src/veins/modules/application/platooning/scenarios/%.o: ../src/veins/modules/application/platooning/scenarios/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


