set title 'Multiplicacao de Matrizes - AMD HP (AMD Phenom X4)'
# set ytics 590,5,610
set mytics 5
# set yrange[590:610]
set mxtics 10
set grid
set xlabel 'Nro de Processadores'
set ylabel 'Speedup'
set data style linespoints 
set pointsize 1.25
set encoding iso_8859_1
set key left top 
set terminal postscript eps color dashed "Helvetica" 20
set output 'amd-hp.eps'
set yrange [0:20]
#plot 'valores-soma.data' notitle lw 2 lc 3 pt 7
plot 'amd-hp.dad.500' using 1:2 title "Teorico" lw 2 lc 3 pt 7, 'amd-hp.dad.500' using 1:5 title "M 500x500", 'amd-hp.dad.750' using 1:5 title "M 750x750", 'amd-hp.dad.1000' using 1:5 title "M 1000x1000", 'amd-hp.dad.1250' using 1:5 title "M 1250x1250", 'amd-hp.dad.1500' using 1:5 title "M 1500x1500", 'amd-hp.dad.1750' using 1:5 title "M 1750x1750", 'amd-hp.dad.2000' using 1:5 title "M 2000x2000", 'amd-hp.dad.3000' using 1:5 title "M 3000x3000"
