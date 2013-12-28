#!/bin/sh

echo "Checking for required environment variables"
if [ -z ${CZ01_THIRD_PARTY+x} ];
then
	echo "ERROR: environment variable CZ01_THIRD_PARTY is missing";
	exit 1
elif [ ! -d $CZ01_THIRD_PARTY/include ] || [ ! -d $CZ01_THIRD_PARTY/lib ];
then
	echo "ERROR: environment variable CZ01_THIRD_PARTY must point to a path";
	echo "containing directories \"include\" and \"lib\"";
	exit 1
fi

echo "Compiling Box2D"
g++ -c ./Collision/*.cpp ./Collision/Shapes/*.cpp ./Common/*.cpp -I../ -I./Common -I./Collision -I./Collision/Shapes

echo "Making static library (.a)"
ar rvs Box2D.a *.o
rm *.o

echo "Moving the library into the corresponding project path"
mv Box2D.a $CZ01_THIRD_PARTY/lib

echo "Copying header files into the corresponding project path"
mkdir -p $CZ01_THIRD_PARTY/include/Box2D/Collision/Shapes
mkdir $CZ01_THIRD_PARTY/include/Box2D/Common

cp ./*.h $CZ01_THIRD_PARTY/include/Box2D/
cp ./Collision/*.h $CZ01_THIRD_PARTY/include/Box2D/Collision/
cp ./Collision/Shapes/*.h $CZ01_THIRD_PARTY/include/Box2D/Collision/Shapes/
cp ./Common/*.h $CZ01_THIRD_PARTY/include/Box2D/Common/

echo "Done, everything ready."
exit 0

