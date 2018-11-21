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

#include "veins/modules/application/platooning/apps/ManualDrivingApp.h"

#include "veins/modules/messages/WaveShortMessage_m.h"
#include "veins/base/messages/MacPkt_m.h"
#include "veins/modules/mac/ieee80211p/Mac1609_4.h"

#include "veins/modules/application/platooning/protocols/BaseProtocol.h"

Define_Module(ManualDrivingApp);

void ManualDrivingApp::initialize(int stage) {
	BaseApp::initialize(stage);

	if (stage == 1) {
	    ipAddress = par("IP_DS").stdstringValue();
	    if (traciVehicle->getVType() == "manual_car") {
            traciVehicle->setActiveController(Plexe::CC);
            ds_control = Veins::TraCIConnection::connect(ipAddress.c_str(), 8855);
            readDS = new cMessage("readDS");
            scheduleAt(simTime() + SimTime(0.01), readDS);
            std::cout << "found a manual car" << std::endl;
            time_gap = -1.00;
        }
        else {
            readDS = 0;
	    }
	    receivedDelay.setName("receivedDelay");
	    max_delay_step= 0;
	    delay_counter = 0;
	    distance_to_preceding = -1.0;
	}
}

void ManualDrivingApp::finish() {
	BaseApp::finish();
	if (readDS) {
        cancelAndDelete(readDS);
        readDS = 0;
	}
}

void ManualDrivingApp::onData(WaveShortMessage *wsm) {
}

void ManualDrivingApp::handleSelfMsg(cMessage *msg) {
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
        //ds_control->sendTCPMessage(Veins::makeTraCICommand(0x40, Veins::TraCIBuffer()));  //original
        ds_control->sendTCPMessage(Veins::makeTraCICommand(0x40, Veins::TraCIBuffer() << time_gap));
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

        //speed = 10;

        //Control the vehicle with received speed
        traciVehicle->setSpeed(speed);
        //For NGEA2 lane ID is always 0 and we use received lane ID as distance instead
        //std::cout << "receiving the distance of " << laneID << std::endl;
        distance_to_preceding = laneID;
        traciVehicle->setFixedLane(0);
        if(intention >= 0) {
            if(intention == 0){
                traci->vehicle("platoon_vehicle.0").setCACCConstantSpacing(20);
                //traci->vehicle("platoon_vehicle.1").setCACCConstantSpacing(20);
                //traci->vehicle("platoon_vehicle.2").setCACCConstantSpacing(20);

                if(max_delay_step == 0) {

                }
                else {
                    while(fake_delay_speed_queue.size() != 0) fake_delay_speed_queue.pop();
                    max_delay_step= 0;
                    delay_counter = 0;
                }

            }
            else if(intention == 1){
                traci->vehicle("platoon_vehicle.0").setCACCConstantSpacing(15);
                //traci->vehicle("platoon_vehicle.1").setCACCConstantSpacing(15);
                //traci->vehicle("platoon_vehicle.2").setCACCConstantSpacing(15);

                if(max_delay_step == 3) {

                }
                else {
                    while(fake_delay_speed_queue.size() != 0) fake_delay_speed_queue.pop();
                    max_delay_step = 3;
                    delay_counter = 0;
                }

            }
            else if(intention == 2){
                traci->vehicle("platoon_vehicle.0").setCACCConstantSpacing(30);
                //traci->vehicle("platoon_vehicle.1").setCACCConstantSpacing(30);
                //traci->vehicle("platoon_vehicle.2").setCACCConstantSpacing(30);

                if(max_delay_step == 5) {

                }
                else {
                    while(fake_delay_speed_queue.size() != 0) fake_delay_speed_queue.pop();
                    max_delay_step = 5;
                    delay_counter = 0;
                }
            }
            else if(intention == 3){
                traci->vehicle("platoon_vehicle.0").setCACCConstantSpacing(45);
                //traci->vehicle("platoon_vehicle.1").setCACCConstantSpacing(45);
                //traci->vehicle("platoon_vehicle.2").setCACCConstantSpacing(45);

                if(max_delay_step == 7) {

                }
                else {
                    while(fake_delay_speed_queue.size() != 0) fake_delay_speed_queue.pop();
                    max_delay_step = 7;
                    delay_counter = 0;
                }
            }
            //do something with the received intention
            // ADD THE PLATOON HERE
            //std::cout << "should be adding the platoon here !!!!!!!!!!!!!!!!!!!!!!!!!!! " << std::endl;
        }
        else {
            //stopSimulation();
        }


        /*gap_v.record(radar_distance);
        speed_fake_controller.record(speed);*/

        scheduleAt(simTime() + SimTime(0.01), readDS);
	}
}

void ManualDrivingApp::onBeacon(WaveShortMessage* wsm) {
}

void ManualDrivingApp::handleLowerMsg(cMessage *msg) {

    UnicastMessage *unicast = dynamic_cast<UnicastMessage *>(msg);
    ASSERT2(unicast, "received a frame not of type UnicastMessage");

    cPacket *enc = unicast->decapsulate();
    ASSERT2(enc, "received a UnicastMessage with nothing inside");

    //our vehicle's data
    double speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime, distance, relSpeed;

    //received data
    double receivedPosX;
    traciVehicle->getVehicleData(speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime);
    traciVehicle->getRadarMeasurements(distance, relSpeed);
    if (enc->getKind() == BaseProtocol::BEACON_TYPE) {  //Similar to BaseApp
        PlatooningBeacon *epkt = dynamic_cast<PlatooningBeacon *>(enc);
        ASSERT2(epkt, "received UnicastMessage does not contain a PlatooningBeacon");
        //receivedDelay.record(simTime().dbl() - epkt->getTime());
        if((epkt->getVehicleId() == 0)){
            //std::cout << "getting packets from vehicle that is at : " << epkt->getPositionX() << '\n';
            fake_delay_speed_queue.push(epkt->getPositionX());
            if(delay_counter < max_delay_step){
                delay_counter = delay_counter + 1;
            }
            else{
                receivedPosX = fake_delay_speed_queue.front();
                std::cout << "before popping size: " << fake_delay_speed_queue.size() << '\n';

                if((distance_to_preceding <= 100) && (receivedPosX > sumoPosX) && (distance_to_preceding > 0)) {
                    std::cout << "there is some time gap" << '\n';
                    time_gap = (receivedPosX - (sumoPosX + distance_to_preceding))/speed;
                }
                else{
                    std::cout << "just sending -1" << '\n';
                    time_gap = -1.0;
                }
                std::cout << "clearing the queue" << '\n';
                while(fake_delay_speed_queue.size() != 0) fake_delay_speed_queue.pop();
                delay_counter = 0;
            }

        }
        else{

        }
        //std::cout << "received a message from vehicle id: " << epkt->getVehicleId() << ", at the position: " <<  epkt->getPositionX() << std::endl;
        //
        //Loggin below does not work with the fake delay for some reason
        //receivedDelay.record(simTime().dbl() - epkt->getTime());
    }

    delete enc;
    delete unicast;
}
