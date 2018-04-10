################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/veins/modules/application/platooning/apps/BaseApp.cc \
../src/veins/modules/application/platooning/apps/IntersectionApp.cc \
../src/veins/modules/application/platooning/apps/ManualDrivingApp.cc \
../src/veins/modules/application/platooning/apps/PlatoonMergingApp.cc \
../src/veins/modules/application/platooning/apps/SimplePlatooningApp.cc 

CC_DEPS += \
./src/veins/modules/application/platooning/apps/BaseApp.d \
./src/veins/modules/application/platooning/apps/IntersectionApp.d \
./src/veins/modules/application/platooning/apps/ManualDrivingApp.d \
./src/veins/modules/application/platooning/apps/PlatoonMergingApp.d \
./src/veins/modules/application/platooning/apps/SimplePlatooningApp.d 

OBJS += \
./src/veins/modules/application/platooning/apps/BaseApp.o \
./src/veins/modules/application/platooning/apps/IntersectionApp.o \
./src/veins/modules/application/platooning/apps/ManualDrivingApp.o \
./src/veins/modules/application/platooning/apps/PlatoonMergingApp.o \
./src/veins/modules/application/platooning/apps/SimplePlatooningApp.o 


# Each subdirectory must supply rules for building sources it contributes
src/veins/modules/application/platooning/apps/%.o: ../src/veins/modules/application/platooning/apps/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


