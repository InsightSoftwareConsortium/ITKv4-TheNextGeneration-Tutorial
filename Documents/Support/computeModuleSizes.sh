#
# Run from the top ITK directory
#
du --max-depth=2 Modules > moduleSize.txt
sort -rn moduleSize.txt  > moduleSizeSorted.txt

