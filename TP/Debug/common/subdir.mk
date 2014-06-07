################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../common/ByteString.cpp \
../common/ComparerRecord.cpp \
../common/Document.cpp \
../common/DocumentGlobalWeight.cpp \
../common/FileBlockManager.cpp \
../common/FileFlags.cpp \
../common/FreeBlockManager.cpp \
../common/Key.cpp \
../common/ListOfPositions.cpp \
../common/ListofID.cpp \
../common/Properties.cpp \
../common/RankingDocument.cpp \
../common/Record.cpp \
../common/Resource.cpp \
../common/Word.cpp \
../common/gammaCoder.cpp 

OBJS += \
./common/ByteString.o \
./common/ComparerRecord.o \
./common/Document.o \
./common/DocumentGlobalWeight.o \
./common/FileBlockManager.o \
./common/FileFlags.o \
./common/FreeBlockManager.o \
./common/Key.o \
./common/ListOfPositions.o \
./common/ListofID.o \
./common/Properties.o \
./common/RankingDocument.o \
./common/Record.o \
./common/Resource.o \
./common/Word.o \
./common/gammaCoder.o 

CPP_DEPS += \
./common/ByteString.d \
./common/ComparerRecord.d \
./common/Document.d \
./common/DocumentGlobalWeight.d \
./common/FileBlockManager.d \
./common/FileFlags.d \
./common/FreeBlockManager.d \
./common/Key.d \
./common/ListOfPositions.d \
./common/ListofID.d \
./common/Properties.d \
./common/RankingDocument.d \
./common/Record.d \
./common/Resource.d \
./common/Word.d \
./common/gammaCoder.d 


# Each subdirectory must supply rules for building sources it contributes
common/%.o: ../common/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -w -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


