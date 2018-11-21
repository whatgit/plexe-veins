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

#include "veins/modules/application/platooning/scenarios/OvertakingScenario.h"

Define_Module(OvertakingScenario);

void OvertakingScenario::initialize(int stage) {

    BaseScenario::initialize(stage);

    if (stage == 1) {

        if (traciVehicle->getVType() == "manual_car") {
            moveNeighbors = new cMessage("moveNeighbors");
            startScenario = new cMessage("startScenario");
            checkDistanceToManual = 0;
            scheduleAt(simTime() + SimTime(1), startScenario);
        }
        else {
            moveNeighbors = 0;
            startScenario = 0;
            checkDistanceToManual = new cMessage("checkDistanceToManual");
            scheduleAt(simTime() + SimTime(0.01), checkDistanceToManual);
        }

    }

}

void OvertakingScenario::finish() {
    BaseScenario::finish();
}

void OvertakingScenario::handleSelfMsg(cMessage *msg) {
    BaseScenario::handleSelfMsg(msg);
    double speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime;
    double Myspeed, Myacceleration, MycontrollerAcceleration, MysumoPosX, MysumoPosY, MysumoTime;
    double diff_ego_platoon, diff_platoon_ego;

    if (msg == checkDistanceToManual) {
        std::string route_id = traciVehicle->getRouteId();
        //std::cout << "checking distance to manual car" << std::endl;
        traci->vehicle("manual_car.0").getVehicleData(speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime);
        traciVehicle->getVehicleData(Myspeed, Myacceleration, MycontrollerAcceleration, MysumoPosX, MysumoPosY, MysumoTime);
        //std::cout << "I am at " <<  MysumoPosX << std::endl;
        //std::cout << "Manual car is at " << sumoPosX << std::endl;
        //std::cout << "Diff = " << MysumoPosX - sumoPosX << std::endl;
        //std::cout << "------------------***********-------------" << std::endl;
        //scheduleAt(simTime() + SimTime(0.01), checkDistanceToManual);
    }
    if (msg == moveNeighbors) {

        traci->vehicle("platoon_vehicle.0").getVehicleData(speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime);
        traciVehicle->getVehicleData(Myspeed, Myacceleration, MycontrollerAcceleration, MysumoPosX, MysumoPosY, MysumoTime);
        //std::cout << "My pos is " << MysumoPosX;
        //std::cout << "Vehicle 0 pos is " << sumoPosX;
        //std::cout << "Diff = " << MysumoPosX - sumoPosX << std::endl;
        //std::cout << "------------------***********-------------" << std::endl;
        diff_ego_platoon = MysumoPosX - sumoPosX;

        if(diff_ego_platoon > 40) {
            //traci->connection.query(0x5c, Veins::TraCIBuffer() << "platoon_vehicle.0" << TYPE_COMPOUND << int(2) << TYPE_STRING << "gneE1_0" << TYPE_DOUBLE << MysumoPosX+500);
            traci->vehicle("platoon_vehicle.0").moveTo("gneE1_0", 10000 - MysumoPosX - 250);
            //traci->vehicle("platoon_vehicle.1").moveTo("gneE1_0", 10000 - MysumoPosX - 370);
            //traci->vehicle("platoon_vehicle.2").moveTo("gneE1_0", 10000 - MysumoPosX - 390);
        }
        scheduleAt(simTime() + SimTime(0.01), moveNeighbors);
    }
    if (msg == startScenario) {
        traci->vehicle("platoon_vehicle.0").moveTo("gneE1_0", 10000 - MysumoPosX - 700);
        //traci->vehicle("platoon_vehicle.1").moveTo("gneE1_0", 10000 - MysumoPosX - 720);
        //traci->vehicle("platoon_vehicle.2").moveTo("gneE1_0", 10000 - MysumoPosX - 740);
        //traci->addVehicle("obstacle", "obstacle_car", "manual_route", simTime(), 30+10, 10, 0);

        scheduleAt(simTime() + SimTime(0.01), moveNeighbors);
    }
}
