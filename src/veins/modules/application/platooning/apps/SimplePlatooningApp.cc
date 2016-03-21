//
// Copright (c) 2012-2015 Michele Segata <segata@ccs-labs.org>
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

#include "veins/modules/application/platooning/apps/SimplePlatooningApp.h"

#include "crng.h"
#include "veins/modules/messages/WaveShortMessage_m.h"
#include "veins/base/messages/MacPkt_m.h"
#include "veins/modules/mac/ieee80211p/Mac1609_4.h"

#include "veins/modules/application/platooning/protocols/BaseProtocol.h"

Define_Module(SimplePlatooningApp);

void SimplePlatooningApp::initialize(int stage) {

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

		VTIcontrol = par("DScontrol").boolValue();

		SUMO_disturbance = par("SUMOdisturb").boolValue();

		//set name for record
		/*gap_d.setName("gap_d");
		gap_v.setName("gap_v");
		speed_fake_controller.setName("speed_fake_control");*/

		if (mySUMOId_int == 0) {
			//ACC speed is 100 km/h
			traciVehicle->setCruiseControlDesiredSpeed(leaderSpeed / 3.6);
			//leader uses the ACC
			traciVehicle->setActiveController(Plexe::ACC);

			//leader speed change
			changeSpeed = new cMessage();
            scheduleAt(simTime() + SimTime(40), changeSpeed);
            /*
			if(!(SUMO_disturbance || VTIcontrol)) {
			    changeSpeed = new cMessage();
			    scheduleAt(simTime() + SimTime(40), changeSpeed);
			}
			else {
			    changeSpeed = 0;
			}
			*/
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

		//ds_control = Veins::TraCIConnection::connect("194.47.15.19", 8855); //can either end with .19 or . 51
		//change to normal CC

		if (mySUMOId_int == 2 && VTIcontrol) {
		    traciVehicle->setActiveController(Plexe::ACC);
		    traciVehicle->setACCHeadwayTime(0.0);
		    //traciVehicle->setCruiseControlDesiredSpeed(leaderSpeed / 3.6);
            ds_control = Veins::TraCIConnection::connect("194.47.15.19", 8855); //can either end with .19 or . 51
            readDS = new cMessage();
            scheduleAt(simTime() + SimTime(0.1), readDS);
		}
		else {
		            //ds_control = 0;
		            readDS = 0;
		}

		//Only to test disturbance
        if (mySUMOId_int == 2 && SUMO_disturbance) {
            //test disturbance from SUMO
            traciVehicle->setActiveController(Plexe::ACC);
            traciVehicle->setACCHeadwayTime(0.0);
            ds_control = Veins::TraCIConnection::connect("194.47.15.19", 8855); //can either end with .19 or . 51
            disturb = new cMessage();
            scheduleAt(simTime() + SimTime(0.1), disturb);
        }
        else {
            //ds_control = 0;
            disturb = 0;
        }

		//new message for making gap
		makeGap = new cMessage();
		scheduleAt(simTime() + SimTime(60), makeGap);

		//every car must run on its own lane
		traciVehicle->setFixedLane(traciVehicle->getLaneIndex());

		newHeadway = 1.0;

	}

}

void SimplePlatooningApp::finish() {
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
}

void SimplePlatooningApp::onData(WaveShortMessage *wsm) {
}

void SimplePlatooningApp::handleSelfMsg(cMessage *msg) {
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
        double speed;
        std::string ds_resp;
        double radar_distance, rel_speed;
        traciVehicle->getRadarMeasurements(radar_distance, rel_speed);

        //send command to request control values from ds (basically speed of ego vehicle)
        ds_control->sendTCPMessage(Veins::makeTraCICommand(0x10, Veins::TraCIBuffer()));  //Command without distance
        //ds_control->sendTCPMessage(Veins::makeTraCICommand(0x11, Veins::TraCIBuffer() << radar_distance));    //Command with distance
        ds_resp = ds_control->receiveMessage();

        Veins::TraCIBuffer buf = Veins::TraCIBuffer(ds_resp);
        buf >> read_a_byte;
        buf >> read_a_byte;
        buf >> read_a_byte;
        buf >> read_a_byte;
        buf >> read_a_byte;
        buf >> speed;

        //Control the vehicle with received speed
        //traciVehicle->setACCHeadwayTime(0.0);
        traciVehicle->setCruiseControlDesiredSpeed(speed);

        /*gap_v.record(radar_distance);
        speed_fake_controller.record(speed);*/

	    scheduleAt(simTime() + SimTime(0.1), readDS);
	}

	if (msg == disturb) {
	    uint8_t read_a_byte;
        double distance;
        double speed;
        std::string ds_resp;

        ds_control->sendTCPMessage(Veins::makeTraCICommand(0x20, Veins::TraCIBuffer()));   //send command to request control values from ds (basically speed of ego vehicle)
        ds_resp = ds_control->receiveMessage();
        Veins::TraCIBuffer buf = Veins::TraCIBuffer(ds_resp);
        buf >> read_a_byte;
        buf >> read_a_byte;
        buf >> read_a_byte;
        buf >> read_a_byte;
        buf >> read_a_byte;
        buf >> distance;

	    double radar_distance, rel_speed;
	    double mySpeed, myAcc, controlAcc, posX, posY, time;
        traciVehicle->getRadarMeasurements(radar_distance, rel_speed);
	    traciVehicle->getVehicleData(mySpeed, myAcc, controlAcc, posX, posY, time);
	    //rel_speed is minus if we are faster
	    if (distance < 12.00) {
	        speed = mySpeed + rel_speed - (5/3.6);
	        traciVehicle->setCruiseControlDesiredSpeed(mySpeed + rel_speed - (5/3.6));
	    }
	    else {
	        speed = 120/3.6;
	        traciVehicle->setCruiseControlDesiredSpeed(120/3.6);
	    }

	    /*gap_d.record(distance);
        gap_v.record(radar_distance);
        speed_fake_controller.record(speed);*/

	    //traciVehicle->setACCHeadwayTime(0.0);
	    scheduleAt(simTime() + SimTime(0.1), disturb);
	}

}

void SimplePlatooningApp::onBeacon(WaveShortMessage* wsm) {
}
