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

#ifndef INTERSECTIONSCENARIO_H_
#define INTERSECTIONSCENARIO_H_

#include "veins/modules/application/platooning/scenarios/BaseScenario.h"

class IntersectionScenario : public BaseScenario
{

	public:

		virtual void initialize(int stage);
		virtual void finish();

	protected:


	public:
		IntersectionScenario() {}

	protected:

		virtual void handleSelfMsg(cMessage *msg);

};

#endif
