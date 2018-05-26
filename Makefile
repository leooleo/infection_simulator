all: 
	clear
	gcc simulator.c -o a -lpthread
	./a
run:
	./a