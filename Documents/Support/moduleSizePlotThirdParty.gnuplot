set output "moduleSizePlotThirdParty.ps"
set terminal postscript color
set style fill solid border -1
set boxwidth 0.5
set xrange [-1:20]
set yrange [0:20000]
plot "./moduleSizeSortedThirdPartyBeta1.txt" using 1 with boxes title "Size distribution (in bytes) of ITK Modules from third party libraries"

