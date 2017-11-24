.PHONY:all

all:main

INCLUDE=-Idriver/ -Imath/

main:main.c driver/graphic.c math/math.c
	gcc $(INCLUDE) -g -m32 -o main $^ -lopengl32 -lglu32 -lgdi32 -lglut32
