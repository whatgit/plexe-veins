//
// Copright (c) 2012-2014 Michele Segata <segata@ccs-labs.org>
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

#include "SimplePlatooningBeaconing.h"
#include <ctime>
#include <cstdlib>
#include <ctime>
#include <iostream>



Define_Module(SimplePlatooningBeaconing)
static struct pre_data{
    double pre_speed;
    double pre_PosX;
    double pre_PosY;
    double pre_time;
    double pre_angle;
}pre_Data;

static std::vector<struct pre_data> data_list;


void SimplePlatooningBeaconing::initialize(int stage) {
	BaseProtocol::initialize(stage);

    double speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime, distance, relSpeed;
    traciVehicle->getVehicleData(speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime);

	if (stage == 0) {

		//random start time
	    std::srand(std::clock());
	    int r = rand() % 1000000;
	    double ran_val = r/1000000.0;
		SimTime beginTime = SimTime(ran_val);

		//comment=====================================
		scheduleAt(simTime() + beginTime, sendBeacon);
		//end comment=====================================
//        Coord Position = this->traci->getPositionAt(simTime());
        pre_Data.pre_time = simTime().dbl();
        pre_Data.pre_PosX = sumoPosX;
        pre_Data.pre_PosY = sumoPosY;
        pre_Data.pre_speed = speed;
//        pre_Data.pre_angle = this->traci->getAngleRad();
        data_list.push_back(pre_Data);
        scheduleAt(simTime() + beginTime, CheckCamGen);
	}
}

void SimplePlatooningBeaconing::handleSelfMsg(cMessage *msg) {

	BaseProtocol::handleSelfMsg(msg);
	//static==========================================================================
//	if (msg == sendBeacon) {
//	    sendPlatooningMessage(-1);

//	    std::srand(std::clock());
//	    int ran = rand() % 50;
//	    double delta = ran/10000.0;

//		scheduleAt(simTime() + beaconingInterval + delta, sendBeacon);
//		  int ID = getParentModule()->getIndex();
//		  nodeIdOutout.record(ID);
//	      msgevent.record(1);
//	}
    double speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime, distance, relSpeed;
    traciVehicle->getVehicleData(speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime);
    //cam===============================================================================
    if (msg == sendBeacon) {
            sendPlatooningMessage(-1);
        double speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime, distance, relSpeed;
        traciVehicle->getVehicleData(speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime);
         double Cur_Speed, Cur_PosX, Cur_PosY, Cur_angle, Cur_Time;

          int ID = getParentModule()->getIndex();
          Cur_Speed = speed;
//          Coord pos = this->traci->getPositionAt(simTime());
          Cur_PosX = sumoPosX;
          Cur_PosY = sumoPosY;
          //Cur_angle = this->traci->getAngleRad();
          Cur_Time = simTime().dbl();
          data_list.at(ID).pre_time =Cur_Time;
          data_list.at(ID).pre_speed = Cur_Speed;
          data_list.at(ID).pre_PosX = Cur_PosX;
          data_list.at(ID).pre_PosY = Cur_PosY;
          //data_list.at(ID).pre_angle = Cur_angle;

          nomsg_scheduled = true; // no more delayed messages schedules -> regular checks

        }

    if (msg == CheckCamGen){
        double Cur_Speed, Cur_PosX, Cur_PosY, Cur_angle, Cur_Time;
        double Pre_Speed, Pre_PosX, Pre_PosY, Pre_angle, Pre_Time;
        double min_interval = 0.05;
        double max_interval = 1;
        double max_degree = 4.0*M_PI/180.0;
        double max_distchange = 2.0;
        double max_speedchange =0.25;
        int ca ;
        // get the current vehicle data
        int ID = getParentModule()->getIndex();
        Cur_Speed = speed;
//        Coord pos = this->traci->getPositionAt(simTime());
        Cur_PosX = sumoPosX;
        Cur_PosY = sumoPosY;
//        Cur_angle = this->traci->getAngleRad();
        Cur_Time = simTime().dbl();

        // get the previous vehicle data
        Pre_Speed = data_list.at(ID).pre_speed;
        Pre_PosX = data_list.at(ID).pre_PosX;
        Pre_PosY = data_list.at(ID).pre_PosY;
        Pre_Time = data_list.at(ID).pre_time;
        //Pre_angle = data_list.at(ID).pre_angle;

        //case choose
        // case 0:max_sending_interval
        // case 1:distance difference between current and previous exceeds 4m
        // case 2:speed difference between current and previous exceeds 0.5m/s
        // case 3:angle difference between previous and current exceeds 4 degres

        if (Cur_Time - Pre_Time >= max_interval){
            ca = 0;
        }
        else if (sqrt((Cur_PosX-Pre_PosX)*(Cur_PosX-Pre_PosX)+(Cur_PosY-Pre_PosY)*(Cur_PosY-Pre_PosY)) >= max_distchange){
            ca = 1;
        }
        else if (fabs(Pre_Speed-Cur_Speed) >= max_speedchange){
            ca = 1;
        }
//        else if (fabs(Cur_angle - Pre_angle )>= max_degree ){
//            ca = 1;
//        }
        else{
            ca = 2;
        }


        if (nomsg_scheduled){
        switch(ca) {
        case 0:{
            sendPlatooningMessage(-1);
            data_list.at(ID).pre_time =Cur_Time;
            data_list.at(ID).pre_speed = Cur_Speed;
            data_list.at(ID).pre_PosX = Cur_PosX;
            data_list.at(ID).pre_PosY = Cur_PosY;
//            data_list.at(ID).pre_angle = Cur_angle;
            nodeIdOutout.record(ID);
            msgevent.record(1);
        }break;

        case 1:{
            if(Cur_Time - Pre_Time >= min_interval){
                sendPlatooningMessage(-1);
                data_list.at(ID).pre_time =Cur_Time;
                data_list.at(ID).pre_speed = Cur_Speed;
                data_list.at(ID).pre_PosX = Cur_PosX;
                data_list.at(ID).pre_PosY = Cur_PosY;
//                data_list.at(ID).pre_angle = Cur_angle;
                nodeIdOutout.record(ID);
                msgevent.record(2);
            }else{
                //Non++;
                double Del = min_interval - (Cur_Time - Pre_Time);
                scheduleAt(simTime() + SimTime(Del), sendBeacon);
                nomsg_scheduled = false;

            }break;
        }

        case 2:{
            Non++;
        }
        }
        }
        scheduleAt(simTime()+SimTime(0.005), CheckCamGen);
    }

}



void SimplePlatooningBeaconing::messageReceived(PlatooningBeacon *pkt, UnicastMessage *unicast) {
	//nothing to do for static beaconing

}

SimplePlatooningBeaconing::SimplePlatooningBeaconing()
{}

SimplePlatooningBeaconing::~SimplePlatooningBeaconing()
{}

void SimplePlatooningBeaconing::finish(){
	BaseProtocol::finish();
}
