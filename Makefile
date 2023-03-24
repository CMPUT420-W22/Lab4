all: datatrim main 
	
main: main.c
	mpicc -o main main.c Lab4_IO.h Lab4_IO.c -lm 

datatrim: datatrim.o
	gcc -o datatrim datatrim.o 

datatrim.o: datatrim.c 
	gcc -c -o datatrim.o datatrim.c

test: 
	gcc serialtester.c Lab4_IO.c -o serialtester -lm 
	./serialtester
clean: 
	rm main
	rm *.o
	rm datatrim