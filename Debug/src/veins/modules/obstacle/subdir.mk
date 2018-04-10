################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/veins/modules/obstacle/Obstacle.cc \
../src/veins/modules/obstacle/ObstacleControl.cc 

CC_DEPS += \
./src/veins/modules/obstacle/Obstacle.d \
./src/veins/modules/obstacle/ObstacleControl.d 

OBJS += \
./src/veins/modules/obstacle/Obstacle.o \
./src/veins/modules/obstacle/ObstacleControl.o 


# Each subdirectory must supply rules for building sources it contributes
src/veins/modules/obstacle/%.o: ../src/veins/modules/obstacle/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


