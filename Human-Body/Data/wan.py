import numpy as np
from scipy.integrate import solve_ivp
import pandas as pd
from scipy.stats import norm

# Parameters (tunable)
m1, m2, m3, m4 = 36, 5.5, 15, 4.17
c1, c2, c3, c4 = 2475, 330, 200, 250  # damping constants
k1, k2, k3, k4 = 49340, 20000, 10000, 134400  # spring constants
c31, k31 = 909.1, 192000

# Step function input
def step_function(t):
    return 0.01 if (t >= 1) else 0.0  # Step function (x_s)

def derivative_of_step_function(t, amplitude=0.01, start_time=1):
    # A numerical approximation of the Dirac delta is a very sharp spike at t = start_time
    epsilon = 1e-5  # Small value to simulate a spike
    return amplitude * norm.pdf(t, loc=start_time, scale=epsilon)

# Define the system of equations
def system(t, y):
    x1, v1, x2, v2, x3, v3, x4, v4 = y

    dx1_dt = v1
    dx2_dt = v2
    dx3_dt = v3
    dx4_dt = v4
    
    dv1_dt = (-c2 * (v1 - v2) - k2 * (x1 - x2) - c31 * (v1 - v2) + k31 * (x1 - x3) + c1 * (derivative_of_step_function(t) - v1) + k1 * (step_function(t) - x1)) / m1
    dv2_dt = (c2 * (v1 - v2) + k2 * (x1 - x2) - c3 * (v2 - v3) - k3 * (x2 - x3)) / m2
    dv3_dt = (c3 * (v2 - v3) + k3 * (x2 - x3) + c31 * (v1 - v3) + k31 * (x1 - x3) - c4 * (v3 - v4) - k4 * (x3 - x4)) / m3
    dv4_dt = (c4 * (v3 - v4) + k4 * (x3 - x4)) / m4

    return [dx1_dt, dv1_dt, dx2_dt, dv2_dt, dx3_dt, dv3_dt, dx4_dt, dv4_dt]

# Initial conditions: x1, v1, x2, v2, x3, v3, x4, v4
initial_conditions = [0, 0, 0, 0, 0, 0, 0, 0]  # Initial positions and velocities

# Time span for the integration
t_span = (0, 10)  # from 0 to 10 seconds
t_eval = np.linspace(0, 10, 1000)  # time points to evaluate the solution

# Solve the system
solution = solve_ivp(system, t_span, initial_conditions, t_eval=t_eval)

# Extract the results
x1, v1, x2, v2, x3, v3, x4, v4 = solution.y[0], solution.y[1], solution.y[2], solution.y[3], solution.y[4], solution.y[5], solution.y[6], solution.y[7]
a1 = np.gradient(v1, solution.t)  # Acceleration is the derivative of velocity
a2 = np.gradient(v2, solution.t)
a3 = np.gradient(v3, solution.t)
a4 = np.gradient(v4, solution.t)

# Create a DataFrame to store the results
df = pd.DataFrame({
    '#Time': solution.t,
    'x1': x1,
    'v1': v1,
    'a1': a1,
    'x2': x2,
    'v2': v2,
    'a2': a2,
    'x3': x3,
    'v3': v3,
    'a3': a3,
    'x4': x4,
    'v4': v4,
    'a4': a4
})

# Save the results to a CSV file
df.to_csv('system_solution_with_velocities_and_accelerations.csv', index=False, sep='\t')

