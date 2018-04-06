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

#include "veins/modules/application/platooning/apps/IntersectionApp.h"

#include "veins/modules/messages/WaveShortMessage_m.h"
#include "veins/base/messages/MacPkt_m.h"
#include "veins/modules/mac/ieee80211p/Mac1609_4.h"

#include "veins/modules/application/platooning/protocols/BaseProtocol.h"

Define_Module(IntersectionApp);

void IntersectionApp::initialize(int stage) {
	BaseApp::initialize(stage);

	if (stage == 1) {
	    ipAddress = par("IP_DS").stdstringValue();
	    if (traciVehicle->getVType() == "manual_car") {
            traciVehicle->setActiveController(Plexe::CC);
            ds_control = Veins::TraCIConnection::connect(ipAddress.c_str(), 8855);
            readDS = new cMessage("readDS");
            scheduleAt(simTime() + SimTime(0.01), readDS);
            std::cout << "found a manual car" << std::endl;
        }
        else {
            readDS = 0;
	    }

	    //Register intersection type
	    protocol->registerApplication(BaseProtocol::INTER_TYPE, gate("lowerLayerIn"), gate("lowerLayerOut"));
	}
}

void IntersectionApp::finish() {
	BaseApp::finish();
	if (readDS) {
        cancelAndDelete(readDS);
        readDS = 0;
	}
}

void IntersectionApp::onData(WaveShortMessage *wsm) {
}

void IntersectionApp::handleSelfMsg(cMessage *msg) {
	BaseApp::handleSelfMsg(msg);
	if (msg == readDS) {
        uint8_t read_a_byte;
        double speed;
        int laneID = 0;
        int intention = 0;
        std::string ds_resp;
        double radar_distance, rel_speed;

        traciVehicle->getRadarMeasurements(radar_distance, rel_speed);

        //send command to request control values from ds (basically speed and laneID of ego vehicle)
        ds_control->sendTCPMessage(Veins::makeTraCICommand(0x40, Veins::TraCIBuffer()));
        ds_resp = ds_control->receiveMessage();
        Veins::TraCIBuffer buf = Veins::TraCIBuffer(ds_resp);
        buf >> read_a_byte;
        buf >> read_a_byte;
        buf >> read_a_byte;
        buf >> read_a_byte;
        buf >> read_a_byte;
        buf >> speed;
        buf >> laneID;
        buf >> intention;

        //Control the vehicle with received speed
        traciVehicle->setSpeed(speed);
        traciVehicle->setFixedLane(laneID);
        if(intention != 0) {
            //do something with the received intention
            // ADD THE PLATOON HERE
            std::cout << "should be adding the platoon here !!!!!!!!!!!!!!!!!!!!!!!!!!! " << std::endl;
            std::string route_id = traciVehicle->getRouteId();
            traci->addVehicle("obstacle", "obstacle_car", route_id, simTime(), 10, 0, 0);
        }


        /*gap_v.record(radar_distance);
        speed_fake_controller.record(speed);*/

        scheduleAt(simTime() + SimTime(0.01), readDS);
	}
}

void IntersectionApp::onBeacon(WaveShortMessage* wsm) {
}

void IntersectionApp::handleLowerMsg(cMessage *msg) {

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
        if(positionHelper->isInSamePlatoon(epkt->getVehicleId())) {
            //if the message comes from the pace maker (OPC), position 0 in the platoon
            if (epkt->getVehicleId() == positionHelper->getLeaderId()) {
                traciVehicle->setPlatoonLeaderData(epkt->getSpeed(), epkt->getAcceleration(), epkt->getPositionX(), epkt->getPositionY(), epkt->getTime());
            }
        }
        else {
            //This message is from a car from the other platoon
        }

    }
    else if (enc->getKind() == BaseProtocol::INTER_TYPE) {
        Intersection *intersect_pkt = dynamic_cast<Intersection *>(enc);
        //do stuff here
        //std::cout << "got a message from " << intersect_pkt->getVehicleId() << " it is on the route " << intersect_pkt->getCurrentRoad() << " with intention 0 " << intersect_pkt->getIntention() << std::endl;
    }

    delete enc;
    delete unicast;
}
