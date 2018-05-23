import networkcomm as net
import subprocess, os, signal

p = 0

def startStopSumo(sumo_option):
    global p
    if sumo_option > 0 :
        #because the configuration goes from 0, so the option 1 is actually 0
        sumo_option = sumo_option-1
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
        #p = subprocess.Popen([command,interface,configuration,run_option], stdout=subprocess.PIPE)
        p = subprocess.Popen([command,interface,configuration,run_option])
        print("run sumo here")
    else:
        #CHANGE replace the place holder with the name of the sumo executable 
        #in linux

        #os.system("/usr/bin/killall -s SIGINT -INT opp_run_release")
        #subprocess.call(["/usr/bin/killall", "-sSIGINT", "-INT", "opp_run_release"])
        p.kill()
        #os.killpg(os.getpgid(p.pid), signal.SIGTERM)
        print("stop sumo here")
        
#Comm constants
ip_own_pc="192.168.65.82" #CHANGE
port_own_pc=44445 #CHANGE

while True:
        #Wait for data
        data_received, sumo_option=net.receiveData(">I",ip_own_pc,port_own_pc) #original

        if data_received:
                print("\r\nReceive sumo order "+str(sumo_option[0])+"\r\n")
                startStopSumo(sumo_option[0])