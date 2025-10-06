all: parent child1 child2

parent: parent.c
	gcc -o parent parent.c

child1: child1.c
	gcc -o child1 child1.c

child2: child2.c
	gcc -o child2 child2.c

clean:
	rm -f parent child1 child2

.PHONY: all clean