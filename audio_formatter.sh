#!/bin/bash
#
# Gives a bunch of stereo WAV audio files the proper format for the game:
# single channel, middle quality OGG.
#
#  Created on: July 31, 2012
#     Company: CordobaZombie
#      Author: Budde, Carlos Esteban.
#

# Auxiliary functions
sox_not_found()
{
	echo "No se encuentra el programa \'sox\' necesario para la conversion."
	exit 1
}
oggenc_not_found()
{
	echo "No se encuentra el programa \'oggenc\' necesario para la conversion."
	exit 1
}
format_error()
{
	echo "Error al formatear un archivo. Abortando."
	exit 1
}


# Tolerate filenames with spaces
IFS_bckp=IFS
IFS=$(echo -en "\n\b")

# Check audio conversion commands exist
type -P sox    &> /dev/null || sox_not_found
type -P oggenc &> /dev/null || oggenc_not_found

# Convert files
for file in *.[wW][aA][vV];
do
	mv $file "$file.tmp"
	sox "$file.tmp" $file remix 1,2 || format_error  # Make mono
	oggenc -C 1 -q 6 $file || format_error           # Encode in ogg
	rm $file && mv "$file.tmp" $file
done

echo -e "\nTodas las conversiones finalizaron con éxito."
IFS=IFS_bckp;

exit 0

