//
// Copyright (C) 2016-2018 Maytheewat Aramrattana <maytheewat.aramrattana@vti.se>
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

#include "veins/modules/messages/WaveShortMessage_m.h"
#include "veins/base/messages/MacPkt_m.h"
#include "veins/modules/mac/ieee80211p/Mac1609_4.h"

#include "veins/modules/application/platooning/protocols/BaseProtocol.h"
#include "veins/modules/application/platooning/scenarios/BaseScenario.h"

Define_Module(PlatoonMergingApp);

void PlatoonMergingApp::initialize(int stage) {
	BaseApp::initialize(stage);

    if (stage == 1) {
        normalCACCSpacing = par("caccSpacing").doubleValue();
        normalHeadway = par("ploegH").doubleValue();
        GapMakingKp = par("gapKp").doubleValue();
        SafeGap = par("safeGap").doubleValue();

        currentGapToFWDPair = 0;
        myTargetGap = 0;
        myPairvehicleLength = 4; //4m according to SUMO

        /** Initialize messages **/
        changeLane = new cMessage("changeLane");
        checkGap = new cMessage("checkGap");
        reformPlatoon = new cMessage("reformPlatoon");
        startMerge = new cMessage("startMerge");

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

        /** ASSIGN head flag**/
        if(positionHelper->isLeader()) { //The OPC car
            headVehicleFlag = false;
        }
        else if(myId == positionHelper->getLeaderId()+2) { //'leader' of the platoon after pace-making vehicle
            headVehicleFlag = true; //this one will merge first, if it's in platoon A
        }
        else { //The rest
            headVehicleFlag = false;
        }

        /** ASSIGN MIO_ID**/
        if(!positionHelper->isLeader()) {
            myMIO_ID = myId - 2;
        }
        else {
            myMIO_ID = 0;
        }

        //Now in Plexe 2.0 Platoon ID is available, the most right lane (in travel direction) is 0, then 1, etc.
        /** ASSIGN tail flag**/
        //This is very bad way to assign it.....
        tailVehicleFlag = false;
        if (myId == 7 || myId == 8) {
            tailVehicleFlag = true; //last vehicle
        }

        //Register application !!, otherwise no message will be received
        protocol->registerApplication(BaseProtocol::iCLCM_TYPE, gate("lowerLayerIn"), gate("lowerLayerOut"));

        scheduleAt(simTime() + SimTime(50), startMerge);
    }
}

void PlatoonMergingApp::finish() {
    cancelAndDelete(changeLane);
    cancelAndDelete(checkGap);
    cancelAndDelete(reformPlatoon);
    cancelAndDelete(startMerge);
    changeLane = 0;
    checkGap = 0;
    reformPlatoon = 0;
    startMerge = 0;
	BaseApp::finish();
}

void PlatoonMergingApp::onData(WaveShortMessage *wsm) {
}

void PlatoonMergingApp::handleSelfMsg(cMessage *msg) {
	BaseApp::handleSelfMsg(msg);


    if (msg == startMerge) {
        mergeRequestFlag = true;
        UpdateProtocolParam();
    }
    if (msg == changeLane) {
        //if(mySUMOId_int == 0) {
        if(1 == 0) {
            //OPC car shouldn't merge (yet)
        }
        else {
            traciVehicle->setFixedLane(0);  //Move to right lane
            traciVehicle->setLaneChangeAction(Plexe::MOVE_TO_FIXED_LANE);
        }
        newMIO = myFWDPairID;
        headVehicleFlag = false;
    }
    if (msg == checkGap) {
        doneGap = true;
    }
    if (msg == reformPlatoon) {
        myMIO_ID = newMIO;
        // and reset the gap
        currentCACCSpacing = normalCACCSpacing;
        currentHeadway = normalHeadway;
        traciVehicle->setGenericInformation(CC_SET_PLOEG_H, &currentHeadway, sizeof(double));
        traciVehicle->setCACCConstantSpacing(currentCACCSpacing);
        traciVehicle->setCruiseControlDesiredSpeed((100) / 3.6);
        if(positionHelper->getPlatoonId() == 1) {
            //Should find another way to change name of platoon
            //BaseApp::myPlatoonName = "platoon0"; //change name to another platoon
            positionHelper->setPlatoonId(0);
            PlatoonID = 0;
        }
        PlatoonMergingApp::UpdateProtocolParam();
    }
}

void PlatoonMergingApp::onBeacon(WaveShortMessage* wsm) {
}

void PlatoonMergingApp::UpdateProtocolParam() {
    UnicastMessage *iCLCM_unicast;
    ICLCM *iCLCM_msg;

    iCLCM_unicast = new UnicastMessage("", BaseProtocol::iCLCM_TYPE);
    iCLCM_unicast->setDestination(-1);
    iCLCM_unicast->setPriority(2);  //I guess...
    iCLCM_unicast->setChannel(Channels::CCH);

    iCLCM_msg = new ICLCM();
    iCLCM_msg->setMIO_ID(myMIO_ID); //ID of most important object (in another lane), basically your pair in B2A phase, 0 mean no pair
    iCLCM_msg->setFWDPairID(myFWDPairID); //ID of the forward pair in A2B phase, 0 mean no pair
    iCLCM_msg->setBWDPairID(myBWDPairID); //ID of the backward pair in A2B phase, 0 mean no pair
    iCLCM_msg->setMergeRequestFlag(mergeRequestFlag);
    iCLCM_msg->setMergingFlag(Merging_flag);
    iCLCM_msg->setSTOMFlag(STOM_flag);
    iCLCM_msg->setHeadVehicle(headVehicleFlag);
    iCLCM_msg->setTailVehicle(tailVehicleFlag);
    iCLCM_msg->setKind(BaseProtocol::iCLCM_TYPE);
    iCLCM_unicast->encapsulate(iCLCM_msg);
    sendDown(iCLCM_unicast);  //send up to the protocol layer
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
        if (!(positionHelper->isLeader()) && epkt->getVehicleId() == myMIO_ID) {
            traciVehicle->setPrecedingVehicleData(epkt->getSpeed(), epkt->getAcceleration(), epkt->getPositionX(), epkt->getPositionY(), epkt->getTime());
        }
        if(positionHelper->isInSamePlatoon(epkt->getVehicleId())) {
            //if the message comes from the pace maker (OPC), position 0 in the platoon
            if (epkt->getVehicleId() == positionHelper->getLeaderId()) {
                traciVehicle->setPlatoonLeaderData(epkt->getSpeed(), epkt->getAcceleration(), epkt->getPositionX(), epkt->getPositionY(), epkt->getTime());
            }
        }
        else {
            //This message is from a car from the other platoon
            //if merge is requested and we are in platoon 0 ("right lane")
            if(mergeRequestFlag && (epkt->getSUMOpositionX() > sumoPosX) && (myFWDPairID == 0) && (positionHelper->getPlatoonId() == 0)) {
                //find a forward pair (B2A)
                if((epkt->getSUMOpositionX() - sumoPosX) < (normalCACCSpacing+4)) { //4 is length of the vehicle
                    myFWDPairID = epkt->getVehicleId();
                    UpdateProtocolParam();
                }
            }
        }

        //So, we are making gap and this message is from my FWDPair(that is not 0)
        if(makingGap && epkt->getVehicleId() == myFWDPairID && myFWDPairID != 0) {
            currentGapToFWDPair = epkt->getSUMOpositionX()- myPairvehicleLength - sumoPosX;
            GapToFWDPair.record(currentGapToFWDPair);
            nodeIdOut.record(myId);
            if(doneGap) { //Done making gap !!
                //Enter merging state and hand over the flag
                if(positionHelper->getPlatoonId() == 1) {
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
                //std::cout << "Setting spacing to " << currentCACCSpacing << std::endl;
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
        //     - my MIO has its BWDPair already -> start making gap then (for platoon B) (platoon id = 0)
        if(!doneGap && iclcm_pkt->getStationID() == myMIO_ID && iclcm_pkt->getBWDPairID() != 0 && (positionHelper->getPlatoonId() == 0 )
                && myMIO_ID != 0) {
            makingGap = true;
        }
        //     - I have a backward pair and I am the leader of platoon A now and this message is from my BWDPair set my FWDPair as its MIO
        if(myBWDPairID != 0 && myFWDPairID == 0 && headVehicleFlag && positionHelper->getPlatoonId() == 1
                && iclcm_pkt->getStationID() == myBWDPairID) {
            myFWDPairID = iclcm_pkt->getMIO_ID();
            makingGap = true;
            scheduleAt(simTime() + SimTime(25), checkGap);   //assume the gap is done in 25 seconds (arbitrary number)
            UpdateProtocolParam();
        }
        //     - I have both FWDPair and BWDPair -> start making gap then (for platoon A)
        //     ---> this is done when receiving CAM (beaconing msg)

        //     - If we are in platoonA and our MIO is merging and we are not the leader -> our turn to pair and make gap
        if(positionHelper->getPlatoonId() == 1 && iclcm_pkt->getStationID() == myMIO_ID && iclcm_pkt->getMergingFlag() && !headVehicleFlag) {
            headVehicleFlag = true;
            UpdateProtocolParam();
        }

        // 2. For merging and reforming phase
        // precondition : we are in merging zone
        //                all vehicle that done with gap making will enter merging state and wait for STOM from its backward pair
        //     - Platoon A: if my BWDPair set STOM -> merge -> reform
        if(inMergingZone && positionHelper->getPlatoonId() == 1 && iclcm_pkt->getStationID() == myBWDPairID && iclcm_pkt->getSTOMFlag()) {
                newMIO = myFWDPairID; //Change target
                inMergingZone = false;
                checkZone = false;
                scheduleAt(simTime() + SimTime(0.1), changeLane); //Merge
                scheduleAt(simTime() + SimTime(0.2), reformPlatoon); //Reform

        }

        //     - Platoon B: if my FWDPair is in merging state -> set STOM -> reform
        if(inMergingZone && positionHelper->getPlatoonId() == 0 && iclcm_pkt->getStationID() == myFWDPairID && iclcm_pkt->getMergingFlag()) {
                STOM_flag = true;
                newMIO = myFWDPairID; //Change target
                inMergingZone = false;
                checkZone = false;
                UpdateProtocolParam();
                scheduleAt(simTime() + SimTime(0.2), reformPlatoon); //Reform
        }

    }
    else if(enc->getKind() == BaseProtocol::STOM_TYPE) {
        //Was not actually used, STOM is assumed to be received when in merging zone
        //STOM *stom_pkt = dynamic_cast<STOM *>(enc);
    }

    delete enc;
    delete unicast;
}
