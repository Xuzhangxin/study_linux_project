
all:
	gcc -c file.c -o file.o
	ar -rcs libfile.a file.o
	gcc main.c -o main -L ./ -lfile