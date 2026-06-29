BOOTCC = arm-none-eabi-gcc

INCLUDES = -I src

ATMEGAINCLUDES = -I src/hal_atmega328p -I F:/Uni/avr8-gnu-toolchain-4.0.0.52-win32.any.x86_64/avr8-gnu-toolchain-win32_x86_64/avr/include/
STMINCLUDES = -I src/hal_stm32 -I src/hal_stm32/CMSIS/Device/ST/STM32F1xx/Include -I src/hal_stm32/STM32F1xx_HAL_Driver/Inc \
	-I src/hal_stm32/CMSIS/Core/Include -I F:\Uni\gcc_arn_none_eabi_10_3_2021_10\arm-none-eabi\include

BOOTSRC = src/bootloader.c

BOOTEXE = build/bootloader.elf

COMMONFLAGS = -Wall -Wextra -Og -g3

#-x assembler-with-cpp startup_stm32f103c8tx.s -x none \

bootloader_stm: $(BOOTEXE)

$(BOOTEXE) : $(BOOTSRC)
	$(STMCC) \
	$(INCLUDES) $(STMINCLUDES) \
	$(COMMONFLAGS) -o $(STMEXE) -TSTM32F103C8TX_FLASH.ld \
	-ffunction-sections -fdata-sections -Wl,--gc-sections \
	-mcpu=cortex-m3 -Wl,-Map=build/bootloader.map \
	-D__ARM_CortexM3__ -DSTM32F103xB
	arm-none-eabi-size build/bootloader.elf
