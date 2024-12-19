set term tikz standalone header "\\usepackage{siunitx}"
set output 'fit-plots.tex'

set grid
set key l t

set xlabel '$t \, [\si{s}]$'
set ylabel '$T \, [\si{C}]$'

data = '../Data/Electric-10.csv'

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

HandFOD(x) = A * Kp * heaviside(x - (Td + b)) * (1 - exp(-(x - (Td + b)) / Tp)) + O
MatlabFOD(x) = A * Kpm * heaviside(x - (Tdm + b)) * (1 - exp(-(x - (Tdm + b)) / Tpm)) + O

#Plot 1st Order
set tit "Modelo de Primer Orden con Delay"
p data u 1:2 tit 'data', HandFOD(x) tit '$H$' dt 2 lw 2, MatlabFOD(x) tit '$M$' dt 1 lw 2
