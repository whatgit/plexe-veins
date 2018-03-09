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

#ifndef STANDSTILLOBSTACLESCENARIO_H_
#define STANDSTILLOBSTACLESCENARIO_H_

#include "veins/modules/application/platooning/scenarios/BaseScenario.h"

#include "veins/modules/application/platooning/apps/BaseApp.h"

class StandstillObstacleScenario : public BaseScenario
{

	public:

		virtual void initialize(int stage);
		virtual void finish();

	protected:

		//leader average speed
		double leaderSpeed;
		//acceleration in m/s/s
		double acceleration;
		//braking intensity in m/s/s
		double brakingDeceleration;
		//message used to tell the car to start accelerating
		cMessage *startAccelerationMsg;
		//message used to tell the car to start braking
		cMessage *startBrakingMsg;
		//message to check if we reach maximum braking
		cMessage *checkMaxBrakingMsg;

		//start accelerating time
		SimTime startAccelerating;
		//start braking time
		SimTime startBraking;
		//application layer, used to stop the simulation
		BaseApp *appl;

		double startingPos;

		double notification_distance;

		double currentHeadway;

	public:
		StandstillObstacleScenario() {
			leaderSpeed = 0;
			acceleration = 0;
			brakingDeceleration = 0;
			startAccelerationMsg = 0;
			startBrakingMsg = 0;
			checkMaxBrakingMsg = 0;
			startAccelerating = SimTime(0);
			startBraking = SimTime(0);
			appl = 0;
		}

	protected:

		virtual void handleSelfMsg(cMessage *msg);

};

#endif
