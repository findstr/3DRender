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
LDFLAG= -lm

macosx:CFLAG+=-framework OpenGL -framework GLUT -Wno-deprecated-declarations

win:LDFLAG+=-lopengl32 -lglu32 -lgdi32 -lglut32

macosx win:all

all:main

SRC=\
    sample/main.c\
    driver/driver.c\
    math/mathlib.c\
    math/mathconst.c\
    engine/plg.c\
    engine/transform.c\
    engine/camera.c\
    engine/engine.c\
    engine/device.c\
    engine/matrial.c\
    engine/light.c\
    engine/primitive.c\

main:$(SRC)
	gcc $(INCLUDE) $(CFLAG) -o $@ $^ $(LDFLAG)
