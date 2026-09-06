CC      ?= cc
CFLAGS  ?= -std=c99 -Wall -Wextra -O2 -fPIC -Iinclude
LDLIBS  := -lm

SRC := 	src/math.c 		\
		src/window.c 	\
		src/taps.c 		\
		src/file.c 		\
		src/fir.c

OBJ := $(SRC:.c=.o)

all: librflib.so librflib.a

librflib.so: $(OBJ)
	$(CC) -shared -o $@ $(OBJ) $(LDLIBS)

librflib.a: $(OBJ)
	ar rcs $@ $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -DRFLIB_BUILDING -DRFLIB_SHARED -c -o $@ $<

clean:
	rm -f $(OBJ) librflib.so librflib.a

.PHONY: all clean
