import random
import numpy as np
import matplotlib.pyplot as plt

f = open("process.txt", 'w')

print("Init Input Process")

process_num = int( input("Input number of Process : ") )# 프로세스


burst_time = np.random.poisson(5, process_num) # Burst Time
count, bins, ignored = plt.hist(burst_time, 14, density=True)
plt.savefig("input_diagram.png")

arrive_time = np.random.poisson(5, process_num) # Burst Time

for i in range(process_num):
    inp = str(arrive_time[i]) + " " + str(burst_time[i])
    if i is not process_num-1:
        inp += "\n"
    f.write(inp)
