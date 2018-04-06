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

#include "veins/modules/application/platooning/scenarios/IntersectionScenario.h"

Define_Module(IntersectionScenario);

void IntersectionScenario::initialize(int stage) {

	BaseScenario::initialize(stage);

	if (stage == 1) {

		if (positionHelper->getId() < positionHelper->getLanesCount()) {
			//set base cruising speed
			traciVehicle->setCruiseControlDesiredSpeed(100/3.6);
			checkforstop = new cMessage("checkforstop");
			scheduleAt(simTime() + SimTime(0.01), checkforstop);
		}
		else {
			//let the follower set a higher desired speed to stay connected
			//to the leader when it is accelerating
			traciVehicle->setCruiseControlDesiredSpeed((100/3.6) + 5);
			checkforstop = 0;
		}

	}

}

void IntersectionScenario::finish() {
	BaseScenario::finish();
	if (checkforstop) {
	        cancelAndDelete(checkforstop);
	        checkforstop = 0;
		}
}

void IntersectionScenario::handleSelfMsg(cMessage *msg) {
	BaseScenario::handleSelfMsg(msg);
	if (msg == checkforstop) {
		std::cout << "distance to route end is " << traciVehicle->getDistanceToRouteEnd() <<std::endl;
		//if(traciVehicle->getDistanceToRouteEnd() < 510.00) {
		    traciVehicle->setActiveController(Plexe::DRIVER);
		    traciVehicle->stopAt("gneE7", 494, 0, 0.0, 100);
		//}
		//else {
		    scheduleAt(simTime() + SimTime(0.01), checkforstop);
		//}
	}

}
