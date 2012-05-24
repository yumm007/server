#!/bin/bash

FILE_NAME=$1

#去掉文件后缀
MAC_ADDR=${FILE_NAME%.*}

#其中##表示从前面删除字符，*/表示删除到最后一个/，如果一个#就表示只删除到第一个
IP=$((16#${MAC_ADDR##*-}))

echo "PRD_ID = 2011072111640011" > $FILE_NAME
echo "AC_ADDR = "$MAC_ADDR >> $FILE_NAME
echo "USER_ID = "`expr $IP + 11000 + 1`>> $FILE_NAME
echo "USER_PWD = "`expr $IP + 11000 + 1`>> $FILE_NAME
echo "" >> $FILE_NAME
echo "USER_NAME = default" >> $FILE_NAME
echo "" >> $FILE_NAME
echo "TRACKER_SERVER = 192.168.66.211" >> $FILE_NAME
echo "TRACKER_PORT = 5080" >> $FILE_NAME
echo "PROXY_SERVER = 192.168.66.211" >> $FILE_NAME
echo "PROXY_PORT = 5080" >> $FILE_NAME
echo "" >> $FILE_NAME
echo "DEF_NW_SSID = SafeToneT01" >> $FILE_NAME
echo "DEF_NW_CH = 1" >> $FILE_NAME
echo "" >> $FILE_NAME
echo "SCAN_NW_SSID = SafeToneT01" >> $FILE_NAME
echo "SCAN_NW_CH = 1" >> $FILE_NAME
echo "" >> $FILE_NAME
echo "STATIC_IP_FLAG = 0" >> $FILE_NAME
echo "DEF_IP_ADDR = 192.168.66."`expr $IP + 1` >> $FILE_NAME
echo "DEF_NET_MASK = 255.255.255.0" >> $FILE_NAME
echo "DEF_NET_SW = 192.168.66.254" >> $FILE_NAME
echo "" >> $FILE_NAME
echo "SENSER_TIMER = 10" >> $FILE_NAME
echo "" >> $FILE_NAME

