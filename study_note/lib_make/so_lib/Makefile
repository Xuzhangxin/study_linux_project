
all:
	gcc -c file.c -o file.o
	gcc -shared -fpic -o libfile.so file.o
	gcc main.c -o main -L ./ -lfile

# 1)export LD_LIBRARY_PATH=/home/xzx/study_project/unix_prime_pro/lib_make/so_lib
# 2)放到/lib  或/usr/lib目录下