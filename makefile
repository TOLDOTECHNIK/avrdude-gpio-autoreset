# The recommended compiler flags for the Raspberry Pi
CCFLAGS= -Ofast -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -std=c++11

# define all programs
PROGRAMS = avrdude-gpio-autoreset
SOURCES = ${PROGRAMS:=.cpp}

all: ${PROGRAMS}

${PROGRAMS}: ${SOURCES}
	g++ ${CCFLAGS} ./includes/GPIOClass.cpp $@.cpp -o ./bin/$@

clean:
	rm -rf $(PROGRAMS)

install: all


.PHONY: install
