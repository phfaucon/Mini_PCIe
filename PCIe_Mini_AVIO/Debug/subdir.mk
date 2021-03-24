################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../HI_8429.cpp \
../PCIe_Mini_AVIO.cpp \
../SpiOpenCore.cpp \
../hostbug.cpp 

OBJS += \
./HI_8429.o \
./PCIe_Mini_AVIO.o \
./SpiOpenCore.o \
./hostbug.o 

CPP_DEPS += \
./HI_8429.d \
./PCIe_Mini_AVIO.d \
./SpiOpenCore.d \
./hostbug.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/alphi/eclipse-workspace/Alphi_PCIe" -I/home/alphi/eclipse-workspace/Alphi_includes -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


