#!/bin/bash
function gen() {
	./main scene/$1.scene output/$1.ppm
	ppmtojpeg output/$1.ppm > output/$1.jpg
}

make
export SPP=1024
gen "ray-whitted"
gen "path-cornellbox"
gen "path-sphere"
gen "path-glass"
gen "path-bunny"


