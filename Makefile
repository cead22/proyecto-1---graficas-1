UNAME := $(shell uname -s)
CC=gcc
CFLAGS=-lGL -lglut -lGLU
ifeq ($(UNAME),Darwin)
	CFLAGS=-framework GLUT -framework OpenGL
endif
DEPS=loader.h nivel.h
OBJ=hellomake.o hellofunc.o 

breakout3D: loader.o breakout3D.o $(DEPS)
	$(CC) -o $@ $(CFLAGS) loader.o breakout3D.o

breakout3D.o: breakout3D.c $(DEPS)
	$(CC) -c breakout3D.c

loader.o: $(DEPS) loader.c
	$(CC) -c loader.c
