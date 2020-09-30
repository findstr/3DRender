.PHONY:target

uname := $(shell sh -c 'uname -s 2>/dev/null || echo not')

ifeq ($(uname),Linux)
CC := g++10
INCLUDE :=
target:linux
else
ifeq ($(uname),Darwin)
CC := g++
INCLUDE = -I/usr/local/Cellar/opencv/4.4.0_1/include/opencv4
marget:macosx
endif
endif

PLATS=linux macosx
platform:
	@echo "'make PLATFORM' where PLATFORM is one of these:"
	@echo "$(PLATS)"

DEBUG := -fsanitize=address -fno-omit-frame-pointer

CCFLAG := -O3 -g3 -std=c++17 -Wall \
	-march=native \
	-D_GLIBCXX_USE_CXX11_ABI=0\

LDFLAG :=-lpthread -lopencv_core -lopencv_highgui \
	-lopencv_imgproc -lopencv_imgcodecs -march=native

INCLUDE+=-Icommon/ -Iraytracer/


linux:CCFLAG += -fopenmp
linux:LDFLAG += -L/usr/local/gcc10/lib64 -lgomp
macosx:CCFLAG += -Xpreprocessor -fopenmp
macosx:LDFLAG += -lomp

SRC=\
	common/primitive.cpp \
	common/auxiliary.cpp \
	common/scene.cpp \
	common/camera.cpp \
	common/material.cpp \
	common/raytracer.cpp \


RST=	rasterizer/main.cpp \
	rasterizer/rasterizer.cpp \
    	rasterizer/camera.cpp

EXAMPLE=examples/main.cpp

OBJS = $(patsubst %.cpp,%.o,$(SRC))
OBJS += $(patsubst %.cpp,%.o,$(EXAMPLE))

.depend:
	@$(CC) $(CCFLAG) $(INCLUDE) -MM $(EXAMPLE) 2>/dev/null |\
		sed 's/\([^.]*\).o[: ]/examples\/\1.o $@: /g' > $@ || true
	@$(CC) $(CCFLAG) $(INCLUDE) -MM $(SRC) 2>/dev/null |\
		sed 's/\([^.]*\).o[: ]/common\/\1.o $@: /g' >> $@ || true
%.o:%.cpp
	$(CC) $(CCFLAG) $(INCLUDE) -c -o $@ $<

-include .depend

macosx linux:all

all:main

main:$(OBJS)
	$(CC) $(CCFLAG) $(INCLUDE) $(DEBUG) -o $@ $^ $(LDFLAG)

clean:
	-rm main
	-rm $(OBJS)
	-rm .depend

