.PHONY:target

uname := $(shell sh -c 'uname -s 2>/dev/null || echo not')

ifeq ($(uname),Linux)
target:linux
else
ifeq ($(uname),Darwin)
target:macosx
endif
endif

PLATS=linux macosx
platform:
	@echo "'make PLATFORM' where PLATFORM is one of these:"
	@echo "$(PLATS)"

DEBUG := -fsanitize=address -fno-omit-frame-pointer

CCFLAG := -O3 -g3 -std=c++17 -D_GLIBCXX_USE_CXX11_ABI=0

LDFLAG :=-lpthread \
	-lopencv_calib3d \
	-lopencv_core -lopencv_features2d \
	-lopencv_flann -lopencv_highgui \
	-lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_stitching \
	-lopencv_superres -lopencv_video -lopencv_videostab \
	-lomp

INCLUDE=-Icommon/ -Iraytracer/

linux:CC=g++10
linux:CCFLAG += -fopenmp
linux:LDFLAG += -L/usr/local/gcc10/lib64
macosx:CC=g++
macosx:CCFLAG += -Xpreprocessor -fopenmp
macosx:INCLUDE += -I/usr/local/Cellar/opencv/4.4.0_1/include/opencv4

SRC= common/primitive.cpp common/auxiliary.cpp

RST=	rasterizer/main.cpp \
	rasterizer/rasterizer.cpp \
    	rasterizer/camera.cpp

RT=	raytracer/main.cpp \
	raytracer/scene.cpp \
	raytracer/raytracer.cpp

PT=	raytracer/main.cpp \
	raytracer/scene.cpp \
	raytracer/pathtracer.cpp

OBJS = $(patsubst %.cpp,%.o,$(SRC) $(PT))


.depend:
	@$(CC) $(INCLUDE) -MM $(SRC) 2>/dev/null |\
		sed 's/\([^.]*\).o[: ]/raytracer\/\1.o $@: /g' > $@ || true

-include .depend

%.o:%.cpp
	$(CC) $(CCFLAG) $(INCLUDE) -c -o $@ $<

macosx linux:all

all:main

main:$(OBJS)
	$(CC) $(CCFLAG) $(INCLUDE) $(DEBUG) -o $@ $^ $(LDFLAG)

clean:
	-rm main
	-rm $(OBJS)

