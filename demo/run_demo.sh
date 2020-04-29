#!/bin/bash

if [[ $EUID -ne 0 ]]; then
	echo "[ERROR] Use sudo!"
	exit 1
fi

make

echo ""

for i in TIME_MEASUREMENT FIFO_1 PSJF_2 RR_3 SJF_4; do
	echo "Now running $i ......"
	./main < "OS_PJ1_Test/$i.txt"
	dmesg -c | grep Project1
	echo ""
done

