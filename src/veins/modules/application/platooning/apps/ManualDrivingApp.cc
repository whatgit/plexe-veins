//Copright (c) 2016 Maytheewat Aramrattana <maytheewat.aramrattana@hh.se>
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

#include "veins/modules/application/platooning/apps/ManualDrivingApp.h"

#include "crng.h"
#include "veins/modules/messages/WaveShortMessage_m.h"
#include "veins/base/messages/MacPkt_m.h"
#include "veins/modules/mac/ieee80211p/Mac1609_4.h"
#include "veins/modules/mobility/traci/TraCIConstants.h"

#include "veins/modules/application/platooning/protocols/BaseProtocol.h"
#include "veins/modules/application/platooning/protocols/ManualDrivingProtocol.h"

Define_Module(ManualDrivingApp);

void ManualDrivingApp::initialize(int stage) {

    BaseApp::initialize(stage);

    if (stage == 1) {

        //get the oscillation frequency of the leader as parameter
        leaderOscillationFrequency = par("leaderOscillationFrequency").doubleValue();
        ipAddress = par("IP_DS").stdstringValue();
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
            controller = Plexe::PLOEG;
        }
        //headway time for ACC
        accHeadway = par("accHeadway").doubleValue();
        //leader speed
        leaderSpeed = par("leaderSpeed").doubleValue();
        VTIcontrol = par("DScontrol").boolValue();
        CACCSpacing =  par("CACCSpacing").doubleValue();

        //set name for record
        /*gap_d.setName("gap_d");
        gap_v.setName("gap_v");
        speed_fake_controller.setName("speed_fake_control");*/

        if (mySUMOId_int == 0) {
            //ACC speed is 100 km/h
            traciVehicle->setCruiseControlDesiredSpeed(leaderSpeed / 3.6);
            //traciVehicle->setCruiseControlDesiredSpeed(30 / 3.6);
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
            traciVehicle->setCACCConstantSpacing(CACCSpacing);
        }

        //Initialize the reading from driving simulator (assuming car is driving with ACC)
        if (myPlatoonName.find("platoon") == std::string::npos && VTIcontrol) {
            traciVehicle->setActiveController(Plexe::CC);
            ds_control = Veins::TraCIConnection::connect(ipAddress.c_str(), 8855);
            readDS = new cMessage();
            scheduleAt(simTime() + SimTime(0.01), readDS);
        }
        else {
            readDS = 0;
        }

        //Prevent all platooning vehicles from changing lane
        if(myPlatoonName.find("platoon") != std::string::npos) traciVehicle->setLaneChangeAction(Plexe::STAY_IN_CURRENT_LANE);
        closeTheGap = new cMessage();
        //every car must run on its own lane
        //traciVehicle->setFixedLane(traciVehicle->getLaneIndex());
        mySUMOLeader = 0;
    }

}

void ManualDrivingApp::finish() {
    BaseApp::finish();
    if (readDS) {
        cancelAndDelete(readDS);
        readDS = 0;
    }
    if (closeTheGap) {
        cancelAndDelete(closeTheGap);
        closeTheGap = 0;
    }
}

void ManualDrivingApp::onData(WaveShortMessage *wsm) {
}

void ManualDrivingApp::handleLowerMsg(cMessage *msg) {

    UnicastMessage *unicast = dynamic_cast<UnicastMessage *>(msg);
    ASSERT2(unicast, "received a frame not of type UnicastMessage");

    cPacket *enc = unicast->decapsulate();
    ASSERT2(enc, "received a UnicastMessage with nothing inside");

    //our vehicle's data
    double speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime, distance, relSpeed;
    traciVehicle->getVehicleData(speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime);
    traciVehicle->getRadarMeasurements(distance, relSpeed);

    if (enc->getKind() == BaseProtocol::BEACON_TYPE) {  //Similar to BaseApp

        PlatooningBeacon *epkt = dynamic_cast<PlatooningBeacon *>(enc);
        ASSERT2(epkt, "received UnicastMessage does not contain a PlatooningBeacon");
        if(strcmp(epkt->getPlatoonName(), myPlatoonName.c_str()) == 0)
        {
            //if the message comes from the leader
            if (epkt->getVehicleId() == mySUMOLeader) {
                traciVehicle->setPlatoonLeaderData(epkt->getSpeed(), epkt->getAcceleration(), epkt->getPositionX(), epkt->getPositionY(), epkt->getTime());
            }
            //if the message comes from the vehicle in front
            if (epkt->getVehicleId() == mySUMOId_int - 1) {
                traciVehicle->setPrecedingVehicleData(epkt->getSpeed(), epkt->getAcceleration(), epkt->getPositionX(), epkt->getPositionY(), epkt->getTime());
            }

        }

        //Maybe add something like tracking the vehicle, making gap, timeouts and so on

    }
    else if (enc->getKind() == BaseProtocol::DENM_TYPE) {
        DENM *denm_pkt = dynamic_cast<DENM *>(enc);

        //then variables can be accessed by
        //mergeRequestFlag = iclcm_pkt->getMergeRequestFlag();

        //Let's try to make the leader speed up
        if (mySUMOId_int == 0) {
            //ACC speed is now 120 km/h
            traciVehicle->setCruiseControlDesiredSpeed(120 / 3.6);
        }
        if (mySUMOId_int == 1) {
            //Switch to ACC
            traciVehicle->setActiveController(Plexe::ACC);
            traciVehicle->setCruiseControlDesiredSpeed(100 / 3.6);
        }
        if (mySUMOId_int == 2) {
            //change leader
            mySUMOLeader = 1;
        }

        //How to make a gap
        //traciVehicle->setCACCConstantSpacing(2*CACCSpacing);
        //if(!closeTheGap->isScheduled()) scheduleAt(simTime() + SimTime(10), closeTheGap);
    }

    delete enc;
    delete unicast;

}

void ManualDrivingApp::handleSelfMsg(cMessage *msg) {
    //this takes car of feeding data into CACC and reschedule the self message
    BaseApp::handleSelfMsg(msg);

    if (msg == readDS) {
        uint8_t read_a_byte;
        double speed;
        int laneID = 0;
        int intention = 0;
        std::string ds_resp;
        double radar_distance, rel_speed;

        traciVehicle->getRadarMeasurements(radar_distance, rel_speed);

        //send command to request control values from ds (basically speed and laneID of ego vehicle)
        ds_control->sendTCPMessage(Veins::makeTraCICommand(0x40, Veins::TraCIBuffer()));
        ds_resp = ds_control->receiveMessage();

        Veins::TraCIBuffer buf = Veins::TraCIBuffer(ds_resp);
        buf >> read_a_byte;
        buf >> read_a_byte;
        buf >> read_a_byte;
        buf >> read_a_byte;
        buf >> read_a_byte;
        buf >> speed;
        buf >> laneID;
        buf >> intention;

        //Control the vehicle with received speed
        traciVehicle->setSpeed(speed);
        traciVehicle->setFixedLane(laneID);
        if(intention != 0)  TriggerDENM(intention, laneID);

        //do something with the received intention
        /*gap_v.record(radar_distance);
        speed_fake_controller.record(speed);*/

        scheduleAt(simTime() + SimTime(0.01), readDS);
    }
    else if (msg == closeTheGap) {
        traciVehicle->setCACCConstantSpacing(CACCSpacing);
    }
}

void ManualDrivingApp::onBeacon(WaveShortMessage* wsm) {
}

void ManualDrivingApp::TriggerDENM(int myIntention, int myLane) {
    UnicastMessage *DENM_unicast;
    DENM *DENM_msg;

    DENM_unicast = new UnicastMessage("", DENM_event);
    DENM_unicast->setDestination(-1);
    DENM_unicast->setPriority(2);
    DENM_unicast->setType(6);

    DENM_msg = new DENM();
    DENM_msg->setIntention(myIntention);
    DENM_msg->setMyLane(myLane);

    DENM_unicast->encapsulate(DENM_msg);
    sendDown(DENM_unicast);
}
