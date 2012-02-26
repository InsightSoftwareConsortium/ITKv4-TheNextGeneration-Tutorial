set output "moduleSizePlotCombined.ps"
set terminal postscript color
set style fill solid border -1
set boxwidth 0.5
set xrange [-1:50]
set xlabel "Module"
set ylabel "Size (bytes)"
set title "Size distribution (in bytes) of ITK Modules"
set xtics border rotate by 90 font "Helvetica,8"
plot \
"./moduleSizeSortedThirdPartyBeta1Indexed.txt" using 1:2:xtic(4) with boxes title "Third Party", \
"./moduleSizeSortedNoThirdPartyBeta1Indexed.txt" using 1:2:xtic(4) with boxes title "ITK Native"
