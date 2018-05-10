import networkcomm as net
import subprocess, os

def startStopSumo(sumo_option):
    if sumo_option > 0 :
        command="./run" 
        interface = "-uCmdenv"
        configuration = "-cManualDriving"
        run_option = "-r "+str(sumo_option)

        #CHANGE add some switch case that choses the converts the received 
        #integer to the desired sumo argument
        

        #CHANGE here we glue to the command to the converted sumo argument. Make
        #sure all spaces and things ar correct since the strings will literaly 
        #be added.
        subprocess.call([command,interface,configuration,run_option])
    else:
        #CHANGE replace the place holder with the name of the sumo executable 
        #in linux
        #subprocess.call("/usr/bin/killall -s SIGINT -INT NAMESUMOEXECUTABLE") 
		print("SUMO option is zero")
        
#Comm constants
ip_own_pc="194.47.15.56" #CHANGE
port_own_pc=44445 #CHANGE

while True:
        #Wait for data
        data_received, sumo_option=net.receiveData(">I",ip_own_pc,port_own_pc)

        if data_received:
                print("Receive sumo order "+str(sumo_option[0]))
                startStopSumo(sumo_option[0])
