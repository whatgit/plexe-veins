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

#include "veins/modules/application/platooning/scenarios/PlatoonMergingScenario.h"

Define_Module(PlatoonMergingScenario);

void PlatoonMergingScenario::initialize(int stage) {

	BaseScenario::initialize(stage);

	if (stage == 1) {

	    //Two OPC cars drive at 60 km/h
	    if (positionHelper->isLeader()) {
            traciVehicle->setActiveController(Plexe::ACC);
            traciVehicle->setCruiseControlDesiredSpeed(60 / 3.6);
        }
	    else {
            traciVehicle->setCruiseControlDesiredSpeed((100) / 3.6);    //followers speed is higher (100 km/h)
	    }
	}

}

void PlatoonMergingScenario::finish() {
	BaseScenario::finish();
}

void PlatoonMergingScenario::handleSelfMsg(cMessage *msg) {
	BaseScenario::handleSelfMsg(msg);
}

