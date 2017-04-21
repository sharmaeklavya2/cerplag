#!/bin/bash
for arg in "$@"
do
	sourcefile=${arg##*/}
	basename=${sourcefile%txt}
	for inputfile in tests/input_files/$basename*
	do
		outputfile=${inputfile##*/}
		outputfile="tests/output_files/"${outputfile%.in}".out"
		correctfile=${inputfile##*/}
		correctfile="tests/correct_outputs/"${correctfile%.in}".out"
		mkdir -p "tests/output_files"
		./erplag_run.sh $arg < $inputfile > $outputfile
		diff -s $outputfile $correctfile
	done
done
