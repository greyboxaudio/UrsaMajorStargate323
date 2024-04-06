#import all the things
import datetime
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

#print current time for benchmarking purposes
tStart = datetime.datetime.now()
print(tStart.time(),"program start")

#modifiable parameters
csvInput1 = "Output_Program8_Delay_noRNG.csv"
csvInput2 = "Output_Program8_Gain_noRNG.csv"
yticks2 = [-1.0, -0.75, -0.5, -0.25, 0, 0.25, 0.5, 0.75, 1.0]
xticks1 = 8191
xticks2 = 8191

#read .csv to a new dataframe
df = pd.read_csv(csvInput1)
for x in range(1,16,1):
    a = df.iloc[:,[x]].to_numpy()
    yMid = np.min(a) + ((np.max(a)-np.min(a))*0.5)
    yMin = yMid - 0.004
    yMax = yMid + 0.004
    y1 = np.min(a)
    y3 = np.min(a) + ((np.max(a)-np.min(a))*0.25)
    y2 = np.min(a) + ((np.max(a)-np.min(a))*0.75)
    y4 = np.max(a)
    yticks1 = [yMin, y1, y3, yMid, y2, y4, yMax]
    plt.figure().set_figwidth(24)
    plt.plot(df.Time,df.iloc[:,[x]])
    plt.xticks(df.Time[::xticks1])
    plt.yticks(yticks1)
    plt.grid()
    plt.savefig(("plots/delay_tap"+str(x)+".png"),dpi=600,transparent=True)
    print(datetime.datetime.now()-tStart,"saved plot delay "+str(x))
    plt.close()

df = pd.read_csv(csvInput2)
for x in range(1,16,1):
    plt.figure().set_figwidth(24)
    plt.plot(df.Time,df.iloc[:,[x]])
    plt.xticks(df.Time[::xticks2])
    plt.yticks(yticks2)
    plt.grid()
    plt.savefig(("plots/gain_tap"+str(x)+".png"),dpi=600,transparent=True)
    print(datetime.datetime.now()-tStart,"saved plot gain "+str(x))
    plt.close()

#print elapsed time for benchmarking purposes
print(datetime.datetime.now()-tStart,"program complete")