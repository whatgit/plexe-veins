import networkcomm as net
import subprocess, os

def startStopSumo(sumo_option):
    if sumo_option > 0 :
        command="./run" 
        interface = "-uCmdenv"
        configuration = "-cManualDrivingAuto"
        run_option = "-r "+str(sumo_option)

        #CHANGE add some switch case that choses the converts the received 
        #integer to the desired sumo argument
        

        #CHANGE here we glue to the command to the converted sumo argument. Make
        #sure all spaces and things ar correct since the strings will literaly 
        #be added.

        #subprocess.call([command,interface,configuration,run_option])
        #os.system("./run -u Cmdenv -c ManualDriving -r "+str(sumo_option))
        p = subprocess.Popen([command,interface,configuration,run_option], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        print("run sumo here")
    else:
        #CHANGE replace the place holder with the name of the sumo executable 
        #in linux

        #os.system("/usr/bin/killall -s SIGINT -INT opp_run_release")
        #subprocess.call(["/usr/bin/killall", "-sSIGINT", "-INT", "opp_run_release"])
        print("stop sumo here")
        
#Comm constants
ip_own_pc="194.47.15.56" #CHANGE
port_own_pc=44445 #CHANGE

while True:
        #Wait for data
        data_received, sumo_option=net.receiveData(">I",ip_own_pc,port_own_pc)

        if data_received:
                print("Receive sumo order "+str(sumo_option[0]))
                startStopSumo(sumo_option[0])
