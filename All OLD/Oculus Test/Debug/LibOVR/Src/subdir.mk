################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../LibOVR/Src/OVR_DeviceHandle.cpp \
../LibOVR/Src/OVR_DeviceImpl.cpp \
../LibOVR/Src/OVR_JSON.cpp \
../LibOVR/Src/OVR_LatencyTestImpl.cpp \
../LibOVR/Src/OVR_Linux_DeviceManager.cpp \
../LibOVR/Src/OVR_Linux_HIDDevice.cpp \
../LibOVR/Src/OVR_Linux_HMDDevice.cpp \
../LibOVR/Src/OVR_Linux_SensorDevice.cpp \
../LibOVR/Src/OVR_OSX_DeviceManager.cpp \
../LibOVR/Src/OVR_OSX_HIDDevice.cpp \
../LibOVR/Src/OVR_OSX_HMDDevice.cpp \
../LibOVR/Src/OVR_OSX_SensorDevice.cpp \
../LibOVR/Src/OVR_Profile.cpp \
../LibOVR/Src/OVR_SensorFilter.cpp \
../LibOVR/Src/OVR_SensorFusion.cpp \
../LibOVR/Src/OVR_SensorImpl.cpp \
../LibOVR/Src/OVR_ThreadCommandQueue.cpp \
../LibOVR/Src/OVR_Win32_DeviceManager.cpp \
../LibOVR/Src/OVR_Win32_DeviceStatus.cpp \
../LibOVR/Src/OVR_Win32_HIDDevice.cpp \
../LibOVR/Src/OVR_Win32_HMDDevice.cpp \
../LibOVR/Src/OVR_Win32_SensorDevice.cpp 

OBJS += \
./LibOVR/Src/OVR_DeviceHandle.o \
./LibOVR/Src/OVR_DeviceImpl.o \
./LibOVR/Src/OVR_JSON.o \
./LibOVR/Src/OVR_LatencyTestImpl.o \
./LibOVR/Src/OVR_Linux_DeviceManager.o \
./LibOVR/Src/OVR_Linux_HIDDevice.o \
./LibOVR/Src/OVR_Linux_HMDDevice.o \
./LibOVR/Src/OVR_Linux_SensorDevice.o \
./LibOVR/Src/OVR_OSX_DeviceManager.o \
./LibOVR/Src/OVR_OSX_HIDDevice.o \
./LibOVR/Src/OVR_OSX_HMDDevice.o \
./LibOVR/Src/OVR_OSX_SensorDevice.o \
./LibOVR/Src/OVR_Profile.o \
./LibOVR/Src/OVR_SensorFilter.o \
./LibOVR/Src/OVR_SensorFusion.o \
./LibOVR/Src/OVR_SensorImpl.o \
./LibOVR/Src/OVR_ThreadCommandQueue.o \
./LibOVR/Src/OVR_Win32_DeviceManager.o \
./LibOVR/Src/OVR_Win32_DeviceStatus.o \
./LibOVR/Src/OVR_Win32_HIDDevice.o \
./LibOVR/Src/OVR_Win32_HMDDevice.o \
./LibOVR/Src/OVR_Win32_SensorDevice.o 

CPP_DEPS += \
./LibOVR/Src/OVR_DeviceHandle.d \
./LibOVR/Src/OVR_DeviceImpl.d \
./LibOVR/Src/OVR_JSON.d \
./LibOVR/Src/OVR_LatencyTestImpl.d \
./LibOVR/Src/OVR_Linux_DeviceManager.d \
./LibOVR/Src/OVR_Linux_HIDDevice.d \
./LibOVR/Src/OVR_Linux_HMDDevice.d \
./LibOVR/Src/OVR_Linux_SensorDevice.d \
./LibOVR/Src/OVR_OSX_DeviceManager.d \
./LibOVR/Src/OVR_OSX_HIDDevice.d \
./LibOVR/Src/OVR_OSX_HMDDevice.d \
./LibOVR/Src/OVR_OSX_SensorDevice.d \
./LibOVR/Src/OVR_Profile.d \
./LibOVR/Src/OVR_SensorFilter.d \
./LibOVR/Src/OVR_SensorFusion.d \
./LibOVR/Src/OVR_SensorImpl.d \
./LibOVR/Src/OVR_ThreadCommandQueue.d \
./LibOVR/Src/OVR_Win32_DeviceManager.d \
./LibOVR/Src/OVR_Win32_DeviceStatus.d \
./LibOVR/Src/OVR_Win32_HIDDevice.d \
./LibOVR/Src/OVR_Win32_HMDDevice.d \
./LibOVR/Src/OVR_Win32_SensorDevice.d 


# Each subdirectory must supply rules for building sources it contributes
LibOVR/Src/%.o: ../LibOVR/Src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


