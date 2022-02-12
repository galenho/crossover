#!/bin/sh

cd ..
cd engine/Release
make clean

cd ../..
cd crossover/Release
make clean

cd ../../bin
rm crossover
cd ../lib
rm libengine.a