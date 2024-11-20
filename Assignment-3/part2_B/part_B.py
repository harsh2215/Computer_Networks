import subprocess

import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

rng_runs = range(1, 11, 1)
error_rates = [0.2,0.4,0.6,0.8,1.0]
errorUnit = "bit"

pdr_values = []

for rng_run in rng_runs:
    for error_rate in error_rates:
        command = f'./ns3 run "scratch/12241300/2_B.cc --RngRun={rng_run} --errorRate={error_rate} --errorUnit={errorUnit}"'
        output = subprocess.check_output(command, shell=True).decode("utf-8")
        last_line = output.strip().split("\n")[-1]
        pdr = float(last_line.split(": ")[-1])
        pdr_round = round(pdr, 3)
        pdr_values.append((rng_run, error_rate, pdr_round))

# Convert pdr_values to a pandas DataFrame
df = pd.DataFrame(pdr_values, columns=["RngRun", "ErrorRate", "PDR"])

# Pivot the DataFrame to create the desired matrix-like output
df_pivot = df.pivot(index="RngRun", columns="ErrorRate", values="PDR")

# Print the pivoted DataFrame
print(df_pivot)

# Get the mean of each error rate
mean_pdr = df_pivot.mean().values


# Create graph
fig, ax = plt.subplots(figsize=(10, 6))
ax.axis('tight')
ax.axis('off')

# Create table
table = ax.table(cellText=df_pivot.values,
                 colLabels=df_pivot.columns,
                 rowLabels=df_pivot.index,
                 cellLoc='center',
                 loc='center')

# Adjust layout to fit the table within the figure
plt.title('PDR Matrix')
plt.tight_layout()

# Show the plot
plt.show()

# Generate x-axis values with a step size of 0.2
x_ticks = np.arange(0.2, 1.2, 0.2)

plt.figure(figsize=(8, 6))
plt.plot(error_rates, mean_pdr, marker='o', color='blue')
plt.xlabel('Error Rate')
plt.ylabel('Average PDR')
plt.title('PDR vs Error Rate')
plt.xticks(x_ticks)  # Set the x-axis ticks
plt.tight_layout()
plt.show()
