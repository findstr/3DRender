.PHONY:macosx win all target

uname := $(shell sh -c 'uname -s 2>/dev/null || echo not')

CC := gcc -std=gnu99

ifeq ($(uname),Darwin)
target:macosx
else
target:win
endif

INCLUDE=-Idriver/ -Imath/ -Iengine/
CFLAG=-g -m32

macosx:CFLAG+=-framework OpenGL -framework GLUT -Wno-deprecated-declarations

win:CFLAG+=-lopengl32 -lglu32 -lgdi32 -lglut32

macosx win:all

all:main

SRC=\
    sample/main.c\
    driver/graphic.c\
    math/mathlib.c\
    math/mathconst.c\
    engine/plg.c\
    engine/transform.c\
    engine/camera.c\

main:$(SRC)
	gcc $(INCLUDE) $(CFLAG) -o $@ $^
