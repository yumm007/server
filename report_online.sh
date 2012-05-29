#!/bin/bash
#watch -n 5 'grep -v "TOTAL" `$1`  |sed "s/\t/\n/g" | grep -v "^$" | grep OFF | sort -n | uniq -c '

#while true
#do
#	clear
	grep -v "TOTAL" $1  |sed "s/\t/\n/g" | grep -v "^$" | grep ON | sort -n | uniq -c | paste - - - - -
#	sleep 5
#done



