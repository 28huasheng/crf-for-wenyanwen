#!/bin/sh
../../crf_learn.exe -c 10.0 template train.data model
../../crf_test.exe  -m model test.data

../../crf_learn.exe -a MIRA template train.data model
../../crf_test.exe  -m model test.data
read -n 1