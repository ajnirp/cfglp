#!/bin/bash

./cfglp $1 -d -tokens > file0
./cfglp64_l1 $1 -d -tokens > file1
diff -b file0 file1