CC= gcc
CFLAGS = -g -Wall
DEPS = commandes.h
EXEC = jsh

build : ${EXEC} 

run : build
	./$(EXEC)

clean :
	rm -rf $(EXEC) 

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $<

jsh : jsh.o commandes.o -lreadline
	$(CC) $(CFLAGS) -o $@ $^

