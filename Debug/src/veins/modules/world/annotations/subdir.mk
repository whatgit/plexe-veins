################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/veins/modules/world/annotations/AnnotationDummy.cc \
../src/veins/modules/world/annotations/AnnotationManager.cc 

CC_DEPS += \
./src/veins/modules/world/annotations/AnnotationDummy.d \
./src/veins/modules/world/annotations/AnnotationManager.d 

OBJS += \
./src/veins/modules/world/annotations/AnnotationDummy.o \
./src/veins/modules/world/annotations/AnnotationManager.o 


# Each subdirectory must supply rules for building sources it contributes
src/veins/modules/world/annotations/%.o: ../src/veins/modules/world/annotations/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


