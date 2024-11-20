import pandas as pd
import matplotlib.pyplot as plt

# Function to read data from a file and return as a list of lines
def read_file(file_path):
    with open(file_path, 'r') as file:
        return file.readlines()

# Parse the data from the files
def parse_cwnd_data(data):
    # Split each line into time, previous cwnd, and current cwnd
    parsed_data = [line.strip().split('\t') for line in data]
    # Create a DataFrame with appropriate columns and convert to numeric
    df = pd.DataFrame(parsed_data, columns=['time', 'prev_cwnd', 'curr_cwnd']).astype(float)
    return df

# Read data from files
hystart_cwnd_data = read_file('Harsh0.cwnd')
newreno_cwnd_data = read_file('NewReno0.cwnd')

# Parse both datasets
hystart_cwnd_df = parse_cwnd_data(hystart_cwnd_data)
newreno_cwnd_df = parse_cwnd_data(newreno_cwnd_data)

# Plotting congestion window changes over time for both algorithms
plt.figure(figsize=(12, 6))
plt.plot(hystart_cwnd_df['time'], hystart_cwnd_df['curr_cwnd'], label="TCP Harsh", color='blue')
plt.plot(newreno_cwnd_df['time'], newreno_cwnd_df['curr_cwnd'], label="TCP NewReno", color='orange')
plt.xlabel("Time (s)")
plt.ylabel("Congestion Window (cwnd)")
plt.title("Congestion Window Changes over Time")
plt.legend()
plt.grid(True)
plt.show()


