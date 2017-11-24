.PHONY:all

all:main

INCLUDE=-Idriver/ -Imath/ -Iengine/

SRC=\
    sample/main.c\
    driver/graphic.c\
    math/mathlib.c\
    math/mathconst.c\
    engine/plg.c\

main:$(SRC)
	gcc $(INCLUDE) -g -m32 -o $@ $^ -lopengl32 -lglu32 -lgdi32 -lglut32
