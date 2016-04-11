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

#ifndef PLATOONMERGINGPROTOCOL_H_
#define PLATOONMERGINGPROTOCOL_H_

#include "BaseProtocol.h"

class PlatoonMergingProtocol : public BaseProtocol
{
    protected:

        virtual void handleSelfMsg(cMessage *msg);
        virtual void handleUpperMsg(cMessage *msg); //override the one in BaseProtocol
        virtual void messageReceived(PlatooningBeacon *pkt, UnicastMessage *unicast);
        cMessage *sendiCLCM;
        cMessage *startMerge;   //Message to start the scenario
        cMessage *PairB2A;
        cMessage *PairA2B;
        cMessage *PrepareGap;
        cMessage *sendSTOMmsg;
        void sendSTOM(int destinationAddress);
        void startMergingScenario(int destinationAddress);
        void sendiCLCMMessage(int destinationAddress);

        unsigned long myMIO_ID; //ID of most important object (in another lane), basically your pair in B2A phase, 0 mean no pair
        unsigned int myMIO_RANGE; //distance to MIO, 65535 mean n/a
        int myMIO_speed;
        unsigned long myFWDPairID; //ID of the forward pair in A2B phase, 0 mean no pair
        unsigned long myBWDPairID; //ID of the backward pair in A2B phase, 0 mean no pair
        bool mergeRequestFlag;
        bool STOM_flag;
        bool Merging_flag;

    public:
        PlatoonMergingProtocol() {
            sendSTOMmsg = 0;
        }
        virtual ~PlatoonMergingProtocol();

        virtual void initialize(int stage);
        virtual void finish();

        unsigned long getMyMIO_ID() {return myMIO_ID;}
        void setMyMIO_ID(unsigned long id) {myMIO_ID=id;}
        unsigned long getMyMIO_RANGE() {return myMIO_RANGE;}
        void setMyMIO_RANGE(unsigned long range) {myMIO_RANGE = range;}
        int getMyMIO_Speed() {return myMIO_speed;}
        void setMyMIO_Speed(int speed) {myMIO_speed = speed;}
        unsigned long getMyFWDPairID() {return myFWDPairID;}
        void setMyFWDPairID(unsigned long id) {myFWDPairID = id;}
        unsigned long getMyBWDPairID() {return myBWDPairID;}
        void setMyBWDPairID(unsigned long id) {myBWDPairID = id;}
        bool getMergeRequestedFlag() {return mergeRequestFlag;}
        void setMergeRequestedFlag(bool flag) {mergeRequestFlag = flag;}
        bool getSTOMFlag() {return STOM_flag;}
        void setSTOMFlag(bool flag) {STOM_flag = flag;}
        bool getMergingFlag() {return Merging_flag;}
        void setMergingFlag(bool flag) {Merging_flag = flag;}

};


#endif /* PLATOONMERGINGPROTOCOL_H_ */
