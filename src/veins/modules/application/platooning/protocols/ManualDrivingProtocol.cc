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

#include "ManualDrivingProtocol.h"

Define_Module(ManualDrivingProtocol)

void ManualDrivingProtocol::initialize(int stage) {
    BaseProtocol::initialize(stage);


    if (stage == 0) {
            //random start time
            SimTime beginTime = SimTime(uniform(0.001, 1.0));
            //send beacon messages only if I am in platoon
            if(myPlatoonName.find("platoon") != std::string::npos) scheduleAt(simTime() + beaconingInterval + beginTime, sendBeacon);
            sendDENM_msg = new cMessage("sendDENM_msg");
            intention = 0;
        }
}

void ManualDrivingProtocol::handleSelfMsg(cMessage *msg) {

    BaseProtocol::handleSelfMsg(msg);

    if (msg == sendBeacon) {
        sendPlatooningMessage(-1);
        scheduleAt(simTime() + beaconingInterval, sendBeacon);
    }
    if (msg == sendDENM_msg) {
        //sendDENM(-1);
    }

}

void ManualDrivingProtocol::sendDENM(int destinatinAddress, bool withRecvData) {

    UnicastMessage *DENM_msg_unicast;
    DENM *DENM_msg;

    DENM_msg_unicast = new UnicastMessage("", DENM_event);
    DENM_msg_unicast->setDestination(-1);
    DENM_msg_unicast->setPriority(priority);
    DENM_msg_unicast->setType(6);
    DENM_msg = new DENM();
    if(withRecvData) {
        DENM_msg->setIntention(intention);
        DENM_msg->setMyLane(myLane); //Right lane = 1, Left lane = 2
    }
    else {
        DENM_msg->setIntention(intention);
        DENM_msg->setMyLane(2-traciVehicle->getLaneIndex()); //Right lane = 1, Left lane = 2
    }

    DENM_msg->setKind(DENM_TYPE);
    DENM_msg_unicast->encapsulate(DENM_msg);
    sendDown(DENM_msg_unicast);  //send down to lower layer
}

void ManualDrivingProtocol::messageReceived(PlatooningBeacon *pkt, UnicastMessage *unicast) {
    //nothing to do for static beaconing
}

ManualDrivingProtocol::~ManualDrivingProtocol()
{}

void ManualDrivingProtocol::finish(){
    BaseProtocol::finish();
    if (sendDENM_msg) {
        if (sendDENM_msg->isScheduled()) {
            cancelEvent(sendDENM_msg);
        }
        delete sendDENM_msg;
    }
}

void ManualDrivingProtocol::handleUpperMsg(cMessage *msg) {
    UnicastMessage *unicast;
    unicast = dynamic_cast<UnicastMessage *>(msg);
    assert(unicast);

    if (unicast->getType() == UnicastMessageType::DENM_event) {  //get some updates before sending down
        cPacket *enc = unicast->decapsulate();
        DENM *denm_pkt = dynamic_cast<DENM *>(enc);
        intention = denm_pkt->getIntention();
        myLane = denm_pkt->getMyLane();
        sendDENM(-1, true);
        delete unicast;
        delete enc;
    }
    else {
        sendDown(msg);
    }
}

