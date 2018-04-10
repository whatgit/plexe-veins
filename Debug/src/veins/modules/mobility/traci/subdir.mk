################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/veins/modules/mobility/traci/SumoTrafficManager.cc \
../src/veins/modules/mobility/traci/TestTrafficManager.cc \
../src/veins/modules/mobility/traci/TraCIBaseTrafficManager.cc \
../src/veins/modules/mobility/traci/TraCIBuffer.cc \
../src/veins/modules/mobility/traci/TraCIColor.cc \
../src/veins/modules/mobility/traci/TraCICommandInterface.cc \
../src/veins/modules/mobility/traci/TraCIConnection.cc \
../src/veins/modules/mobility/traci/TraCILauncher.cc \
../src/veins/modules/mobility/traci/TraCIMobility.cc \
../src/veins/modules/mobility/traci/TraCIScenarioManager.cc \
../src/veins/modules/mobility/traci/TraCIScenarioManagerForker.cc \
../src/veins/modules/mobility/traci/TraCIScenarioManagerLaunchd.cc \
../src/veins/modules/mobility/traci/TraCIScreenRecorder.cc 

CC_DEPS += \
./src/veins/modules/mobility/traci/SumoTrafficManager.d \
./src/veins/modules/mobility/traci/TestTrafficManager.d \
./src/veins/modules/mobility/traci/TraCIBaseTrafficManager.d \
./src/veins/modules/mobility/traci/TraCIBuffer.d \
./src/veins/modules/mobility/traci/TraCIColor.d \
./src/veins/modules/mobility/traci/TraCICommandInterface.d \
./src/veins/modules/mobility/traci/TraCIConnection.d \
./src/veins/modules/mobility/traci/TraCILauncher.d \
./src/veins/modules/mobility/traci/TraCIMobility.d \
./src/veins/modules/mobility/traci/TraCIScenarioManager.d \
./src/veins/modules/mobility/traci/TraCIScenarioManagerForker.d \
./src/veins/modules/mobility/traci/TraCIScenarioManagerLaunchd.d \
./src/veins/modules/mobility/traci/TraCIScreenRecorder.d 

OBJS += \
./src/veins/modules/mobility/traci/SumoTrafficManager.o \
./src/veins/modules/mobility/traci/TestTrafficManager.o \
./src/veins/modules/mobility/traci/TraCIBaseTrafficManager.o \
./src/veins/modules/mobility/traci/TraCIBuffer.o \
./src/veins/modules/mobility/traci/TraCIColor.o \
./src/veins/modules/mobility/traci/TraCICommandInterface.o \
./src/veins/modules/mobility/traci/TraCIConnection.o \
./src/veins/modules/mobility/traci/TraCILauncher.o \
./src/veins/modules/mobility/traci/TraCIMobility.o \
./src/veins/modules/mobility/traci/TraCIScenarioManager.o \
./src/veins/modules/mobility/traci/TraCIScenarioManagerForker.o \
./src/veins/modules/mobility/traci/TraCIScenarioManagerLaunchd.o \
./src/veins/modules/mobility/traci/TraCIScreenRecorder.o 


# Each subdirectory must supply rules for building sources it contributes
src/veins/modules/mobility/traci/%.o: ../src/veins/modules/mobility/traci/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


