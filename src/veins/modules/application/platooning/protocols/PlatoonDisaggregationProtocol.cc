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

#include "PlatoonDisaggregationProtocol.h"

Define_Module(PlatoonDisaggregationProtocol)

void PlatoonDisaggregationProtocol::initialize(int stage) {
    BaseProtocol::initialize(stage);

    if (stage == 0) {
        //ALWAYS initialize everything inside "stage"!!!
        checkPos = new cMessage("checkPos");

        ReqRampPosX = 9610; //Position of the ramp to take (x)
        ReqRampPosY = 0;    //Position of the ramp to take (y) [n/a]
        requestSent = false;
        requestStat.setName("RequestStat");
        //random start time
        SimTime beginTime = SimTime(uniform(0.001, 1.0));
        SimTime offset = SimTime(uniform(0.001,1.0));
        //scheduleAt(simTime() + beaconingInterval + beginTime, sendBeacon);
        if(myPlatoonName.find("car") != std::string::npos) {
            scheduleAt(simTime()  + beaconingInterval + beginTime + offset, checkPos);
        }
        else {
            scheduleAt(simTime() + beaconingInterval + beginTime, sendBeacon);
        }

    }
}

void PlatoonDisaggregationProtocol::handleSelfMsg(cMessage *msg) {

    BaseProtocol::handleSelfMsg(msg);

    requestStat.record(requestSent);
    //First vehicle might have left the simulation (through off-ramp), so check here as well
    if (myId == 1 && simTime() > communicationDuration) {
        endSimulation();
    }

    if (msg == sendBeacon) {
        sendPlatooningMessage(-1);
        scheduleAt(simTime() + beaconingInterval, sendBeacon);
    }

    if (msg == checkPos) {
        double acceleration, speed, controllerAcceleration, posX, posY, time;
        traciVehicle->getVehicleData(speed, acceleration, controllerAcceleration, posX, posY, time);

        if(((ReqRampPosX - posX) < 300) && (posX < ReqRampPosX) && !requestSent) {    //It's before the ramp within 1 km
            sendReqRamp(-1);
            requestSent = true;
        }
        else {
            scheduleAt(simTime() + SimTime(1), checkPos);
        }
    }
}

void PlatoonDisaggregationProtocol::finish(){
    BaseProtocol::finish();
    if (checkPos) {
        if (checkPos->isScheduled()) {
            cancelEvent(checkPos);
        }
        delete checkPos;
    }
}

//void PlatoonDisaggregationProtocol::handleUpperMsg(cMessage *msg) { //Handle messages from "Application" layer
//    UnicastMessage *unicast;
//    unicast = dynamic_cast<UnicastMessage *>(msg);
//    assert(unicast);
//
//    if (unicast->getType() == UnicastMessageType::iCLCM) {  //get some updates before sending down
//        cPacket *enc = unicast->decapsulate();
//        ICLCM *iclcm_pkt = dynamic_cast<ICLCM *>(enc);
//        myMIO_ID = iclcm_pkt->getMIO_ID();
//        myFWDPairID = iclcm_pkt->getFWDPairID();
//        myBWDPairID = iclcm_pkt->getBWDPairID();
//        Merging_flag = iclcm_pkt->getMergingFlag();
//        STOM_flag = iclcm_pkt->getSTOMFlag();
//        headVehicleFlag = iclcm_pkt->getHeadVehicle();
//        tailVehicleFlag = iclcm_pkt->getTailVehicle();
//        delete unicast;
//        delete enc;
//    }
//    else {
//        sendDown(msg);
//    }
//}

void PlatoonDisaggregationProtocol::sendReqRamp(int destinationAddress) {
    //vehicle's data to be included in the message
    double speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime;

    //actual packets
    UnicastMessage *reqRamp_unicast;
    RequestRamp *reqRamp_msg;

    //get information about the vehicle via traci
    traciVehicle->getVehicleData(speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime);
    //get current vehicle position
    Coord veinsPosition = mobility->getPositionAt(simTime());
    double veinsTime = simTime().dbl();

    //TODO: use veins or sumo data?
    Coord position(veinsPosition.x, veinsPosition.y, veinsPosition.z);
    double time = veinsTime;

    //create and send beacon
    reqRamp_unicast = new UnicastMessage("", REQ_RAMP);
    reqRamp_unicast->setDestination(-1);
    reqRamp_unicast->setPriority(priority);
    reqRamp_unicast->setType(6);   //REQ_RAMP = 6

    //create platooning beacon with data about the car
    reqRamp_msg = new RequestRamp();
    if (useControllerAcceleration) {
        reqRamp_msg->setAcceleration(controllerAcceleration);
    }
    else {
        reqRamp_msg->setAcceleration(acceleration);
    }
    reqRamp_msg->setSpeed(speed);
    reqRamp_msg->setVehicleId(99);  //Let's fix it to 99?
    //send SUMO position instead
    reqRamp_msg->setPositionX(position.x);
    reqRamp_msg->setPositionY(position.y);
    reqRamp_msg->setSUMOpositionX(sumoPosX);
    reqRamp_msg->setSUMOpositionY(sumoPosY);
    //set the time to now
    reqRamp_msg->setTime(time);
    //i generated the message, i send it
    reqRamp_msg->setRelayerId(myId);    //This is OMNeT++ id
    reqRamp_msg->setKind(REQRAMP_TYPE);
    reqRamp_msg->setSequenceNumber(seq_n++);

    reqRamp_msg->setRampX(ReqRampPosX);
    reqRamp_msg->setRampY(ReqRampPosY);

    //put content into the message for the UnicastProtocol
    reqRamp_unicast->encapsulate(reqRamp_msg);
    sendDown(reqRamp_unicast);
}

void PlatoonDisaggregationProtocol::messageReceived(PlatooningBeacon *pkt, UnicastMessage *unicast) {
    //nothing to do for static beaconing
}

PlatoonDisaggregationProtocol::~PlatoonDisaggregationProtocol() {
    // TODO Auto-generated destructor stub
}

