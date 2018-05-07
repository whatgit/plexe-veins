//
// Copyright (c) 2012-2016 Michele Segata <segata@ccs-labs.org>
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

#include "veins/modules/application/platooning/utilities/OneLaneMassiveHelper.h"

Define_Module(OneLaneMassiveHelper);

void OneLaneMassiveHelper::initialize(int stage) {

	BasePositionHelper::initialize(stage);

	if (stage == 0) {
	    nCars = par("nCars").longValue();
	    if (strcmp(traciVehicle->getVType().c_str(), "platoon_vehicle") == 0) {
            myId = getIdFromExternalId(getExternalId());
            leaderId = getPlatoonLeader(myId, nLanes, platoonSize);
            leader = myId == leaderId;
            frontId = getFrontVehicle(myId, nLanes, platoonSize);
            position = getPositionInPlatoon(myId, nLanes, platoonSize);
            platoonId = getPlatoonNumber(myId, nLanes, platoonSize);
            platoonLane = getPlatoonLane(myId, nLanes);
	    }
	    else {
	        myId = getIdFromExternalId(getExternalId()) + nCars;
	        leaderId = -1;
	        frontId = -1;
	        platoonId = -1;
	        platoonLane = -1;
	        position = -1;
	    }
	}

}

void OneLaneMassiveHelper::finish() {
	BasePositionHelper::finish();
}

int OneLaneMassiveHelper::getPosition() {
	return position;
}

int OneLaneMassiveHelper::getMemberId(int position) {
	return leaderId + position * nLanes;
}

int OneLaneMassiveHelper::getMemberPosition(int vehicleId) {
	return (vehicleId - leaderId) / nLanes;
}

int OneLaneMassiveHelper::getLeaderId() {
	return leaderId;
}

bool OneLaneMassiveHelper::isLeader() {
	return leader;
}

int OneLaneMassiveHelper::getFrontId() {
	return frontId;
}

int OneLaneMassiveHelper::getPlatoonId() {
	return platoonId;
}

int OneLaneMassiveHelper::getPlatoonLane() {
	return platoonLane;
}

bool OneLaneMassiveHelper::isInSamePlatoon(int vehicleId) {
    int vehicleIdsLeader = getPlatoonLeader(vehicleId, nLanes, platoonSize);

    if((myId < (vehicleIdsLeader + platoonSize)) && (myId > vehicleIdsLeader)) {
        return true;
    }
    else {
        return false;
    }
}

int OneLaneMassiveHelper::getIdFromExternalId(std::string externalId) {
	int dotIndex = externalId.find_last_of('.');
	std::string strId = externalId.substr(dotIndex + 1);
	return strtol(strId.c_str(), 0, 10);
}

bool OneLaneMassiveHelper::isLeader(int vehicleId, int nLanes, int platoonSize) {
	return (vehicleId / nLanes) % platoonSize == 0;
}
int OneLaneMassiveHelper::getPlatoonNumber(int vehicleId, int nLanes, int platoonSize) {
	return getPlatoonColumn(vehicleId, nLanes, platoonSize) * nLanes + getPlatoonLane(vehicleId, nLanes);
}
int OneLaneMassiveHelper::getPlatoonLane(int vehicleId, int nLanes) {
	return vehicleId % nLanes;
}
int OneLaneMassiveHelper::getPlatoonColumn(int vehicleId, int nLanes, int platoonSize) {
	return vehicleId / (nLanes * platoonSize);
}
int OneLaneMassiveHelper::getPlatoonLeader(int vehicleId, int nLanes, int platoonSize) {
	if(vehicleId > (platoonSize - 1)) {
	    if((vehicleId % platoonSize) == 0) {
	        return vehicleId;
	    }
	    else{
	        int leader = vehicleId -(vehicleId % platoonSize);
	        return leader;
	    }
	}
	else {
	    return 0;
	}
}
int OneLaneMassiveHelper::getFrontVehicle(int vehicleId, int nLanes, int platoonSize) {
	if (getPlatoonLeader(vehicleId, nLanes, platoonSize) == vehicleId)
		return -1;
	else
		return vehicleId - 1;
}
bool OneLaneMassiveHelper::isInSamePlatoon(int vehicleId, int myId, int nLanes, int platoonSize) {
	return getPlatoonNumber(vehicleId, nLanes, platoonSize) == getPlatoonNumber(myId, nLanes, platoonSize);
}
bool OneLaneMassiveHelper::isFrontVehicle(int vehicleId, int myId, int nLanes, int platoonSize) {
	return getFrontVehicle(myId, nLanes, platoonSize) == vehicleId;
}
int OneLaneMassiveHelper::getPositionInPlatoon(int vehicleId, int nLanes, int platoonSize) {
	return (vehicleId - getPlatoonLeader(vehicleId, nLanes, platoonSize)) / nLanes;
}
