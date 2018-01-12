make: main.c
	gcc -march=native -O2 -fopenmp -o main main.c;
clean: main
	rm main
	
