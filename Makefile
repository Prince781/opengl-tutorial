EXECFILE=demos
CFLAGS=$(shell pkg-config --cflags gtk+-3.0 epoxy) -Wall
LDFLAGS=$(shell pkg-config --libs gtk+-3.0 epoxy) -lm

$(EXECFILE):main.c gl_program.c util.c math_3d.c $(wildcard progs/*.c)
	$(CC) $(CFLAGS) -ggdb3 $^ $(LDFLAGS) -o $(EXECFILE)

clean:
	rm $(EXECFILE)
