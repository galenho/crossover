#!/bin/sh

cd ..
cd engine/Debug
make clean

cd ../..
cd crossover/Debug
make clean

cd ../../bin
rm crossoverD
cd ../lib
rm libengineD.a