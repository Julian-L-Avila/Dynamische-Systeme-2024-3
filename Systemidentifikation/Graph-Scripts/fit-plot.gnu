set term tikz standalone header "\\usepackage{siunitx}"
set output 'fit-plots.tex'

# Grid and legend setup
set grid
set key L l t spacing 1.1

# Axis labels
set xlabel '$t \, [\si{s}]$'
set ylabel '$T \, [\si{C}]$'

# Data files
data = '../Data/electric-10.dat'
errorData = '../Data/error.dat'

# Range settings
set xrange [0:1200]
set yrange [0:250]

# Heaviside function
heaviside(x) = (x >= 0)

# Model parameters
A = 110.0
b = 170.0
O = 20.0

Kp = 2.0
Tp = 303.0
Td = 30.0

Kpm = 2.000635678404079
Tpm = 298.8828592591657
Tdm = 36.91

# First-Order Delay Model Output Function
OutputFOD(amplitude, tOn, offset, kConstant, tPole, tDelay, x) = \
  amplitude * kConstant * heaviside(x - (tDelay + tOn)) * (1.0 - exp(-(x - (tDelay + tOn)) / tPole)) + offset

HandFOD(x) = OutputFOD(A, b, O, Kp, Tp, Td, x)
MatlabFOD(x) = OutputFOD(A, b, O, Kpm, Tpm, Tdm, x)

# Plotting First-Order Model with Delay
set tit "Modelo de Primer Orden con Delay"
plot data u 1:2 title 'Data' with linespoints, \
  HandFOD(x) title '$H_{f}$' with lines dt 2 lw 2, \
  MatlabFOD(x) title '$M_{f}$' with lines lw 2

# Second-Order Delay Model parameters
Kp  = 2.0
Tp1 = 25.0
Tp2 = 290.0
Td  = 14.5

Kp1  = 2.1
Tp11 = 26.0
Tp21 = 291.9
Td1  = 14.0

Kpm  = 1.989015963633353
Tp1m = 25.57774292268281
Tp2m = 290.5659847649038
Tdm  = 14.49

# Second-Order Delay Model Output Function
OutputSOD(amplitude, tOn, offset, kConstant, tPole1, tPole2, tDelay, x) = \
  amplitude * kConstant * heaviside(x - (tDelay + tOn)) * (1.0 - tPole1 * exp(-(x - (tDelay + tOn)) / tPole1) / (tPole1 - tPole2) + tPole2 * exp(-(x - (tDelay + tOn)) / tPole2) / (tPole1 - tPole2)) + offset

HandSOD1(x)  = OutputSOD(A, b, O, Kp, Tp1, Tp2, Td, x)
HandSOD2(x)  = OutputSOD(A, b, O, Kp1, Tp11, Tp21, Td1, x)
MatlabSOD(x) = OutputSOD(A, b, O, Kpm, Tp1m, Tp2m, Tdm, x)

# Plotting Second-Order Model with Delay
set tit "Modelo de Segundo Orden con Delay"
plot data u 1:2 title 'Data' with linespoints, \
  HandSOD1(x) title '$H_{s_1}$' lc 4 lw 2, \
  HandSOD2(x) title '$H_{s_2}$' lc 5 lw 2, \
  MatlabSOD(x) title '$M_{s}$' lc 6 lw 2

# Error plot settings
set auto y
set ylabel 'Error'

# Plot Error Analysis
set tit "IEAP Numérico"
plot errorData u 1:2 title '$H_{f}$' with linespoints lc 2 lw 2, \
  '' u 1:3 title '$M_{f}$' with linespoints lc 3 lw 2, \
  '' u 1:4 title '$H_{s_1}$' with linespoints lc 4 lw 2, \
  '' u 1:5 title '$H_{s_2}$' with linespoints lc 5 lw 2, \
  '' u 1:6 title '$M_{s}$' with linespoints lc 6 lw 2

