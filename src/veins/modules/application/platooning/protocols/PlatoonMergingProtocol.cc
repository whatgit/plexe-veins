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
        sendSTOMmsg = new cMessage("sendSTOMmsg");
        //random start time
        SimTime beginTime = SimTime(uniform(0.001, 1.0));
        scheduleAt(simTime() + beaconingInterval + beginTime, sendBeacon);
        scheduleAt(simTime() + SimTime(70), sendSTOMmsg);
    }
}

void PlatoonMergingProtocol::handleSelfMsg(cMessage *msg) {

    BaseProtocol::handleSelfMsg(msg);

    if (msg == sendBeacon) {
        sendPlatooningMessage(-1);
        scheduleAt(simTime() + beaconingInterval, sendBeacon);
    }
    if (msg == sendSTOMmsg) {
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
}

