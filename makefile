CC = gcc
FLAGS = -Wall -g

run: clean myShell
	./myShell

leak:
	valgrind --leak-check=full ./myShell

myShell: myShell.o myFunction.o
	$(CC) $(FLAGS) -o myShell myShell.o myFunction.o

myShell.o: myShell.c myShell.h
	$(CC) $(FLAGS) -c myShell.c

myFunction.o: myFunction.c myFunction.h
	$(CC) $(FLAGS) -c myFunction.c

clean:
	rm -f *.o *.out myShell
