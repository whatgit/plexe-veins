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

#include "veins/modules/application/platooning/apps/PlatoonDisaggregationApp.h"

#include "veins/modules/messages/WaveShortMessage_m.h"
#include "veins/base/messages/MacPkt_m.h"
#include "veins/modules/mac/ieee80211p/Mac1609_4.h"

#include "veins/modules/application/platooning/protocols/BaseProtocol.h"

Define_Module(PlatoonDisaggregationApp);

void PlatoonDisaggregationApp::initialize(int stage) {

	BaseApp::initialize(stage);

	if (stage == 1) {
        disAggregate = new cMessage();
        resumePlatooning = new cMessage();

        scheduleAt(simTime() + SimTime(10), disAggregate);
	}

}

void PlatoonDisaggregationApp::finish() {
	BaseApp::finish();
	if (disAggregate) {
		cancelAndDelete(disAggregate);
		disAggregate = 0;
	}
	if (resumePlatooning) {
	    cancelAndDelete(resumePlatooning);
	    resumePlatooning = 0;
	}
}

void PlatoonDisaggregationApp::onData(WaveShortMessage *wsm) {
}

void PlatoonDisaggregationApp::handleSelfMsg(cMessage *msg) {

	//this takes car of feeding data into CACC and reschedule the self message
	BaseApp::handleSelfMsg(msg);

	if (msg == disAggregate) {

        traciVehicle->setCACCConstantSpacing(50);

	    //Resume in 20 seconds
		scheduleAt(simTime() + SimTime(20), resumePlatooning);
	}
	if (msg == resumePlatooning) {

	    traciVehicle->setCACCConstantSpacing(10);
	}
}

void PlatoonDisaggregationApp::onBeacon(WaveShortMessage* wsm) {

}

