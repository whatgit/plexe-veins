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
            oppositeDrive = true;
            scheduleAt(simTime() + SimTime(0.01), readDS);
            std::cout << "found a manual car" << std::endl;
        }
        else {
            readDS = 0;
	    }
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
        if (oppositeDrive) {
            if (laneID == 1) {
                traciVehicle->setFixedLane(-1);
                traciVehicle->changeRoute("e2",10);
                std::cout << "trying to change lane from " << traciVehicle->getRoadId() << std::endl;
            }
            else {
                traciVehicle->changeRoute("e1",10);
            }
        }
        else {
            traciVehicle->setFixedLane(laneID);
        }

        //if(intention != 0)  TriggerDENM(intention, laneID);

        //do something with the received intention
        /*gap_v.record(radar_distance);
        speed_fake_controller.record(speed);*/

        scheduleAt(simTime() + SimTime(0.01), readDS);
	}
}

void ManualDrivingApp::onBeacon(WaveShortMessage* wsm) {
}
