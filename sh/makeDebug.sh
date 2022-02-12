#!/bin/sh

./automake

cd ..
cd engine/Debug
rm -rf engine_build.log
rm -rf libengineD.a
make all -j6 2>&1 | tee engine_build.log
cp libengineD.a ../../lib/libengineD.a

cd ../..
cd crossover/Debug
rm -rf crossover_build.log
rm -rf crossoverD
make all -j6 2>&1 | tee crossover_build.log
cp crossoverD ../../bin/crossoverD

echo make Debug ok!
