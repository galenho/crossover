################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables
CPP_SRCS += \
../clog.cpp \
../common.cpp \
../connection.cpp \
../console_poller.cpp \
../database.cpp \
../database_impl.cpp \
../datatime.cpp \
../http_accessor.cpp \
../http_accessor_impl.cpp \
../lua_bson.cpp \
../lua_fix.cpp \
../mongo_connection.cpp \
../mongo_database.cpp \
../mongo_database_impl.cpp \
../mutex.cpp \
../referable.cpp \
../resultset.cpp \
../rwlock.cpp \
../scheduler.cpp \
../scheduler_impl.cpp \
../sequence_buffer.cpp \
../serialize_stream.cpp \
../socket.cpp \
../socket_linux.cpp \
../socket_mgr_linux.cpp \
../socket_mgr_win32.cpp \
../socket_ops_linux.cpp \
../socket_ops_win32.cpp \
../socket_win32.cpp \
../spinlock.cpp \
../statement.cpp \
../task.cpp \
../tcp_client.cpp \
../tcp_client_impl.cpp \
../tcp_server.cpp \
../tcp_server_impl.cpp \
../thread_base.cpp \
../work_thread.cpp 

CPP_DEPS += \
./clog.d \
./common.d \
./connection.d \
./console_poller.d \
./database.d \
./database_impl.d \
./datatime.d \
./http_accessor.d \
./http_accessor_impl.d \
./lua_bson.d \
./lua_fix.d \
./mongo_connection.d \
./mongo_database.d \
./mongo_database_impl.d \
./mutex.d \
./referable.d \
./resultset.d \
./rwlock.d \
./scheduler.d \
./scheduler_impl.d \
./sequence_buffer.d \
./serialize_stream.d \
./socket.d \
./socket_linux.d \
./socket_mgr_linux.d \
./socket_mgr_win32.d \
./socket_ops_linux.d \
./socket_ops_win32.d \
./socket_win32.d \
./spinlock.d \
./statement.d \
./task.d \
./tcp_client.d \
./tcp_client_impl.d \
./tcp_server.d \
./tcp_server_impl.d \
./thread_base.d \
./work_thread.d 

OBJS += \
./clog.o \
./common.o \
./connection.o \
./console_poller.o \
./database.o \
./database_impl.o \
./datatime.o \
./http_accessor.o \
./http_accessor_impl.o \
./lua_bson.o \
./lua_fix.o \
./mongo_connection.o \
./mongo_database.o \
./mongo_database_impl.o \
./mutex.o \
./referable.o \
./resultset.o \
./rwlock.o \
./scheduler.o \
./scheduler_impl.o \
./sequence_buffer.o \
./serialize_stream.o \
./socket.o \
./socket_linux.o \
./socket_mgr_linux.o \
./socket_mgr_win32.o \
./socket_ops_linux.o \
./socket_ops_win32.o \
./socket_win32.o \
./spinlock.o \
./statement.o \
./task.o \
./tcp_client.o \
./tcp_client_impl.o \
./tcp_server.o \
./tcp_server_impl.o \
./thread_base.o \
./work_thread.o 

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++14 -D_GLIBCXX_DEBUG  -I../../ -I../../include -I../../include/lua -I../../include/libbson-1.0 -I../../include/libmongoc-1.0 -DDEBUG -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ''

