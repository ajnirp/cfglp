#bin/bash

TEST_DIR="test_files"
REFERENCE_CFGLP="cfglp64_l0"
TEST_CFGLP="cfglp"

cd $TEST_DIR; x="$(ls *.ecfg)"
cd ..

for i in $x
do
	echo "testing $i"
	./$REFERENCE_CFGLP -d -compile $TEST_DIR/$i > f0
	./$TEST_CFGLP -d -compile $TEST_DIR/$i > f1
	diff -b -B f0 f1
	rm f0 f1
done