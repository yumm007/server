#!/bin/bash

TOTAL=`cat scan_num.log | sed '1d' | awk '{sum += $2 + $3};END{print sum}'`

TOTAL_2=`cat scan_num.log | sed '1d' | awk '{sum += $3};END{print sum}'`

SCAN_RES_2=`echo "scale=2;$TOTAL_2/$TOTAL * 100" | bc `%

echo SCAN_RES_2 $SCAN_RES_2

#====================================

on=`grep -v "TOTAL" $1  |sed "s/\t/\n/g" | grep -v "^$" | grep ON | wc -l`
off=`grep -v "TOTAL" $1  |sed "s/\t/\n/g" | grep -v "^$" | grep OFF | wc -l`
ON=`echo "scale=2;$on/($on+$off)*100" | bc `%

echo TOTAL_ON $ON

ap=`head ap.list -n 1`
AVE=`grep $ap $2 | cut -d';' -f3 | cut -b 8-| awk '{n++;sum+=$1};END{printf "%0.2f", sum/n}'`

echo RSSI_AVA $AVE
#cp --backup=numbered $1 $2 .

