import pandas as pd
import matplotlib.pyplot as plt

# Read data from csv files
lin1 = pd.read_csv('lowcut_filt_data.csv',header=None)
line2 = pd.read_csv('test_data.csv',header=None)
lin3 = pd.read_csv('butter_filt_data.csv',header=None)
lin4 = pd.read_csv('cancel_data.csv',header=None)
# lin5 = pd.read_csv('rev.csv',header=None)

# Convert the data to lists
lin1 = lin1.values.tolist()[0]  # get the first row as list
line2 = line2.values.tolist()[0]
lin3 = lin3.values.tolist()[0]
lin4 = lin4.values.tolist()[0]
# lin5 = lin5.values.tolist()[0]

x = range(len(lin1))  # assuming all lines have the same length

plt.figure(figsize=(10,6))

# Plot the data
plt.plot(x, lin1, label='Line 1 - Full Filtterd +-80', color='blue')
plt.plot(x, line2, label='Line 2 - Test data', color='red')
plt.plot(x, lin3, label='Line 3 - Butter filtered', color='green')
plt.plot(x, lin4, label='Line 4 - Cancel data', color='purple')
# plt.plot(x, lin5, label='Line 5 - rev data', color='black')

# Add a legend
plt.legend()

# Show the plot
plt.show()