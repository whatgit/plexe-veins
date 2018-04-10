################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/veins/modules/messages/AirFrame11p_m.cc \
../src/veins/modules/messages/BasicSafetyMessage_m.cc \
../src/veins/modules/messages/Mac80211Pkt_m.cc \
../src/veins/modules/messages/PhyControlMessage_m.cc \
../src/veins/modules/messages/WaveServiceAdvertisement_m.cc \
../src/veins/modules/messages/WaveShortMessage_m.cc 

CC_DEPS += \
./src/veins/modules/messages/AirFrame11p_m.d \
./src/veins/modules/messages/BasicSafetyMessage_m.d \
./src/veins/modules/messages/Mac80211Pkt_m.d \
./src/veins/modules/messages/PhyControlMessage_m.d \
./src/veins/modules/messages/WaveServiceAdvertisement_m.d \
./src/veins/modules/messages/WaveShortMessage_m.d 

OBJS += \
./src/veins/modules/messages/AirFrame11p_m.o \
./src/veins/modules/messages/BasicSafetyMessage_m.o \
./src/veins/modules/messages/Mac80211Pkt_m.o \
./src/veins/modules/messages/PhyControlMessage_m.o \
./src/veins/modules/messages/WaveServiceAdvertisement_m.o \
./src/veins/modules/messages/WaveShortMessage_m.o 


# Each subdirectory must supply rules for building sources it contributes
src/veins/modules/messages/%.o: ../src/veins/modules/messages/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


