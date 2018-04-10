################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/veins/base/connectionManager/BaseConnectionManager.cc \
../src/veins/base/connectionManager/ChannelAccess.cc \
../src/veins/base/connectionManager/ConnectionManager.cc \
../src/veins/base/connectionManager/NicEntryDebug.cc \
../src/veins/base/connectionManager/NicEntryDirect.cc 

CC_DEPS += \
./src/veins/base/connectionManager/BaseConnectionManager.d \
./src/veins/base/connectionManager/ChannelAccess.d \
./src/veins/base/connectionManager/ConnectionManager.d \
./src/veins/base/connectionManager/NicEntryDebug.d \
./src/veins/base/connectionManager/NicEntryDirect.d 

OBJS += \
./src/veins/base/connectionManager/BaseConnectionManager.o \
./src/veins/base/connectionManager/ChannelAccess.o \
./src/veins/base/connectionManager/ConnectionManager.o \
./src/veins/base/connectionManager/NicEntryDebug.o \
./src/veins/base/connectionManager/NicEntryDirect.o 


# Each subdirectory must supply rules for building sources it contributes
src/veins/base/connectionManager/%.o: ../src/veins/base/connectionManager/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


