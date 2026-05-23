TESTCC = gcc
TARGETCC = avr-gcc

SRC = src/common/global.c src/controller.c src/system.c

INCLUDES = -I src -I src/common

TESTINCLUDES = -I test
TESTFILES = test/run_simulation.c test/motor.c test/test_abstraction.c
TESTEXE = test/run_simulation.exe

TARGETINCLUDES = -I test -I src/bsw \
	-I F:/Uni/avr8-gnu-toolchain-4.0.0.52-win32.any.x86_64/avr8-gnu-toolchain-win32_x86_64/avr/include/
TARGETFILES = src/main.c src/bsw/encoder.c src/bsw/pwm.c
TARGETEXE = src/main.elf

TARGETFLAGS = -Wall -Wextra -Og -mmcu=atmega328p

test: $(TESTEXE)

$(TESTEXE): $(SRC) $(TESTFILES)
	$(TESTCC) $(SRC) $(TESTFILES) \
	$(INCLUDES)	$(TESTINCLUDES) \
	-o $(TESTEXE) -DTEST_BUILD

target: $(TARGETEXE)

$(TARGETEXE) : $(SRC) $(TARGETFILES)
	$(TARGETCC) $(SRC) $(TARGETFILES) \
	$(INCLUDES) $(TARGETINCLUDES) \
	$(TARGETFLAGS) -o $(TARGETEXE)