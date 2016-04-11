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

//#include "veins/modules/application/platooning/protocols/BaseProtocol.h"
#include "veins/modules/application/platooning/protocols/PlatoonMergingProtocol.h"

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
		    if(strcmp("platoon0", myPlatoonName.c_str()) == 0) {
	            //OPC B control PlatoonB to start at 60km/h
	            traciVehicle->setCruiseControlDesiredSpeed(60 / 3.6);
		    }
		    else {
	            //OPC A control PlatoonA to start at 80km/h
	            traciVehicle->setCruiseControlDesiredSpeed(60 / 3.6);
		    }
			//leader uses the ACC
			traciVehicle->setActiveController(Plexe::ACC);
		}
		else {
			//followers speed is higher (100 km/h)
			traciVehicle->setCruiseControlDesiredSpeed((100) / 3.6);
			//followers use controller specified by the user
			traciVehicle->setActiveController(controller);
			//use headway time specified by the user (if ACC is employed)
			traciVehicle->setACCHeadwayTime(accHeadway);
			traciVehicle->setCACCConstantSpacing(currentCACCSpacing);
		}

		//new message for making gap
		makeGap = new cMessage("makeGap");
		//scheduleAt(simTime() + SimTime(15), makeGap);

		changeLane = new cMessage();
		//scheduleAt(simTime() + SimTime(70), changeLane);

		ProtocolParUpdates = new cMessage("update");
		//scheduleAt(simTime() + SimTime(70), ProtocolParUpdates);
		checkGap = new cMessage();

		myMIO_ID = 0; //ID of most important object (in another lane), basically your pair in B2A phase, 0 mean no pair
        myMIO_RANGE = 65535; //distance to MIO, 65535 mean n/a
        myMIO_speed = 32767;
        myFWDPairID = 0; //ID of the forward pair in A2B phase, 0 mean no pair
        myBWDPairID = 0; //ID of the backward pair in A2B phase, 0 mean no pair
        mergeRequestFlag = false;
        makingGap = false;

		//every car must run on its own lane
		traciVehicle->setFixedLane(traciVehicle->getLaneIndex());

		newHeadway = 1.0;
		lane_change_count = 0;
	}

}

void PlatoonMergingApp::finish() {
	BaseApp::finish();
	if (makeGap) {
	    cancelAndDelete(makeGap);
	    makeGap = 0;
	}
	if (changeLane) {
        cancelAndDelete(changeLane);
        changeLane = 0;
	}
	if (ProtocolParUpdates) {
        cancelAndDelete(ProtocolParUpdates);
        ProtocolParUpdates = 0;
    }
	if (checkGap) {
        cancelAndDelete(checkGap);
        checkGap = 0;
    }
}

void PlatoonMergingApp::onData(WaveShortMessage *wsm) {
}

void PlatoonMergingApp::handleSelfMsg(cMessage *msg) {
	//this takes car of feeding data into CACC and reschedule the self message
	BaseApp::handleSelfMsg(msg);     //the function actually does nothing

	//if (msg == makeGap && !(strcmp("platoon0", myPlatoonName.c_str()))) {
	if (msg == makeGap) {
	    //make 10m gap or 1 seconds headway
	    /*if (traciVehicle->getActiveController() == Plexe::CACC) {
	        traciVehicle->setCACCConstantSpacing(20);
	    }
	    else {
            traciVehicle->setGenericInformation(CC_SET_PLOEG_H,&newHeadway,sizeof(double));
	    }*/
	}
	if (msg == ProtocolParUpdates) {

        UnicastMessage *iCLCM_unicast;
        ICLCM *iCLCM_msg;

        iCLCM_unicast = new UnicastMessage("", iCLCM);
        iCLCM_unicast->setDestination(-1);
        iCLCM_unicast->setPriority(2);  //I guess...
        iCLCM_unicast->setType(5);

        iCLCM_msg = new ICLCM();
        iCLCM_msg->setMIO_ID(myMIO_ID); //ID of most important object (in another lane), basically your pair in B2A phase, 0 mean no pair
        iCLCM_msg->setFWDPairID(myFWDPairID); //ID of the forward pair in A2B phase, 0 mean no pair
        iCLCM_msg->setBWDPairID(myBWDPairID); //ID of the backward pair in A2B phase, 0 mean no pair
        iCLCM_unicast->encapsulate(iCLCM_msg);
        sendDown(iCLCM_unicast);  //send up to the application layer
	}
	if (msg == changeLane) {
        traciVehicle->setFixedLane(0);  //Move to right lane
        traciVehicle->setLaneChangeAction(Plexe::MOVE_TO_FIXED_LANE);
	}
	if (msg == checkGap) {
	    makingGap = false;
	}

}

void PlatoonMergingApp::onBeacon(WaveShortMessage* wsm) {
}

void PlatoonMergingApp::handleLowerMsg(cMessage *msg) {

    UnicastMessage *unicast = dynamic_cast<UnicastMessage *>(msg);
    ASSERT2(unicast, "received a frame not of type UnicastMessage");

    cPacket *enc = unicast->decapsulate();
    ASSERT2(enc, "received a UnicastMessage with nothing inside");

    //our vehicle's data
    double speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime;
    traciVehicle->getVehicleData(speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime);

    if (enc->getKind() == BaseProtocol::BEACON_TYPE) {  //Similar to BaseApp

        PlatooningBeacon *epkt = dynamic_cast<PlatooningBeacon *>(enc);
        ASSERT2(epkt, "received UnicastMessage does not contain a PlatooningBeacon");
        if(strcmp(epkt->getPlatoonName(), myPlatoonName.c_str()) == 0)
        {
            //if the message comes from the leader
            if (epkt->getVehicleId() == 0) {
                traciVehicle->setPlatoonLeaderData(epkt->getSpeed(), epkt->getAcceleration(), epkt->getPositionX(), epkt->getPositionY(), epkt->getTime());
            }
            //if the message comes from the vehicle in front
            if (epkt->getVehicleId() == mySUMOId_int - 1) {
                traciVehicle->setPrecedingVehicleData(epkt->getSpeed(), epkt->getAcceleration(), epkt->getPositionX(), epkt->getPositionY(), epkt->getTime());
            }
        }
        else {  //This message is from a car from the other platoon
            if(mergeRequestFlag && epkt->getPositionX() > sumoPosX && myMIO_ID == 0 && (strcmp(myPlatoonName.c_str(), "platoon0") == 0)) {
                //find a pair (B2A)
                if((epkt->getPositionX() - sumoPosX) < currentCACCSpacing) {
                    myMIO_ID = epkt->getRelayerId();
                    scheduleAt(simTime() + SimTime(0.01), ProtocolParUpdates);
                }
            }
            if(myBWDPairID != 0 && epkt->getPositionX() > sumoPosX && myFWDPairID == 0) { //A vehicle chose me as its MIO
                //find a forward pair (A2B) in other platoon
                if((epkt->getPositionX() - sumoPosX) < currentCACCSpacing) {
                    myFWDPairID = epkt->getRelayerId();
                    scheduleAt(simTime() + SimTime(0.01), ProtocolParUpdates);
                }
            }
        }
        if(epkt->getRelayerId() == myMIO_ID && myMIO_ID != 0 && !makingGap) {   //If not making gap
            if((epkt->getPositionX() - sumoPosX) < SafeGap) {
                currentCACCSpacing = currentCACCSpacing + epkt->getPositionX() - sumoPosX;
                traciVehicle->setCACCConstantSpacing(currentCACCSpacing);
                makingGap = true;
                scheduleAt(simTime() + SimTime(5), checkGap);   //check again in 5 seconds
            }
            else {
                //Gap is already good
            }
        }
        if(epkt->getRelayerId() == myFWDPairID && myFWDPairID != 0 && !makingGap) { //Use OMNeT++ id, it will be unique
            if((epkt->getPositionX()-sumoPosX) < SafeGap) {
                currentCACCSpacing = currentCACCSpacing + epkt->getPositionX() - sumoPosX;
                traciVehicle->setCACCConstantSpacing(currentCACCSpacing);
                makingGap = true;
                scheduleAt(simTime() + SimTime(5), checkGap);
            }
            else {
                //Gap is already good
            }
        }
    }
    else if (enc->getKind() == BaseProtocol::iCLCM_TYPE) {
        ICLCM *iclcm_pkt = dynamic_cast<ICLCM *>(enc);
        mergeRequestFlag = iclcm_pkt->getMergeRequestFlag();
        if(iclcm_pkt->getMIO_ID() == myId) {
           myBWDPairID = iclcm_pkt->getStationID();
        }
    }
    else if(enc->getKind() == BaseProtocol::STOM_TYPE) {
        STOM *stom_pkt = dynamic_cast<STOM *>(enc);
        if((stom_pkt->getVehicleId()+1 == mySUMOId_int) && (strcmp(stom_pkt->getPlatoonName(), myPlatoonName.c_str()) == 0)) { //just example of how second vehicle tell first one to change lane
            scheduleAt(simTime() + SimTime(0.05), changeLane);
        }
    }
    else if(enc->getKind() == BaseProtocol::ROAD_CLOSED) {
        if((strcmp("platoon1", myPlatoonName.c_str()) == 0)) {
            SyncSpeedToB();
        }
    }

    delete enc;
    delete unicast;
}

void PlatoonMergingApp::SyncSpeedToB() {
    traciVehicle->setCruiseControlDesiredSpeed(60 / 3.6);
}
