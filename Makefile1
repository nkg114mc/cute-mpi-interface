################################################################################
# Automatically-generated file. Do not edit!
################################################################################

RM := rm -rf

CPP_SRCS += \
../cute_mpi.cpp \
../main.cpp \
../open_split.cpp \
../pgn_parser.cpp \
../util.cpp 

OBJS += \
./cute_mpi.o \
./main.o \
./open_split.o \
./pgn_parser.o \
./util.o 


MPICC = mpic++

# Each subdirectory must supply rules for building sources it contributes
%.o: %.cpp
	$(MPICC) -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"


# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: cute-mpi-interface

# Tool invocations
cute-mpi-interface: $(OBJS) $(USER_OBJS)
	$(MPICC)  -o "cute-mpi-interface" $(OBJS) $(USER_OBJS) $(LIBS)

# Other Targets
clean:
	-$(RM) $(OBJS)$(C++_DEPS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) cute-mpi-interface
	-@echo ' '

