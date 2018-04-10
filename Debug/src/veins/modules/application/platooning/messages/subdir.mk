################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/veins/modules/application/platooning/messages/ICLCM_m.cc \
../src/veins/modules/application/platooning/messages/InterferingBeacon_m.cc \
../src/veins/modules/application/platooning/messages/Intersection_m.cc \
../src/veins/modules/application/platooning/messages/PlatooningBeacon_m.cc \
../src/veins/modules/application/platooning/messages/UnicastMessage_m.cc \
../src/veins/modules/application/platooning/messages/UnicastProtocolControlMessage_m.cc 

CC_DEPS += \
./src/veins/modules/application/platooning/messages/ICLCM_m.d \
./src/veins/modules/application/platooning/messages/InterferingBeacon_m.d \
./src/veins/modules/application/platooning/messages/Intersection_m.d \
./src/veins/modules/application/platooning/messages/PlatooningBeacon_m.d \
./src/veins/modules/application/platooning/messages/UnicastMessage_m.d \
./src/veins/modules/application/platooning/messages/UnicastProtocolControlMessage_m.d 

OBJS += \
./src/veins/modules/application/platooning/messages/ICLCM_m.o \
./src/veins/modules/application/platooning/messages/InterferingBeacon_m.o \
./src/veins/modules/application/platooning/messages/Intersection_m.o \
./src/veins/modules/application/platooning/messages/PlatooningBeacon_m.o \
./src/veins/modules/application/platooning/messages/UnicastMessage_m.o \
./src/veins/modules/application/platooning/messages/UnicastProtocolControlMessage_m.o 


# Each subdirectory must supply rules for building sources it contributes
src/veins/modules/application/platooning/messages/%.o: ../src/veins/modules/application/platooning/messages/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


