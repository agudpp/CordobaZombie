#!/bin/sh
RESULT="0"
for i in `find . -name *.h` 
do echo "archivo: $i"
	for j in `cat $i | wc -l`
	do echo "sumando $j"
		RESULT=`expr $j "+" $RESULT`
	done
done
for i in `find . -name *.cpp` 
do echo "archivo: $i"
	for j in `cat $i | wc -l`
	do echo "sumando $j"
		RESULT=`expr $j "+" $RESULT`
	done
done

echo "La cantidad de lineas en *.{h,c} es: $RESULT"
