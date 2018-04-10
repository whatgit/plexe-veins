################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subprojects/veins_inet/src/veins_inet/subdir.mk
-include src/veins/modules/world/annotations/subdir.mk
-include src/veins/modules/phy/subdir.mk
-include src/veins/modules/obstacle/subdir.mk
-include src/veins/modules/mobility/traci/subdir.mk
-include src/veins/modules/mobility/subdir.mk
-include src/veins/modules/messages/subdir.mk
-include src/veins/modules/mac/ieee80211p/subdir.mk
-include src/veins/modules/application/traci/subdir.mk
-include src/veins/modules/application/platooning/utilities/subdir.mk
-include src/veins/modules/application/platooning/traffic/subdir.mk
-include src/veins/modules/application/platooning/scenarios/subdir.mk
-include src/veins/modules/application/platooning/protocols/subdir.mk
-include src/veins/modules/application/platooning/messages/subdir.mk
-include src/veins/modules/application/platooning/apps/subdir.mk
-include src/veins/modules/application/platooning/subdir.mk
-include src/veins/modules/application/ieee80211p/subdir.mk
-include src/veins/modules/analogueModel/subdir.mk
-include src/veins/base/utils/subdir.mk
-include src/veins/base/phyLayer/subdir.mk
-include src/veins/base/modules/subdir.mk
-include src/veins/base/messages/subdir.mk
-include src/veins/base/connectionManager/subdir.mk
-include out/gcc-release/src/veins/modules/world/annotations/subdir.mk
-include out/gcc-release/src/veins/modules/phy/subdir.mk
-include out/gcc-release/src/veins/modules/obstacle/subdir.mk
-include out/gcc-release/src/veins/modules/mobility/traci/subdir.mk
-include out/gcc-release/src/veins/modules/mobility/subdir.mk
-include out/gcc-release/src/veins/modules/messages/subdir.mk
-include out/gcc-release/src/veins/modules/mac/ieee80211p/subdir.mk
-include out/gcc-release/src/veins/modules/application/traci/subdir.mk
-include out/gcc-release/src/veins/modules/application/platooning/utilities/subdir.mk
-include out/gcc-release/src/veins/modules/application/platooning/traffic/subdir.mk
-include out/gcc-release/src/veins/modules/application/platooning/scenarios/subdir.mk
-include out/gcc-release/src/veins/modules/application/platooning/protocols/subdir.mk
-include out/gcc-release/src/veins/modules/application/platooning/messages/subdir.mk
-include out/gcc-release/src/veins/modules/application/platooning/apps/subdir.mk
-include out/gcc-release/src/veins/modules/application/platooning/subdir.mk
-include out/gcc-release/src/veins/modules/application/ieee80211p/subdir.mk
-include out/gcc-release/src/veins/modules/analogueModel/subdir.mk
-include out/gcc-release/src/veins/base/utils/subdir.mk
-include out/gcc-release/src/veins/base/phyLayer/subdir.mk
-include out/gcc-release/src/veins/base/modules/subdir.mk
-include out/gcc-release/src/veins/base/messages/subdir.mk
-include out/gcc-release/src/veins/base/connectionManager/subdir.mk
-include out/gcc-relase/src/veins/modules/world/annotations/subdir.mk
-include out/gcc-relase/src/veins/modules/phy/subdir.mk
-include out/gcc-relase/src/veins/modules/obstacle/subdir.mk
-include out/gcc-relase/src/veins/modules/mobility/traci/subdir.mk
-include out/gcc-relase/src/veins/modules/mobility/subdir.mk
-include out/gcc-relase/src/veins/modules/messages/subdir.mk
-include out/gcc-relase/src/veins/modules/mac/ieee80211p/subdir.mk
-include out/gcc-relase/src/veins/modules/application/traci/subdir.mk
-include out/gcc-relase/src/veins/modules/application/platooning/utilities/subdir.mk
-include out/gcc-relase/src/veins/modules/application/platooning/traffic/subdir.mk
-include out/gcc-relase/src/veins/modules/application/platooning/scenarios/subdir.mk
-include out/gcc-relase/src/veins/modules/application/platooning/protocols/subdir.mk
-include out/gcc-relase/src/veins/modules/application/platooning/messages/subdir.mk
-include out/gcc-relase/src/veins/modules/application/platooning/apps/subdir.mk
-include out/gcc-relase/src/veins/modules/application/platooning/subdir.mk
-include out/gcc-relase/src/veins/modules/application/ieee80211p/subdir.mk
-include out/gcc-relase/src/veins/modules/analogueModel/subdir.mk
-include out/gcc-relase/src/veins/base/utils/subdir.mk
-include out/gcc-relase/src/veins/base/phyLayer/subdir.mk
-include out/gcc-relase/src/veins/base/modules/subdir.mk
-include out/gcc-relase/src/veins/base/messages/subdir.mk
-include out/gcc-relase/src/veins/base/connectionManager/subdir.mk
-include .metadata/.plugins/org.eclipse.cdt.managedbuilder.core/subdir.mk
-include .metadata/.plugins/org.eclipse.cdt.make.core/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: plexe-veins

# Tool invocations
plexe-veins: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++  -o "plexe-veins" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(CC_DEPS)$(C++_DEPS)$(EXECUTABLES)$(C_UPPER_DEPS)$(CXX_DEPS)$(OBJS)$(CPP_DEPS)$(C_DEPS) plexe-veins
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
