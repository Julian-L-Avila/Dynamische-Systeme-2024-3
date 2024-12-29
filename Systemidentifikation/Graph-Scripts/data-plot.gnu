set term tikz standalone header "\\usepackage{siunitx}"
set output 'data-plots.tex'

set grid
set key l t

set xlabel '{$t \, [\si{\s}]$}'
set ylabel '{$T \, [\si{\celsius}]$}'

set tit 'Horno Eléctrico'
set label '$\Delta t = \qty{10}{s}$' at graph 0.0, graph 1.02
p '../Data/electric-10.dat' u 1:2 tit 'Data' lc -1
