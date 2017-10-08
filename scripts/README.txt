README.txt for bbg/scripts

All of the *.sh files that start with "internet" are utility scripts for giving internet
to either the BBG (beaglebone green) or Raspberry PI when they are connected to a computer.
The scripts create a bridge connection from pi/bbg to the internet.

MISC INFO
The scripts assume the IP of the ___ is ___ and the IP of the computer will become ___ .
a) PI,  192.168.197.227, 192.168.197.226
b) BBG, 192.168.7.2,     192.168.7.3

PROCEDURE
1. 
Connect the device to the computer:
The scripts must be run when the connection between the ___ and the computer is ___.
a) PI,  ethernet
b) BBG, ethernet-over-usb (a usb cable)

2. 
On the computer, run the script internetRunOnComputerFor*.sh
Make sure that the line that looks like
"sudo ifconfig enx04a316df32ab 192.168.7.1"
actually uses the ethernet interface you connect to when you
plug in the device. "enx04a316df32ab" is the interface on my computer
at the time the script was made, but it may change. Make sure your IP is the
expected IP after you run this script using "ifconfig".

3.
On the ___ , run the script internetRunOn*.sh.

4.
While ssh'ed into the device, try to ping google like so:
ping google.com
You should get a response.
If you don't get a response, then you have to check your "iptables" to see if you have 
given correct permissions to allow the device's packets to pass through your computer.

ex.
IPTABLES are the kernels internal tables for rules that control the flow of packets.
There are 5 main tables, but we care about the table called "filter".
Look at the table called "filter" and see what the rules are:


GOOD OUTPUT
kyle: ~/beasley-weather-station/bbg/scripts
$ sudo iptables -t filter --list --line-numbers
Chain INPUT (policy ACCEPT)
num  target     prot opt source               destination         

Chain FORWARD (policy ACCEPT)
num  target     prot opt source               destination         
1    ACCEPT     all  --  anywhere             anywhere      <<<< This is good.      
2    ACCEPT     all  --  anywhere             anywhere      <<<< Repeat, but also good.     

Chain OUTPUT (policy ACCEPT)
num  target     prot opt source               destination


BAD OUTPUT
kyle: ~/beasley-weather-station/bbg/scripts
$ sudo iptables -t filter --list --line-numbers
Chain INPUT (policy ACCEPT)
num  target     prot opt source               destination         
1    ACCEPT     udp  --  anywhere             anywhere             udp dpt:bootps
2    ACCEPT     tcp  --  anywhere             anywhere             tcp dpt:bootps
3    ACCEPT     udp  --  anywhere             anywhere             udp dpt:domain
4    ACCEPT     tcp  --  anywhere             anywhere             tcp dpt:domain

Chain FORWARD (policy ACCEPT)
num  target     prot opt source               destination         
1    ACCEPT     all  --  anywhere             10.42.0.0/24         state RELATED,ESTABLISHED
2    ACCEPT     all  --  10.42.0.0/24         anywhere            
3    ACCEPT     all  --  anywhere             anywhere            
4    REJECT     all  --  anywhere             anywhere             reject-with icmp-port-unreachable <<<<<<<<< BAD, it says "REJECT ALL"
5    REJECT     all  --  anywhere             anywhere             reject-with icmp-port-unreachable <<<<<<<<< BAD, it says "REJECT ALL"
6    ACCEPT     all  --  anywhere             anywhere            
7    ACCEPT     all  --  anywhere             anywhere            
8    ACCEPT     all  --  anywhere             anywhere            
9    ACCEPT     all  --  anywhere             anywhere            

Chain OUTPUT (policy ACCEPT)
num  target     prot opt source               destination


If you have BAD OUTPUT, then remedy by doing these steps:

1.
Look at the line numbers that say they are rejected everything. In this case it is line 4 and 5 of the ouptut, in
the FORWARD section.

2.
sudo iptables -t filter -D FORWARD 4

3.
sudo iptables -t filter --list --line-numbers
Is there still that other pesky reject rule? Yes. Rule 5 got renamed to 4.

4.
sudo iptables -t filter -D FORWARD 4

5.
sudo iptables -t filter --list --line-numbers
There should be no rules getting in your way now.

6.
Go to device and try to ping google. 
If you can't, god help you.