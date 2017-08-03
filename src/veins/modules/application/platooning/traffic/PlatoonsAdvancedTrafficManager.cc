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

#include "PlatoonsAdvancedTrafficManager.h"

Define_Module(PlatoonsAdvancedTrafficManager);

void PlatoonsAdvancedTrafficManager::initialize(int stage) {

	TraCIBaseTrafficManager::initialize(stage);

	if (stage == 0) {

		nCars = par("nCars").longValue();
		platoonSize = par("platoonSize").longValue();
		nLanes = par("nLanes").longValue();
		platoonInsertTime = SimTime(par("platoonInsertTime").doubleValue());
		platoonInsertSpeed = par("platoonInsertSpeed").doubleValue();
		platoonInsertDistance = par("platoonInsertDistance").doubleValue();
		platoonInsertHeadway = par("platoonInsertHeadway").doubleValue();
		platoonLeaderHeadway = par("platoonLeaderHeadway").doubleValue();
		platooningVType = par("platooningVType").stdstringValue();
		insertPlatoonMessage = new cMessage("");
		insertManualCarMessage = new cMessage("");
		nManualCars = par("nManualCars").longValue();
		laneManualCars = par("laneManualCars").longValue();
		scheduleAt(platoonInsertTime, insertPlatoonMessage);
		if (nManualCars)    scheduleAt(platoonInsertTime, insertManualCarMessage);

	}

}

void PlatoonsAdvancedTrafficManager::scenarioLoaded() {
	automated.id = findVehicleTypeIndex(platooningVType);
	automated.lane = -1;
	automated.position = 0;
	automated.speed = platoonInsertSpeed/3.6;

}

void PlatoonsAdvancedTrafficManager::handleSelfMsg(cMessage *msg) {

	TraCIBaseTrafficManager::handleSelfMsg(msg);

	if (msg == insertPlatoonMessage) {
		insertPlatoons();
	}
	if (msg == insertManualCarMessage) {
        insertManualCars();
    }

}

void PlatoonsAdvancedTrafficManager::insertPlatoons() {

	//compute intervehicle distance
	double distance = platoonInsertSpeed / 3.6 * platoonInsertHeadway + platoonInsertDistance;
	//total number of platoons per lane
	int nPlatoons = nCars / platoonSize / nLanes;
	//length of 1 platoon
	double platoonLength = platoonSize * 4 + (platoonSize - 1) * distance;
	//inter-platoon distance
	double platoonDistance = platoonInsertSpeed / 3.6 * platoonLeaderHeadway;

	if (nCars % nLanes != 0) nPlatoons++;

	//total length for one lane
	double totalLength = nPlatoons * platoonLength + (nPlatoons - 1) * platoonDistance;

	//for each lane, we create an offset to have misaligned platoons
	double *laneOffset = new double[nLanes];
	for (int l = 0; l < nLanes; l++)
		laneOffset[l] = uniform(0, 20);

	double currentPos = totalLength;
	int currentCar = 0;

    for (int i = 0; i < nCars/nLanes; i++) {
            for (int l = 0; l < nLanes; l++) {
                automated.position = currentPos + laneOffset[l];
                automated.lane = l;
                addVehicleToQueue(0, automated);
            }
            currentCar++;
            if (currentCar == platoonSize && i != (nCars/nLanes)-1) {
                currentCar = 0;
                //add inter platoon gap
                currentPos -= (platoonDistance + 4);
            }
            else {
                //add intra platoon gap
                currentPos -= (4 + distance);
            }
    }
    if (nCars % nLanes != 0) {
        automated.position = currentPos + laneOffset[0];
        automated.lane = 0;
        addVehicleToQueue(0, automated);
    }

	delete [] laneOffset;

}

void PlatoonsAdvancedTrafficManager::insertManualCars() {
    manual.id = findVehicleTypeIndex("manual_car");
    manual.lane = 1;
    manual.position = 0;
    manual.speed = 0;
    addVehicleToQueue(0, manual);
}

void PlatoonsAdvancedTrafficManager::finish() {
	TraCIBaseTrafficManager::finish();
	if (insertPlatoonMessage) {
		cancelAndDelete(insertPlatoonMessage);
		insertPlatoonMessage = 0;
	}
	if (insertManualCarMessage) {
        cancelAndDelete(insertManualCarMessage);
        insertManualCarMessage = 0;
    }
}
