//
// Copright (c) 2016
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#include "veins/modules/application/platooning/apps/PlatoonMergingApp.h"

#include "crng.h"
#include "veins/modules/messages/WaveShortMessage_m.h"
#include "veins/base/messages/MacPkt_m.h"
#include "veins/modules/mac/ieee80211p/Mac1609_4.h"

#include "veins/modules/application/platooning/protocols/BaseProtocol.h"

Define_Module(PlatoonMergingApp);

void PlatoonMergingApp::initialize(int stage) {

	BaseApp::initialize(stage);

	if (stage == 1) {

		//get the oscillation frequency of the leader as parameter
		leaderOscillationFrequency = par("leaderOscillationFrequency").doubleValue();

		//should the follower use ACC or CACC?
		const char *strController = par("controller").stringValue();
		//for now we have only two possibilities
		if (strcmp(strController, "ACC") == 0) {
			controller = Plexe::ACC;
		}
		else if (strcmp(strController, "CACC") == 0) {
			controller = Plexe::CACC;
		}
		else {
			controller = Plexe::PLOEG;
		}
		//headway time for ACC
		accHeadway = par("accHeadway").doubleValue();
		//leader speed
		leaderSpeed = par("leaderSpeed").doubleValue();

		if (mySUMOId_int == 0) {
			//ACC speed is 100 km/h
			traciVehicle->setCruiseControlDesiredSpeed(leaderSpeed / 3.6);
			//leader uses the ACC
			traciVehicle->setActiveController(Plexe::ACC);
			//leader speed must oscillate
			changeSpeed = new cMessage();
			scheduleAt(simTime() + SimTime(40), changeSpeed);
		}
		else {
			//followers speed is higher
			traciVehicle->setCruiseControlDesiredSpeed((leaderSpeed + 30) / 3.6);
			//followers use controller specified by the user
			traciVehicle->setActiveController(controller);
			//use headway time specified by the user (if ACC is employed)
			traciVehicle->setACCHeadwayTime(accHeadway);
			traciVehicle->setCACCConstantSpacing(10);

			changeSpeed = 0;
		}

		//change to normal cc
		if (mySUMOId_int == 2 && USE_DS) {
		    traciVehicle->setActiveController(Plexe::ACC);
		    traciVehicle->setCruiseControlDesiredSpeed(leaderSpeed / 3.6);
            ds_control = Veins::TraCIConnection::connect("194.47.15.19", 8855); //can either end with .19 or . 51
            readDS = new cMessage();
            scheduleAt(simTime() + SimTime(0.1), readDS);
		}
		else {
		            ds_control = 0;
		            readDS = 0;
		}

		disturb = 0;

		//Only to test disturbance
		/*
        if (mySUMOId_int == 2) {
            //test disturbance from SUMO
            traciVehicle->setActiveController(Plexe::ACC);
            disturb = new cMessage();
            scheduleAt(simTime() + SimTime(0.1), disturb);
        }
        */

		//new message for changing lane
		changeLane = new cMessage();
		scheduleAt(simTime() + SimTime(70), changeLane);

		//new message for making gap
		makeGap = new cMessage();
		scheduleAt(simTime() + SimTime(45), makeGap);

		//every car must run on its own lane
		traciVehicle->setFixedLane(traciVehicle->getLaneIndex());

		newHeadway = 1.0;

	}

}

void PlatoonMergingApp::finish() {
	BaseApp::finish();
	if (changeSpeed) {
		cancelAndDelete(changeSpeed);
		changeSpeed = 0;
	}
	if (makeGap) {
	    cancelAndDelete(makeGap);
	    makeGap = 0;
	}
	if (readDS) {
        cancelAndDelete(readDS);
        readDS = 0;
    }
	if (disturb) {
        cancelAndDelete(disturb);
        disturb = 0;
    }
	if (changeLane) {
	        cancelAndDelete(changeLane);
	        changeLane = 0;
	}
}

void PlatoonMergingApp::onData(WaveShortMessage *wsm) {
}

void PlatoonMergingApp::handleSelfMsg(cMessage *msg) {
	//this takes car of feeding data into CACC and reschedule the self message
	BaseApp::handleSelfMsg(msg);

	if (msg == changeSpeed && mySUMOId_int == 0) {
		//make leader speed oscillate
		//traciVehicle->setCruiseControlDesiredSpeed((leaderSpeed + 10 * sin(2 * M_PI * simTime().dbl() * leaderOscillationFrequency)) / 3.6);
	    if(simTime() < 60) {
	        traciVehicle->setCruiseControlDesiredSpeed(25);
	    }
	    else {
	        traciVehicle->setCruiseControlDesiredSpeed(30);
	    }
		scheduleAt(simTime() + SimTime(60), changeSpeed);
	}
	if (msg == makeGap && !(strcmp("platoon0", myPlatoonName.c_str()))) {
	    //make 10m gap or 1 seconds headway
	    if (traciVehicle->getActiveController() == Plexe::CACC) {
	        traciVehicle->setCACCConstantSpacing(20);
	    }
	    else {
            traciVehicle->setGenericInformation(CC_SET_PLOEG_H,&newHeadway,sizeof(double));
	    }
	}
	if (msg == readDS) {
        uint8_t read_a_byte;
        double speed = 0.00;
        std::string ds_resp;

        ds_control->sendTCPMessage(Veins::makeTraCICommand(0x10, Veins::TraCIBuffer()));   //send command to request control values from ds (basically speed of ego vehicle)
        ds_resp = ds_control->receiveMessage();
        Veins::TraCIBuffer buf = Veins::TraCIBuffer(ds_resp);
        buf >> read_a_byte;
        buf >> read_a_byte;
        buf >> read_a_byte;
        buf >> read_a_byte;
        buf >> read_a_byte;
        buf >> speed;

        //Control the vehicle with received speed
        traciVehicle->setACCHeadwayTime(0.1);
        traciVehicle->setCruiseControlDesiredSpeed(speed);

	    scheduleAt(simTime() + SimTime(0.1), readDS);
	}

	if (msg == changeLane) {
	    traciVehicle->setLaneChangeAction(Plexe::MOVE_TO_FIXED_LANE);
	    traciVehicle->setFixedLane(1);
	}


	if (msg == disturb) {
	    double distance, rel_speed;
	    double mySpeed, myAcc, controlAcc, posX, posY, time;
	    traciVehicle->getRadarMeasurements(distance, rel_speed);
	    traciVehicle->getVehicleData(mySpeed, myAcc, controlAcc, posX, posY, time);
	    if (distance < 12) {
	        traciVehicle->setCruiseControlDesiredSpeed(mySpeed + rel_speed - (5/3.6));
	    }
	    else {
	        traciVehicle->setCruiseControlDesiredSpeed(120/3.6);
	    }
	    traciVehicle->setACCHeadwayTime(0.1);
	    scheduleAt(simTime() + SimTime(0.1), disturb);
	}

}

void PlatoonMergingApp::onBeacon(WaveShortMessage* wsm) {
}

void PlatoonMergingApp::handleLowerMsg(cMessage *msg) {

    BaseApp::handleLowerMsg(msg);
}
