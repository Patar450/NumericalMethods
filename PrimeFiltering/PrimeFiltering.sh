#!/bin/bash

gcc PrimeFiltering.c -pthread -o PrimeFiltering;
./PrimeFiltering 50 PrimeData1.txt PrimeData2.txt PrimeData3.txt; 
rm PrimeFiltering