################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/veins/base/messages/AirFrame_m.cc \
../src/veins/base/messages/BorderMsg_m.cc \
../src/veins/base/messages/ChannelSenseRequest_m.cc \
../src/veins/base/messages/MacPkt_m.cc 

CC_DEPS += \
./src/veins/base/messages/AirFrame_m.d \
./src/veins/base/messages/BorderMsg_m.d \
./src/veins/base/messages/ChannelSenseRequest_m.d \
./src/veins/base/messages/MacPkt_m.d 

OBJS += \
./src/veins/base/messages/AirFrame_m.o \
./src/veins/base/messages/BorderMsg_m.o \
./src/veins/base/messages/ChannelSenseRequest_m.o \
./src/veins/base/messages/MacPkt_m.o 


# Each subdirectory must supply rules for building sources it contributes
src/veins/base/messages/%.o: ../src/veins/base/messages/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


