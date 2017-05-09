################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/bme280.c 

CPP_SRCS += \
../src/BME280_BB.cpp \
../src/BusDevice.cpp \
../src/Capteur.cpp \
../src/GPIO.cpp \
../src/I2CDevice.cpp \
../src/US2066.cpp \
../src/util.cpp 

OBJS += \
./src/BME280_BB.o \
./src/BusDevice.o \
./src/Capteur.o \
./src/GPIO.o \
./src/I2CDevice.o \
./src/US2066.o \
./src/bme280.o \
./src/util.o 

C_DEPS += \
./src/bme280.d 

CPP_DEPS += \
./src/BME280_BB.d \
./src/BusDevice.d \
./src/Capteur.d \
./src/GPIO.d \
./src/I2CDevice.d \
./src/US2066.d \
./src/util.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


