######################################
# target
######################################
TARGET = hv-bms


######################################
# building variables
######################################

# debug build needs to be specified in make call
#DEBUG = 1

# optimization
OPT = -Og

#######################################
# paths
#######################################
# Build path
BUILD_DIR = build

######################################
# source
######################################
# C sources

#standard call for building wrt POSIX emulator:
#make TESTBENCH=1 

ifndef TESTBENCH

C_SOURCES =  \
../../shared/dbc_to_c/generated_code/CHARGER_CAN.c \
../../shared/dbc_to_c/generated_code/VEHICLE_CAN.c \
../../shared/hal_tick/hal_tick.c \
../../shared/nms_can/nms_can.c \
../../shared/util/util.c \
Core/Src/can.c \
Core/Src/gpio.c \
Core/Src/main.c \
Core/Src/spi.c \
Core/Src/stm32f4xx_hal_msp.c \
Core/Src/stm32f4xx_it.c \
Core/Src/syscalls.c \
Core/Src/sysmem.c \
Core/Src/system_stm32f4xx.c \
Core/Src/tim.c \
Core/Src/usart.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_can.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c \
Drivers_Custom/ADBMS2950/lib/src/adBms2950Data.c \
Drivers_Custom/ADBMS2950/lib/src/adBms2950GenericType.c \
Drivers_Custom/ADBMS2950/program/src/adBms2950Driver.c \
Drivers_Custom/ADBMS2950/program/src/adBms2950PrintResult.c \
Drivers_Custom/ADBMS6830/lib/src/adBms6830GenericType.c \
Drivers_Custom/ADBMS6830/lib/src/adBms6830ParseCreate.c \
Drivers_Custom/ADBMS6830/program/src/adBms6830Driver.c \
Drivers_Custom/ADBMS6830/program/src/adBms6830PrintResult.c \
Drivers_Custom/ADBMS_common/src/adBmsCommonDriver.c \
Drivers_Custom/ADBMS_util/src/adBmsCmdList.c \
Drivers_Custom/ADBMS_util/src/adBmsUtility.c \
Drivers_Custom/ADBMS_util/src/mcuWrapper.c \
Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.c \
Middlewares/Third_Party/FreeRTOS/Source/croutine.c \
Middlewares/Third_Party/FreeRTOS/Source/event_groups.c \
Middlewares/Third_Party/FreeRTOS/Source/list.c \
Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c \
Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c \
Middlewares/Third_Party/FreeRTOS/Source/queue.c \
Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c \
Middlewares/Third_Party/FreeRTOS/Source/tasks.c \
Middlewares/Third_Party/FreeRTOS/Source/timers.c

# ASM sources
ASM_SOURCES =  \
startup_stm32f446xx.s

# ASM sources
ASMM_SOURCES = 

else #building for POSIX FreeRTOS port

C_SOURCES =  \
Drivers_Custom/ADBMS2950/lib/src/adBms2950Data.c \
Drivers_Custom/ADBMS2950/lib/src/adBms2950GenericType.c \
Drivers_Custom/ADBMS2950/program/src/adBms2950Driver.c \
Drivers_Custom/ADBMS2950/program/src/adBms2950PrintResult.c \
Drivers_Custom/ADBMS6830/lib/src/adBms6830GenericType.c \
Drivers_Custom/ADBMS6830/lib/src/adBms6830ParseCreate.c \
Drivers_Custom/ADBMS6830/program/src/adBms6830Driver.c \
Drivers_Custom/ADBMS6830/program/src/adBms6830PrintResult.c \
Drivers_Custom/ADBMS_common/src/adBmsCommonDriver.c \
Drivers_Custom/ADBMS_util/src/adBmsCmdList.c \
Drivers_Custom/ADBMS_util/src/adBmsUtility.c \
FreeRTOS/Source/croutine.c \
../FreeRTOS/Source/event_groups.c \
../FreeRTOS/Source/list.c \
../FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c \
../FreeRTOS/Source/portable/MemMang/heap_4.c \
../FreeRTOS/Source/queue.c \
../FreeRTOS/Source/stream_buffer.c \
../FreeRTOS/Source/tasks.c \
../FreeRTOS/Source/timers.c
Core/Src/main.c

#######################################
# binaries
#######################################

ifndef TESTBENCH
PREFIX = arm-none-eabi-
else
PREFIX =
endif

# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif

#only need these if we're building for the microcontroller
ifndef TESTBENCH
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
endif
 
#######################################
# CFLAGS
#######################################
ifndef TESTBENCH
# cpu
CPU = -mcpu=cortex-m4

# fpu
FPU = -mfpu=fpv4-sp-d16

# float-abi
FLOAT-ABI = -mfloat-abi=hard

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)
else
#no need to specify architecture traits when building for testbench

MCU = 

CPU =

FPU =

endif

ifndef TESTBENCH
# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \
-DUSE_HAL_DRIVER \
-DSTM32F446xx

# AS includes
AS_INCLUDES =  \
-ICore/Inc

# C includes
C_INCLUDES =  \
-I../../shared/dbc_to_c/generated_code \
-I../../shared/nms_can \
-I../../shared/util \
-ICore/Inc \
-IDrivers/CMSIS/Device/ST/STM32F4xx/Include \
-IDrivers/CMSIS/Include \
-IDrivers/STM32F4xx_HAL_Driver/Inc \
-IDrivers/STM32F4xx_HAL_Driver/Inc/Legacy \
-IDrivers_Custom/ADBMS2950/lib/inc \
-IDrivers_Custom/ADBMS2950/program/inc \
-IDrivers_Custom/ADBMS6830/lib/inc \
-IDrivers_Custom/ADBMS6830/program/inc \
-IDrivers_Custom/ADBMS_common/inc \
-IDrivers_Custom/ADBMS_util/inc \
-IMiddlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 \
-IMiddlewares/Third_Party/FreeRTOS/Source/include \
-IMiddlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F

else
#building for testbench simulator

#C_DEFS likely will need to be updated w/ macros/const vars
C_DEFS = \
-DTESTBENCH=1

#have to include the POSIX flavor of FreeRTOS implementations
C_INCLUDES = \
-I../FreeRTOS/Source/include \
-ICore/Inc \
-Inms_wrappers/util \
-Inms_wrappers/nms_wrappers/can_wrappers \
-Inms_wrappers/adbms_drivers \
# -IDrivers_Custom/ADBMS2950/lib/inc \
# -IDrivers_Custom/ADBMS2950/program/inc \
# -IDrivers_Custom/ADBMS6830/lib/inc \
# -IDrivers_Custom/ADBMS6830/program/inc \
# -IDrivers_Custom/ADBMS_common/inc \
# -IDrivers_Custom/ADBMS_util/inc

endif

# compile gcc flags
ifndef TESTBENCH
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections
else
ASFLAGS =
endif
CFLAGS += $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -Werror=implicit-function-declaration -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
	ifdef TESTBENCH
		CFLAGS += -g
	else	
		CFLAGS += -g -gdwarf-2
	endif
endif

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################
# link script
ifndef TESTBENCH
LDSCRIPT = stm32f446retx_flash.ld
# libraries
LIBS = -lc -lm -lnosys 
LIBDIR =
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections
else
LIBS = -lpthread -lrt -lm
LDFLAGS = $(LIBS)
endif

ifndef TESTBENCH
# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin
else
all: $(BUILD_DIR)/$(TARGET).elf

#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

ifndef TESTBENCH
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASMM_SOURCES:.S=.o)))
vpath %.S $(sort $(dir $(ASMM_SOURCES)))
endif

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

ifndef TESTBENCH
$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@
$(BUILD_DIR)/%.o: %.S Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@
endif

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

ifndef TESTBENCH
$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@
endif	
	
$(BUILD_DIR):
	mkdir $@		

#######################################
# clean up
#######################################
clean:
ifeq ($(OS), Windows_NT)
	if exist build rmdir /S /Q build
else
	rm -fR build
endif
  
#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

ifndef TESTBENCH
include ../../shared/build_util/stm32f446re_flash.make
endif

# *** EOF ***