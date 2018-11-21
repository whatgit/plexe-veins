//
// Copyright (C) 2018 Maytheewat Aramrattana <maytheewat.aramrattana@vti.se>
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

#include "CEBP.h"

#include "veins/modules/mac/ieee80211p/Mac1609_4.h"

Define_Module(CEBP)

void CEBP::initialize(int stage) {
    BaseProtocol::initialize(stage);

    if (stage == 0) {

       TriggerBrakeACK = new cMessage("TriggerBrakeACK");
       sendBrakeRequest = new cMessage("sendBrakeRequest");
       sendBrakeAck = new cMessage("sendBrakeAck");

       //random start time
       SimTime beginTime = SimTime(uniform(0.001, 1.0));
       SimTime offset = SimTime(uniform(0.001,1.0));
       scheduleAt(simTime() + beaconingInterval + beginTime, sendBeacon);

    }
}

void CEBP::handleSelfMsg(cMessage *msg) {

    BaseProtocol::handleSelfMsg(msg);
    if (msg == sendBeacon) {
        sendPlatooningMessage(-1);
        scheduleAt(simTime() + beaconingInterval, sendBeacon);
    }
    if (msg == sendBrakeRequest) {
        sendEBrakeRequest(-1);
    }
    if (msg == sendBrakeAck) {
        sendEBrakeAck(-1);
    }

}

void CEBP::sendEBrakeRequest(int destinationAddress) {

    //vehicle's data to be included in the message
    double speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime;

    UnicastMessage *eBReq_unicast;
    eBrakeRequest *eBReq_msg;

    eBReq_unicast = new UnicastMessage("", EBREQ_TYPE);
    eBReq_unicast->setDestination(-1);
    eBReq_unicast->setPriority(priority);
    eBReq_unicast->setChannel(Channels::CCH);

    traciVehicle->getVehicleData(speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime);
    eBReq_msg = new eBrakeRequest();
    eBReq_msg->setVehicleId(myId); //my vehicle id (OMNeT++ id in this case)

    eBReq_msg->setKind(EBREQ_TYPE);
    eBReq_unicast->encapsulate(eBReq_msg);
    sendDown(eBReq_unicast);  //send down to PHY layer
}

void CEBP::sendEBrakeAck(int destinationAddress) {

    //vehicle's data to be included in the message
    double speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime;

    UnicastMessage *eBAck_unicast;
    eBrakeAck *eBAck_msg;

    eBAck_unicast = new UnicastMessage("", EBACK_TYPE);
    eBAck_unicast->setDestination(-1);
    eBAck_unicast->setPriority(priority);
    eBAck_unicast->setChannel(Channels::CCH);

    traciVehicle->getVehicleData(speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime);
    eBAck_msg = new eBrakeAck();
    eBAck_msg->setVehicleId(myId); //my vehicle id (OMNeT++ id in this case)

    eBAck_msg->setKind(EBACK_TYPE);
    eBAck_unicast->encapsulate(eBAck_msg);
    sendDown(eBAck_unicast);  //send down to PHY layer
}

void CEBP::sendEBrakeDirectly(int destinationAddress) {
    //vehicle's data to be included in the message
    double speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime;

    UnicastMessage *eBDir_unicast;
    eBrakeDirectly *eBDir_msg;

    eBDir_unicast = new UnicastMessage("", EBDIR_TYPE);
    eBDir_unicast->setDestination(-1);
    eBDir_unicast->setPriority(priority);
    eBDir_unicast->setChannel(Channels::CCH);

    traciVehicle->getVehicleData(speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime);
    eBDir_msg = new eBrakeDirectly();
    eBDir_msg->setVehicleId(myId); //my vehicle id (OMNeT++ id in this case)

    eBDir_msg->setKind(EBDIR_TYPE);
    eBDir_unicast->encapsulate(eBDir_msg);
    sendDown(eBDir_unicast);  //send down to PHY layer
}


void CEBP::messageReceived(PlatooningBeacon *pkt, UnicastMessage *unicast) {
    //nothing to do for static beaconing
}

void CEBP::handleUpperMsg(cMessage *msg) {
//    UnicastMessage *unicast;
//    unicast = dynamic_cast<UnicastMessage *>(msg);
//    assert(unicast);

    if (msg->getKind() == 'r') {  //get some updates before sending down
        std::cout << "Received BrakeRequest" << std::endl;
        scheduleAt(simTime() + 0.01, sendBrakeRequest);
    }
    else if (msg->getKind() == 'a') {
        std::cout << "Received ack" << std::endl;
        scheduleAt(simTime() + 0.01, sendBrakeAck);
    }
    else {
        sendDown(msg);
    }
}

CEBP::~CEBP()
{}

void CEBP::finish(){
    BaseProtocol::finish();
}
