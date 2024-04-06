set term qt font "DejaVu Math TeX Gyre, 22" size 1000,800

set xtics nomirror
set ytics nomirror

set xlabel "Block size"
set ylabel "Variance"

set title "Variance vs. block size"

set colorsequence podo

set xrange [-800:21100]
set yrange [:]

plot \
'./average_n99999999_c2_2.block' u 1:4 w l lw 3 lt 6 notitle
