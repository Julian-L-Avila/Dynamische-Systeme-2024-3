using ControlSystems

# Define transfer functions
g1 = tf(1, [1, 10])
g2 = tf(1, [1, 1])
g3 = tf([1, 0, 1], [1, 4, 4])
g4 = tf([1, 1], [1, 6])
h1 = tf([1, 1], [1, 2])
h2 = 2
h3 = 1

# Calculate feedback systems
gf1 = feedback(g3 * g4, h1)
gf2 = feedback(gf1 * g2, h2 / g4)
t = feedback(gf2 * g1, h3)

# Minimize the transfer function
t = minreal(t)

# Print the transfer function in zero-pole-gain form
println(zpk(t))

# Plot the step response, impulse response, Bode plot, and pole-zero map
using Plots

plot(step(t), title="Step Response", grid=true)
plot(impulse(t), title="Impulse Response", grid=true)
plot(bode(t), title="Bode Plot", grid=true)
plot(pzmap(t), title="Pole-Zero Map", grid=true)
