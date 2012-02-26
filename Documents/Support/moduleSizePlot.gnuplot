set output "moduleSizePlot.ps"
set terminal postscript color
set style fill solid border -1
set boxwidth 0.5
set xrange [-1:100]
set xlabel "Module"
set ylabel "Size (bytes)"
set nokey
plot "./moduleSizeSortedBeta1.txt" using 1 with boxes
