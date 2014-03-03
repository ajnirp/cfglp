#bin/bash

TEST_DIR="test_files"
REFERENCE_CFGLP="cfglp64_l3"

if [ $1 == c ]
then
	cd $TEST_DIR; x="$(ls *.c)"
	cd ..
	echo -e "\nCHECKING TOKENS\n"
	for words in $x
	do
	echo "testing $words"
		make -f Makefile.cfg FILE=$words 1> /dev/null  2> /dev/null 
		./$REFERENCE_CFGLP -d -tokens -parse $TEST_DIR/"$words"s306.cfg  | awk '{print $2}' > f1
		./cfglp -d -tokens $TEST_DIR/"$words"s306.cfg  | awk '{print $2}' > f0
		diff -b f1 f0
		rm f0 f1
	done;
	# echo -e "\nCHECKING AST\n"
	# for words in $x
	# do
	# echo "testing $words"
	# 	make -f Makefile.cfg FILE=$words 1> /dev/null  2> /dev/null 
	# 	./$REFERENCE_CFGLP -d -ast $TEST_DIR/"$words"s306.cfg  | awk '{print $2}' > f1
	# 	./cfglp -d -ast $TEST_DIR/"$words"s306.cfg  | awk '{print $2}' > f0
	# 	diff -b f1 f0
	# 	rm f0 f1
	# done;
	# echo -e "\nCHECKING EVAL\n"
	# for words in $x
	# do
	# echo "testing $words"
	# 	make -f Makefile.cfg FILE=$words 1> /dev/null  2> /dev/null 
	# 	./$REFERENCE_CFGLP -d -eval $TEST_DIR/"$words"s306.cfg  | awk '{print $2}' > f1
	# 	./cfglp -d -eval $TEST_DIR/"$words"s306.cfg  | awk '{print $2}' > f0
	# 	diff -b f1 f0
	# 	rm f0 f1
	# done;
elif [ $1 == tokens ]
then
	cd $TEST_DIR; x="$(ls *.c)"
	cd ..
	echo -e "\nCHECKING TOKENS\n"
	for words in $x
	do
	echo "testing $words"
		make -f Makefile.cfg FILE=$words 1> /dev/null  2> /dev/null 
		./cfglp64_l3 -d -tokens $TEST_DIR/"$words"s306.cfg  | awk '{print $2}' > f1
		./cfglp -d -tokens $TEST_DIR/"$words"s306.cfg  | awk '{print $2}' > f0
		diff -b f1 f0
		rm f0 f1
	done;
elif [ $1 == ecfg ]
then
	cd $TEST_DIR; x="$(ls *.ecfg)"
	cd ..
	for file in $x
	do
		echo "testing $file"
		./cfglp -d -tokens $TEST_DIR/"$file" 1> file2 2> file0
		./$REFERENCE_CFGLP -d -tokens $TEST_DIR/"$file" 1> file3 2> file1
		diff -b file0 file1
		diff -b file2 file3
		rm file0 file1
		rm file2 file3
	done;

elif [ $1 == ast ]
then
	cd $TEST_DIR; x="$(ls *.cfg)"; y=x="$(ls *.ecfg)"; x=$x+$y

	cd ..
	for file in $x
	do
		echo "testing $file"
		./cfglp -d -ast $TEST_DIR/"$file" 1> file0 2> file2
		./$REFERENCE_CFGLP -d -ast $TEST_DIR/"$file" 1> file1 2> file3
		diff -b file0 file1
		diff -b file2 file3
		rm file0 file1
		rm file2 file3
	done;
	

elif [ $1 == eval ]
then
	cd $TEST_DIR; x="$(ls *.cfg)"; y=x="$(ls *.ecfg)"; x=$x+$y
	cd ..
	# x="ContDoWhile.cs306.cfg"
	for file in $x
	do
		if [ $file == "InfiniteLoop.cs306.cfg" ] || [ $file == "NestedLoop.cs306.cfg" ] || [ $file == "complex_expression.cs306.cfg" ]
		then
			echo "Skipping testing of $file"
		else
			echo "testing $file"
			./cfglp -d -eval $TEST_DIR/"$file" 1> file0 2> file2
			./$REFERENCE_CFGLP -d -eval $TEST_DIR/"$file" 1> file1 2> file3
			cat file2 file3
			diff -b file0 file1
			diff -b file2 file3
			rm file0 file1
			rm file2 file3
		fi
	done;

elif [ $1 == "cfg" ]
then
	cd $TEST_DIR; x="$(ls *.cfg)"
	# echo $x
	cd ..
	for words in $x
	do
		echo "testing $words"
		./$REFERENCE_CFGLP -d -tokens $TEST_DIR/$words > f1
		./cfglp -d -tokens $TEST_DIR/$words > f0
		diff -b f1 f0
		rm f0 f1
	done;

else
	# test all
	./check.sh c
	./check.sh ast
	./check.sh eval
	./check.sh ecfg
fi
