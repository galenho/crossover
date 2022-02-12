################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables
CPP_SRCS += \
../lua_crossover.cpp \
../lua_mongo.cpp \
../lua_mysql.cpp \
../lua_network.cpp \
../lua_wrapper.cpp \
../main.cpp 

CPP_DEPS += \
./lua_crossover.d \
./lua_mongo.d \
./lua_mysql.d \
./lua_network.d \
./lua_wrapper.d \
./main.d 

C_SRCS += \
../lua_pb.c \
../lua_seri.c 

C_DEPS += \
./lua_pb.d \
./lua_seri.d 

OBJS += \
./lua_crossover.o \
./lua_mongo.o \
./lua_mysql.o \
./lua_network.o \
./lua_pb.o \
./lua_seri.o \
./lua_wrapper.o \
./main.o 

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++14 -D_GLIBCXX_DEBUG  -I../.. -I../../include -I../../include/lua -I../../include/libmongoc-1.0 -I../../include/libbson-1.0 -DDEBUG -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ''

%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc  -I../.. -I../../include -I../../include/lua -I../../include/libmongoc-1.0 -I../../include/libbson-1.0 -DDEBUG -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ''
