all: 
	clear
	gcc simulator.c -o a -lpthread
	./a
comp:
	clear
	gcc simulator.c -o a -lpthread
run:
	./a