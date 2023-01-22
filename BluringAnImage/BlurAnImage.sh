#!/bin/bash

gcc BlurAnImage.c -lm lodepng.c -lpthread -o BlurAnImage;
./BlurAnImage 400 selfie.png; 
rm BlurAnImage
