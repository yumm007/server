#/bin/bash

TMP_CAL=tmp_dir/tmp_cal.sh
TMP_FILE=tmp_dir/tmp_file_for_ap

echo '#/bin/bash' > $TMP_CAL
echo 'echo $1 $3 >> tmp_dir/$1.ap.value.cal' > $TMP_CAL
chmod +x $TMP_CAL

rm -rf tmp_dir/*.ap.value.cal

cat $TMP_FILE | ./cal_value_from_ap

for i in tmp_dir/*.ap.value.cal
do
	tmp_file=`basename $i | cut -d'.' -f1 | sed -e 's/:/-/g' | tr a-z A-Z`
	tmp_id=`cat ../conf/conf-curr-file/$tmp_file.ini 2>/dev/null | grep USER_ID | cut -d' ' -f3`
	if [ ! $tmp_id ]; then
		tmp_id=00000
	fi
	echo $tmp_id 'ave = '`cat $i | awk '{n+=$1;sum+=$2*$1};END{printf "%d\n", sum/n}'` dBm | sort
done

