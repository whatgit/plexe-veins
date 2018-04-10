################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/veins/modules/application/traci/MyVeinsApp.cc \
../src/veins/modules/application/traci/TraCIDemo11p.cc \
../src/veins/modules/application/traci/TraCIDemoRSU11p.cc \
../src/veins/modules/application/traci/TraCITestApp.cc 

CC_DEPS += \
./src/veins/modules/application/traci/MyVeinsApp.d \
./src/veins/modules/application/traci/TraCIDemo11p.d \
./src/veins/modules/application/traci/TraCIDemoRSU11p.d \
./src/veins/modules/application/traci/TraCITestApp.d 

OBJS += \
./src/veins/modules/application/traci/MyVeinsApp.o \
./src/veins/modules/application/traci/TraCIDemo11p.o \
./src/veins/modules/application/traci/TraCIDemoRSU11p.o \
./src/veins/modules/application/traci/TraCITestApp.o 


# Each subdirectory must supply rules for building sources it contributes
src/veins/modules/application/traci/%.o: ../src/veins/modules/application/traci/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


