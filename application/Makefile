
# compilers
TESTCC = gcc
ATMEGACC = avr-gcc
STMCC = arm-none-eabi-gcc

# includes
INCLUDES = -I src -I src/common
TESTINCLUDES = -I test
TARGETINCLUDES = -I test -I src/bsw
ATMEGAINCLUDES = -I src/hal_atmega328p \
				-I F:/Uni/avr8-gnu-toolchain-4.0.0.52-win32.any.x86_64/avr8-gnu-toolchain-win32_x86_64/avr/include/
STMINCLUDES = -I src/hal_stm32 \
				-I src/hal_stm32/CMSIS/Device/ST/STM32F1xx/Include \
				-I src/hal_stm32/STM32F1xx_HAL_Driver/Inc \
				-I src/hal_stm32/CMSIS/Core/Include \
				-I F:\Uni\gcc_arn_none_eabi_10_3_2021_10\arm-none-eabi\include

# source files to compile
COMMONSRC = src/common/global.c src/controller.c src/system.c
TESTSRC = test/run_simulation.c test/motor.c test/test_abstraction.c
TARGETSRC = src/bsw/current.c src/bsw/diag.c src/bsw/encoder.c src/main.c \
	src/bsw/ringbuffer.c
ATMEGASRC = src/hal_atmega328p/atmega328p_hal.c src/hal_atmega328p/atmega328p_init.c \
	src/hal_atmega328p/atmega328p_pwm.c src/hal_atmega328p/atmega328p_uart.c \
	src/hal_atmega328p/atmega328p_adc.c
STMSRC = src/hal_stm32/stm32_init.c src/hal_stm32/stm32_gpio.c \
	src/hal_stm32/system_stm32f1xx.c src/hal_stm32/stm32_hal.c src/hal_stm32/stm32_pwm.c \
	src/hal_stm32/stm32_uart.c src/hal_stm32/stm32_adc.c src/hal_stm32/stm32_syscalls.c \
	src/hal_stm32/stm32f1xx_hal_msp.c src/hal_stm32/stm32f1xx_it.c \
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

# stm objects to link
STMOBJ = $(STMSRC:%.c=build/stm/%.o)
STMOBJ += $(COMMONSRC:%.c=build/stm/%.o)
STMOBJ += $(TARGETSRC:%.c=build/stm/%.o)

# atmega objects to link
ATMEGAOBJ = $(ATMEGASRC:%.c=build/stm/%.o)
ATMEGAOBJ += $(COMMONSRC:%.c=build/stm/%.o)
ATMEGAOBJ += $(TARGETSRC:%.c=build/stm/%.o)

# test objects to link
TESTOBJ = $(COMMONSRC:%.c=build/stm/%.o)
TESTOBJ += $(TESTSRC:%.c=build/stm/%.o)

# results of compilation
TESTEXE = build/test/run_simulation.exe
ATMEGAEXE = build/avr/main_atmega328p.elf
STMEXE = build/stm/main_stm32.elf

# compiler flags
COMMONFLAGS = -Wall -Wextra -Og -g3
STMFLAGS = -ffunction-sections -fdata-sections \
			-mcpu=cortex-m3  -D__ARM_CortexM3__ -DSTM32F103xB
ATMEGAFLAGS = -fstack-usage -mmcu=atmega328p -D__AVR_ATmega328P__

# atmega build rules
build/avr/%.o: %.c
	$(Q) $(ATMEGACC) $(INCLUDES) $(TARGETINCLUDES) $(ATMEGAINCLUDES) \
	$(COMMONFLAGS) $(ATMEGAFLAGS) -c $< -o $@

# stm build rules
build/stm/%.o: %.c
	$(Q) $(STMCC) $(INCLUDES) $(TARGETINCLUDES) $(STMINCLUDES) \
	$(COMMONFLAGS) $(STMFLAGS) -c $< -o $@
build/stm/%.o: %.s
	$(Q) $(STMCC) $(INCLUDES) $(TARGETINCLUDES) $(STMINCLUDES) \
	$(COMMONFLAGS) $(STMFLAGS) -x assembler-with-cpp -c $< -o $@

# test build rules
build/test/%.o: %.c
	$(Q) $(TESTCC) $(INCLUDES) $(TESTINCLUDES) \
	$(COMMONFLAGS) -DTEST_BUILD -c $< -o $@

# build commands
test: $(TESTEXE)
target_stm: $(STMEXE)
target_atmega: $(ATMEGAEXE)

# test linking
$(TESTEXE): $(TESTOBJ)
	$(TESTCC) $(TESTOBJ) -o $(TESTEXE)

# atmega linking
$(ATMEGAEXE) : $(ATMEGAOBJ)
	$(ATMEGACC) $(ATMEGAOBJ) -o $(ATMEGAEXE) \
	-Wl,-Map=build/main_atmega328p.map
	avr-objdump -d $(ATMEGAEXE) > build/avr/atmega_dump.txt
	avr-size $(ATMEGAEXE)

# stm linking
$(STMEXE) : $(STMOBJ)
	$(STMCC) $(STMOBJ) -o $(STMEXE) \
	-TSTM32F103C8TX_FLASH.ld \
	-Wl,--gc-sections -Wl,-Map=build/stm/main_stm32.map
	arm-none-eabi-size build/stm/main_stm32.elf
