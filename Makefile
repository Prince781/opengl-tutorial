EXECFILE=demos
CFLAGS=$(shell pkg-config --cflags gtk4 epoxy) -Wall
LDFLAGS=$(shell pkg-config --libs gtk4 epoxy) -lm

$(EXECFILE):main.c gl_program.c util.c math_3d.c $(wildcard progs/*.c)
	$(CC) $(CFLAGS) -ggdb3 $^ $(LDFLAGS) -o $(EXECFILE)

clean:
	rm $(EXECFILE)
