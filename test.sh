#bin/bash

if [ $1 == c ]	
then
	cd test_files; x="$(ls *.c)"
	# echo $x
	cd ..
	for words in $x
	do
	echo "testing $words"
		make -f Makefile.cfg FILE=$words  1> /dev/null  2> /dev/null 
		./cfglp64_l1 -d -tokens test_files/"$words"s306.cfg > f1
		./cfglp -d -tokens test_files/"$words"s306.cfg > f0
		diff -b f1 f0
		rm f0 f1
	done;

else
	cd test_files; x="$(ls *.cfg)"
	# echo $x
	cd ..
	for words in $x
	do
		echo "testing $words"
		./cfglp64_l1 -d -tokens test_files/$words > f1
		./cfglp -d -tokens test_files/$words > f0
		diff -b f1 f0
		rm f0 f1
	done;
fi
