TESTCC = gcc
TARGETCC = avr-gcc

COMMONSRC = src/common/global.c src/controller.c src/system.c

INCLUDES = -I src -I src/common

TESTINCLUDES = -I test
TESTSRC = test/run_simulation.c test/motor.c test/test_abstraction.c
TESTEXE = test/run_simulation.exe

TARGETINCLUDES = -I test -I src/bsw -I src/hal \
	-I F:/Uni/avr8-gnu-toolchain-4.0.0.52-win32.any.x86_64/avr8-gnu-toolchain-win32_x86_64/avr/include/
TARGETSRC = src/bsw/current.c src/bsw/diag.c src/bsw/encoder.c src/bsw/pwm.c src/main.c \
	src/bsw/ringbuffer.c src/hal/atmega328p_hal.c src/hal/atmega328p_init.c src/hal/atmega328p_uart.c
TARGETEXE = build/main.elf

TARGETFLAGS = -Wall -Wextra -Og -mmcu=atmega328p

test: $(TESTEXE)

$(TESTEXE): $(COMMONSRC) $(TESTSRC)
	$(TESTCC) $(COMMONSRC) $(TESTSRC) \
	$(INCLUDES)	$(TESTINCLUDES) \
	-o $(TESTEXE) -DTEST_BUILD

target: $(TARGETEXE)

$(TARGETEXE) : $(COMMONSRC) $(TARGETSRC)
	$(TARGETCC) $(COMMONSRC) $(TARGETSRC) \
	$(INCLUDES) $(TARGETINCLUDES) \
	$(TARGETFLAGS) -o $(TARGETEXE) \
	-fstack-usage \
	-Wl,-Map=build/main.map
	avr-objdump -d $(TARGETEXE) > dump.txt
	avr-size $(TARGETEXE)