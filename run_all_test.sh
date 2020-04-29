#!/bin/bash

if [[ $EUID -ne 0 ]]; then
	echo "[ERROR] Use sudo!"
	exit 1
fi

make
mkdir -p output
rm output/* -f
cd OS_PJ1_Test

for i in * ; do
	echo "Now running $i ......"
	../main < "$i" > "../output/${i:0:-4}_stdout.txt"
	dmesg -c | grep Project1 > "../output/${i:0:-4}_dmesg.txt"
done

