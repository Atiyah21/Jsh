CC= gcc
CFLAGS = -g -Wall
DEPS = commandes.h redirection.h affichage.h
EXEC = jsh

build : ${EXEC} 

run : build
	./$(EXEC)

clean :
	rm -rf $(EXEC) *.o

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $<

jsh : commandes.o redirection.o affichage.o jsh.o -lreadline
	$(CC) $(CFLAGS) -o $@ $^
