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
packet RequestRamp{
	//id of the originator
	int vehicleId = 0;
	//id of the sender. relayerId == vehicleId on first hope
	int relayerId = 0;
	
	//Position of the off-ramp
	double RampX = 0;	//obtained from the road network
	double RampY = 0;	//not used at the moment
	
	//The vehicle's data
	double acceleration = 0;
	double speed = 0;
	double positionX = 0;
	double positionY = 0;
	double SUMOpositionX = 0;
	double SUMOpositionY = 0;
	double time = 0;
	int sequenceNumber = 0;
}