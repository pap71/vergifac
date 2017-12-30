#!/bin/sh

ARCH=`uname -m`  
if [ $ARCH = "x86_64" ]
then
prog="vergifac64"
else
prog="vergifac32"
fi

echo $PWD
DIRT=$PWD

cd $DIRT

echo $DIRT/$prog

./$prog
