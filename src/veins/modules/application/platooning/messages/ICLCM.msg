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
packet ICLCM {
	unsigned char header = 10;  //10 is header for iCLCM
	unsigned long stationID = 0;  //my vehicle id
	unsigned int rearAxleLocation = 0;  //Should range from 0 to 4095 meters, no idea where it will be used
	unsigned char controllerType = 0; //Range from 0 to 3 | 0:Manual | 1:CC | 2:ACC | 3: CACC |
	unsigned int vehicleResponseTime = 1001;  //Yet another no idea variable, just put 1001 as info n/a
	unsigned int vehicleResponseTimeDelay = 1001;  //Yet another no idea variable, just put 1001 as info n/a
	int desiredLongitudinalAcceleration = 1001; //Should range from -1001 to 1001 m/s2, no idea where it will be used,  just put 1001 as info n/a
	unsigned long MIO_ID = 0; //ID of most important object (in another lane), basically your pair in B2A phase, 0 mean no pair
	unsigned int MIO_RANGE = 65535; //distance to MIO, 65535 mean n/a
	unsigned int MIO_BEARING = 1572; //Should range from -1572 to 1572 rad, no idea where it will be used
	int MIO_RANGE_RATE = 32767;  //Should range from -32767 to 32767 rad, 32767 mean n/a
	unsigned int timeHeadway = 361; //seconds, time head way, 361 = n/a
	unsigned int cruiseSpeed = 5001; //Speed of platoon leader, 5001 = n/a
	bool mergeRequestFlag = false; //should be 1 or 0. 
	bool STOMFlag = false; //Safe-to-merge flag
	bool mergingFlag = false;  //Merging status of vehicle: gaps are made and indicate vehicle is ready for merging(?)
	unsigned long FWDPairID = 0; //ID of the forward pair in A2B phase, 0 mean no pair
	unsigned long BWDPairID = 0; //ID of the backward pair in A2B phase, 0 mean no pair
	bool tailVehicle = false; //Last vehicle in platoon?
	bool headVehicle = false; //Leader of the platoon?
	unsigned char PlatoonID = 0; //PlatoonA = 1, PlatoonB = 2
	int someRandomStuffForScenatio2 = 0;
	int anotherRandomStuffForScenatio2 = 0; 
	unsigned char laneEnterCZ = 0; //Lane id when enter CZ, maybe for scenario2...
	int stillRandomStuffForScenatio2 = 0; 
	bool pairACK = false; //defined but currently not used
	bool readyFlag = false; //ready or not to start scenario?
	unsigned char startPlatoon = 0; //for starting scenario
	bool EoS = false; //End of scenario? after receiving this, do whatever, will not be judged
}