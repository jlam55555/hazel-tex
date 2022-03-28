#!/bin/sh

# http://www.bibtex.org/Using/

MAIN="main"
BUILD="./build.sh"

$BUILD
bibtex $MAIN
$BUILD
$BUILD
