set term tikz standalone header "\\usepackage{siunitx}"
set output 'data-plots.tex'

set grid
set key l t

set xlabel '{$t \, [\si{s}]$}'
set ylabel '{$T \, [\si{C}]$}'

# Plot 1
set tit 'Horno Eléctrico'
set label '$\Delta t = \qty{10}{s}$' at graph 0.0, graph 1.02
p './Data/Electric-10.csv' u 1:2 tit 'data'

# Plot 2
unset label
set tit 'Horno Eléctrico'
set label '$\Delta t = \qty{20}{s}$' at graph 0.0, graph 1.02
p './Data/Electric-20.csv' u 1:2 tit 'data'

# Plot 3
set tit 'Horno a Gas'
p './Data/Natural-20.csv' u 1:2 tit 'data'
