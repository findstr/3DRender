.PHONY:all

all:main

main:main.c
	gcc -m32 -o main main.c -lopengl32 -lglu32 -lgdi32 -lglut32
