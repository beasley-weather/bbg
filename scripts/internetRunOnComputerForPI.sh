sudo ifconfig enp4s0 192.168.192.226
sudo sysctl net.ipv4.ip_forward=1
sudo iptables --table nat --append POSTROUTING --out-interface wlp5s0 -j MASQUERADE
sudo iptables --append FORWARD --in-interface enp4s0 -j ACCEPT
