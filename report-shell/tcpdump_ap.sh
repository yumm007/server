#/bin/bash
sudo tcpdump -i wlan0 port 5080 -A -n -l | sed -e '/GetDateTime/d' -e '/length/d' -e 's/mac(wlan0):/\n/g' -e 's/:/-/g' -u | grep --line-buffered  signal

