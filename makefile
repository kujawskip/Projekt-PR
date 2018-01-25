make: main.c
	mpicc -march=native -O2 -o main main.c;
clean: main
	rm main
	
