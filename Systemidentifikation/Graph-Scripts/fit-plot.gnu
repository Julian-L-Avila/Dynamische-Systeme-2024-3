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

ffd(x) = -61944176908382505.0 * heaviside(x - 206.91) * (exp((15166663393542144.0 / 21908345273226025.0) - (8796093022208.0 * x / 2629001432787123.0)) - 1.0) / 281474976710656.0

#Plot 1
set tit "Primer Orden con Delay"
p data u 1:2 tit "data" ps 1, ffd(x) + 20.0 tit 'matlab' lw 2
