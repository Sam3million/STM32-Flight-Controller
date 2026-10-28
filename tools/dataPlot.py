import pandas as pd, matplotlib.pyplot as plt

df = pd.read_csv("dt_us-recorded.csv")
#df = pd.read_csv("displayUpdateTimeUs-recorded.csv")
ms = df.dt_us / 1000
#ms = df.imu_us / 1000
#ms = df.displayUpdateTimeUs / 1000

fig, (a, b) = plt.subplots(1, 2, figsize=(12, 4))
#fig, a = plt.subplots(1, 1, figsize=(7, 4))
a.plot(ms, lw=0.6);  a.set_xlabel("iteration");  a.set_ylabel("loop period (ms)")
#a.plot(ms, lw=0.6); a.set_xlabel("iteration"); a.set_ylabel("imu read time (ms)")
b.hist(ms, bins=60, log=True);  b.set_xlabel("loop period (ms)");  b.set_ylabel("count (log)")

a.set_ylim(0, 0.6)

plt.tight_layout(); 
plt.savefig("my_plot.svg", format="svg", bbox_inches="tight")
plt.show()
print((df.dt_us / 1000).describe(percentiles=[0.5, 0.99, 0.999]))