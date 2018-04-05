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

#ifndef INTERSECTIONPROTOCOL_H_
#define INTERSECTIONPROTOCOL_H_

#include "BaseProtocol.h"

class IntersectionProtocol : public BaseProtocol
{
	protected:

		virtual void handleSelfMsg(cMessage *msg);
		virtual void handleUpperMsg(cMessage *msg); //override the one in BaseProtocol
		virtual void messageReceived(PlatooningBeacon *pkt, UnicastMessage *unicast);

		cMessage *sendIntersecion;
		void sendIntersectionMessage(int destinationAddress);

	public:
		IntersectionProtocol(){
		    sendIntersecion = 0;
		}
		virtual ~IntersectionProtocol();

		virtual void initialize(int stage);
		virtual void finish();
};

#endif /* INTERSECTIONPROTOCOL_H_ */
