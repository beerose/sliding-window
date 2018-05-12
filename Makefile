CC = g++
CFLAGS = -std=gnu++11 -lstdc++ -Wall -Wextra -Werror -O2 -I.
LFLAGS = -lm

DEPS = utils.h
OBJ = utils.o main.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

transport: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LFLAGS)

clean:
	rm $(OBJ)

distclean:
	rm transport $(OBJ)
