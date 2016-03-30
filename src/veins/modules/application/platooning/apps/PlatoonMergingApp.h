//
// Copright (c) 2016
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

#ifndef PLATOONMERGINGAPP_H_
#define PLATOONMERGINGAPP_H_

#include "veins/modules/application/platooning/apps/BaseApp.h"
#include "veins/modules/mobility/traci/TraCIConnection.h"
#include "veins/modules/mobility/traci/TraCIBuffer.h"

#define USE_DS false

class PlatoonMergingApp : public BaseApp
{

	public:

		virtual void initialize(int stage);
		virtual void finish();

	protected:
		virtual void onBeacon(WaveShortMessage* wsm);
		virtual void onData(WaveShortMessage* wsm);

	private:

		//frequency at which the leader speed is oscillating
		double leaderOscillationFrequency;
		//controller to be used for platooning
		enum Plexe::ACTIVE_CONTROLLER controller;
		//headway time for ACC
		double accHeadway;
		//leader average speed
		double leaderSpeed;

		//message used to tell the leader to continuously change its desired speed
		cMessage *changeSpeed;

		//message used to tell platoon to make a gap
		cMessage *makeGap;

		//message used to tell platoon to change lane
		cMessage *changeLane;

		double newHeadway;

		virtual void handleLowerMsg(cMessage *msg);

		Veins::TraCIConnection* ds_control;

        int lane_change_count;

	public:
		PlatoonMergingApp() {
			changeSpeed = 0;
			makeGap = 0;
		}

	protected:

		virtual void handleSelfMsg(cMessage *msg);



};

#endif /* PLATOONMERGINGAPP_H_ */
