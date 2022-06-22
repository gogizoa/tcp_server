#!/bin/bash

echo "OS Final Project Team 4 Test"

python3 init_input.py
gcc scheduler.c -o scheduler

./scheduler
python3 gantt_chart.py
eog result_gantt.png
