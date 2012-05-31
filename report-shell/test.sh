#!/bin/bash

if [ $# -ne 1 ]
then 
	LOG_FILE=../log/`date +%F`-location.log
	ONLINE_FILE=../log/`date +%F`-online.log
else
	LOG_FILE=../log/`echo -n $1`-location.log
	ONLINE_FILE=../log/`echo -n $1`-online.log
fi

#LOG_FILE=../log/`date +%F`-location.log
#ONLINE_FILE=../log/`date +%F`-online.log

cut -b 22-26 $LOG_FILE | sort | uniq > t01.list
n=`wc -l t01.list | cut -b -2`
w=$n*280

cut -d';' -f 2,4 $LOG_FILE | sed "s/;/\n/" | cut -b 3- | sort | uniq > ap.list
sed -i '/000000/d' ap.list

if [ ! -d tmp_dir ]; then
	mkdir tmp_dir
fi

echo 'set terminal png truecolor' > conf
echo 'set output "T01_Scan.png"' >> conf
echo 'set term pngcairo size 1300,'$w >> conf
echo 'set multiplot layout '$n',1' >> conf
echo 'set autoscale' >> conf
echo 'set xdata time' >> conf
echo 'set timefmt "%H:%M:%S"' >> conf
echo 'set style data linespoints' >> conf

n=`wc -l ap.list | cut -b -2`
LP="w lp pt 7"

echo -e "ID \t<=1 \t>1" > scan_num.log

for i in `cat t01.list` 
do
	grep "\[$i\]" $LOG_FILE | ./collect_ap > tmp_dir/$i.log
	echo -n "$i" >> scan_num.log
	grep "\[$i\]" $LOG_FILE | ./collect_scan_ap 2>> scan_num.log
	echo "set title \"T01 $i\"" >> conf
	echo -n "plot \"tmp_dir/$i.log\"" >> conf
	for ((p = 1, j = 2; j < $n+1; j++, p++))
	do
		echo -n " using 1:$j $LP title \"AP$p\", "\'\' >> conf
	done
	echo  " using 1:$j $LP title \"AP$p\"" >> conf

done

cat conf | gnuplot
cat scan_num_ap.conf | gnuplot

n=`grep "TOTAL IDs" $ONLINE_FILE -n | tail -n 1 | cut -d":" -f1`
n=`expr $n - 1`
sed "1,$n d" $ONLINE_FILE | tee t01_on_line.log
echo

echo Total Off Line Counter
grep -v "TOTAL" $ONLINE_FILE  |sed "s/\t/\n/g" | grep -v "^$" | grep OFF | sort -n | uniq -c | paste - - - - -
echo

./make-one-record.sh $ONLINE_FILE $LOG_FILE

rm -rf conf ap.list t01.list tmp_dir/ scan_num.log

