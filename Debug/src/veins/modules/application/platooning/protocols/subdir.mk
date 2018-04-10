################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/veins/modules/application/platooning/protocols/BaseProtocol.cc \
../src/veins/modules/application/platooning/protocols/HumanInterferingProtocol.cc \
../src/veins/modules/application/platooning/protocols/IntersectionProtocol.cc \
../src/veins/modules/application/platooning/protocols/PlatoonMergingProtocol.cc \
../src/veins/modules/application/platooning/protocols/SimplePlatooningBeaconing.cc \
../src/veins/modules/application/platooning/protocols/SlottedBeaconing.cc 

CC_DEPS += \
./src/veins/modules/application/platooning/protocols/BaseProtocol.d \
./src/veins/modules/application/platooning/protocols/HumanInterferingProtocol.d \
./src/veins/modules/application/platooning/protocols/IntersectionProtocol.d \
./src/veins/modules/application/platooning/protocols/PlatoonMergingProtocol.d \
./src/veins/modules/application/platooning/protocols/SimplePlatooningBeaconing.d \
./src/veins/modules/application/platooning/protocols/SlottedBeaconing.d 

OBJS += \
./src/veins/modules/application/platooning/protocols/BaseProtocol.o \
./src/veins/modules/application/platooning/protocols/HumanInterferingProtocol.o \
./src/veins/modules/application/platooning/protocols/IntersectionProtocol.o \
./src/veins/modules/application/platooning/protocols/PlatoonMergingProtocol.o \
./src/veins/modules/application/platooning/protocols/SimplePlatooningBeaconing.o \
./src/veins/modules/application/platooning/protocols/SlottedBeaconing.o 


# Each subdirectory must supply rules for building sources it contributes
src/veins/modules/application/platooning/protocols/%.o: ../src/veins/modules/application/platooning/protocols/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


