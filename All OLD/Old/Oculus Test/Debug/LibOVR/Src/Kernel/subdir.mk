################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../LibOVR/Src/Kernel/OVR_Alg.cpp \
../LibOVR/Src/Kernel/OVR_Allocator.cpp \
../LibOVR/Src/Kernel/OVR_Atomic.cpp \
../LibOVR/Src/Kernel/OVR_File.cpp \
../LibOVR/Src/Kernel/OVR_FileFILE.cpp \
../LibOVR/Src/Kernel/OVR_Log.cpp \
../LibOVR/Src/Kernel/OVR_Math.cpp \
../LibOVR/Src/Kernel/OVR_RefCount.cpp \
../LibOVR/Src/Kernel/OVR_Std.cpp \
../LibOVR/Src/Kernel/OVR_String.cpp \
../LibOVR/Src/Kernel/OVR_String_FormatUtil.cpp \
../LibOVR/Src/Kernel/OVR_String_PathUtil.cpp \
../LibOVR/Src/Kernel/OVR_SysFile.cpp \
../LibOVR/Src/Kernel/OVR_System.cpp \
../LibOVR/Src/Kernel/OVR_ThreadsPthread.cpp \
../LibOVR/Src/Kernel/OVR_ThreadsWinAPI.cpp \
../LibOVR/Src/Kernel/OVR_Timer.cpp \
../LibOVR/Src/Kernel/OVR_UTF8Util.cpp 

OBJS += \
./LibOVR/Src/Kernel/OVR_Alg.o \
./LibOVR/Src/Kernel/OVR_Allocator.o \
./LibOVR/Src/Kernel/OVR_Atomic.o \
./LibOVR/Src/Kernel/OVR_File.o \
./LibOVR/Src/Kernel/OVR_FileFILE.o \
./LibOVR/Src/Kernel/OVR_Log.o \
./LibOVR/Src/Kernel/OVR_Math.o \
./LibOVR/Src/Kernel/OVR_RefCount.o \
./LibOVR/Src/Kernel/OVR_Std.o \
./LibOVR/Src/Kernel/OVR_String.o \
./LibOVR/Src/Kernel/OVR_String_FormatUtil.o \
./LibOVR/Src/Kernel/OVR_String_PathUtil.o \
./LibOVR/Src/Kernel/OVR_SysFile.o \
./LibOVR/Src/Kernel/OVR_System.o \
./LibOVR/Src/Kernel/OVR_ThreadsPthread.o \
./LibOVR/Src/Kernel/OVR_ThreadsWinAPI.o \
./LibOVR/Src/Kernel/OVR_Timer.o \
./LibOVR/Src/Kernel/OVR_UTF8Util.o 

CPP_DEPS += \
./LibOVR/Src/Kernel/OVR_Alg.d \
./LibOVR/Src/Kernel/OVR_Allocator.d \
./LibOVR/Src/Kernel/OVR_Atomic.d \
./LibOVR/Src/Kernel/OVR_File.d \
./LibOVR/Src/Kernel/OVR_FileFILE.d \
./LibOVR/Src/Kernel/OVR_Log.d \
./LibOVR/Src/Kernel/OVR_Math.d \
./LibOVR/Src/Kernel/OVR_RefCount.d \
./LibOVR/Src/Kernel/OVR_Std.d \
./LibOVR/Src/Kernel/OVR_String.d \
./LibOVR/Src/Kernel/OVR_String_FormatUtil.d \
./LibOVR/Src/Kernel/OVR_String_PathUtil.d \
./LibOVR/Src/Kernel/OVR_SysFile.d \
./LibOVR/Src/Kernel/OVR_System.d \
./LibOVR/Src/Kernel/OVR_ThreadsPthread.d \
./LibOVR/Src/Kernel/OVR_ThreadsWinAPI.d \
./LibOVR/Src/Kernel/OVR_Timer.d \
./LibOVR/Src/Kernel/OVR_UTF8Util.d 


# Each subdirectory must supply rules for building sources it contributes
LibOVR/Src/Kernel/%.o: ../LibOVR/Src/Kernel/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


