#!/bin/bash

echo "OS Final Project Team 4 Test"

gcc scheduler.c -o scheduler

./scheduler
python3 gantt_chart.py
eog result_gantt.png
