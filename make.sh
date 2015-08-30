#!/bin/sh

PROJECTS="./*";

for project in PROJECTS
do
	build_project();
done
echo "Done";

build_project(){
# main parameters:
APPNAME="$1"
FULLAPPNAME="$1"
MODULENAME="main"

# compiler/linker parameters:
CC=gcc
CFLAGS="-O2 -Wall "
LDFLAGS="-s -shared -Wl"

cmake ./CMakeLists.txt
echo "Compiling..."

OUTPUT=${PWD}/${APPNAME}"_BUILD"

mkdir -p ${OUTPUT}

$CC $CFLAGS -c $MODULENAME.c -o $MODULENAME.o
$CC $LDFLAGS -o ${OUTPUT}/main.so $MODULENAME.o

rm ./*.o
echo "Application files:"
ls -l ${OUTPUT}/*

};
