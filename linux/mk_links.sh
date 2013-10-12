#!/bin/bash

for item in `grep -v SRCS .sources |awk '{print $1}'`
do 
	ln -s ../src/$item .
done
	
ln -s ../*.h .


