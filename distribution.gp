set term qt font "DejaVu Math TeX Gyre, 22" size 1000,800

set xtics nomirror
set ytics nomirror

set xlabel "value (reduced between min. and max.)"
set ylabel "count (reduced)"

set title "Distribution vs. block size"

set key spacing 1.5 horizontal outside

set colorsequence podo

set xrange [0.2:0.85]
set yrange [:]

plot \
'./blockDistributions/size_50.dist' u 1:($3/65) w lp lw 3 lt 1 title "block size = 50", \
'./blockDistributions/size_100.dist' u 1:($3/29) w lp lw 3 lt 2 title "block size = 100", \
'./blockDistributions/size_2.dist' u 1:($3/2409) w lp lw 3 lt 3 title "block size = 2", \
'./blockDistributions/size_4.dist' u 1:($3/875) w lp lw 3 lt 4 title "block size = 4"
