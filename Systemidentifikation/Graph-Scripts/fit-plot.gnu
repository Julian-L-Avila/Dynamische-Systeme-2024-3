set term tikz standalone header "\\usepackage{siunitx}"
set output 'fit-plots.tex'

set grid
set key l t

set xlabel '$t \, [\si{s}]$'
set ylabel '$T \, [\si{C}]$'

data = '../Data/electric-10.dat'
errorData = '../Data/error.dat'

set xrange[0:1200]
set yrange[0:250]

heaviside(x) = (x >= 0) ? 1 : 0

A   = 110.0
b   = 170.0
Kp  = 2.0
Tp  = 303.0
Td  = 30.0
Kpm = 2.000635678404079
Tpm = 298.8828592591657
Tdm = 36.91
O   = 20.0

OutputFOD(amplitude, tOn, offset, kConstant, tPole, tDelay, x) = amplitude * kConstant * heaviside(x - (tDelay + tOn)) * (1.0 - exp(-(x - (tDelay + tOn)) / tPole)) + offset

HandFOD(x) = OutputFOD(A, b, O, Kp, Tp, Td, x)
MatlabFOD(x) = OutputFOD(A, b, O, Kpm, Tpm, Tdm, x)

#Plot 1st Order
set tit "Modelo de Primer Orden con Delay"
p data u 1:2 tit 'data', HandFOD(x) tit '$H_{f}$' dt 2 lw 2, MatlabFOD(x) tit '$M_{f}$' dt 1 lw 2

Kpm = 1.989015963633353
Tp1m = 25.57774292268281
Tp2m = 290.5659847649038
Tdm = 14.49

OutputSOD(amplitude, tOn, offset, kConstant, tPole1, tPole2, tDelay, x) = amplitude * kConstant * heaviside(x - (tDelay + tOn)) * (1.0 - tPole1 * exp(-(x - (tDelay + tOn)) / tPole1) / (tPole1 - tPole2) + tPole2 * exp(-(x - (tDelay + tOn)) / tPole2) / (tPole1 - tPole2)) + offset

MatlabSOD(x) = OutputSOD(A, b, O, Kpm, Tp1m, Tp2m, Tdm, x)

#Plot 2nd Order
set tit "Modelo de Segundo Orden con Delay"
p data u 1:2 tit 'data', MatlabSOD(x) tit '$M_{s}$' lc 6 dt 1 lw 2

set auto y
set ylabel 'Error'

#Plot Error
set tit "Numerical Integral EAP"
p errorData u 1:2 tit ' $H_{f}$' w lp lc 2 lw 2, '' u 1:3 tit '$M_{f}$' w lp lc 3 lw 2, '' u 1:6 tit '$M_{s}$' w lp lc 6 lw 2
