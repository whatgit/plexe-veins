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

#ifndef PLATOONSADVANCEDTRAFFICMANAGER_H_
#define PLATOONSADVANCEDTRAFFICMANAGER_H_

#include <veins/modules/mobility/traci/TraCIBaseTrafficManager.h>

class PlatoonsAdvancedTrafficManager : public TraCIBaseTrafficManager
{

	public:

		virtual void initialize(int stage);
		virtual void finish();

		PlatoonsAdvancedTrafficManager() {
			insertPlatoonMessage = 0;
			insertManualCarMessage = 0;
			platoonInsertDistance = 0;
			platoonInsertHeadway = 0;
			platoonInsertSpeed = 0;
			platoonInsertTime = SimTime(0);
			platoonLeaderHeadway = 0;
			platoonSize = 0;
			nCars = 0;
			nLanes = 0;
			nManualCars = 0;
			laneManualCars = 0;
			platoon_routeId = 0;
			manual_routeId = 0;
		}

	protected:

		//this is used to start traffic generation
		cMessage *insertPlatoonMessage, *insertManualCarMessage;

		void insertPlatoons();
		void insertManualCars();

		virtual void handleSelfMsg(cMessage *msg);

		SimTime platoonInsertTime;
		double platoonInsertSpeed;
		//vehicles to be inserted
		struct Vehicle automated, manual;

		//total number of vehicles to be injected
		int nCars;
		//vehicles per platoon
		int platoonSize;
		//number of lanes
		int nLanes;
		//insert distance
		double platoonInsertDistance;
		//insert headway
		double platoonInsertHeadway;
		//headway for leader vehicles
		double platoonLeaderHeadway;
		//number of manual car(s) driven by driving simulator
		int nManualCars;
		//which lane to put it?
		int laneManualCars;
		//sumo vehicle type of platooning cars
		std::string platooningVType;
		//the desired route
		const char *platoon_route, *manual_route;
		//the route ids
		int platoon_routeId, manual_routeId;

		double offset_manual;
		double offset_platoon;


		virtual void scenarioLoaded();

};

#endif
