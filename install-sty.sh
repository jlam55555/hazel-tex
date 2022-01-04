#!/bin/sh

# see: https://tex.stackexchange.com/a/124001
LATEX_STY_DIR=~/texmf/tex/latex/mystyles/

mkdir -p $LATEX_STY_DIR
cp latex-packages/* $LATEX_STY_DIR
