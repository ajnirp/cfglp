#bin/bash

TEST_DIR="test_files"
REFERENCE_CFGLP="cfglp64_l0"
TEST_CFGLP="cfglp"

if [ $1 == c ]
then
	cd $TEST_DIR; x="$(ls *.c)"
	cd ..
	echo -e "\nCHECKING TOKENS\n"
	for words in $x
	do
	echo "testing $words"
		make -f Makefile.cfg FILE=$words 1> /dev/null  2> /dev/null 
		./$REFERENCE_CFGLP -d -eval $TEST_DIR/"$words"s306.cfg  | awk '{print $2}' > f1
		./$TEST_CFGLP -d -eval $TEST_DIR/"$words"s306.cfg  | awk '{print $2}' > f0
		diff -b -B f1 f0
		rm f0 f1
	done;
elif [ $1 == icode ]
then
	cd $TEST_DIR; x="$(ls *.cfg)"
	cd ..
	echo -e "\nCHECKING ICODE\n"
	for words in $x
	do
	echo "testing $words"
		# make -f Makefile.cfg FILE=$words 1> /dev/null  2> /dev/null 
		./$REFERENCE_CFGLP -d -icode $TEST_DIR/$words > f1
		./$TEST_CFGLP -d -icode $TEST_DIR/$words > f0
		diff -b -B f1 f0
		rm f0 f1
	done;
elif [ $1 == icodecfgs ]
then
	cd $TEST_DIR; x="$(ls *.cfg)"
	cd ..
	echo -e "\nCHECKING ICODE-CFG\n"
	for words in $x
	do
	echo "testing $words"
		./$REFERENCE_CFGLP -d -icode $TEST_DIR/$words  | awk '{print $2}' > f1
		./$TEST_CFGLP -d -icode $TEST_DIR/$words  | awk '{print $2}' > f0
		diff -b -B f1 f0
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
elif [ $1 == icodelra ]
then
	cd $TEST_DIR; x="$(ls *.cfg)"
	cd ..
	echo -e "\nCHECKING LRA\n"
	for words in $x
	do
	echo "testing $words"
		./$REFERENCE_CFGLP -d -icode -lra $TEST_DIR/$words > f1
		./$TEST_CFGLP -d -icode -lra $TEST_DIR/$words > f0
		diff -b -B f1 f0
		rm f0 f1
	done;
elif [ $1 == program ]
then
	cd $TEST_DIR; x="$(ls *.cfg)"
	cd ..
	echo -e "\nCHECKING PROGRAM\n"
	for words in $x
	do
	echo "testing $words"
		./$REFERENCE_CFGLP -d -program $TEST_DIR/$words > f1
		./$TEST_CFGLP -d -program $TEST_DIR/$words > f0
		diff -b -B f1 f0
		rm f0 f1
	done;
elif [ $1 == compile ]
then
	cd $TEST_DIR; x="$(ls *.cfg)"
	cd ..
	echo -e "\nCHECKING COMPILE\n"
	for words in $x
	do
	echo "testing $words"
		./$REFERENCE_CFGLP -d -compile $TEST_DIR/$words > f1
		./$TEST_CFGLP -d -compile $TEST_DIR/$words > f0
		diff -b -B f1 f0
		rm f0 f1
	done;
elif [ $1 == compilelra ]
then
	cd $TEST_DIR; x="$(ls *.cfg)"
	cd ..
	echo -e "\nCHECKING COMPILE\n"
	for words in $x
	do
	echo "testing $words"
		./$REFERENCE_CFGLP -d -compile -lra $TEST_DIR/$words > f1
		./$TEST_CFGLP -d -compile -lra $TEST_DIR/$words > f0
		diff -b -B f1 f0
		rm f0 f1
	done;
elif [ $1 == programlra ]
then
	cd $TEST_DIR; x="$(ls *.cfg)"
	cd ..
	echo -e "\nCHECKING PROGRAM\n"
	for words in $x
	do
	echo "testing $words"
		./$REFERENCE_CFGLP -d -program -lra $TEST_DIR/$words > f1
		./$TEST_CFGLP -d -program -lra $TEST_DIR/$words > f0
		diff -b -B f1 f0
		rm f0 f1
	done;
elif [ $1 == symtab ]
then
	cd $TEST_DIR; x="$(ls *.cfg)"
	cd ..
	echo -e "\nCHECKING SYMTAB\n"
	for words in $x
	do
	echo "testing $words"
		./$REFERENCE_CFGLP -d -symtab $TEST_DIR/$words  | awk '{print $2}' > f1
		./$TEST_CFGLP -d -symtab $TEST_DIR/$words  | awk '{print $2}' > f0
		diff -b -B f1 f0
		rm f0 f1
	done;
elif [ $1 == symtablra ]
then
	cd $TEST_DIR; x="$(ls *.cfg)"
	cd ..
	echo -e "\nCHECKING SYMTAB LRA\n"
	for words in $x
	do
	echo "testing $words"
		./$REFERENCE_CFGLP -d -symtab -lra $TEST_DIR/$words  | awk '{print $2}' > f1
		./$TEST_CFGLP -d -symtab -lra $TEST_DIR/$words  | awk '{print $2}' > f0
		diff -b -B f1 f0
		rm f0 f1
	done;
elif [ $1 == tokens ]
then
	cd $TEST_DIR; x="$(ls *.cfg)"
	cd ..
	echo -e "\nCHECKING TOKENS\n"
	for words in $x
	do
	echo "testing $words"
		./$REFERENCE_CFGLP -d -tokens $TEST_DIR/"$words" > f1
		./$TEST_CFGLP -d -tokens $TEST_DIR/"$words" > f0
		diff -b -B f1 f0
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
		./$REFERENCE_CFGLP -d -tokens -parse $TEST_DIR/"$file" 1> file3 2> file1
		diff -b -B file0 file1
		diff -b -B file2 file3
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
		./$TEST_CFGLP -d -ast $TEST_DIR/"$file" 1> file0 2> file2
		./$REFERENCE_CFGLP -d -ast $TEST_DIR/"$file" 1> file1 2> file3
		diff -b -B file0 file1
		diff -b -B file2 file3
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
			./$TEST_CFGLP -d -eval $TEST_DIR/"$file" 1> file0 2> file2
			./$REFERENCE_CFGLP -d -eval $TEST_DIR/"$file" 1> file1 2> file3
			cat file2 file3
			diff -b -B file0 file1
			diff -b -B file2 file3
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
		./$REFERENCE_CFGLP -d -eval $TEST_DIR/$words > f1
		./cfglp -d -eval $TEST_DIR/$words > f0
		diff -b -B f1 f0
		rm f0 f1
	done;
elif [ $1 == "makecfgs" ]
then
	cd $TEST_DIR; x="$(ls *.c)"
	cd ..
	for words in $x
	do
		echo "making" $words "ka cfg"
		make -f Makefile.cfg FILE=$words 1> /dev/null
	done;

elif [ $1 == "clean" ]
then
	make -f Makefile.cfg clean
	make -f Makefile.cfglp clean
	cd test_files;make clean;cd ..
else
	# test all
	# ./check.sh c
	# ./check.sh ast
	# ./check.sh eval
	./check.sh tokens
	./check.sh icode
	./check.sh icodelra
	./check.sh program
	./check.sh programlra
	./check.sh compile
	./check.sh compilelra
fi
