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

#include "veins/modules/application/platooning/apps/EmergencyBrakingApp.h"

#include "veins/modules/messages/WaveShortMessage_m.h"
#include "veins/base/messages/MacPkt_m.h"
#include "veins/modules/mac/ieee80211p/Mac1609_4.h"

#include "veins/modules/application/platooning/protocols/BaseProtocol.h"
#include "veins/modules/application/platooning/scenarios/BaseScenario.h"

Define_Module(EmergencyBrakingApp);

void EmergencyBrakingApp::initialize(int stage) {
    BaseApp::initialize(stage);

    if (stage == 1) {

        //start braking time
        startBraking = SimTime(par("startBraking").doubleValue());
        brakingDeceleration = 8.00;

        if (positionHelper->getId() < positionHelper->getLanesCount()) {
            startAccelerationMsg = new cMessage("startAccelerationMsg");
            startBrakingMsg = new cMessage("startBrakingMsg");
            //for the leader
            //enable ACC
            //traciVehicle->setActiveController(Plexe::ACC);
            traciVehicle->setCruiseControlDesiredSpeed(100/3.6);

            //let the vehicle start from standstill
            //traciVehicle->setFixedAcceleration(1, -8);

            //schedule messages
            //scheduleAt(startAccelerating, startAccelerationMsg);
            scheduleAt(startBraking, startBrakingMsg);
        }
        else {
            //for the follower
            startAccelerationMsg = 0;
            startBrakingMsg = 0;
            traciVehicle->setCruiseControlDesiredSpeed((100/3.6)+10);
        }

        //Register application !!, otherwise no message will be received
        protocol->registerApplication(BaseProtocol::EBREQ_TYPE, gate("lowerLayerIn"), gate("lowerLayerOut"));
        protocol->registerApplication(BaseProtocol::EBACK_TYPE, gate("lowerLayerIn"), gate("lowerLayerOut"));
        protocol->registerApplication(BaseProtocol::EBDIR_TYPE, gate("lowerLayerIn"), gate("lowerLayerOut"));
    }
}

void EmergencyBrakingApp::finish() {
    cancelAndDelete(startAccelerationMsg);
    cancelAndDelete(startBrakingMsg);
    startAccelerationMsg = 0;
    startBrakingMsg = 0;
    brakeRequest = 0;
    BaseApp::finish();
}

void EmergencyBrakingApp::onData(WaveShortMessage *wsm) {
}

void EmergencyBrakingApp::handleSelfMsg(cMessage *msg) {
    BaseApp::handleSelfMsg(msg);


    if (msg == startBrakingMsg) {
        brakeRequest = new cMessage("BrakeRequest");
        brakeRequest->setKind('r');
        sendDown(brakeRequest);
    }
}

void EmergencyBrakingApp::onBeacon(WaveShortMessage* wsm) {
}

void EmergencyBrakingApp::handleLowerMsg(cMessage *msg) {

    UnicastMessage *unicast = dynamic_cast<UnicastMessage *>(msg);
    ASSERT2(unicast, "received a frame not of type UnicastMessage");

    cPacket *enc = unicast->decapsulate();
    ASSERT2(enc, "received a UnicastMessage with nothing inside");

    //our vehicle's data
    double speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime, distance, relSpeed;
    traciVehicle->getVehicleData(speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime);
    traciVehicle->getRadarMeasurements(distance, relSpeed);
    if (enc->getKind() == BaseProtocol::BEACON_TYPE) {

        PlatooningBeacon *epkt = dynamic_cast<PlatooningBeacon *>(enc);
        ASSERT2(epkt, "received UnicastMessage does not contain a PlatooningBeacon");

        if (positionHelper->isInSamePlatoon(epkt->getVehicleId())) {
            //if the message comes from the leader
            if (epkt->getVehicleId() == positionHelper->getLeaderId()) {
                traciVehicle->setPlatoonLeaderData(epkt->getSpeed(), epkt->getAcceleration(), epkt->getPositionX(), epkt->getPositionY(), epkt->getTime());
            }
            //if the message comes from the vehicle in front
            if (epkt->getVehicleId() == positionHelper->getFrontId()) {
                traciVehicle->setPrecedingVehicleData(epkt->getSpeed(), epkt->getAcceleration(), epkt->getPositionX(), epkt->getPositionY(), epkt->getTime());
            }
            //send data about every vehicle to the CACC. this is needed by the consensus controller
            struct Plexe::VEHICLE_DATA vehicleData;
            vehicleData.index = positionHelper->getMemberPosition(epkt->getVehicleId());
            vehicleData.acceleration = epkt->getAcceleration();
            //for now length is fixed to 4 meters. TODO: take it from sumo
            vehicleData.length = 4;
            vehicleData.positionX = epkt->getPositionX();
            vehicleData.positionY = epkt->getPositionY();
            vehicleData.speed = epkt->getSpeed();
            vehicleData.time = epkt->getTime();
            //send information to CACC
            traciVehicle->setGenericInformation(CC_SET_VEHICLE_DATA, &vehicleData, sizeof(struct Plexe::VEHICLE_DATA));

        }

    }
    else if (enc->getKind() == BaseProtocol::EBACK_TYPE) {
        //What if we get "E-brake ACK" message?
        eBrakeAck *eBAck_pkt = dynamic_cast<eBrakeAck *>(enc);

        std::cout << "vehicle " << myId << " receiving ACK from " << eBAck_pkt->getVehicleId() << std::endl;
        // Is it from the vehicle behind us?
        if(eBAck_pkt->getVehicleId() == myId+1) {
            std::cout << "vehicle " << myId << " sending ACK" << std::endl;
            //start braking
            traciVehicle->setFixedAcceleration(1, -brakingDeceleration);
            //send ACK
            sendACKRequest = new cMessage("TriggerBrakeACK");
            sendACKRequest->setKind('a');
            sendDown(sendACKRequest);
        }
        else {
            //prepare to brake
        }

    }
    else if(enc->getKind() == BaseProtocol::EBDIR_TYPE) {
        //What if we get "E-brake directly" message?
        eBrakeDirectly *eBDir_pkt = dynamic_cast<eBrakeDirectly *>(enc);
    }
    else if(enc->getKind() == BaseProtocol::EBREQ_TYPE) {
        //What if we get "E-brake request" message?
        eBrakeRequest *eBReq_pkt = dynamic_cast<eBrakeRequest *>(enc);

        //Are we the last vehicle?
        if (positionHelper->getId() == positionHelper->getHighestId()) {
            //start braking
            traciVehicle->setFixedAcceleration(1, -brakingDeceleration);
            //send ACK
            sendACKRequest = new cMessage("TriggerBrakeACK");
            sendACKRequest->setKind('a');
            sendDown(sendACKRequest);
        }
        else {
            //prepare to brake
        }
    }

    delete enc;
    delete unicast;
}
