CC= gcc
CFLAGS = -g -Wall
DEPS = jsh.h
EXEC = jsh
O = jsh.o

build : $(O)
gcc -Wall -o jsh jsh.c -lreadline

run : build
	./$(EXEC)

clean :
	rm -rf $(EXEC) 

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $<

