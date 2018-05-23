import networkcomm as net
import subprocess, os, signal
import sys

if len(sys.argv) != 0:
    sumo_option = int(sys.argv[1])
    if sumo_option > 0 :
        #because the configuration goes from 0, so the option 1 is actually 0
        sumo_option = sumo_option-1
        command="./run" 
        interface = "-uCmdenv"
        configuration = "-cSimplePlatooningApp"
        #configuration = "-cManualDrivingAuto"
        run_option = "-r "+str(sumo_option)   

        #CHANGE add some switch case that choses the converts the received 
        #integer to the desired sumo argument
        

        #CHANGE here we glue to the command to the converted sumo argument. Make
        #sure all spaces and things ar correct since the strings will literaly 
        #be added.

        #subprocess.call([command,interface,configuration,run_option])
        #os.system("./run -u Cmdenv -c ManualDriving -r "+str(sumo_option))
        #p = subprocess.Popen([command,interface,configuration,run_option], stdout=subprocess.PIPE)
        p = subprocess.Popen([command,interface,configuration,run_option], stdout=subprocess.PIPE)
        #p = subprocess.Popen([command,interface,configuration,run_option], stdout=subprocess.PIPE, stderr=subprocess.STDOUT, preexec_fn=os.setsid)
        print("run sumo here")
    else:
        #CHANGE replace the place holder with the name of the sumo executable 
        #in linux

        #os.system("/usr/bin/killall -s SIGINT -INT opp_run_release")
        subprocess.call(["/usr/bin/killall", "-sSIGINT", "-INT", "opp_run_release"])
        #p.kill()
        #os.killpg(os.getpgid(p.pid), signal.SIGTERM)
        print("stop sumo here")

else:
    print("Please provide argument")