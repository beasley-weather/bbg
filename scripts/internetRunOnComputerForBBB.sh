sudo ifconfig enx04a316df32ab 192.168.7.1
sudo sysctl net.ipv4.ip_forward=1
sudo iptables --table nat --append POSTROUTING --out-interface wlp5s0 -j MASQUERADE
sudo iptables --append FORWARD --in-interface enx04a316df32ab -j ACCEPT
sudo iptables -L --line-numbers
