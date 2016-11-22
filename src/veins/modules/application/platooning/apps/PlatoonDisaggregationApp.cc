//
// Copright (c) 2016 Maytheewat Aramrattana <maytheewat.aramrattana@hh.se>
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

#include "crng.h"
#include "veins/modules/messages/WaveShortMessage_m.h"
#include "veins/base/messages/MacPkt_m.h"
#include "veins/modules/mac/ieee80211p/Mac1609_4.h"

#include "veins/modules/application/platooning/protocols/BaseProtocol.h"

Define_Module(PlatoonDisaggregationApp);

void PlatoonDisaggregationApp::initialize(int stage) {

	BaseApp::initialize(stage);

	if (stage == 1) {


        qichen_Kv = par("qichen_Kv").doubleValue();
        qichen_Kr = par("qichen_Kr").doubleValue();
        qichen_Rdes = par("qichen_Rdes").doubleValue();

	    //should the follower use ACC or CACC?
		const char *strController = par("controller").stringValue();
		//for now we have only two possibilities
		if (strcmp(strController, "ACC") == 0) {
			controller = Plexe::ACC;
		}
		else if (strcmp(strController, "CACC") == 0) {
			controller = Plexe::CACC;
		}
		else {
			controller = Plexe::QICHEN;
		}
		//headway time for ACC
		accHeadway = par("accHeadway").doubleValue();
		//leader speed
		leaderSpeed = par("leaderSpeed").doubleValue();

		VTIcontrol = par("DScontrol").boolValue();

		SUMO_disturbance = par("SUMOdisturb").boolValue();

		if (mySUMOId_int == 0) {
			//ACC speed is 100 km/h
			traciVehicle->setCruiseControlDesiredSpeed(leaderSpeed / 3.6);
			//leader uses the ACC
			traciVehicle->setActiveController(Plexe::ACC);

		}
		else {
			//followers speed is higher
			traciVehicle->setCruiseControlDesiredSpeed((leaderSpeed + 30) / 3.6);
			//followers use controller specified by the user
			traciVehicle->setActiveController(controller);
			//use headway time specified by the user (if ACC is employed)
			traciVehicle->setACCHeadwayTime(accHeadway);
			traciVehicle->setCACCConstantSpacing(10);
		}
		if(myPlatoonName.find("platoon") != std::string::npos) traciVehicle->setLaneChangeAction(Plexe::STAY_IN_CURRENT_LANE);

		disAggregateCounter = 1;
        //Disaggregate
        disAggregate = new cMessage();
        //scheduleAt(simTime() + SimTime(100), disAggregate);

        //Resume
        resumePlatooning = new cMessage();

        //every car must run on its own lane
        //traciVehicle->setFixedLane(traciVehicle->getLaneIndex());
        //if((strcmp("platoon0", myPlatoonName.c_str())) == 0) //this fix the problem, so that normal cars can go to their destination instead of fixed to a lane
        //if(myPlatoonName.find("platoon") != std::string::npos) traciVehicle->setFixedLane(traciVehicle->getLaneIndex());


		newHeadway = 1.0;

        traciVehicle->setGenericInformation(CC_SET_QICHEN_KV, &qichen_Kv, sizeof(double));
        traciVehicle->setGenericInformation(CC_SET_QICHEN_KR, &qichen_Kr, sizeof(double));
        traciVehicle->setGenericInformation(CC_SET_QICHEN_RDES, &qichen_Rdes, sizeof(double));

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
//	if (readDS) {
//        cancelAndDelete(readDS);
//        readDS = 0;
//    }
//	if (disturb) {
//        cancelAndDelete(disturb);
//        disturb = 0;
//    }
}

void PlatoonDisaggregationApp::onData(WaveShortMessage *wsm) {
}

void PlatoonDisaggregationApp::handleSelfMsg(cMessage *msg) {
	//this takes car of feeding data into CACC and reschedule the self message
	BaseApp::handleSelfMsg(msg);

	double bigR = 50;
	double normalR = 10;

	if (msg == disAggregate) {

	    //DO THE DISAGRREGATE
	    //For time headway
	    //traciVehicle->setGenericInformation(CC_SET_PLOEG_H, &currentHeadway, sizeof(double));
	    //For fixed distance controller
        //traciVehicle->setCACCConstantSpacing(50);
	    traciVehicle->setGenericInformation(CC_SET_QICHEN_RDES, &bigR, sizeof(double));

	    //Resume in 20 seconds
		//scheduleAt(simTime() + SimTime(20), resumePlatooning);
	}
	if (msg == resumePlatooning) {

	    //RESUME TO NORMAL PLATOONING
	    //traciVehicle->setCACCConstantSpacing(10);
	    traciVehicle->setGenericInformation(CC_SET_QICHEN_RDES, &normalR, sizeof(double));
	}
}

void PlatoonDisaggregationApp::handleLowerMsg(cMessage *msg) {

    //our vehicle's data
    double speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime, distance, relSpeed;
    traciVehicle->getVehicleData(speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime);
    traciVehicle->getRadarMeasurements(distance, relSpeed);

    UnicastMessage *unicast = dynamic_cast<UnicastMessage *>(msg);
    ASSERT2(unicast, "received a frame not of type UnicastMessage");

    cPacket *enc = unicast->decapsulate();
    ASSERT2(enc, "received a UnicastMessage with nothing inside");

    if (enc->getKind() == BaseProtocol::BEACON_TYPE) {

        PlatooningBeacon *epkt = dynamic_cast<PlatooningBeacon *>(enc);
        ASSERT2(epkt, "received UnicastMessage does not contain a PlatooningBeacon");
        if(strcmp(epkt->getPlatoonName(), myPlatoonName.c_str()) == 0)
        {
            //if the message comes from the leader
            if (epkt->getVehicleId() == 0) {
                traciVehicle->setPlatoonLeaderData(epkt->getSpeed(), epkt->getAcceleration(), epkt->getPositionX(), epkt->getPositionY(), epkt->getTime());
            }
            //if the message comes from the vehicle in front
            if (epkt->getVehicleId() == mySUMOId_int - 1) {
                traciVehicle->setPrecedingVehicleData(epkt->getSpeed(), epkt->getAcceleration(), epkt->getPositionX(), epkt->getPositionY(), epkt->getTime());
            }

        }
    }

    /*
    if(sumoPosX >= 1000*disAggregateCounter) {
        disAggregateCounter++;
        scheduleAt(simTime() + SimTime(0.1), disAggregate);
    }
    */

    delete enc;
    delete unicast;
}


void PlatoonDisaggregationApp::onBeacon(WaveShortMessage* wsm) {

}
