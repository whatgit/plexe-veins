################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/veins/base/utils/Coord.cc \
../src/veins/base/utils/NetwToMacControlInfo.cc \
../src/veins/base/utils/SimpleAddress.cc \
../src/veins/base/utils/asserts.cc \
../src/veins/base/utils/winsupport.cc 

CC_DEPS += \
./src/veins/base/utils/Coord.d \
./src/veins/base/utils/NetwToMacControlInfo.d \
./src/veins/base/utils/SimpleAddress.d \
./src/veins/base/utils/asserts.d \
./src/veins/base/utils/winsupport.d 

OBJS += \
./src/veins/base/utils/Coord.o \
./src/veins/base/utils/NetwToMacControlInfo.o \
./src/veins/base/utils/SimpleAddress.o \
./src/veins/base/utils/asserts.o \
./src/veins/base/utils/winsupport.o 


# Each subdirectory must supply rules for building sources it contributes
src/veins/base/utils/%.o: ../src/veins/base/utils/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


