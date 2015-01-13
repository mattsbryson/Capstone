################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../LibOVR/Src/Util/Util_LatencyTest.cpp \
../LibOVR/Src/Util/Util_Render_Stereo.cpp 

OBJS += \
./LibOVR/Src/Util/Util_LatencyTest.o \
./LibOVR/Src/Util/Util_Render_Stereo.o 

CPP_DEPS += \
./LibOVR/Src/Util/Util_LatencyTest.d \
./LibOVR/Src/Util/Util_Render_Stereo.d 


# Each subdirectory must supply rules for building sources it contributes
LibOVR/Src/Util/%.o: ../LibOVR/Src/Util/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


