import pandas as pd
import matplotlib.pyplot as plt

# File paths
file_harsh_rtt = 'Harsh0_rtt.txt'  # Replace with actual path to Harsh RTT file
file_newreno_rtt = 'NewReno0_rtt.txt'  # Replace with actual path to NewReno RTT file

# Function to parse RTT data
def parse_rtt_data(file_path):
    data = pd.read_csv(file_path, sep='\t', header=None, names=['time', 'prev_rtt', 'curr_rtt'])
    data = data.astype(float)
    return data

# Parse both RTT datasets
hystart_rtt_df = parse_rtt_data(file_harsh_rtt)
newreno_rtt_df = parse_rtt_data(file_newreno_rtt)

# Plotting RTT changes over time for both algorithms
plt.figure(figsize=(12, 6))
plt.plot(hystart_rtt_df['time'], hystart_rtt_df['curr_rtt'], label="TCP Harsh RTT", color='blue')
plt.plot(newreno_rtt_df['time'], newreno_rtt_df['curr_rtt'], label="TCP NewReno RTT", color='orange')
plt.xlabel("Time (s)")
plt.ylabel("RTT (s)")
plt.title("RTT Changes over Time for TCP Harsh vs TCP NewReno")
plt.legend()
plt.grid(True)
plt.show()

