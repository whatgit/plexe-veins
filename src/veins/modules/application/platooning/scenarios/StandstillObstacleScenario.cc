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

#include "veins/modules/application/platooning/scenarios/StandstillObstacleScenario.h"


Define_Module(StandstillObstacleScenario);

void StandstillObstacleScenario::initialize(int stage) {

    BaseScenario::initialize(stage);

    if (stage == 0)
        //get pointer to application
        appl = FindModule<BaseApp*>::findSubModule(getParentModule());

    if (stage == 1) {
        //get acceleration
        acceleration = par("acceleration").doubleValue();
        //get braking deceleration
        brakingDeceleration = par("brakingDeceleration").doubleValue();
        //average speed
        leaderSpeed = par("leaderSpeed").doubleValue() / 3.6;
        //start accelerating time
        startAccelerating = SimTime(par("startAccelerating").doubleValue());
        //start braking time
        startBraking = SimTime(par("startBraking").doubleValue());

        notification_distance = par("notification_d").doubleValue();

        //messages to schedule actions
        startAccelerationMsg = new cMessage("startAccelerationMsg");
        startBrakingMsg = new cMessage("startBrakingMsg");
        checkMaxBrakingMsg = new cMessage("checkMaxBrakingMsg");

        //enable ACC
        //traciVehicle->setActiveController(Plexe::ACC);
        //let the vehicle start from standstill
        //traciVehicle->setFixedAcceleration(1, -8);

        //schedule messages
        if(positionHelper->isLeader()) {
            //traciVehicle->setActiveController(Plexe::ACC);
            double speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime;
            traciVehicle->getVehicleData(speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime);
            startingPos = sumoPosX;
            //let the vehicle start from standstill
            traciVehicle->setCruiseControlDesiredSpeed(0);
            scheduleAt(startAccelerating, startAccelerationMsg);
            scheduleAt(simTime() + startBraking, startBrakingMsg);
        }
    }

}

void StandstillObstacleScenario::finish() {
    cancelAndDelete(startAccelerationMsg);
    startAccelerationMsg = 0;
    cancelAndDelete(startBrakingMsg);
    startBrakingMsg = 0;
    cancelAndDelete(checkMaxBrakingMsg);
    checkMaxBrakingMsg = 0;
    BaseScenario::finish();
}

void StandstillObstacleScenario::handleSelfMsg(cMessage *msg) {
    BaseScenario::handleSelfMsg(msg);
    if (msg == startAccelerationMsg) {
        traciVehicle->setCruiseControlDesiredSpeed(90/3.6);
    }
    if (msg == startBrakingMsg) {
        double speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime;
        traciVehicle->getVehicleData(speed, acceleration, controllerAcceleration, sumoPosX, sumoPosY, sumoTime);

        if(sumoPosX >= startingPos+800-notification_distance) {
            //reached notification distance
            std::string route_id = traciVehicle->getRouteId();
            //put the obstacle
            traci->addVehicle("obstacle", "obstacle_car", route_id, simTime(), (sumoPosX+notification_distance+6),0, 0);
            //scheduleAt(simTime() + SimTime(0.01), checkMaxBrakingMsg);
        }
        else {
            //keep checking every 0.1 second
            scheduleAt(simTime() + SimTime(0.1), startBrakingMsg);
        }
    }
    if (msg == checkMaxBrakingMsg) {

    }
}
