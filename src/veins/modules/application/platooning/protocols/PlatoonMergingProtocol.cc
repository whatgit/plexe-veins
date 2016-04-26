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

#include "PlatoonMergingProtocol.h"

Define_Module(PlatoonMergingProtocol)

void PlatoonMergingProtocol::initialize(int stage) {
    BaseProtocol::initialize(stage);

    if (stage == 0) {
        //ALWAYS initialize everything inside "stage"!!!
        sendiCLCM = new cMessage("sendiCLCM");
        sendSTOMmsg = new cMessage("sendSTOMmsg");
        startMerge = new cMessage("startMerge");

        myMIO_ID = 0; //ID of most important object (in another lane), basically your pair in B2A phase, 0 mean no pair
        myMIO_RANGE = 65535; //distance to MIO, 65535 mean n/a
        myMIO_speed = 32767;
        myFWDPairID = 0; //ID of the forward pair in A2B phase, 0 mean no pair
        myBWDPairID = 0; //ID of the backward pair in A2B phase, 0 mean no pair
        mergeRequestFlag = false;
        STOM_flag = false;
        Merging_flag = false;
        headVehicleFlag = false;
        tailVehicleFlag = false;

        //random start time
        SimTime beginTime = SimTime(uniform(0.001, 1.0));
        SimTime offset = SimTime(uniform(0.001,1.0));
        scheduleAt(simTime() + beaconingInterval + beginTime, sendBeacon);
        scheduleAt(simTime() + beaconingInterval + beginTime + offset, sendiCLCM);
        scheduleAt(simTime() + SimTime(50), startMerge);
        scheduleAt(simTime() + SimTime(100), sendSTOMmsg);
    }
}

void PlatoonMergingProtocol::handleSelfMsg(cMessage *msg) {

    BaseProtocol::handleSelfMsg(msg);

    if (msg == sendBeacon) {
        sendPlatooningMessage(-1);
        scheduleAt(simTime() + beaconingInterval, sendBeacon);
    }
    if (msg == sendiCLCM) {
        sendiCLCMMessage(-1);
        scheduleAt(simTime() + beaconingInterval, sendiCLCM);
    }
    if (msg == startMerge) {
        mergeRequestFlag = true;
        //startMergingScenario(-1);
    }
    if (msg == sendSTOMmsg && mySUMOId_int == 0 && strcmp("platoon1", myPlatoonName.c_str()) == 0) {
        //We enter the merging zone
        sendSTOM(-1);
    }

}

void PlatoonMergingProtocol::sendSTOM(int destinationAddress) {

    //actual packets
    UnicastMessage *stom_unicast;
    STOM *stom_pkt;

    double veinsTime = simTime().dbl();

    double time = veinsTime;

    //create and send beacon
    stom_unicast = new UnicastMessage("", STOM_TYPE);
    stom_unicast->setDestination(-1);
    stom_unicast->setPriority(priority);
    stom_unicast->setType(3);    //STOM_MESSAGE = 3

    //create platooning beacon with data about the car
    stom_pkt = new STOM();
    stom_pkt->setVehicleId(mySUMOId_int);
    stom_pkt->setTime(time);
    stom_pkt->setPlatoonName(myPlatoonName.c_str());
    stom_pkt->setKind(STOM_TYPE);
    stom_pkt->setByteLength(packetSize);
    stom_pkt->setSequenceNumber(seq_n++);

    //put platooning beacon into the message for the UnicastProtocol
    stom_unicast->encapsulate(stom_pkt);
    sendDown(stom_unicast);
}

void PlatoonMergingProtocol::startMergingScenario(int destinationAddress) {

    //actual packets
    UnicastMessage *rdClosed_unicast;
    RoadClosed *road_closed;

    double veinsTime = simTime().dbl();

    double time = veinsTime;

    //create and send beacon
    rdClosed_unicast = new UnicastMessage("", ROAD_CLOSED);
    rdClosed_unicast->setDestination(-1);
    rdClosed_unicast->setPriority(priority);
    rdClosed_unicast->setType(3);    //STOM_MESSAGE = 3

    //create platooning beacon with data about the car
    road_closed = new RoadClosed();
    road_closed->setVehicleId(mySUMOId_int);
    road_closed->setTime(time);
    road_closed->setPlatoonName(myPlatoonName.c_str());
    road_closed->setKind(ROAD_CLOSED);
    road_closed->setByteLength(packetSize);
    road_closed->setSequenceNumber(seq_n++);

    //put platooning beacon into the message for the UnicastProtocol
    rdClosed_unicast->encapsulate(road_closed);
    sendDown(rdClosed_unicast);
}

void PlatoonMergingProtocol::sendiCLCMMessage(int destinatinAddress) {

    //vehicle's data to be included in the message
    double speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime;

    UnicastMessage *iCLCM_unicast;
    ICLCM *iCLCM_msg;

    iCLCM_unicast = new UnicastMessage("", iCLCM);
    iCLCM_unicast->setDestination(-1);
    iCLCM_unicast->setPriority(priority);
    iCLCM_unicast->setType(5);

    traciVehicle->getVehicleData(speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime);
    iCLCM_msg = new ICLCM();
    iCLCM_msg->setStationID(myId); //my vehicle id (OMNeT++ id in this case)
    iCLCM_msg->setRearAxleLocation(sumoPosX-4.70); //this is rear bumper position, should actually be rearAxleLocation and should range from 0 to 4095 meters
    iCLCM_msg->setControllerType(traciVehicle->getActiveController());  //Range from 0 to 3 | 0:Manual | 1:CC | 2:ACC | 3: CACC |
    iCLCM_msg->setDesiredLongitudinalAcceleration(controllerAcceleration); //Should range from -1001 to 1001 m/s2
    iCLCM_msg->setMIO_ID(myMIO_ID); //ID of most important object (in front), 0 mean no pair
    iCLCM_msg->setMIO_RANGE(myMIO_RANGE); //distance to MIO, 65535 mean n/a
    iCLCM_msg->setMIO_RANGE_RATE(myMIO_speed); //Speed of MIO (?) 32767 = n/a
    iCLCM_msg->setMergeRequestFlag(mergeRequestFlag);
    iCLCM_msg->setSTOMFlag(STOM_flag); //bool STOMFlag = false; //Safe-to-merge flag
    iCLCM_msg->setMergingFlag(Merging_flag); //bool mergingFlag = false;  //Merging status of vehicle: gaps are made and indicate vehicle is ready for merging(?)
    iCLCM_msg->setFWDPairID(myFWDPairID); //ID of the forward pair, 0 mean no pair
    iCLCM_msg->setBWDPairID(myBWDPairID); //ID of the backward pair, 0 mean no pair
    iCLCM_msg->setHeadVehicle(headVehicleFlag);
    iCLCM_msg->setTailVehicle(tailVehicleFlag);
    //bool tailVehicle = false; //Last vehicle in platoon?
    //bool headVehicle = false; //Leader of the platoon? it should merge first
    iCLCM_msg->setPlatoonID(2-traciVehicle->getLaneIndex()); //PlatoonA = 1, PlatoonB = 2
    iCLCM_msg->setKind(iCLCM_TYPE);
    iCLCM_unicast->encapsulate(iCLCM_msg);
    sendDown(iCLCM_unicast);  //send up to the application layer
}

void PlatoonMergingProtocol::messageReceived(PlatooningBeacon *pkt, UnicastMessage *unicast) {
    //nothing to do for static beaconing
}

PlatoonMergingProtocol::~PlatoonMergingProtocol()
{}

void PlatoonMergingProtocol::finish(){
    BaseProtocol::finish();
    if (sendSTOMmsg) {
        if (sendSTOMmsg->isScheduled()) {
            cancelEvent(sendSTOMmsg);
        }
        delete sendSTOMmsg;
    }
    if (startMerge) {
        if (startMerge->isScheduled()) {
            cancelEvent(startMerge);
        }
        delete startMerge;
    }
}

void PlatoonMergingProtocol::handleUpperMsg(cMessage *msg) {
    UnicastMessage *unicast;
    unicast = dynamic_cast<UnicastMessage *>(msg);
    assert(unicast);


    if (unicast->getType() == UnicastMessageType::iCLCM) {  //get some updates before sending down
        cPacket *enc = unicast->decapsulate();
        ICLCM *iclcm_pkt = dynamic_cast<ICLCM *>(enc);
        myMIO_ID = iclcm_pkt->getMIO_ID();
        myFWDPairID = iclcm_pkt->getFWDPairID();
        myBWDPairID = iclcm_pkt->getBWDPairID();
        Merging_flag = iclcm_pkt->getMergingFlag();
        STOM_flag = iclcm_pkt->getSTOMFlag();
        headVehicleFlag = iclcm_pkt->getHeadVehicle();
        tailVehicleFlag = iclcm_pkt->getTailVehicle();
        delete unicast;
        delete enc;
    }
    else {
        sendDown(msg);
    }
}

