//
// Copyright (C) 2012 David Eckhoff <eckhoff@cs.fau.de>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "Mac1609_4.h"
#include <iterator>
#include <cstring>
#include "veins/modules/phy/DeciderResult80211.h"
#include "veins/base/phyLayer/PhyToMacControlInfo.h"
#include <algorithm>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <iostream>
//#include "LMacLayer.h"
//#include "LMacPkt_m.h"

#define DBG_MAC EV
//#define DBG_MAC std::cerr << "[" << simTime().raw() << "] " << myId << " "

Define_Module(Mac1609_4);
// add state machine

static std::vector<int> sendcam;
std::vector<std::vector <int> > pre_sendcam;
static std::vector<std::string> state_machine;
/**@brief the minimum packet interval. */
static std::vector<double> bound;
/**@brief packet duration.*/
static double T;
//static double last_veh_mac;

void Mac1609_4::initialize(int stage) {
    BaseMacLayer::initialize(stage);
    if (stage == 0) {

        phy11p = FindModule<Mac80211pToPhy11pInterface*>::findSubModule(
                getParentModule());
        assert(phy11p);

        //this is required to circumvent double precision issues with constants from CONST80211p.h
        assert(simTime().getScaleExp() == -12);

        sigChannelBusy = registerSignal("sigChannelBusy");
        sigCollision = registerSignal("sigCollision");

        txPower = par("txPower").doubleValue();
        bitrate = par("bitrate");
        checkBitrate(bitrate);

        //mac-adresses
//        myMacAddress = intuniform(0,0xFFFFFFFE);
//        myId = getParentModule()->getParentModule()->getFullPath();
//        ID = getParentModule()->getParentModule()->getIndex();

        myId = getParentModule()->getParentModule()->getFullPath();
        ID = getParentModule()->getParentModule()->getIndex();
        myMacAddress = ID;



//        if (ID == 14){
//            last_veh_mac = myMacAddres;
//        };
        //create frequency mappings
        frequency.insert(std::pair<int, double>(Channels::CRIT_SOL, 5.86e9));
        frequency.insert(std::pair<int, double>(Channels::SCH1, 5.87e9));
        frequency.insert(std::pair<int, double>(Channels::SCH2, 5.88e9));
        frequency.insert(std::pair<int, double>(Channels::CCH, 5.89e9));
        frequency.insert(std::pair<int, double>(Channels::SCH3, 5.90e9));
        frequency.insert(std::pair<int, double>(Channels::SCH4, 5.91e9));
        frequency.insert(std::pair<int, double>(Channels::HPPS, 5.92e9));

        //create two edca systems

        myEDCA[type_CCH] = new EDCA(type_CCH,par("queueSize").longValue());
        myEDCA[type_CCH]->myId = myId;
        myEDCA[type_CCH]->myId.append(" CCH");

        myEDCA[type_CCH]->createQueue(2,(((CWMIN_11P+1)/4)-1),(((CWMIN_11P +1)/2)-1),AC_VO);
        myEDCA[type_CCH]->createQueue(3,(((CWMIN_11P+1)/2)-1),CWMIN_11P,AC_VI);
        myEDCA[type_CCH]->createQueue(6,CWMIN_11P,CWMAX_11P,AC_BE);
        myEDCA[type_CCH]->createQueue(9,CWMIN_11P,CWMAX_11P,AC_BK);

        myEDCA[type_SCH] = new EDCA(type_SCH,par("queueSize").longValue());
        myEDCA[type_SCH]->myId = myId;
        myEDCA[type_SCH]->myId.append(" SCH");
        myEDCA[type_SCH]->createQueue(2,(((CWMIN_11P+1)/4)-1),(((CWMIN_11P +1)/2)-1),AC_VO);
        myEDCA[type_SCH]->createQueue(3,(((CWMIN_11P+1)/2)-1),CWMIN_11P,AC_VI);
        myEDCA[type_SCH]->createQueue(6,CWMIN_11P,CWMAX_11P,AC_BE);
        myEDCA[type_SCH]->createQueue(9,CWMIN_11P,CWMAX_11P,AC_BK);

        useSCH = par("useServiceChannel").boolValue();
        if (useSCH) {
            //set the initial service channel
            switch (par("serviceChannel").longValue()) {
            case 1: mySCH = Channels::SCH1; break;
            case 2: mySCH = Channels::SCH2; break;
            case 3: mySCH = Channels::SCH3; break;
            case 4: mySCH = Channels::SCH4; break;
            default: opp_error("Service Channel must be between 1 and 4"); break;
            }
        }

        headerLength = par("headerLength");

        nextMacEvent = new cMessage("next Mac Event");

        if (useSCH) {
            // introduce a little asynchronization between radios, but no more than .3 milliseconds
            uint64_t currenTime = simTime().raw();
            uint64_t switchingTime = SWITCHING_INTERVAL_11P.raw();
            double timeToNextSwitch = (double)(switchingTime
                    - (currenTime % switchingTime)) / simTime().getScale();
            if ((currenTime / switchingTime) % 2 == 0) {
                setActiveChannel(type_CCH);
            }
            else {
                setActiveChannel(type_SCH);
            }

            // channel switching active
            nextChannelSwitch = new cMessage("Channel Switch");
            simtime_t offset = dblrand() * par("syncOffset").doubleValue();
            scheduleAt(simTime() + offset + timeToNextSwitch, nextChannelSwitch);
        }
        else {
            // no channel switching
            nextChannelSwitch = 0;
            setActiveChannel(type_CCH);
        }


        //stats
        statsReceivedPackets = 0;
        statsReceivedBroadcasts = 0;
        statsSentPackets = 0;
        statsTXRXLostPackets = 0;
        statsSNIRLostPackets = 0;
        statsDroppedPackets = 0;
        statsNumTooLittleTime = 0;
        statsNumInternalContention = 0;
        statsNumBackoff = 0;

        statsNumBackoff1 = 0;
        statsNumBackoff2 = 0;
        statsNumBackoff3 = 0;
       // last_veh_mac = 0;

        statsSlotsBackoff = 0;
        statsTotalBusyTime = 0;
        idleChannel = true;
        lastBusy = simTime();
        rejectNum = 0;
        stateswitch = 0;
        channelIdle(true);

        //add Measuring interval,schedule,here 1000ms one time
        measuring = 0;
        CL_pre = 0;
        for(int i=0;i<5;i++){
            CL_list.push_back(0);
        }

        measuring = new cMessage("measuring interval");
        M_state.setName("state");
        C_load.setName("channel_load");
        num_pack_cbr.setName("number_of_packet_cbr");
        nodeIDout.setName("nodeID2");
        nodeIDout2.setName("PacketsendID");
        nodeIDout3.setName("collision ID");
        collisiontime.setName("collision time");
        PAC.setName("PacketSend");
        pkt_rec.setName("pkt_recieved");
        data_a.setName("data_age");
        pkt_timestamp.setName("pkt_generation_time");
        //bostore.setName("val_backoff");
        botime.setName("time_backoff");
        std::srand(std::clock());
        int r = rand() % 1000000;
        double ran_val = r/1000000.0;
        SimTime beginTime = SimTime(ran_val);
        scheduleAt(simTime() + beginTime, measuring);

//        buff = NULL;
//        ptkinbuff = false;
        //initial state machine for each vehicle
        state_machine.push_back("RELAXED");
        stateID = 0;
        bound.push_back(0.1);
        permit = 1;
        timestamp = 0;


        if (ID!= 0){
            std::vector<int> a(sendcam);
            pre_sendcam.push_back(a);
        }
        else{
            std::vector<int> a;
            pre_sendcam.push_back(a);

        }
        sendcam.push_back(0);


        for(int i=0;i<=ID;i++)
            pre_sendcam.at(i).push_back(0);



    }
}

void Mac1609_4::handleSelfMsg(cMessage* msg) {
    if (msg == nextChannelSwitch) {
        ASSERT(useSCH);

        scheduleAt(simTime() + SWITCHING_INTERVAL_11P, nextChannelSwitch);

        switch (activeChannel) {
        case type_CCH:
            DBG_MAC << "CCH --> SCH" << std::endl;
            channelBusySelf(false);
            setActiveChannel(type_SCH);
            channelIdle(true);
            phy11p->changeListeningFrequency(frequency[mySCH]);
            break;
        case type_SCH:
            DBG_MAC << "SCH --> CCH" << std::endl;
            channelBusySelf(false);
            setActiveChannel(type_CCH);
            channelIdle(true);
            phy11p->changeListeningFrequency(frequency[Channels::CCH]);
            break;
        }
        //schedule next channel switch in 50ms

    }
    //    else if (msg == buffsend){
    //        channelBusySelf(true);
    //        WaveShortMessage* pktToSend;
    //
    //    }
    else if (msg ==  nextMacEvent) {
        permit = 1;
        //check the boundary
        double timeslot = simTime().dbl()- timestamp ;
        if (timeslot <= bound.at(ID)){
            permit = 0;
        }

        channelBusySelf(true);
        WaveShortMessage* pktToSend;
        pktToSend = myEDCA[activeChannel]->initiateTransmit(lastIdle);
        // buff first
        //        if (ptkinbuff){
        //            // take message from buff
        //            pktToSend = &buff;
        //        }
        //        else{
        //            // otherwise take message from queue
        //            pktToSend = myEDCA[activeChannel]->initiateTransmit(lastIdle);
        //        }
        if(permit == 1){

            //we actually came to the point where we can send a packet

            lastAC = mapPriority(pktToSend->getPriority());
            DBG_MAC << "MacEvent received. Trying to send packet with priority" << lastAC << std::endl;
            //send the packet
            Mac80211Pkt* mac = new Mac80211Pkt(pktToSend->getName(), pktToSend->getKind());
            mac->setDestAddr(LAddress::L2BROADCAST);
            mac->setSrcAddr(myMacAddress);
            mac->encapsulate(pktToSend->dup());

            simtime_t sendingDuration = RADIODELAY_11P +  PHY_HDR_PREAMBLE_DURATION +
                    PHY_HDR_PLCPSIGNAL_DURATION +
                    ((mac->getBitLength() + PHY_HDR_PSDU_HEADER_LENGTH)/bitrate);

            DBG_MAC << "Sending duration will be" << sendingDuration << std::endl;
            simtime_t messageDuration = PHY_HDR_PREAMBLE_DURATION + PHY_HDR_PLCPSIGNAL_DURATION +
                    (mac->getBitLength() + PHY_HDR_PSDU_HEADER_LENGTH)/bitrate;
            T = messageDuration.dbl();



            if ((!useSCH) || (timeLeftInSlot() > sendingDuration)) {
                if (useSCH) DBG_MAC << " Time in this slot left: " << timeLeftInSlot() << std::endl;
                // give time for the radio to be in Tx state before transmitting
                phy->setRadioState(Radio::TX);


                double freq = (activeChannel == type_CCH) ? frequency[Channels::CCH] : frequency[mySCH];

                attachSignal(mac, simTime()+RADIODELAY_11P, freq);
                MacToPhyControlInfo* phyInfo = dynamic_cast<MacToPhyControlInfo*>(mac->getControlInfo());
                assert(phyInfo);
                DBG_MAC << "Sending a Packet. Frequency " << freq << " Priority" << lastAC << std::endl;
                sendDelayed(mac, RADIODELAY_11P, lowerLayerOut);
                statsSentPackets++;
                sendcam.at(ID)++ ;
                timestamp = simTime().dbl();
                double P = statsSentPackets;
                nodeIDout2.record(ID);
                PAC.record(timestamp);

            }
            else {   //not enough time left now
                DBG_MAC << "Too little Time left. This packet cannot be send in this slot." << std::endl;
                statsNumTooLittleTime++;
                //revoke TXOP
                myEDCA[activeChannel]->revokeTxOPs();
                delete mac;
                channelIdle();
                //do nothing. contention will automatically start after channel switch
            }
        }
        else {
            DBG_MAC << "no permit. This packet cannot be send in this slot." << std::endl;
            //            buff =  *pktToSend;
            //            ptkinbuff = true;
            //            simtime_t del = SimTime(bound) - (simTime() - SimTime(timestamp));
            //            scheduleAt(simTime() + del,buffsend);
            myEDCA[activeChannel]->cleanonce();
            channelIdle();
            rejectNum ++;

        }

    }


    //add measuring interval
    else if (msg == measuring){
        //collection
        int Num = 0;
        int state;
        for (int i = 0; i<sendcam.size(); i++){
            Num += sendcam.at(i) - pre_sendcam.at(ID).at(i);
            pre_sendcam.at(ID).at(i) = sendcam.at(i);}



        double CL = Num*T/1.00;
        //        if (CL >=0.22){
        //            error("not");
        //        }

        // channel load increase OR decrease
        //        if (CL > CL_pre){
        //            increase_count += 1;
        //            CL_list_up.push_back(CL);
        //            decrease_count = 0;
        //
        //        }
        //        else if(CL < CL_pre){
        //            decrease_count += 1;
        //            CL_list_down.push_back(CL);
        //            increase_count = 0;

        //        }

        //set the previous data

        // do we need state transition
        //NDL_timeup(need min_cl only)
        //        if (increase_count >=  1){
        //            //            std::vector<double>::iterator result_max = std::max_element(std::begin(CL_list), std::endl(CL_list));
        //            //            double max_load = std::distance(std::begin(CL_list),result_max);
        //            std::vector<double>::iterator result_min = std::min_element(CL_list_up.begin(), CL_list_up.end());
        //            int stp1 = std::distance(CL_list_up.begin(),result_min);
        //            min_load = CL_list_up.at(stp1);
        //            max_load = 0;
        //            increase_count = 0;
        //            CL_list_up.clear();
        //        }
        //        //NDL_timedown(need max_cl only)
        //        else if(decrease_count >= 5){
        //            std::vector<double>::iterator result_max = std::max_element(CL_list_down.begin(), CL_list_down.end());
        //            int stp2 = std::distance(CL_list_down.begin(),result_max);
        //            //            std::vector<double>::iterator result_min = std::max_element(std::begin(CL_list), std::endl(CL_list));
        //            //            double min_load = std::distance(std::begin(CL_list),result_min);
        //            max_load = CL_list_down.at(stp2);
        //            min_load = 0;
        //            decrease_count = 4;
        //            CL_list_down.erase(CL_list_down.begin());
        //        }

               //  2+1 state==================================================================================================
//                        CL_list.push_back(CL);
//                        CL_list.erase(CL_list.begin());
//                        std::vector<double>::iterator result_max = std::max_element(CL_list.begin(), CL_list.end());
//                        int stp = std::distance(CL_list.begin(),result_max);
//                        double CL_max = CL_list.at(stp);
//
//                        //which state we occupy
//                        if (state_machine.at(ID) == "RELAXED"){
//                            state = 0;
//                        } else if(state_machine.at(ID) == "ACTIVE"){
//                            state = 1;
//                        } else if(state_machine.at(ID) == "RESTRICTIVE"){
//                            state = 2;
//                        }
//                        //state transaction
//                        switch(state){
//                        //Relax state, 25HZ, channel load less than 0.15
//                        //Relax state, 10HZ, channel load less than 0.15
//                        case 0:{
//                            if (CL >= 0.15){
//                                state_machine.at(ID) = "ACTIVE";
//                                bound.at(ID) = 0.5;
//                                stateID = 1;
//                            }break;
//                        }
//                        // active step 1, 4HZ, channel load from 0.15 to 0.4
//                        // active step 1, 2HZ, channel load from 0.15 to 0.4
//                        case 1:{
//
//                            if(CL >=0.4){
//                                state_machine.at(ID) = "RESTRICTIVE";
//                                bound.at(ID) = 1;
//                                stateID = 2;
//                            }
//
//                            else if(CL_max < 0.15){
//                                state_machine.at(ID) = "RELAXED";
//                                bound.at(ID) = 0.1;
//                                 stateID = 0;
//                            } break;
//                        }
//                        // RESTRICTIVE state , 1HZ, channel load larger than 0.4
//                        case 2:{
//                            if(CL_max < 0.4){
//                                state_machine.at(ID) = "ACTIVE";
//                                bound.at(ID) = 0.5;
//                              stateID = 1;
//                            }break;
//                        }
//                        }



        // 2+3 state==================================================================================================
                CL_list.push_back(CL);
                CL_list.erase(CL_list.begin());
                std::vector<double>::iterator result_max = std::max_element(CL_list.begin(), CL_list.end());
                int stp = std::distance(CL_list.begin(),result_max);
                double CL_max = CL_list.at(stp);

                //which state we occupy
                if (state_machine.at(ID) == "RELAXED"){
                    state = 0;
                } else if(state_machine.at(ID) == "ACTIVE1"){
                    state = 1;
                } else if(state_machine.at(ID) == "ACTIVE2"){
                    state = 2;
                } else if(state_machine.at(ID) == "ACTIVE3"){
                    state = 3;
                } else if(state_machine.at(ID) == "RESTRICTIVE"){
                    state = 4;
                }
                //state transaction
                switch(state){
                // active step 0, 25HZ, channel load less than 0.15
                // active step 0, 10HZ, channel load less than 0.15
                case 0:{
                    if (CL >= 0.3){
                        stateID = subtate_stateup_3(CL);
                        bound.at(ID) = bound_cal_3(stateID,ID);
                        stateswitch ++;
                    }
                    break;
                }
                // active step 1, 10HZ, channel load from 0.15 to 0.25
                // active step 1, 5HZ, channel load from 0.15 to 0.25
                case 1:{
                    if(CL >=0.4){
                        stateID = subtate_stateup_3(CL);
                        bound.at(ID) = bound_cal_3(stateID,ID);
                        stateswitch ++;
                    }
                    else if(CL_max < 0.3){
                        stateID = subtate_statedown_3(CL_max);
                        bound.at(ID) = bound_cal_3(stateID,ID);
                        stateswitch ++;
                    } break;
                }
                // active step 2, 5HZ, channel load from 0.25 to 0.35
                // active step 2, 3.33HZ, channel load from 0.25 to 0.35
                case 2:{
                    if(CL >= 0.5){
                        stateID = subtate_stateup_3(CL);
                        bound.at(ID) = bound_cal_3(stateID,ID);
                        stateswitch ++;

                    }
                    else if( CL_max < 0.4){
                        stateID = subtate_statedown_3(CL_max);
                        bound.at(ID) = bound_cal_3(stateID,ID);
                        stateswitch ++;

                    } break;
                }
                // active step 3, 2HZ, channel load from 0.35 to 0.4
                case 3:{
                    if(CL >=0.6){
                        stateID = subtate_stateup_3(CL);
                        bound.at(ID) = bound_cal_3(stateID,ID);
                        stateswitch ++;

                    }
                    else if(CL_max < 0.5){
                        stateID = subtate_statedown_3(CL_max);
                        bound.at(ID) = bound_cal_3(stateID,ID);
                        stateswitch ++;
                    } break;
                }
                // active step 4, 1HZ, channel load larger than 0.4
                case 4:{
                    if(CL_max < 0.6){
                        stateID = subtate_statedown_3(CL_max);
                        bound.at(ID) = bound_cal_3(stateID,ID);
                        stateswitch ++;
                    }
                    break;
                }
                }



        // 2+5 state==================================================================================================
//                CL_list.push_back(CL);
//                CL_list.erase(CL_list.begin());
//                std::vector<double>::iterator result_max = std::max_element(CL_list.begin(), CL_list.end());
//                int stp = std::distance(CL_list.begin(),result_max);
//                double CL_max = CL_list.at(stp);
//
//                //which state we occupy
//                if (state_machine.at(ID) == "RELAXED"){
//                    state = 0;
//                } else if(state_machine.at(ID) == "ACTIVE1"){
//                    state = 1;
//                } else if(state_machine.at(ID) == "ACTIVE2"){
//                    state = 2;
//                } else if(state_machine.at(ID) == "ACTIVE3"){
//                    state = 3;
//                }  else if(state_machine.at(ID) == "ACTIVE4"){
//                    state = 4;
//                }  else if(state_machine.at(ID) == "ACTIVE5"){
//                    state = 5;
//                } else if(state_machine.at(ID) == "RESTRICTIVE"){
//                    state = 6;
//                }//                CL_list.push_back(CL);
//
//                //state transaction
//                switch(state){
//                // active step 0, 25HZ, channel load less than 0.15
//                // active step 0, 10HZ, channel load less than 0.15
//                case 0:{
//                    if (CL >= 0.19){
//                        stateID = subtate_stateup_5(CL);
//                        bound.at(ID) = bound_cal_5(stateID,ID);
//                        stateswitch ++;
//                    }
//                    break;
//                }
//                // active step 1, 10HZ, channel load from 0.15 to 0.25
//                // active step 1, 5HZ, channel load from 0.15 to 0.25
//                case 1:{
//                    if(CL >=0.27){
//                        stateID = subtate_stateup_5(CL);
//                        bound.at(ID) = bound_cal_5(stateID,ID);
//                        stateswitch ++;
//                    }
//                    else if(CL_max < 0.19){
//                        stateID = subtate_statedown_5(CL_max);
//                        bound.at(ID) = bound_cal_5(stateID,ID);
//                        stateswitch ++;
//                    } break;
//                }
//                // active step 2, 5HZ, channel load from 0.25 to 0.35
//                // active step 2, 3.33HZ, channel load from 0.25 to 0.35
//                case 2:{
//                    if(CL >= 0.35){
//                        stateID = subtate_stateup_5(CL);
//                        bound.at(ID) = bound_cal_5(stateID,ID);
//                        stateswitch ++;
//
//                    }
//                    else if( CL_max < 0.27){
//                        stateID = subtate_statedown_5(CL_max);
//                        bound.at(ID) = bound_cal_5(stateID,ID);
//                        stateswitch ++;
//
//                    } break;
//                }
//                // active step 3, 2HZ, channel load from 0.35 to 0.4
//                case 3:{
//                    if(CL >=0.43){
//                        stateID = subtate_stateup_5(CL);
//                        bound.at(ID) = bound_cal_5(stateID,ID);
//                        stateswitch ++;
//
//                    }
//                    else if(CL_max < 0.35){
//                        stateID = subtate_statedown_5(CL_max);
//                        bound.at(ID) = bound_cal_5(stateID,ID);
//                        stateswitch ++;
//                    } break;
//                }
//                case 4:{
//                                    if(CL >=0.51){
//                                        stateID = subtate_stateup_5(CL);
//                                        bound.at(ID) = bound_cal_5(stateID,ID);
//                                        stateswitch ++;
//
//                                    }
//                                    else if(CL_max < 0.43){
//                                        stateID = subtate_statedown_5(CL_max);
//                                        bound.at(ID) = bound_cal_5(stateID,ID);
//                                        stateswitch ++;
//                                    } break;
//                                }
//
//                case 5:{
//                                    if(CL >=0.59){
//                                        stateID = subtate_stateup_5(CL);
//                                        bound.at(ID) = bound_cal_5(stateID,ID);
//                                        stateswitch ++;
//
//                                    }
//                                    else if(CL_max < 0.51){
//                                        stateID = subtate_statedown_5(CL_max);
//                                        bound.at(ID) = bound_cal_5(stateID,ID);
//                                        stateswitch ++;
//                                    } break;
//                                }
//                // active step 4, 1HZ, channel load larger than 0.4
//                case 6:{
//                    if(CL_max < 0.59){
//                        stateID = subtate_statedown_5(CL_max);
//                        bound.at(ID) = bound_cal_5(stateID,ID);
//                        stateswitch ++;
//                    }
//                    break;
//                }
//                }
      // 2+5 state end==================================================================================================


        M_state.record(stateID);
        C_load.record(CL);
        num_pack_cbr.record(Num);
        nodeIDout.record(ID);

        scheduleAt(simTime() + SimTime(1), measuring);
    }

}


void Mac1609_4::handleUpperControl(cMessage* msg) {
    assert(false);
}

void Mac1609_4::handleUpperMsg(cMessage* msg) {

    WaveShortMessage* thisMsg;
    if ((thisMsg = dynamic_cast<WaveShortMessage*>(msg)) == NULL) {
        error("WaveMac only accepts WaveShortMessages");
    }

    t_access_category ac = mapPriority(thisMsg->getPriority());

    DBG_MAC << "Received a message from upper layer for channel "
            << thisMsg->getChannelNumber() << " Access Category (Priority):  "
            << ac << std::endl;

    t_channel chan;

    //rewrite SCH channel to actual SCH the Mac1609_4 is set to
    if (thisMsg->getChannelNumber() == Channels::SCH1) {
        ASSERT(useSCH);
        thisMsg->setChannelNumber(mySCH);
        chan = type_SCH;
    }


    //put this packet in its queue
    if (thisMsg->getChannelNumber() == Channels::CCH) {
        chan = type_CCH;
    }

    int num = myEDCA[chan]->queuePacket(ac,thisMsg);

    //packet was dropped in Mac
    if (num == -1) {
        statsDroppedPackets++;
        return;
    }

    //if this packet is not at the front of a new queue we dont have to reevaluate times
    DBG_MAC << "sorted packet into queue of EDCA " << chan << " this packet is now at position: " << num << std::endl;

    if (chan == activeChannel) {
        DBG_MAC << "this packet is for the currently active channel" << std::endl;
    }
    else {
        DBG_MAC << "this packet is NOT for the currently active chgetFullPathannel" << std::endl;
    }

    if (num == 1 && idleChannel == true && chan == activeChannel) {

        simtime_t nextEvent = myEDCA[chan]->startContent(lastIdle,guardActive());

        if (nextEvent != -1) {
            if ((!useSCH) || (nextEvent <= nextChannelSwitch->getArrivalTime())) {
                if (nextMacEvent->isScheduled()) {
                    cancelEvent(nextMacEvent);
                }
                scheduleAt(nextEvent,nextMacEvent);
                DBG_MAC << "Updated nextMacEvent:" << nextMacEvent->getArrivalTime().raw() << std::endl;
            }
            else {
                DBG_MAC << "Too little time in this interval. Will not schedule nextMacEvent" << std::endl;
                //it is possible that this queue has an txop. we have to revoke it
                myEDCA[activeChannel]->revokeTxOPs();
                statsNumTooLittleTime++;
            }
        }
        else {
            cancelEvent(nextMacEvent);
        }
    }
    if (num == 1 && idleChannel == false && myEDCA[chan]->myQueues[ac].currentBackoff == 0 && chan == activeChannel) {
        myEDCA[chan]->backoff(ac);
    }

}

void Mac1609_4::handleLowerControl(cMessage* msg) {
    if (msg->getKind() == MacToPhyInterface::TX_OVER) {

        DBG_MAC << "Successfully transmitted a packet on " << lastAC << std::endl;

        phy->setRadioState(Radio::RX);

        //message was sent

        //update EDCA queue. go into post-transmit backoff and set cwCur to cwMin
        //        if (ptkinbuff)
        //        {
        //            myEDCA[activeChannel]->postTransmitbuff(lastAC);
        //            ptkinbuff = false;
        //            buff = NULL;
        //        }
        //        else{
        myEDCA[activeChannel]->postTransmit(lastAC);
        //        }
        //channel just turned idle.
        channelIdle();
        //don't set the chan to idle. the PHY layer decides, not us.

        if (guardActive()) {
            opp_error("We shouldnt have sent a packet in guard!");
        }
    }
    else if (msg->getKind() == Mac80211pToPhy11pInterface::CHANNEL_BUSY) {
        channelBusy();
    }
    else if (msg->getKind() == Mac80211pToPhy11pInterface::CHANNEL_IDLE) {
        channelIdle();
    }
    else if (msg->getKind() == Decider80211p::BITERROR) {
        statsSNIRLostPackets++;

        DBG_MAC << "A packet was not received due to biterrors" << std::endl;
    }
    else if(msg->getKind() == Decider80211p::COLLISION) {
        nodeIDout3.record(ID);
        double T = simTime().dbl();
        collisiontime.record(T);
    }
    else if (msg->getKind() == Decider80211p::RECWHILESEND) {
        statsTXRXLostPackets++;
        DBG_MAC << "A packet was not received because we were sending while receiving" << std::endl;
    }
    else if (msg->getKind() == MacToPhyInterface::RADIO_SWITCHING_OVER) {
        DBG_MAC << "Phylayer said radio switching is done" << std::endl;
    }
    else if (msg->getKind() == BaseDecider::PACKET_DROPPED) {
        phy->setRadioState(Radio::RX);
        DBG_MAC << "Phylayer said packet was dropped" << std::endl;
    }
    else {
        DBG_MAC << "Invalid control message type (type=NOTHING) : name=" << msg->getName() << " modulesrc=" << msg->getSenderModule()->getFullPath() << "." << std::endl;
        assert(false);
    }

    if (msg->getKind() == Decider80211p::COLLISION) {
        emit(sigCollision, 1L);
    }

    delete msg;
}

void Mac1609_4::setActiveChannel(t_channel state) {
    activeChannel = state;
    assert(state == type_CCH || (useSCH && state == type_SCH));
}

void Mac1609_4::finish() {
    //clean up queues.

    for (std::map<t_channel,EDCA*>::iterator iter = myEDCA.begin(); iter != myEDCA.end(); iter++) {
        statsNumInternalContention += iter->second->statsNumInternalContention;
        statsNumBackoff += iter->second->statsNumBackoff;

        statsNumBackoff1 += iter->second->statsNumBackoff1;
        statsNumBackoff2 += iter->second->statsNumBackoff2;
        statsNumBackoff3 += iter->second->statsNumBackoff3;

        statsSlotsBackoff += iter->second->statsSlotsBackoff;
        iter->second->cleanUp();
        delete iter->second;
    }

    myEDCA.clear();

    if (nextMacEvent->isScheduled()) {
        cancelAndDelete(nextMacEvent);
    }
    else {
        delete nextMacEvent;
    }
    if (nextChannelSwitch && nextChannelSwitch->isScheduled())
        cancelAndDelete(nextChannelSwitch);

    //stats
    recordScalar("ReceivedUnicastPackets",statsReceivedPackets);
    recordScalar("ReceivedBroadcasts",statsReceivedBroadcasts);
    recordScalar("SentPackets",statsSentPackets);
    recordScalar("SNIRLostPackets",statsSNIRLostPackets);
    recordScalar("RXTXLostPackets",statsTXRXLostPackets);
    recordScalar("TotalLostPackets",statsSNIRLostPackets+statsTXRXLostPackets);
    recordScalar("DroppedPacketsInMac",statsDroppedPackets);
    recordScalar("TooLittleTime",statsNumTooLittleTime);
    recordScalar("TimesIntoBackoff",statsNumBackoff);

    recordScalar("TimesIntoBackoff1",statsNumBackoff1);
    recordScalar("TimesIntoBackoff2",statsNumBackoff2);
    recordScalar("TimesIntoBackoff3",statsNumBackoff3);

    recordScalar("SlotsBackoff",statsSlotsBackoff);
    recordScalar("NumInternalContention",statsNumInternalContention);
    recordScalar("totalBusyTime",statsTotalBusyTime.dbl());
    recordScalar("RejectpacketNumber",rejectNum);
    recordScalar("statesswitchtimes",stateswitch ++);

}

void Mac1609_4::attachSignal(Mac80211Pkt* mac, simtime_t startTime, double frequency) {

    int macPktlen = mac->getBitLength();
    simtime_t duration =
            PHY_HDR_PREAMBLE_DURATION +
            PHY_HDR_PLCPSIGNAL_DURATION +
            ((macPktlen + PHY_HDR_PSDU_HEADER_LENGTH)/bitrate);

    Signal* s = createSignal(startTime, duration, txPower, bitrate, frequency);
    MacToPhyControlInfo* cinfo = new MacToPhyControlInfo(s);

    mac->setControlInfo(cinfo);
}

Signal* Mac1609_4::createSignal(simtime_t start, simtime_t length, double power, double bitrate, double frequency) {
    simtime_t end = start + length;
    //create signal with start at current simtime and passed length
    Signal* s = new Signal(start, length);

    //create and set tx power mapping
    ConstMapping* txPowerMapping = createSingleFrequencyMapping(start, end, frequency, 5.0e6, power);
    s->setTransmissionPower(txPowerMapping);

    Mapping* bitrateMapping = MappingUtils::createMapping(DimensionSet::timeDomain, Mapping::STEPS);

    Argument pos(start);
    bitrateMapping->setValue(pos, bitrate);

    pos.setTime(phyHeaderLength / bitrate);
    bitrateMapping->setValue(pos, bitrate);

    s->setBitrate(bitrateMapping);

    return s;
}

/* checks if guard is active */
bool Mac1609_4::guardActive() const {
    if (!useSCH) return false;
    if (simTime().dbl() - nextChannelSwitch->getSendingTime() <= GUARD_INTERVAL_11P)
        return true;
    return false;
}

/* returns the time until the guard is over */
simtime_t Mac1609_4::timeLeftTillGuardOver() const {
    ASSERT(useSCH);
    simtime_t sTime = simTime();
    if (sTime - nextChannelSwitch->getSendingTime() <= GUARD_INTERVAL_11P) {
        return GUARD_INTERVAL_11P
                - (sTime - nextChannelSwitch->getSendingTime());
    }
    else
        return 0;
}

/* returns the time left in this channel window */
simtime_t Mac1609_4::timeLeftInSlot() const {
    ASSERT(useSCH);
    return nextChannelSwitch->getArrivalTime() - simTime();
}

/* Will change the Service Channel on which the mac layer is listening and sending */
void Mac1609_4::changeServiceChannel(int cN) {
    ASSERT(useSCH);
    if (cN != Channels::SCH1 && cN != Channels::SCH2 && cN != Channels::SCH3 && cN != Channels::SCH4) {
        opp_error("This Service Channel doesnt exit: %d",cN);
    }

    mySCH = cN;

    if (activeChannel == type_SCH) {
        //change to new chan immediately if we are in a SCH slot,
        //otherwise it will switch to the new SCH upon next channel switch
        phy11p->changeListeningFrequency(frequency[mySCH]);
    }
}

void Mac1609_4::setTxPower(double txPower_mW) {
    txPower = txPower_mW;
}

void Mac1609_4::handleLowerMsg(cMessage* msg) {
    Mac80211Pkt* macPkt = static_cast<Mac80211Pkt*>(msg);
    ASSERT(macPkt);

    WaveShortMessage*  wsm =  dynamic_cast<WaveShortMessage*>(macPkt->decapsulate());

    //pass information about received frame to the upper layers
    DeciderResult80211 *macRes = dynamic_cast<DeciderResult80211 *>(PhyToMacControlInfo::getDeciderResult(msg));
    ASSERT(macRes);
    DeciderResult80211 *res = new DeciderResult80211(*macRes);
    wsm->setControlInfo(new PhyToMacControlInfo(res));

    long dest = macPkt->getDestAddr();
    long src = macPkt->getSrcAddr();
    //long mes_time_stamp = macPkt->getCreationTime();//getTimestamp;


    DBG_MAC << "Received frame name= " << macPkt->getName()
                                                                    << ", myState=" << " src=" << macPkt->getSrcAddr()
                                                                    << " dst=" << macPkt->getDestAddr() << " myAddr="
                                                                    << myMacAddress << std::endl;




    if (macPkt->getDestAddr() == myMacAddress) {
        DBG_MAC << "Received a data packet addressed to me." << std::endl;
        statsReceivedPackets++;
        sendUp(wsm);
    }
    else if (dest == LAddress::L2BROADCAST) {
        statsReceivedBroadcasts++;

        sendUp(wsm);

        if (myMacAddress==0){

                pkt_rec.record(simTime().dbl());
                data_a.record(macPkt->getSrcAddr());
                pkt_timestamp.record(macPkt->getCreationTime());


        };

    }
    else {
        DBG_MAC << "Packet not for me, deleting..." << std::endl;
        delete wsm;
    }
    delete macPkt;
}

int Mac1609_4::EDCA::queuePacket(t_access_category ac,WaveShortMessage* msg) {

    if (maxQueueSize && myQueues[ac].queue.size() >= maxQueueSize) {
        delete msg;
        return -1;
    }
    myQueues[ac].queue.push(msg);
    return myQueues[ac].queue.size();
}

int Mac1609_4::EDCA::createQueue(int aifsn, int cwMin, int cwMax,t_access_category ac) {

    if (myQueues.find(ac) != myQueues.end()) {
        opp_error("You can only add one queue per Access Category per EDCA subsystem");
    }

    EDCAQueue newQueue(aifsn,cwMin,cwMax,ac);
    myQueues[ac] = newQueue;

    return ++numQueues;
}

Mac1609_4::t_access_category Mac1609_4::mapPriority(int prio) {
    //dummy mapping function
    switch (prio) {
    case 0: return AC_BK;
    case 1: return AC_BE;
    case 2: return AC_VI;
    case 3: return AC_VO;
    default: opp_error("MacLayer received a packet with unknown priority"); break;
    }
    return AC_VO;
}

WaveShortMessage* Mac1609_4::EDCA::initiateTransmit(simtime_t lastIdle) {

    //iterate through the queues to return the packet we want to send
    WaveShortMessage* pktToSend = NULL;

    simtime_t idleTime = simTime() - lastIdle;

    DBG_MAC << "Initiating transmit at " << simTime() << ". I've been idle since " << idleTime << std::endl;

    for (std::map<t_access_category, EDCAQueue>::iterator iter = myQueues.begin(); iter != myQueues.end(); iter++) {
        if (iter->second.queue.size() != 0) {
            if (idleTime >= iter->second.aifsn* SLOTLENGTH_11P + SIFS_11P && iter->second.txOP == true) {

                DBG_MAC << "Queue " << iter->first << " is ready to send!" << std::endl;

                iter->second.txOP = false;
                //this queue is ready to send
                if (pktToSend == NULL) {
                    pktToSend = iter->second.queue.front();
                }
                else {
                    //there was already another packet ready. we have to go increase cw and go into backoff. It's called internal contention and its wonderful

                    statsNumInternalContention++;
                    iter->second.cwCur = std::min(iter->second.cwMax,iter->second.cwCur*2);
                    iter->second.currentBackoff = intuniform(0,iter->second.cwCur);
                    DBG_MAC << "Internal contention for queue " << iter->first  << " : "<< iter->second.currentBackoff << ". Increase cwCur to " << iter->second.cwCur << std::endl;
                }
            }
        }
    }

    if (pktToSend == NULL) {
        opp_error("No packet was ready");
    }
    return pktToSend;
}

simtime_t Mac1609_4::EDCA::startContent(simtime_t idleSince,bool guardActive) {

    DBG_MAC << "Restarting contention." << std::endl;

    simtime_t nextEvent = -1;

    simtime_t idleTime = SimTime().setRaw(std::max((int64_t)0,(simTime() - idleSince).raw()));;

    lastStart = idleSince;

    DBG_MAC << "Channel is already idle for:" << idleTime << " since " << idleSince << std::endl;

    //this returns the nearest possible event in this EDCA subsystem after a busy channel

    for (std::map<t_access_category, EDCAQueue>::iterator iter = myQueues.begin(); iter != myQueues.end(); iter++) {
        if (iter->second.queue.size() != 0) {

            /* 1609_4 says that when attempting to send (backoff == 0) when guard is active, a random backoff is invoked */

/*            if (guardActive == true && iter->second.currentBackoff == 0) {
                //cw is not increased
                std::srand(std::clock());
                int r = rand() % (iter->second.cwCur);
                //iter->second.currentBackoff = intuniform(0,iter->second.cwCur);
                iter->second.currentBackoff = r;
//                botime.record(r);

                statsNumBackoff++;
            }*/

            if (guardActive == true ) {
                if (iter->second.currentBackoff == 0) {

                //cw is not increased
                std::srand(std::clock());
                int r = rand() % (iter->second.cwCur);
                //iter->second.currentBackoff = intuniform(0,iter->second.cwCur);
                iter->second.currentBackoff = r;
//                botime.record(r);

                statsNumBackoff++;
                }

            }






            simtime_t DIFS = iter->second.aifsn * SLOTLENGTH_11P + SIFS_11P;

            //the next possible time to send can be in the past if the channel was idle for a long time, meaning we COULD have sent earlier if we had a packet
            simtime_t possibleNextEvent = DIFS + iter->second.currentBackoff * SLOTLENGTH_11P;
            //simtime_t possibleNextEvent = inf;


            DBG_MAC << "Waiting Time for Queue " << iter->first <<  ":" << possibleNextEvent << "=" << iter->second.aifsn << " * "  << SLOTLENGTH_11P << " + " << SIFS_11P << "+" << iter->second.currentBackoff << "*" << SLOTLENGTH_11P << "; Idle time: " << idleTime << std::endl;

            if (idleTime > possibleNextEvent) {
                DBG_MAC << "Could have already send if we had it earlier" << std::endl;
                //we could have already sent. round up to next boundary
                simtime_t base = idleSince + DIFS;
                possibleNextEvent =  simTime() - simtime_t().setRaw((simTime() - base).raw() % SLOTLENGTH_11P.raw()) + SLOTLENGTH_11P;
            }
            else {
                //we are gonna send in the future
                DBG_MAC << "Sending in the future" << std::endl;
                possibleNextEvent =  idleSince + possibleNextEvent;
            }
            nextEvent == -1? nextEvent =  possibleNextEvent : nextEvent = std::min(nextEvent,possibleNextEvent);
        }
    }
    //double bo = iter; crap
    //bostore.record(bo); crap    int flag = 0;
    //    long last_veh_mac_store =0;
    //double TT = simTime().dbl();
    //botime.record(TT);

    return nextEvent;
}


void Mac1609_4::EDCA::stopContent(bool allowBackoff, bool generateTxOp) {
    //update all Queues

    DBG_MAC << "Stopping Contention at " << simTime().raw() << std::endl;

    simtime_t passedTime = simTime() - lastStart;

    DBG_MAC << "Channel was idle for " << passedTime << std::endl;

    lastStart = -1; //indicate that there was no last start

    for (std::map<t_access_category, EDCAQueue>::iterator iter = myQueues.begin(); iter != myQueues.end(); iter++) {
        if (iter->second.currentBackoff != 0 || iter->second.queue.size() != 0) {
            //check how many slots we already waited until the chan became busy

            int oldBackoff = iter->second.currentBackoff;

            std::string info;
            if (passedTime < iter->second.aifsn * SLOTLENGTH_11P + SIFS_11P) {
                //we didnt even make it one DIFS :(
                info.append(" No DIFS");
            }
            else {
                //decrease the backoff by one because we made it longer than one DIFS
                iter->second.currentBackoff--;

                //check how many slots we waited after the first DIFS
                int passedSlots = (int)((passedTime - SimTime(iter->second.aifsn * SLOTLENGTH_11P + SIFS_11P)) / SLOTLENGTH_11P);

                DBG_MAC << "Passed slots after DIFS: " << passedSlots << std::endl;


                if (iter->second.queue.size() == 0) {
                    //this can be below 0 because of post transmit backoff -> backoff on empty queues will not generate macevents,
                    //we dont want to generate a txOP for empty queues
                    iter->second.currentBackoff -= std::min(iter->second.currentBackoff,passedSlots);
                    info.append(" PostCommit Over");
                }
                else {
                    iter->second.currentBackoff -= passedSlots;
                    if (iter->second.currentBackoff <= -1) {
                        if (generateTxOp) {
                            iter->second.txOP = true; info.append(" TXOP");
                        }
                        //else: this packet couldnt be sent because there was too little time. we could have generated a txop, but the channel switched
                        iter->second.currentBackoff = 0;
                    }

                }
            }
            DBG_MAC << "Updating backoff for Queue " << iter->first << ": " << oldBackoff << " -> " << iter->second.currentBackoff << info <<std::endl;
        }
    }
}
void Mac1609_4::EDCA::backoff(t_access_category ac) {
    myQueues[ac].currentBackoff = intuniform(0,myQueues[ac].cwCur);
    statsSlotsBackoff += myQueues[ac].currentBackoff;
    statsNumBackoff++;
    statsNumBackoff1++;
    DBG_MAC << "Going into Backoff because channel was busy when new packet arrived from upperLayer" << std::endl;
}

void Mac1609_4::EDCA::postTransmit(t_access_category ac) {


    delete myQueues[ac].queue.front();
    myQueues[ac].queue.pop();

    myQueues[ac].cwCur = myQueues[ac].cwMin;
    //post transmit backoff
    myQueues[ac].currentBackoff = intuniform(0,myQueues[ac].cwCur);
    statsSlotsBackoff += myQueues[ac].currentBackoff;
    statsNumBackoff++;
    statsNumBackoff2++;
    DBG_MAC << "Queue " << ac << " will go into post-transmit backoff for " << myQueues[ac].currentBackoff << " slots" << std::endl;
}

void Mac1609_4::EDCA::postTransmitbuff(t_access_category ac) {

    myQueues[ac].cwCur = myQueues[ac].cwMin;
    //post transmit backoff
    myQueues[ac].currentBackoff = intuniform(0,myQueues[ac].cwCur);
    statsSlotsBackoff += myQueues[ac].currentBackoff;
    statsNumBackoff++;
    statsNumBackoff3++;
    DBG_MAC << "Queue " << ac << " will go into post-transmit backoff for " << myQueues[ac].currentBackoff << " slots" << std::endl;
}



void Mac1609_4::EDCA::cleanUp() {
    for (std::map<t_access_category, EDCAQueue>::iterator iter = myQueues.begin(); iter != myQueues.end(); iter++) {
        while (iter->second.queue.size() != 0) {
            delete iter->second.queue.front();
            iter->second.queue.pop();
        }
    }
    myQueues.clear();
}

void Mac1609_4::EDCA::revokeTxOPs() {
    for (std::map<t_access_category, EDCAQueue>::iterator iter = myQueues.begin(); iter != myQueues.end(); iter++) {
        if (iter->second.txOP == true) {
            iter->second.txOP = false;
            iter->second.currentBackoff = 0;
        }
    }
}

void Mac1609_4::EDCA::cleanonce() {
    for (std::map<t_access_category, EDCAQueue>::iterator iter = myQueues.begin(); iter != myQueues.end(); iter++) {
        if (iter->second.queue.size() != 0) {
            delete iter->second.queue.front();
            iter->second.queue.pop();
            break;
        }
    }
}



void Mac1609_4::channelBusySelf(bool generateTxOp) {

    //the channel turned busy because we're sending. we don't want our queues to go into backoff
    //internal contention is already handled in initiateTransmission

    if (!idleChannel) return;
    idleChannel = false;
    DBG_MAC << "Channel turned busy: Switch or Self-Send" << std::endl;

    lastBusy = simTime();

    //channel turned busy
    if (nextMacEvent->isScheduled() == true) {
        cancelEvent(nextMacEvent);
    }
    else {
        //the edca subsystem was not doing anything anyway.
    }
    myEDCA[activeChannel]->stopContent(false, generateTxOp);

    emit(sigChannelBusy, 1L);
}

void Mac1609_4::channelBusy() {

    if (!idleChannel) return;

    //the channel turned busy because someone else is sending
    idleChannel = false;
    DBG_MAC << "Channel turned busy: External sender" << std::endl;
    lastBusy = simTime();

    //channel turned busy
    if (nextMacEvent->isScheduled() == true) {
        cancelEvent(nextMacEvent);
    }
    else {
        //the edca subsystem was not doing anything anyway.
    }
    myEDCA[activeChannel]->stopContent(true,false);

    emit(sigChannelBusy, 1L);
}

void Mac1609_4::channelIdle(bool afterSwitch) {

    DBG_MAC << "Channel turned idle: Switch: " << afterSwitch << std::endl;

    if (nextMacEvent->isScheduled() == true) {
        //this rare case can happen when another node's time has such a big offset that the node sent a packet although we already changed the channel
        //the workaround is not trivial and requires a lot of changes to the phy and decider
        return;
        //opp_error("channel turned idle but contention timer was scheduled!");
    }

    idleChannel = true;

    simtime_t delay = 0;

    //account for 1609.4 guards
    if (afterSwitch) {
        //  delay = GUARD_INTERVAL_11P;
    }
    if (useSCH) {
        delay += timeLeftTillGuardOver();
    }

    //channel turned idle! lets start contention!
    lastIdle = delay + simTime();
    statsTotalBusyTime += simTime() - lastBusy;

    //get next Event from current EDCA subsystem
    simtime_t nextEvent = myEDCA[activeChannel]->startContent(lastIdle,guardActive());
    if (nextEvent != -1) {
        if ((!useSCH) || (nextEvent < nextChannelSwitch->getArrivalTime())) {
            scheduleAt(nextEvent,nextMacEvent);
            DBG_MAC << "next Event is at " << nextMacEvent->getArrivalTime().raw() << std::endl;
        }
        else {
            DBG_MAC << "Too little time in this interval. will not schedule macEvent" << std::endl;
            statsNumTooLittleTime++;
            myEDCA[activeChannel]->revokeTxOPs();
        }
    }
    else {
        DBG_MAC << "I don't have any new events in this EDCA sub system" << std::endl;
    }

    emit(sigChannelBusy, 0L);

}

void Mac1609_4::checkBitrate(int bitrate) const {
    for (unsigned int i = 0; i < sizeof(BITRATES_80211P); i++) {
        if (bitrate == BITRATES_80211P[i]) return;
    }
    opp_error("Chosen Bitrate is not valid for 802.11p: Valid rates are: 3Mbps, 4.5Mbps, 6Mbps, 9Mbps, 12Mbps, 18Mbps, 24Mbps and 27Mbps. Please adjust your omnetpp.ini file accordingly.");
}


//double Mac1609_4::subtate_stateup_1(double CL) const {
//    int state;
//
//    state = 1;
//
//    return state ;
//}
//
//double Mac1609_4::subtate_statedown_1(double CL_max) const {
//    int state;
//
//    state = 1;
//
//    return state ;
//}
//
//double Mac1609_4::bound_cal_1(int stateID, int ID) const {
//    double bound;
//    bound = 0.5;
//    state_machine.at(ID) = "ACTIVE";
//    return bound;
//}


double Mac1609_4::subtate_stateup_3(double CL) const {
    int state;
    if ((CL >= 0.3)&&(CL < 0.4)){
        state = 1;

    }else if((CL >= 0.4)&&(CL < 0.5)){
        state = 2;
    }
    else if((CL >= 0.5)&&(CL < 0.6)){
        state = 3;

    }else if(CL >= 0.6){
        state = 4;
    }
    return state ;
}

double Mac1609_4::subtate_statedown_3(double CL_max) const {
    int state;
    if (CL_max < 0.3){
        state = 0;
    }
    else if ((CL_max < 0.4)&&(CL_max >= 0.3)){
            state = 1;
        }
    else if ((CL_max < 0.5)&&(CL_max >= 0.4)){
        state = 2;
    }
    else if ((CL_max < 0.6)&&(CL_max >= 0.5)){
        state = 3;
    }

    return state ;
}

double Mac1609_4::bound_cal_3(int stateID, int ID) const {
    double bound;
    switch (stateID){
    case 0:
            bound = 0.1;
            state_machine.at(ID) = "RELAXED";
            break;
    case 1:
        bound = 0.2;
        state_machine.at(ID) = "ACTIVE1";
        break;
    case 2:
        bound = 0.4;
        state_machine.at(ID) = "ACTIVE2";
        break;
    case 3:
        bound = 0.5;
        state_machine.at(ID) = "ACTIVE3";
        break;
    case 4:
            bound = 1;
            state_machine.at(ID) = "RESTRICTIVE";
            break;
    }
    return bound;
}


double Mac1609_4::subtate_stateup_5(double CL) const {
    int state;
    if ((CL >= 0.19)&&(CL < 0.27)){
        state = 1;

    }else if((CL >= 0.27)&&(CL < 0.35)){
        state = 2;
    }
    else if((CL >= 0.35)&&(CL < 0.43)){
        state = 3;
    }
    else if((CL >= 0.43)&&(CL < 0.51)){
        state = 4;
    }
    else if((CL >= 0.51)&&(CL < 0.59)){
       state = 5;
    }else if(CL >= 0.59){
        state = 6;
    }
    return state ;
}

double Mac1609_4::subtate_statedown_5(double CL_max) const {
    int state;
    if (CL_max < 0.19){
        state = 0;
    }
    else if ((CL_max < 0.27)&&(CL_max >= 0.19)){
            state = 1;
        }
    else if ((CL_max < 0.35)&&(CL_max >= 0.27)){
        state = 2;
    }
    else if ((CL_max < 0.43)&&(CL_max >= 0.35)){
        state = 3;
    }
    else if ((CL_max < 0.51)&&(CL_max >= 0.43)){
        state = 4;
    }
    else if ((CL_max < 0.59)&&(CL_max >= 0.51)){
        state = 4;
    }

    return state ;
}

double Mac1609_4::bound_cal_5(int stateID, int ID) const {
    double bound;
    switch (stateID){
    case 0:
            bound = 0.06;
            state_machine.at(ID) = "RELAXED";
            break;
    case 1:
        bound = 0.1;
        state_machine.at(ID) = "ACTIVE1";
        break;
    case 2:
        bound = 0.18;
        state_machine.at(ID) = "ACTIVE2";
        break;
    case 3:
        bound = 0.26;
        state_machine.at(ID) = "ACTIVE3";
        break;
    case 4:
        bound = 0.34;
        state_machine.at(ID) = "ACTIVE4";
        break;
    case 5:
        bound = 0.42;
        state_machine.at(ID) = "ACTIVE5";
        break;
    case 6:
        bound = 0.46;
        state_machine.at(ID) = "RESTRICTIVE";
        break;
    }
    return bound;
}


