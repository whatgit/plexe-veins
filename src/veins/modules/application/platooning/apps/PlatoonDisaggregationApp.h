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

#ifndef PLATOONDISAGGREGATIONAPP_H_
#define PLATOONDISAGGREGATIONAPP_H_

#include "veins/modules/application/platooning/apps/BaseApp.h"
#include "veins/modules/mobility/traci/TraCIConnection.h"
#include "veins/modules/mobility/traci/TraCIBuffer.h"

class PlatoonDisaggregationApp : public BaseApp
{

    public:

        virtual void initialize(int stage);
        virtual void finish();

    protected:
        virtual void onBeacon(WaveShortMessage* wsm);
        virtual void onData(WaveShortMessage* wsm);

    private:

        //cOutVector gap_d, gap_v, speed_fake_controller;

        //frequency at which the leader speed is oscillating
        double leaderOscillationFrequency;
        //controller to be used for platooning
        enum Plexe::ACTIVE_CONTROLLER controller;
        //headway time for ACC
        double accHeadway;
        //leader average speed
        double leaderSpeed;

        int disAggregateCounter;

        //parameters for QiChen's controller
        double qichen_Kv, qichen_Kr, qichen_Rdes;

        //message used to tell the vehicle to dis-aggregate from platoon
        cMessage *disAggregate;

        //message used to tell the vehicle to resume normal platooning
        cMessage *resumePlatooning;

        //message used to tell to read control input from driving simulator
        cMessage *readDS;

        //message to test disturbance
        cMessage *sendSomething;

        double newHeadway;

        bool VTIcontrol;

        bool SUMO_disturbance;

        //car's parameter
        double d_car, v_car, t_car;

        //platoon's parameter
        double d_platoon, v_platoon, t_platoon_front, t_platoon_rear;
        double L_HDV = 25;

        Veins::TraCIConnection* ds_control;
        void Optimal_Speed(double &speed_profile, double &acceleration_profile, double &travel_distance, double &travel_time);

    public:
        PlatoonDisaggregationApp() {
        }

    protected:
        virtual void handleSelfMsg(cMessage *msg);
        virtual void handleLowerMsg(cMessage *msg);

};

#endif /* PLATOONDISAGGREGATIONAPP_H_ */
