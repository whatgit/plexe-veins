################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../subprojects/veins_inet/src/veins_inet/VeinsInetManager.cc \
../subprojects/veins_inet/src/veins_inet/VeinsInetMobility.cc 

CC_DEPS += \
./subprojects/veins_inet/src/veins_inet/VeinsInetManager.d \
./subprojects/veins_inet/src/veins_inet/VeinsInetMobility.d 

OBJS += \
./subprojects/veins_inet/src/veins_inet/VeinsInetManager.o \
./subprojects/veins_inet/src/veins_inet/VeinsInetMobility.o 


# Each subdirectory must supply rules for building sources it contributes
subprojects/veins_inet/src/veins_inet/%.o: ../subprojects/veins_inet/src/veins_inet/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


