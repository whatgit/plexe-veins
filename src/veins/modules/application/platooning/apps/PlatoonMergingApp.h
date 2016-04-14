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

        //message used to tell platoon to make a gap
        cMessage *makeGap;
        //message used to tell platoon to change lane
        cMessage *changeLane;
        cMessage *ProtocolParUpdates;
        cMessage *checkGap;
        cMessage *reformPlatoon;

        Veins::TraCIConnection* ds_control;
        int lane_change_count;
        double newHeadway;
		//frequency at which the leader speed is oscillating
		double leaderOscillationFrequency;
		//controller to be used for platooning
		enum Plexe::ACTIVE_CONTROLLER controller;
		//headway time for ACC
		double accHeadway;
		//leader average speed
		double leaderSpeed;

		double currentCACCSpacing = 10.00; //default spacing is 10 meters
		double SafeGap = 10.00; //safe gap for platoon merging is 10 meters
		double vehicleLength = 4.70;

		virtual void handleLowerMsg(cMessage *msg);
        void SyncSpeedToB();

        int myTargetInPlatoon;
        unsigned long myMIO_ID; //ID of most important object (in front), 0 mean no pair
        unsigned long newMIO; //ID of new MIO after mergning, will be used in reforming the platoon
        unsigned int myMIO_RANGE; //distance to MIO, 65535 mean n/a
        int myMIO_speed;
        unsigned long myFWDPairID; //ID of the forward pair, 0 mean no pair
        unsigned long myBWDPairID; //ID of the backward pair, 0 mean no pair
        unsigned char PlatoonID;
        bool makingGap;
        bool mergeRequestFlag;
        bool STOM_flag;
        bool Merging_flag;
        bool headVehicleFlag;
        bool tailVehicleFlag;


	public:
		PlatoonMergingApp() {
			makeGap = 0;
		}
		void UpdateProtocolParam();

	protected:

		virtual void handleSelfMsg(cMessage *msg);



};

#endif /* PLATOONMERGINGAPP_H_ */
