CFLAGS = -Wall -Wextra -O2
LIBS =
# LIBS = -lm -lrt
# LIBS += $(shell pkg-config --cflags --libs alsa)
# LIBS += $(shell pkg-config --cflags --libs fftw3)


%.o:%.c
	@echo "building objects '$^' to executeable '$@'"
	$(CC) $(CFLAGS) -c -o $@ $^ $(LIBS)

all: main.o
	$(CC) $(CFLAGS) -o timcalc $^ $(LIBS)

run: all
	./timcalc 72 4

clean:
	rm -f *.o timcalc

.PHONY: all run clean
