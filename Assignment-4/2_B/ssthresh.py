import matplotlib.pyplot as plt

# Initialize lists for time and ssthreshold values
time = []
ssthreshold = []

# Path to the NS3-generated file
filename = "Harsh0-ssthresh.txt"  # Replace with the actual filename if different

# Try to open and read the file
try:
    with open(filename, 'r') as file:
        for line in file:
            # Skip empty lines
            if line.strip() == "":
                continue
            
            # Split each line by whitespace
            parts = line.strip().split()
            
            # Check if the line has at least two values
            if len(parts) >= 2:
                try:
                    # Convert parts to float and append to lists
                    time.append(float(parts[0].strip()))
                    ssthreshold.append(float(parts[2].strip()))  # ssthresh is the third value
                except ValueError as ve:
                    print(f"Skipping line due to value error: {line} - {ve}")
            else:
                print(f"Skipping malformed line: {line}")

except FileNotFoundError:
    print(f"File '{filename}' not found. Please check the path and filename.")

# Check if we have data to plot
if not time or not ssthreshold:
    print("No valid data to plot. Please check the file contents.")
else:
    # Plot ssthreshold vs time
    plt.figure(figsize=(10, 6))
    plt.plot(time, ssthreshold, label="ssthreshold", color='r', marker='o')

    # Label the graph
    plt.xlabel("Time (s)")
    plt.ylabel("ssthreshold")
    plt.title("ssthreshold vs Time for Newreno")
    plt.legend()
    plt.grid(True)

    # Show the plot
    plt.show()

