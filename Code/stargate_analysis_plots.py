#import all the things
import datetime
import pandas as pd
import matplotlib.pyplot as plt

#print current time for benchmarking purposes
tStart = datetime.datetime.now()
print(tStart.time(),"program start")

#modifiable parameters
csvInput1 = "Output_Program8_Delay_noRNG.csv"
csvInput2 = "Output_Program8_Gain_noRNG.csv"
yticks1 = [0.00, 0.02, 0.04, 0.06, 0.08, 0.10, 0.12, 0.14, 0.16, 0.18, 0.20, 0.22, 0.24, 0.26]
yticks2 = [-1.0, -0.75, -0.5, -0.25, 0, 0.25, 0.5, 0.75, 1.0]
xticks1 = 400
xticks2 = 400

#read .csv to a new dataframe
df = pd.read_csv(csvInput1)
for x in range(1,16,1):
    plt.figure().set_figwidth(24)
    plt.plot(df.Time,df.iloc[:,[x]])
    #plt.xticks(df.Time[::xticks1])
    #plt.yticks(yticks1)
    #plt.grid()
    plt.savefig(("plots/delay_tap"+str(x)+".png"),dpi=300,transparent=True)
    print(datetime.datetime.now()-tStart,"saved plot delay "+str(x))

df = pd.read_csv(csvInput2)
for x in range(1,16,1):
    plt.figure().set_figwidth(24)
    plt.plot(df.Time,df.iloc[:,[x]])
    #plt.xticks(df.Time[::xticks2])
    #plt.yticks(yticks2)
    #plt.grid()
    plt.savefig(("plots/gain_tap"+str(x)+".png"),dpi=300,transparent=True)
    print(datetime.datetime.now()-tStart,"saved plot gain "+str(x))

#print elapsed time for benchmarking purposes
print(datetime.datetime.now()-tStart,"program complete")