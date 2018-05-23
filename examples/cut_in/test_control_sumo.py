import networkcomm as net
import subprocess, os, signal
        
#Comm constants
ip_own_pc="192.168.65.82" #CHANGE
port_own_pc=44445 #CHANGE

net.sendData(net.packData([0],"I"))
