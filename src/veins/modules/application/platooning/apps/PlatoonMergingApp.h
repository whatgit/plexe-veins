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

#ifndef PLATOONMERGINGAPP_H_
#define PLATOONMERGINGAPP_H_

#include "veins/modules/application/platooning/apps/BaseApp.h"

class PlatoonMergingApp : public BaseApp
{

	public:

		virtual void initialize(int stage);
		virtual void finish();

	protected:
		virtual void onBeacon(WaveShortMessage* wsm);
		virtual void onData(WaveShortMessage* wsm);
		virtual void handleLowerMsg(cMessage *msg);

	public:
		PlatoonMergingApp() {}
		void UpdateProtocolParam();

	protected:
        double currentCACCSpacing;
        double currentHeadway;
        double normalCACCSpacing;
        double normalHeadway;
        //message used to tell the leader to continuously change its desired speed
        cMessage *changeLane;
        cMessage *checkGap;
        cMessage *reformPlatoon;
        cMessage *startMerge; //Event to start the scenario

		virtual void handleSelfMsg(cMessage *msg);

	private:
        //for making gap to fwd pair
        double myTargetGap;
        double SafeGap;
        double myPairvehicleLength;
        double currentGapToFWDPair;

        cOutVector GapToFWDPair, nodeIdOut;
        unsigned long myMIO_ID; //ID of most important object (in front), 0 mean no pair
        unsigned long newMIO; //ID of new MIO after mergning, will be used in reforming the platoon
        unsigned int myMIO_RANGE; //distance to MIO, 65535 mean n/a
        int myMIO_speed;
        unsigned long myFWDPairID; //ID of the forward pair, 0 mean no pair
        unsigned long myBWDPairID; //ID of the backward pair, 0 mean no pair
        unsigned char PlatoonID;
        bool makingGap;
        bool doneGap;
        bool inMergingZone;
        bool checkZone;
        bool mergeRequestFlag;
        bool STOM_flag;
        bool Merging_flag;
        bool headVehicleFlag;
        bool tailVehicleFlag;
        double GapMakingKp;

};

#endif /* PLATOONMERGINGAPP_H_ */
