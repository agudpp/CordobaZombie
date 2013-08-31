#!/bin/sh

echo "Compiling Box2D libraries"
g++ -c ./Collision/*.cpp ./Collision/Shapes/*.cpp ./Common/*.cpp -I../ -I./Common -I./Collision -I./Collision/Shapes
echo "Done"

echo "Making static library .a"
ar rvs Box2D.a *.o
echo "Done..."

echo "Removing .o"
rm *.o
echo "Done"

echo "Copying the library into the third party library: " $CZ01_THIRD_PARTY
cp Box2D.a $CZ01_THIRD_PARTY/lib

echo "Removing the .a local file"
rm Box2D.a

echo "Creating the folders and copying the headers files there (include folder)"
mkdir $CZ01_THIRD_PARTY/include/Box2D
mkdir $CZ01_THIRD_PARTY/include/Box2D/Collision
mkdir $CZ01_THIRD_PARTY/include/Box2D/Collision/Shapes
mkdir $CZ01_THIRD_PARTY/include/Box2D/Common

cp ./*.h $CZ01_THIRD_PARTY/include/Box2D/
cp ./Collision/*.h $CZ01_THIRD_PARTY/include/Box2D/Collision/
cp ./Collision/Shapes/*.h $CZ01_THIRD_PARTY/include/Box2D/Collision/Shapes/
cp ./Common/*.h $CZ01_THIRD_PARTY/include/Box2D/Common/

echo "Done. Everything is ready"
