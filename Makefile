CC= gcc
CFLAGS = -g -Wall
DEPS = commandes.h redirection.h 
EXEC = jsh

build : ${EXEC} 

run : build
	./$(EXEC)

clean :
	rm -rf $(EXEC) 

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $<

jsh : commandes.o redirection.o jsh.o -lreadline
	$(CC) $(CFLAGS) -o $@ $^

