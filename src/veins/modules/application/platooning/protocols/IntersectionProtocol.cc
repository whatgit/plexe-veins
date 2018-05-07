//
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

#include "IntersectionProtocol.h"

#include "veins/modules/mac/ieee80211p/Mac1609_4.h"

Define_Module(IntersectionProtocol)

void IntersectionProtocol::initialize(int stage) {
	BaseProtocol::initialize(stage);

	if (stage == 0) {

	    sendIntersecion = new cMessage("sendIntersecion");

       //random start time
       SimTime beginTime = SimTime(uniform(0.001, 1.0));
       SimTime offset = SimTime(uniform(0.001,1.0));
       scheduleAt(simTime() + beaconingInterval + beginTime, sendBeacon);
       scheduleAt(simTime() + beaconingInterval + beginTime + offset, sendIntersecion);

	}
}

void IntersectionProtocol::handleSelfMsg(cMessage *msg) {

	BaseProtocol::handleSelfMsg(msg);
	if (msg == sendBeacon) {
        sendPlatooningMessage(-1);
        scheduleAt(simTime() + beaconingInterval, sendBeacon);
    }
	if (msg == sendIntersecion) {
	    sendIntersectionMessage(-1);
        scheduleAt(simTime() + beaconingInterval, sendIntersecion);
    }

}

void IntersectionProtocol::sendIntersectionMessage(int destinatinAddress) {

    //vehicle's data to be included in the message
    double speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime;

    UnicastMessage *Intersection_unicast;
    Intersection *Intersection_msg;

    Intersection_unicast = new UnicastMessage("", INTER_TYPE);
    Intersection_unicast->setDestination(-1);
    Intersection_unicast->setPriority(priority);
    Intersection_unicast->setChannel(Channels::CCH);

    traciVehicle->getVehicleData(speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime);
    Intersection_msg = new Intersection();

    Intersection_msg->setSpeed(speed);
    Intersection_msg->setVehicleId(myId);
    Intersection_msg->setSUMOpositionX(sumoPosX);
    Intersection_msg->setSUMOpositionY(sumoPosY);
    //i generated the message, i send it
    Intersection_msg->setRelayerId(myId);
    Intersection_msg->setTime(simTime().dbl());
    Intersection_msg->setByteLength(packetSize);
    Intersection_msg->setSequenceNumber(seq_n++);
    Intersection_msg->setMyPlatoonID(positionHelper->getPlatoonId());

    Intersection_msg->setKind(INTER_TYPE);
    Intersection_unicast->encapsulate(Intersection_msg);
    sendDown(Intersection_unicast);  //send up to the application layer
}

void IntersectionProtocol::messageReceived(PlatooningBeacon *pkt, UnicastMessage *unicast) {
	//nothing to do for static beaconing
}


IntersectionProtocol::~IntersectionProtocol()
{}

void IntersectionProtocol::finish(){
	BaseProtocol::finish();
}
