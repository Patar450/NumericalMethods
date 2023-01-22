#!/bin/bash

gcc LeibnizFormaula.c -pthread -lm -o LeibnizFormaula; ./LeibnizFormaula 9999 4000; rm LeibnizFormaula