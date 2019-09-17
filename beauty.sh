#!/bin/sh


WHITE=" -e \E[1;37m"
GRAY_LIGHT=" -e \E[0;37m"
GRAY_DARK=" -e \E[1;30m"
BLUE=" -e \E[0;34m"
BLUE_LIGHT=" -e \E[1;34m"
GREEN=" -e \E[0;32m"
GREEN_LIGHT=" -e \E[1;32m"
CYAN=" -e \E[0;36m"
CYAN_LIGHT=" -e \E[1;36m"
RED=" -e \E[0;31m"
RED_LIGHT=" -e \E[1;31m"
PURPLE=" -e \E[0;35m"
PURPLE_LIGHT=" -e \E[1;35m"
BROWN=" -e \E[0;33m"
YELLOW=" -e \E[1;33m"
BLACK=" -e \E[0;30m"
REPLACE=" -e \E[0m"


PATH=$(pwd);

if [ "$1" != "" ]
then 
	PATH=$1
fi
echo $(echo $GREEN) " Beautify directory :" $(echo $PURPLE) " $PATH " $(echo $REPLACE)
/usr/bin/astyle -n --quiet --style=ansi $PATH/*.cpp
/usr/bin/astyle -n --quiet --style=ansi $PATH/*.h
echo $(echo $YELLOW) " Beautify done ... " $(echo $REPLACE)
