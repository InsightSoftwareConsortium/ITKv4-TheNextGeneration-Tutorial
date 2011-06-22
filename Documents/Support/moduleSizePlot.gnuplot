set output "moduleSizePlot.ps"
set terminal postscript color
set style fill solid border -1
set boxwidth 0.5
set xrange [-1:100]
plot "./moduleSizeSorted.txt" using 1 with boxes title "Size distribution (in bytes) of ITK Modules"

