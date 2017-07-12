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
		void sendiCLCMMessage(int destinationAddress);

        unsigned long myMIO_ID; //ID of most important object (in front), 0 mean no pair
        unsigned int myMIO_RANGE; //distance to MIO, 65535 mean n/a
        int myMIO_speed;
        unsigned long myFWDPairID; //ID of the forward pair, 0 mean no pair
        unsigned long myBWDPairID; //ID of the backward pair, 0 mean no pair
        bool mergeRequestFlag;
        bool STOM_flag;
        bool Merging_flag;
        bool headVehicleFlag;
		bool tailVehicleFlag;

	public:
		PlatoonMergingProtocol(){
		    sendiCLCM = 0;
		}
		virtual ~PlatoonMergingProtocol();

		virtual void initialize(int stage);
		virtual void finish();
};

#endif /* PLATOONMERGINGPROTOCOL_H_ */
