#!/bin/sh
swig -python -l mssim.i
g++ -g -fPIC -c mssim.cpp mssim_wrap.c -I/home/amink/anaconda3/include/python3.7m
g++ -g -shared mssim.o mssim_wrap.o -o _mssim.so -lstdc++ -lpython3.7m