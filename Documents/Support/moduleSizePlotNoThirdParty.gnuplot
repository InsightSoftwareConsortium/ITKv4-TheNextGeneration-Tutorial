set output "moduleSizePlotNoThirdParty.ps"
set terminal postscript color
set style fill solid border -1
set boxwidth 0.5
set xrange [-1:85]
set yrange [0:20000]
plot "./moduleSizeSortedNoThirdPartyBeta1.txt" using 1 with boxes title "Size distribution (in bytes) of ITK Modules excluding third party libraries"

