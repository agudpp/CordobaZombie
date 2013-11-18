#!/bin/bash

###############################################################################
## Global variables
##
DEV_ROOT_PATH="$CZ01_DEV_PATH"
CLI_TOOLS_PATH="$DEV_ROOT_PATH/tools/cli_tools"
TOCOMPILE_PATH="$DEV_ROOT_PATH/../build"
TOCOMPILE_FILE_PATH="$DEV_ROOT_PATH/../build/ToCompile.cmake"
OUTPUT_FOLDER="$DEV_ROOT_PATH/../build/debug"           # where we will compile the tools

###############################################################################
## General common methods
##

## Define the colors
black='\E[30;47m'
red='\E[31;47m'
green='\E[32;47m'
yellow='\E[33;47m'
blue='\E[34;47m'
magenta='\E[35;47m'
cyan='\E[36;47m'
white='\E[37;47m'

alias ResetColor="tput sgr0"        #  Reset text attributes to normal
                                    #+ without clearing screen.

## Different print methods
printNormal()
{
    echo "$1"
    return
}
printError()
{
#    echo -e "$red"
    echo "\033[33;31m"
    echo "$1"
    ResetColor
    return
}
printFine()
{
#    echo -e "$green" 
    echo "\033[33;32m"
    echo "$1"
    ResetColor
}


###############################################################################
# Print the help and previous method and wait for the user input
printMainMessage()
{
    clear
    echo ""
    echo "---------------------------------------------------------------------"
    echo "This script is used to compile all the command line tools that we currently"
    echo "have and will put them in a specific folder $OUTPUT_FOLDER".
    echo "We will modify the ToCompile.cmake file (but we will create a backup"
    echo "of it named ToCompile.cmake.back"
    echo "We will iterate over all the cli_tools and compile one by one and put"
    echo "all of them in the output folder. We will also create a .bashrc file"
    echo "with all the aliases to be used for all these tools".
    echo "---------------------------------------------------------------------"
    echo ""
    echo "Hit any key to continue, or cancel this script now"
    read keypress
}

###############################################################################
## Compile a tool having already set the
## #param1      The path to the autogen.cmake we want to compile
## #param2      The name of the tool
##
compileTool()
{
    printFine "Setting the path $1 to the ToCompile.cmake"
    printFine "The name of the tool will be $2"
    echo "include($1)" > $TOCOMPILE_FILE_PATH
    cd $OUTPUT_FOLDER
    printFine "Executing cmake .. && make -j 4"
    cmake ..
    if [ $? -ne 0 ]
    then
        printError "Error doing the cmake .."
        return 1
    fi
    # make
    make -j 4
    if [ $? -ne 0 ]
    then
        printError "Error compiling tool $2"
        return 1
    else
        printFine "Compiled successfully, renaming the tool to $2"
        mv OgreApp $2
    fi
    
    return 0        
}

###############################################################################
## Check that we have everything we need
###############################################################################

printMainMessage


## Check environment variable sets
printNormal "Checking for the CZ01_DEV_PATH environment variable"
if test -z $DEV_ROOT_PATH
then
    printError "Environment variable CZ01_DEV_PATH not found"
    exit 1
else
    printFine "Environment variable CZ01_DEV_PATH found and is pointing to $DEV_ROOT_PATH"
fi

# check if we have the cli folder path we want
printNormal "Checking for the cli_tools path"
if test -d "$CLI_TOOLS_PATH"
then
    printFine "Path $CLI_TOOLS_PATH found"
else
    printError "Path $CLI_TOOLS_PATH not found! Check that we are in a branch"
    printError "containning the command line tools"
    exit 1
fi

# now check the to compile path
printNormal "Checking ToCompile.cmake file path"
if test -d "$TOCOMPILE_PATH"
then
    printFine "Path $TOCOMPILE_PATH exists"
else
    printError "Path $TOCOMPILE_PATH not found! Something is wrong...."
    exit 1
fi

# create the output folder or just go there
printNormal "Going to $OUTPUT_FOLDER folder"
if test -d "$OUTPUT_FOLDER"
then
    cd $OUTPUT_FOLDER
else
    printNormal "Creating folder $OUTPUT_FOLDER"
    mkdir $OUTPUT_FOLDER
    cd $OUTPUT_FOLDER
fi

# create the ToCompile.cmake, and create the copy of it
if test -e "$TOCOMPILE_FILE_PATH"
then
    printFine "Backuping the file $TOCOMPILE_FILE_PATH"
    cp $TOCOMPILE_FILE_PATH "$TOCOMPILE_FILE_PATH.backup"
fi

appName=""
autoGenPath=""
for tool in `ls -l $CLI_TOOLS_PATH | grep ^d | awk '{print $9}'`
do
    # we need to analize each tool here
    appName=`basename $tool`
    autoGenPath="$CLI_TOOLS_PATH/$tool/AutoGen.cmake"
    printNormal "Processing tool $appName"
    printNormal "with autogen $autoGenPath"
    if test -e "$autoGenPath"
    then
        printFine "Trying to compile $autoGenPath"
        compileTool "$autoGenPath" $appName
    else
        printError "We couldn't find the $autoGenPath file for $appName"
    fi

done


# Done
printNormal "We finish here... Restoring the ToCompile.cmake file"
cp "$TOCOMPILE_FILE_PATH.backup" $TOCOMPILE_FILE_PATH
 

# try to get the src folder directory first
#for dir in `ls -l | grep ^d | awk '{print $9}'`
#do
#    echo "folder: " $dir " is the one we want" > t1.txt
#done

