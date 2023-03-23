all: datatrim main 
	
main: Lab4_IO.o
	mpic++ main.cpp Lab4_IO.o -o main  

Lab4_IO.o: Lab4_IO.c Lab4_IO.h
	gcc -c -o Lab4_IO.o Lab4_IO.c

datatrim: datatrim.o
	gcc -o datatrim datatrim.o 

datatrim.o: datatrim.c 
	gcc -c -o datatrim.o datatrim.c

test: 
	gcc serialtester.c Lab4_IO.c -o serialtester -lm 
	./serialtester
clean: 
	rm *.o
	rm datatrim
	rm main