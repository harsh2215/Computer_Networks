import matplotlib.pyplot as plt
import pandas as pd

# Load data from the RTT log file
data = pd.read_csv("NewReno0_rtt.txt", sep="\t", header=None, names=["Time", "Old RTT", "New RTT"])

# Plot the RTT vs. Time graph
plt.figure(figsize=(10, 6))
plt.plot(data["Time"], data["New RTT"], label="RTT", color="blue", linewidth=1)

# Labeling the plot
plt.xlabel("Time (s)")
plt.ylabel("RTT (s)")
plt.title("RTT vs. Time")
plt.legend()
plt.grid(True)
plt.show()

