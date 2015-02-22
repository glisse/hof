SRC = hof.c
OBJ = $(SRC:.c=.o)
TARGET = libhof.a
CFLAGS = -std=c99 -Wall -Wstrict-prototypes

all: $(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)

%.a: $(OBJ)
	$(AR) rcs $@ $<

%.o: %.c hof.h
	$(CC) -c $(CFLAGS) $< -o $@
