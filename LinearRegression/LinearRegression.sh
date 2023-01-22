#!/bin/bash

gcc LinearRegression.c -o LinearRegression;./LinearRegression datasetLR1.txt datasetLR2.txt datasetLR3.txt datasetLR4.txt; rm LinearRegression;