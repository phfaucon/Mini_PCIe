################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../AlphiBoard.cpp \
../AlphiBoard_irq.cpp \
../AlteraSpi.cpp \
../PCIeMini_error.cpp \
../PcieCra.cpp \
../TestProgram.cpp 

OBJS += \
./AlphiBoard.o \
./AlphiBoard_irq.o \
./AlteraSpi.o \
./PCIeMini_error.o \
./PcieCra.o \
./TestProgram.o 

CPP_DEPS += \
./AlphiBoard.d \
./AlphiBoard_irq.d \
./AlteraSpi.d \
./PCIeMini_error.d \
./PcieCra.d \
./TestProgram.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/alphi/eclipse-workspace/Alphi_includes -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


