.PHONY:all

all:main

INCLUDE=-Idriver/

main:main.c driver/graphic.c
	gcc $(INCLUDE) -g -m32 -o main $^ -lopengl32 -lglu32 -lgdi32 -lglut32
