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

	    currentCACCSpacing = 10.00; //default spacing is 10 meters
        CACCSpacing = 10.00; //default spacing is 10 meters
        currentGapToFWDPair = 0;
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

		//Initilize cMessage(s)
		makeGap = new cMessage("makeGap");
		changeLane = new cMessage();
		checkGap = new cMessage();
		reformPlatoon = new cMessage();

		/** ASSIGN head flag**/
		if(mySUMOId_int - 1 > 0) { //follower
		    //myMIO_ID = mySUMOId_int - 1;
		    headVehicleFlag = false;
		}
		else if(mySUMOId_int - 1 == 0) { //'leader' of the platoon after pace-making vehicle
		    //myMIO_ID = 0;
		    headVehicleFlag = true; //this one will merge first, if it's in platoon A
		}
		else { //The OPC car
		    //myMIO_ID = 0; //ID of most important object (in front), 0 mean no pair
		    headVehicleFlag = false;
		}

		/** ASSIGN MIO_ID**/
		if(myId -2 > 0) {
		    myMIO_ID = myId - 2;
		}
		else {
		    myMIO_ID = 0;
		}

		/** ASSIGN tail flag**/
		tailVehicleFlag = false;
		if (strcmp(myPlatoonName.c_str(), "platoon0") == 0) {
		    PlatoonID = 2; //Platoon B
		    if(mySUMOId_int == 4) {
                tailVehicleFlag = true; //last vehicle
            }
		}
		else {
		    PlatoonID = 1; //Platoon A
		    if(mySUMOId_int == 3) {
		        tailVehicleFlag = true; //last vehicle
		    }
		}

		/** Initialize the rest**/
		GapToFWDPair.setName("gap_to_fwd");
		nodeIdOut.setName("nodeId");
		newMIO = 0;
        myMIO_RANGE = 65535; //distance to MIO, 65535 mean n/a
        myMIO_speed = 32767;
        myFWDPairID = 0; //ID of the forward pair in another lane, 0 mean no pair
        myBWDPairID = 0; //ID of the backward pair in another lane, 0 mean no pair
        mergeRequestFlag = false;
        Merging_flag = false;
        makingGap = false;
        refineGap = false;
        STOM_flag = false;
		//every car must run on its own lane
		traciVehicle->setFixedLane(traciVehicle->getLaneIndex());

		newHeadway = 1.0;
		lane_change_count = 0;
		UpdateProtocolParam();
	}

}

void PlatoonMergingApp::finish() {
	BaseApp::finish();
	if (changeLane) {
        cancelAndDelete(changeLane);
        changeLane = 0;
	}
	if (checkGap) {
        cancelAndDelete(checkGap);
        checkGap = 0;
    }
	if (reformPlatoon) {
        cancelAndDelete(reformPlatoon);
        reformPlatoon = 0;
    }
}

void PlatoonMergingApp::onData(WaveShortMessage *wsm) {
}

void PlatoonMergingApp::handleSelfMsg(cMessage *msg) {
	//this takes car of feeding data into CACC and reschedule the self message
	BaseApp::handleSelfMsg(msg);     //the function actually does nothing

	if (msg == changeLane) {
	    if(mySUMOId_int == 0) {
	        //OPC car shouldn't merge (yet)
	    }
	    else {
	        traciVehicle->setFixedLane(0);  //Move to right lane
            traciVehicle->setLaneChangeAction(Plexe::MOVE_TO_FIXED_LANE);
	    }
	    newMIO = myFWDPairID;
	    headVehicleFlag = false;
	    UpdateProtocolParam();
	}
	if (msg == checkGap) {
	    makingGap = false;
	    refineGap = true;
	}
	if (msg == reformPlatoon) {
	    myMIO_ID = newMIO;
	    // and reset the gap
	    currentCACCSpacing = 10;
	    traciVehicle->setCACCConstantSpacing(10);
	    if(PlatoonID == 1) {
	        BaseApp::myPlatoonName = "platoon0"; //change name to another platoon
	        PlatoonID = 2;
	    }
	    makingGap = true;
	    UpdateProtocolParam();
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
        if (mySUMOId_int != 0 && epkt->getRelayerId() == myMIO_ID) {
            traciVehicle->setPrecedingVehicleData(epkt->getSpeed(), epkt->getAcceleration(), epkt->getPositionX(), epkt->getPositionY(), epkt->getTime());
        }
        if(strcmp(epkt->getPlatoonName(), myPlatoonName.c_str()) == 0)
        {
            //if the message comes from the pace maker (OPC) [assuming the ID is known, in this case = 0]
            if (epkt->getVehicleId() == 0) {
                traciVehicle->setPlatoonLeaderData(epkt->getSpeed(), epkt->getAcceleration(), epkt->getPositionX(), epkt->getPositionY(), epkt->getTime());
            }
            //if the message comes from the vehicle in front and I am not a leader
            //if (mySUMOId_int != 0 && epkt->getVehicleId() == myMIO_ID) {
            /*if (mySUMOId_int != 0 && epkt->getRelayerId() == myMIO_ID) {
                traciVehicle->setPrecedingVehicleData(epkt->getSpeed(), epkt->getAcceleration(), epkt->getPositionX(), epkt->getPositionY(), epkt->getTime());
            }*/
        }
        else {  //This message is from a car from the other platoon
            if(mergeRequestFlag && epkt->getSUMOpositionX() > sumoPosX && myFWDPairID == 0 && (strcmp(myPlatoonName.c_str(), "platoon0") == 0)) {
                //find a forward pair (B2A)
                if((epkt->getSUMOpositionX() - sumoPosX) < currentCACCSpacing) {
                    myFWDPairID = epkt->getRelayerId();
                    UpdateProtocolParam();
                }
            }
            //BWDPair!=0 means someone paired with us, then find our FWDPair
            //TODO:if there is no car behind the last vehicle of platoonA, it will never find FWDPair
            if(myBWDPairID != 0 && epkt->getSUMOpositionX() > sumoPosX && myFWDPairID == 0) { //A vehicle chose me as its FWDPair
                //find a forward pair (A2B) in other platoon
                if((epkt->getSUMOpositionX() - sumoPosX) < currentCACCSpacing) {
                    myFWDPairID = epkt->getRelayerId();
                    UpdateProtocolParam();
                }
            }
        }
        if(epkt->getRelayerId() == myFWDPairID && myFWDPairID != 0 && !makingGap && !STOM_flag && !Merging_flag) {   //If not making gap and STOM not sent

            if((epkt->getSUMOpositionX() - sumoPosX) < SafeGap && !refineGap) {
                 currentCACCSpacing = (2*currentCACCSpacing) + myPairvehicleLength;
                 traciVehicle->setCACCConstantSpacing(currentCACCSpacing);
                 makingGap = true;
                 scheduleAt(simTime() + SimTime(6), checkGap);   //check again in 6 seconds (arbitrary number)
            }
            else {
                //refine the gap
                if((strcmp(myPlatoonName.c_str(), "platoon1") == 0)) {
                    currentGapToFWDPair = epkt->getSUMOpositionX()- myPairvehicleLength - sumoPosX;
                    traciVehicle->setCACCConstantSpacing(currentCACCSpacing + currentGapToFWDPair);
                }
                makingGap = true;
            }
            /*  CONSTANT GAP APPROACH */
            /*if((epkt->getSUMOpositionX() - sumoPosX) < SafeGap) {
                 currentCACCSpacing = (2*currentCACCSpacing) + myPairvehicleLength;
                 traciVehicle->setCACCConstantSpacing(currentCACCSpacing);
                 makingGap = true;
                //scheduleAt(simTime() + SimTime(3), checkGap);   //check again in 3 seconds
            }
            else {
                //Gap already good
            }*/
            /** 'DYNAMIC' GAP APPROACH **/
            /*makingGap = true;
            currentGapToFWDPair = epkt->getSUMOpositionX()- myPairvehicleLength - sumoPosX;
            GapToFWDPair.record(currentGapToFWDPair);
            nodeIdOut.record(myId);
            if(currentGapToFWDPair < 0 || currentGapToFWDPair < SafeGap) {
                currentCACCSpacing = currentCACCSpacing + 0.1;
                traciVehicle->setCACCConstantSpacing(currentCACCSpacing);
            }
            scheduleAt(simTime() + SimTime(1), checkGap);   //check again in 1 seconds*/
        }
    }
    else if (enc->getKind() == BaseProtocol::iCLCM_TYPE) {
        ICLCM *iclcm_pkt = dynamic_cast<ICLCM *>(enc);
        mergeRequestFlag = iclcm_pkt->getMergeRequestFlag();
        if(iclcm_pkt->getFWDPairID() == myId && myBWDPairID == 0) {
           myBWDPairID = iclcm_pkt->getStationID();
        }
        //This vehicle is our MIO and is merging and already reform, we should merge next
        if(iclcm_pkt->getStationID() == myMIO_ID && iclcm_pkt->getMergingFlag() && iclcm_pkt->getPlatoonID() == 2 && !headVehicleFlag) {
            headVehicleFlag = true;
            UpdateProtocolParam();
        }
        //This vehicle has me as a FWDPair and is sending STOM and we are not merging and we are the head vehicle
        if(iclcm_pkt->getFWDPairID() == myId && iclcm_pkt->getSTOMFlag() && !Merging_flag && (strcmp(myPlatoonName.c_str(), "platoon1") == 0) && headVehicleFlag) {
            Merging_flag = true;
            headVehicleFlag = false;
            newMIO = myFWDPairID; //Change target
            UpdateProtocolParam();
            scheduleAt(simTime() + SimTime(0.05), changeLane);
            scheduleAt(simTime() + SimTime(0.1), reformPlatoon);
        }
        //This vehicle has me as a BWDPair and is merging -> it will come in front of us soon
        if(iclcm_pkt->getBWDPairID() == myId && iclcm_pkt->getMergingFlag() && !Merging_flag) {
            Merging_flag = true;    //pretend to be merging (acknowledge the merge)
            newMIO = iclcm_pkt->getStationID();
            scheduleAt(simTime() + SimTime(0.1), reformPlatoon);
        }
    }
    else if(enc->getKind() == BaseProtocol::STOM_TYPE) {
        STOM *stom_pkt = dynamic_cast<STOM *>(enc);

        STOM_flag = true;
        UpdateProtocolParam();
        //just example of how the first vehicle tell next one to change lane
        if((stom_pkt->getVehicleId()+1 == mySUMOId_int) && (strcmp(stom_pkt->getPlatoonName(), myPlatoonName.c_str()) == 0)) {
            //scheduleAt(simTime() + SimTime(0.05), changeLane);
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

void PlatoonMergingApp::UpdateProtocolParam() {
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
    iCLCM_msg->setMergingFlag(Merging_flag);
    iCLCM_msg->setSTOMFlag(STOM_flag);
    iCLCM_msg->setHeadVehicle(headVehicleFlag);
    iCLCM_msg->setTailVehicle(tailVehicleFlag);
    iCLCM_unicast->encapsulate(iCLCM_msg);
    sendDown(iCLCM_unicast);  //send up to the application layer
}
