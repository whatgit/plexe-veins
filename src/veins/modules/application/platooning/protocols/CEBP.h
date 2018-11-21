//
// Copyright (C) 2018 Maytheewat Aramrattana <maytheewat.aramrattana@vti.se>
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

#ifndef CEBP_H_
#define CEBP_H_

#include "BaseProtocol.h"

class CEBP : public BaseProtocol
{
    protected:

        cMessage *sendBrakeRequest;
        cMessage *sendBrakeAck;
        cMessage *TriggerBrakeACK;

        virtual void handleSelfMsg(cMessage *msg);
        virtual void handleUpperMsg(cMessage *msg); //override the one in BaseProtocol
        virtual void messageReceived(PlatooningBeacon *pkt, UnicastMessage *unicast);

        void sendEBrakeRequest(int destinationAddress);
        void sendEBrakeAck(int destinationAddress);
        void sendEBrakeDirectly(int destinationAddress);

    public:
        CEBP(){

        }
        virtual ~CEBP();

        virtual void initialize(int stage);
        virtual void finish();
};

#endif /* CEBP_H_ */
