import pandas as pd
import matplotlib.pyplot as plt

# Load the data
cwnd_data = pd.read_csv('NewReno0.cwnd', sep='\t', header=None, names=['Time', 'OldCwnd', 'NewCwnd'])


# Plotting
plt.figure(figsize=(12, 6))
plt.plot(cwnd_data['Time'], cwnd_data['NewCwnd'], label='Congestion Window (cwnd)', color='blue')
plt.title('Congestion Window vs. Time')
plt.xlabel('Time (seconds)')
plt.ylabel('Congestion Window Size (cwnd)')
plt.grid()
plt.legend()
plt.xlim(0, cwnd_data['Time'].max())
plt.ylim(0, cwnd_data['NewCwnd'].max() + 10)  # Add some padding to the upper limit
plt.show()

