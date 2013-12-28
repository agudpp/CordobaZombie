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

echo "Compiling tinyxml"
g++ -c *.cpp 

echo "Making static library (.a)"
ar rvs tinyxml.a *.o
rm *.o

echo "Moving the library into the corresponding project path"
mv tinyxml.a $CZ01_THIRD_PARTY/lib

echo "Copying header files into the corresponding project path"
mkdir -p $CZ01_THIRD_PARTY/include/tinyxml
cp *.h $CZ01_THIRD_PARTY/include/tinyxml/

echo "Done, everything ready."
exit 0

