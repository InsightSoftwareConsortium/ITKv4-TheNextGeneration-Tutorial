#
# Run from the top ITK directory
#
du --max-depth 0 $(find Modules -mindepth 2 -maxdepth 2 -type d) | sort -rn > moduleSizeSorted.txt
du --max-depth 0 $(find Modules -mindepth 2 -maxdepth 2 -type d) | sed "/ThirdParty/d" | sort -rn > moduleSizeNoThirdPartySorted.txt
