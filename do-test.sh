cut -d' ' -f6 11035.log | cut -c 28 | sort | uniq -c
grep "A=00:5a:59:00:12:0e" 11035.log | cut -d';' -f3 | cut -c 9-10 | sort | uniq -c


