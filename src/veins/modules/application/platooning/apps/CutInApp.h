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

#ifndef CUTINAPP_H_
#define CUTINAPP_H_

#include "veins/modules/application/platooning/apps/BaseApp.h"
#include <veins/modules/mobility/traci/TraCIBaseTrafficManager.h>

class CutInApp : public BaseApp
{

	public:

		virtual void initialize(int stage);
		virtual void finish();

	protected:
		virtual void onBeacon(WaveShortMessage* wsm);
		virtual void onData(WaveShortMessage* wsm);

	public:
		CutInApp() {}

	protected:

		virtual void handleSelfMsg(cMessage *msg);

};

#endif /* CUTINAPP_H_ */
