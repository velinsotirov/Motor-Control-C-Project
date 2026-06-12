TESTCC = gcc
ATMEGACC = avr-gcc
STMCC = arm-none-eabi-gcc

COMMONSRC = src/common/global.c src/controller.c src/system.c

INCLUDES = -I src -I src/common

TESTINCLUDES = -I test
TESTSRC = test/run_simulation.c test/motor.c test/test_abstraction.c
TESTEXE = test/run_simulation.exe

TARGETINCLUDES = -I test -I src/bsw
ATMEGAINCLUDES = -I src/hal_atmega328p -I F:/Uni/avr8-gnu-toolchain-4.0.0.52-win32.any.x86_64/avr8-gnu-toolchain-win32_x86_64/avr/include/
STMINCLUDES = -I src/hal_stm32 -I src/hal_stm32/CMSIS/Device/ST/STM32F1xx/Include -I src/hal_stm32/STM32F1xx_HAL_Driver/Inc \
	-I src/hal_stm32/CMSIS/Core/Include -I F:\Uni\gcc_arn_none_eabi_10_3_2021_10\arm-none-eabi\include

TARGETSRC = src/bsw/current.c src/bsw/diag.c src/bsw/encoder.c src/main.c \
	src/bsw/ringbuffer.c
ATMEGASRC = src/hal_atmega328p/atmega328p_hal.c src/hal_atmega328p/atmega328p_init.c \
	src/hal_atmega328p/atmega328p_pwm.c src/hal_atmega328p/atmega328p_uart.c \
	src/hal_atmega328p/atmega328p_adc.c
STMSRC = src/hal_stm32/stm32_init.c src/hal_stm32/stm32f1xx_hal_msp.c src/hal_stm32/stm32f1xx_it.c \
	src/hal_stm32/system_stm32f1xx.c src/hal_stm32/stm32_hal.c src/hal_stm32/stm32_pwm.c \
	src/hal_stm32/stm32_uart.c src/hal_stm32/stm32_adc.c src/hal_stm32/stm32_syscalls.c \
	src/hal_stm32/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c \
	src/hal_stm32/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c \
	src/hal_stm32/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim.c \
	src/hal_stm32/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c \
	src/hal_stm32/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c \
	src/hal_stm32/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc.c \
	src/hal_stm32/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c \
	src/hal_stm32/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim_ex.c \
	src/hal_stm32/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c \
	src/hal_stm32/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc_ex.c \
	src/hal_stm32/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c

ATMEGAEXE = build/main_atmega328p.elf
STMEXE = build/main_stm32.elf

COMMONFLAGS = -Wall -Wextra -Og

test: $(TESTEXE)

$(TESTEXE): $(COMMONSRC) $(TESTSRC)
	$(TESTCC) $(COMMONSRC) $(TESTSRC) \
	$(INCLUDES)	$(TESTINCLUDES) \
	-o $(TESTEXE) -DTEST_BUILD

target_atmega: $(ATMEGAEXE)

$(ATMEGAEXE) : $(COMMONSRC) $(TARGETSRC) $(ATMEGASRC)
	$(ATMEGACC) $(COMMONSRC) $(TARGETSRC) $(ATMEGASRC) \
	$(INCLUDES) $(TARGETINCLUDES) $(ATMEGAINCLUDES) \
	$(COMMONFLAGS) -o $(ATMEGAEXE) \
	-fstack-usage -mmcu=atmega328p \
	-Wl,-Map=build/main_atmega328p.map -D__AVR_ATmega328P__
	avr-objdump -d $(ATMEGAEXE) > dump.txt
	avr-size $(ATMEGAEXE)

target_stm: $(STMEXE)

$(STMEXE) : $(COMMONSRC) $(TARGETSRC) $(STMSRC)
	$(STMCC) $(COMMONSRC) $(TARGETSRC) $(STMSRC) \
	$(INCLUDES) $(TARGETINCLUDES) $(STMINCLUDES) \
	$(COMMONFLAGS) -o $(STMEXE) \
	-ffunction-sections -fdata-sections -Wl,--gc-sections \
	-mcpu=cortex-m3 -Wl,-Map=build/main_stm32.map \
	-D__ARM_CortexM3__ -DSTM32F103xB
	arm-none-eabi-size build/main_stm32.elf