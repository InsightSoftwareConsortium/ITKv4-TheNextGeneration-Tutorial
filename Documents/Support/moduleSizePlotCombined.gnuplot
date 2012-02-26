set output "moduleSizePlotCombined.ps"
set terminal postscript color
set style fill solid border -1
set boxwidth 0.5
set xrange [0:50]
set yrange [0:20000]
set ylabel "Size (KB)"
set title "Size distribution of 50 largest ITK Modules"
unset xtics
plot \
"./moduleSizeSortedThirdPartyBeta1Indexed.txt" using 1:2 with boxes title "Third Party", \
"./moduleSizeSortedThirdPartyBeta1Indexed.txt" using 1:2:4 with labels rotate by 90 font "Helvetica,8" left offset 0,0.5 notitle, \
"./moduleSizeSortedNoThirdPartyBeta1Indexed.txt" using 1:2 with boxes title "ITK Native", \
"./moduleSizeSortedNoThirdPartyBeta1Indexed.txt" using 1:2:4 with labels rotate by 90 font "Helvetica,8" left offset 0,0.5 notitle \
