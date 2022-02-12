#!/bin/sh

./automake

cd ..
cd engine/Release
rm -rf engine_build.log
rm -rf libengine.a
make all -j6 2>&1 | tee engine_build.log
cp libengine.a ../../lib/libengine.a

cd ../..
cd crossover/Release
rm -rf crossover_build.log
rm -rf crossover
make all -j6 2>&1 | tee crossover_build.log
cp crossover ../../bin/crossover

echo make Release ok!
