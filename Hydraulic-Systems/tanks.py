import sympy as sp

# Define symbolic variables
s, t, r, g = sp.symbols('s t r g')

# Parameters
r_val = 998
g_val = 9.81

# Unit step function f1(t)
f1 = 10 * (sp.Heaviside(t - 1) - sp.Heaviside(t - 10))

# Laplace transform of f1(t) (unit step function at t = 1)
F1 = sp.laplace_transform(f1, t, s)[0]

# Define P1, P2, P3 in the s-domain
P1 = (10*(12*s + 5)*F1 + 30*F1) / ((s + 1)*(12*s + 5) - 3)
P2 = (30*F1 + 3*P1) / (12*s + 5)
P3 = (6*P1 - 5*P2) / (12*s)

# Define h1, h2, h3 in the s-domain
h1 = P1 / (r * g)
h2 = P2 / (r * g)
h3 = P3 / (r * g)

# Inverse Laplace Transform to get solutions in the time domain
h1_time = sp.inverse_laplace_transform(h1, s, t)
h2_time = sp.inverse_laplace_transform(h2, s, t)
h3_time = sp.inverse_laplace_transform(h3, s, t)

# Output the solutions for h1, h2, h3
print("h1(t):", h1_time)
print("h2(t):", h2_time)
print("h3(t):", h3_time)

import numpy as np
import pandas as pd

# Define a time range (e.g., from t=0 to t=10)
time_values = np.linspace(0, 50, 500)

# Convert symbolic solutions to numerical functions
h1_func = sp.lambdify((t, r, g), h1_time, "numpy")
h2_func = sp.lambdify((t, r, g), h2_time, "numpy")
h3_func = sp.lambdify((t, r, g), h3_time, "numpy")

# Compute h1, h2, h3 values for the given time range
h1_values = h1_func(time_values, r_val, g_val)
h2_values = h2_func(time_values, r_val, g_val)
h3_values = h3_func(time_values, r_val, g_val)

# Save the data to a CSV file for plotting in Gnuplot
data = pd.DataFrame({
    '#time': time_values,
    'h1': h1_values,
    'h2': h2_values,
    'h3': h3_values
})

data.to_csv('response_data.csv', index=False, sep='\t')
