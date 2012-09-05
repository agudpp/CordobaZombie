#!/bin/sh
g++ -c ./Collision/*.cpp ./Collision/Shapes/*.cpp ./Common/*.cpp -I../ -I./Common -I./Collision -I./Collision/Shapes &&  ar rvs box2D.a *.o
echo "Done..."
echo "Removing .o"
rm *.o
echo "Done 2"

