#import all the things
import datetime
import pandas as pd
import matplotlib.pyplot as plt

#print current time for benchmarking purposes
tStart = datetime.datetime.now()
print(tStart.time(),"program start")

#modifiable parameters
csvInput = "Output_Program8_Gain_noRNG.csv"
xticks = 400
yticks = [-1.0, -0.75, -0.5, -0.25, 0, 0.25, 0.5, 0.75, 1.0]

#read .csv to a new dataframe
df = pd.read_csv(csvInput)
for x in range(1,16,1):
    plt.figure().set_figwidth(24)
    plt.plot(df.Time,df.iloc[:,[x]])
    plt.xticks(df.Time[::xticks])
    plt.yticks(yticks)
    #plt.grid()
    plt.savefig(("plots/tap"+str(x)+".png"),dpi=1200,transparent=True)
    print(datetime.datetime.now()-tStart,"saved plot"+str(x))

#print elapsed time for benchmarking purposes
print(datetime.datetime.now()-tStart,"program complete")