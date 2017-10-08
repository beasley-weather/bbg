sudo -s
route add default gw 192.168.197.226
echo "nameserver 8.8.8.8" >> /etc/resolv.conf
