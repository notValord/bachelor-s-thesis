#!/bin/bash

if [ $1 == "--build" ]
then
	mkdir -p build
	cmake -S . -B build
	cd build
	make
	cd ..
else
	cd build
	./bakalarka -t min_det
	cd ..
fi