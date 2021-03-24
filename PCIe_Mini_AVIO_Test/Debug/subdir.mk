################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../PCIe_Mini_AVIO_test.cpp 

OBJS += \
./PCIe_Mini_AVIO_test.o 

CPP_DEPS += \
./PCIe_Mini_AVIO_test.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/alphi/eclipse-workspace/Alphi_PCIe" -I/home/alphi/eclipse-workspace/Alphi_includes -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


