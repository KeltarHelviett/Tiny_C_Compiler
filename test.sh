#!/bin/bash
tccpath=$(find -name "tcc" -print0)
if [ "$tccpath" = "" ];
then
	echo "tcc executable not found"
	echo "Trying to compile it by myself"
	mkdir build
	cd build
	cmake .. && make
	cd ..
	tccpath=$(find -name "tcc" -print0)
else
	echo "tcc was found"
fi
ti=0
tj=0
for file in $(ls tests/tokenizer/ --ignore=*.out | sort -n)
do
	tj=$(($tj + 1))
	prefix=$( echo "$file" | sed -e "s/\.cpp//g" )
	eval "./$tccpath -t tests/tokenizer/$file > tests/tokenizer/temp.txt"
	cmp -s "tests/tokenizer/$prefix.out" "tests/tokenizer/temp.txt"
	if [ $? -eq 1 ]; then
		echo "$file test failed"
	else
		echo "$file test succed"
		ti=$(($ti + 1))
	fi
done
rm tests/tokenizer/temp.txt
echo "Tokenizer: passed $ti/$tj"
pi=0
pj=0
for file in $(ls tests/parser/ --ignore=*.out | sort -n)
do
	pj=$(($pj + 1))
	prefix=$( echo "$file" | sed -e "s/\.cpp//g" )
	eval "./$tccpath -t tests/parser/$file > tests/parser/temp.txt"
	cmp -s "tests/parser/$prefix.out" "tests/parser/temp.txt"
	if [ $? -eq 1 ]; then
		echo "$file test failed"
	else
		echo "$file test succed"
		pi=$(($pi + 1))
	fi
done
rm tests/parser/temp.txt
echo "Parser: passed $pi/$pj"
echo "Total:  passed $(($pi+$ti))/$(($tj+$pj))"



