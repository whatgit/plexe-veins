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

#include "veins/modules/application/platooning/apps/CutInApp.h"

#include "veins/modules/messages/WaveShortMessage_m.h"
#include "veins/base/messages/MacPkt_m.h"
#include "veins/modules/mac/ieee80211p/Mac1609_4.h"

#include "veins/modules/application/platooning/protocols/BaseProtocol.h"

Define_Module(CutInApp);

void CutInApp::initialize(int stage) {
	BaseApp::initialize(stage);

	if (stage == 1) {
	    if (positionHelper->getId() < positionHelper->getLanesCount()) {
	        //std::cout << "ADDING VEHICLE" << std::endl;
	        //traci->addVehicle("human0", "human_car", "manual", simTime()+3, 7,0, 0);
	        //traci->addVehicle("human1", "human_car", "manual", simTime()+3, 0,0, 0);
        }
        else if (traciVehicle->getVType() == "manual_car"){
            std::cout << "ADDING MANUAL VEHICLE" << std::endl;
            traciVehicle->setActiveController(Plexe::DRIVER);

	    }
        else {

        }
	}
}

void CutInApp::finish() {
	BaseApp::finish();
}

void CutInApp::onData(WaveShortMessage *wsm) {
}

void CutInApp::handleSelfMsg(cMessage *msg) {
	BaseApp::handleSelfMsg(msg);
}

void CutInApp::onBeacon(WaveShortMessage* wsm) {
}
