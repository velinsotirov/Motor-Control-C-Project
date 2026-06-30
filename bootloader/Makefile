BOOTCC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

INCLUDES = -I src

#ATMEGAINCLUDES = -I src/hal_atmega328p -I F:/Uni/avr8-gnu-toolchain-4.0.0.52-win32.any.x86_64/avr8-gnu-toolchain-win32_x86_64/avr/include/
STMINCLUDES = -I ../application/src/hal_stm32 \
	-I ../application/src/hal_stm32/CMSIS/Device/ST/STM32F1xx/Include \
	-I ../application/src/hal_stm32/STM32F1xx_HAL_Driver/Inc \
	-I ../application/src/hal_stm32/CMSIS/Core/Include \
	-I F:\Uni\gcc_arn_none_eabi_10_3_2021_10\arm-none-eabi\include

BOOTSRC = src/bootloader.c src/init.c
HALSRC = ../application/src/hal_stm32/stm32_syscalls.c \
	../application/src/hal_stm32/system_stm32f1xx.c \
	../application/src/hal_stm32/stm32f1xx_hal_msp.c \
	../application/src/hal_stm32/stm32f1xx_it.c \
	../application/src/hal_stm32/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c \
	../application/src/hal_stm32/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c \
	../application/src/hal_stm32/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c \
	../application/src/hal_stm32/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c \
	../application/src/hal_stm32/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c

# stm objects to link
BOOTOBJ = $(BOOTSRC:%.c=build/%.o)
BOOTOBJ += $(patsubst ../application/%.c, build/%.o, $(HALSRC))
BOOTOBJ += build/startup_stm32f103c8tx.o

BOOTEXE = build/bootloader.elf
BINEXE = build/bootloader.bin

COMMONFLAGS = -Wall -Wextra -Og -g3
STMFLAGS = -ffunction-sections -fdata-sections \
			-mcpu=cortex-m3  -D__ARM_CortexM3__ -DSTM32F103xB

# stm build rules
build/%.o: %.c
	$(Q) $(BOOTCC) $(INCLUDES) $(STMINCLUDES) \
	$(COMMONFLAGS) $(STMFLAGS) -c $< -o $@
# rule for shared HAL sources — maps ../application/src/hal_stm32/... to build/hal/...
build/%.o: ../application/%.c
	$(Q) $(BOOTCC) $(INCLUDES) $(STMINCLUDES) \
	$(COMMONFLAGS) $(STMFLAGS) -c $< -o $@
build/%.o: ../application/%.s
	$(Q) $(BOOTCC) $(INCLUDES) $(STMINCLUDES) \
	$(COMMONFLAGS) $(STMFLAGS) -x assembler-with-cpp -c $< -o $@

#-x assembler-with-cpp startup_stm32f103c8tx.s -x none \
# -Wl,-Map=build/bootloader.map 

bootloader_stm: $(BOOTEXE)

$(BOOTEXE) : $(BOOTOBJ)
	$(BOOTCC) $(BOOTOBJ) -o $(BOOTEXE) \
	-TSTM32F103C8TX_FLASH.ld \
	-Wl,--gc-sections
	arm-none-eabi-size build/bootloader.elf
	$(OBJCOPY) -O binary $(BOOTEXE) $(BINEXE)
	$(Q)python pad_bootloader.py

