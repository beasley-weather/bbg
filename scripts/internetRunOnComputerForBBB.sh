sudo ifconfig enx1cba8ca2ed6a 192.168.7.1
sudo sysctl net.ipv4.ip_forward=1
sudo iptables --table nat --append POSTROUTING --out-interface wlp5s0 -j MASQUERADE
sudo iptables --append FORWARD --in-interface enx1cba8ca2ed6a -j ACCEPT
sudo iptables -L --line-numbers
