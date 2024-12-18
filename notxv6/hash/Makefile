# makes hashprog and hashprogDebug
# $ make
# $ make all
# $ make hashprog
# $ make hashprogDebug
# $ make clean
all: hashprog hashprogDebug

FLAGS = -Wall
hashprog: main.c hash.h hash.c
	@echo 'building hashprog.'
	gcc -o hashprog main.c hash.c $(FLAGS)

hashprogDebug: main.c hash.h hash.c
	@echo 'building hashprogDebug.'
	gcc -o hashprogDebug -DDEBUG main.c hash.c $(FLAGS)

clean:
	rm -f hashprog hashprogDebug *.o

