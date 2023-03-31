#!/bin/bash

#convert image from jpeg to pgm(P5 binary)
#convert from PGM (P5) to PGM (P2)
#cp converted file from <filename> to 01.pgm

touch 01.pgm 1.pgm 2.pgm 3.pgm 4.pgm 5.pgm 
make main1.cpp
./main1

