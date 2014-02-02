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

elif [ $1 == ast ]
then
	cd test_files; x="$(ls *.cfg)"
	cd ..
	# x="GlobTernaryInIfLoop.cs306.cfg"
	for file in $x
	do
		echo "testing $file"
		./cfglp -d -ast test_files/"$file" > file0
		./cfglp64_l1 -d -ast test_files/"$file" > file1
		diff -b file0 file1
	done
	# rm file0 file1

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