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

	    CACCSpacing = par("CACCSpacing").doubleValue(); //read from omnetpp.ini file
	    SafeGap = par("safeGap").doubleValue(); //read from omnetpp.ini file
	    GapMakingKp = par("gapKp").doubleValue();
	    currentCACCSpacing = CACCSpacing;
        currentGapToFWDPair = 0;
        myTargetGap = 0;
        myPairvehicleLength = 4.70;
        CACCHeadway = par("ploegH").doubleValue();
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
		else if (strcmp(strController, "PLOEG") == 0){
			controller = Plexe::PLOEG;
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
        doneGap = false;
        inMergingZone = false;
        checkZone = true;
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
	    //EV << "vehicle number " << myId << "CHANGED LANE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
	}
	if (msg == checkGap) {
	    doneGap = true;
	}
	if (msg == reformPlatoon) {
	    myMIO_ID = newMIO;
	    // and reset the gap
	    currentCACCSpacing = CACCSpacing;
	    currentHeadway = CACCHeadway;
	    traciVehicle->setGenericInformation(CC_SET_PLOEG_H, &currentHeadway, sizeof(double));
	    traciVehicle->setCACCConstantSpacing(currentCACCSpacing);
	    traciVehicle->setCruiseControlDesiredSpeed((100) / 3.6);
	    if(PlatoonID == 1) {
	        BaseApp::myPlatoonName = "platoon0"; //change name to another platoon
	        PlatoonID = 2;
	    }
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
    double speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime, distance, relSpeed;
    traciVehicle->getVehicleData(speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime);
    traciVehicle->getRadarMeasurements(distance, relSpeed);

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
        }
        else {
            //This message is from a car from the other platoon
            //if merge is requested and we are in platoon B ("platoon0")
            if(mergeRequestFlag && epkt->getSUMOpositionX() > sumoPosX && myFWDPairID == 0 && (strcmp(myPlatoonName.c_str(), "platoon0") == 0)) {
                //find a forward pair (B2A)
                if((epkt->getSUMOpositionX() - sumoPosX) < currentCACCSpacing) {
                    myFWDPairID = epkt->getRelayerId();
                    UpdateProtocolParam();
                }
            }
        }

        //So, we are making gap and this message is from my FWDPair(that is not 0)
        if(makingGap && epkt->getRelayerId() == myFWDPairID && myFWDPairID != 0) {
            currentGapToFWDPair = epkt->getSUMOpositionX()- myPairvehicleLength - sumoPosX;
            GapToFWDPair.record(currentGapToFWDPair);
            nodeIdOut.record(myId);
            if(doneGap) { //Done making gap !!
                //Enter merging state and hand over the flag
                if((strcmp(myPlatoonName.c_str(), "platoon1") == 0)) {
                    headVehicleFlag = false;
                    Merging_flag = true;
                    traciVehicle->setCruiseControlDesiredSpeed((60) / 3.6); //Prevent it from speeding up when MIO merge
                }
                else {

                }
                makingGap = false;
                UpdateProtocolParam();
            }
            else {
                currentCACCSpacing = distance + GapMakingKp*(SafeGap - currentGapToFWDPair);
                currentHeadway = (currentCACCSpacing - 2) / speed;
                traciVehicle->setGenericInformation(CC_SET_PLOEG_H, &currentHeadway, sizeof(double));
                traciVehicle->setCACCConstantSpacing(currentCACCSpacing);
            }
        }

        //enter merging zone
        if(sumoPosX >= 2500 && checkZone) {
            inMergingZone = true;
            doneGap = true;
        }

    }
    else if (enc->getKind() == BaseProtocol::iCLCM_TYPE) {
        ICLCM *iclcm_pkt = dynamic_cast<ICLCM *>(enc);
        mergeRequestFlag = iclcm_pkt->getMergeRequestFlag();

        // 1. For pairing phase
        //     - a vehicle has my ID as its FWDPair -> it is my BWDPair
        if(iclcm_pkt->getFWDPairID() == myId && myBWDPairID == 0) {
           myBWDPairID = iclcm_pkt->getStationID();
           UpdateProtocolParam();
        }
        //     - my MIO has its BWDPair already -> start making gap then (for platoon B)
        if(!doneGap && iclcm_pkt->getStationID() == myMIO_ID && iclcm_pkt->getBWDPairID() != 0 && (strcmp(myPlatoonName.c_str(), "platoon0")==0) && myMIO_ID != 0) {
            makingGap = true;
        }
        //     - I have a backward pair and I am the leader of platoon A now and this message is from my BWDPair set my FWDPair as its MIO
        if(myBWDPairID != 0 && myFWDPairID == 0 && headVehicleFlag && (strcmp(myPlatoonName.c_str(), "platoon1") == 0)
           && iclcm_pkt->getStationID() == myBWDPairID) {
            myFWDPairID = iclcm_pkt->getMIO_ID();
            makingGap = true;
            scheduleAt(simTime() + SimTime(25), checkGap);   //assume the gap is done in 25 seconds (arbitrary number)
            UpdateProtocolParam();
        }
        //     - I have both FWDPair and BWDPair -> start making gap then (for platoon A)
        //     ---> this is done when receiving CAM (beaconing msg)

        //     - If we are in platoonA and our MIO is merging and we are not the leader -> our turn to pair and make gap
        if((strcmp(myPlatoonName.c_str(), "platoon1")==0) && iclcm_pkt->getStationID() == myMIO_ID && iclcm_pkt->getMergingFlag() && !headVehicleFlag) {
            headVehicleFlag = true;
            UpdateProtocolParam();
        }

        // 2. For merging and reforming phase
        // precondition : we are in merging zone
        //                all vehicle that done with gap making will enter merging state and wait for STOM from its backward pair
        //     - Platoon A: if my BWDPair set STOM -> merge -> reform
        if(inMergingZone && (strcmp(myPlatoonName.c_str(), "platoon1")==0) && iclcm_pkt->getStationID() == myBWDPairID && iclcm_pkt->getSTOMFlag()) {
                newMIO = myFWDPairID; //Change target
                inMergingZone = false;
                checkZone = false;
                scheduleAt(simTime() + SimTime(0.1), changeLane); //Merge
                scheduleAt(simTime() + SimTime(0.2), reformPlatoon); //Reform

        }

        //     - Platoon B: if my FWDPair is in merging state -> set STOM -> reform
        if(inMergingZone && (strcmp(myPlatoonName.c_str(), "platoon0")==0) && iclcm_pkt->getStationID() == myFWDPairID && iclcm_pkt->getMergingFlag()) {
                STOM_flag = true;
                newMIO = myFWDPairID; //Change target
                inMergingZone = false;
                checkZone = false;
                UpdateProtocolParam();
                scheduleAt(simTime() + SimTime(0.2), reformPlatoon); //Reform
        }

    }
    else if(enc->getKind() == BaseProtocol::STOM_TYPE) {
        STOM *stom_pkt = dynamic_cast<STOM *>(enc);
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
