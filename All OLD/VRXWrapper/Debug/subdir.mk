################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../backend.cpp \
../bridge.cpp \
../debug.cpp \
../frontend.cpp \
../panels.cpp \
../vrx.cpp 

OBJS += \
./backend.o \
./bridge.o \
./debug.o \
./frontend.o \
./panels.o \
./vrx.o 

CPP_DEPS += \
./backend.d \
./bridge.d \
./debug.d \
./frontend.d \
./panels.d \
./vrx.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


