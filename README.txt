Documentation for these modules is at http://veins.car2x.org/

Hello guys, 

First of all, I recommend you to install OMNeT++ IDE, it's very useful to navigate through plexe-veins code. Follow the guide here >> https://omnetpp.org/doc/omnetpp/InstallGuide.pdf

Plexe documentation is here >> http://plexe.car2x.org/documentation/plexe-1.1-documentation.pdf

Not sure if this is the best page but if you're interested in implementing a car-following model >> http://sumo.dlr.de/wiki/Developer/Implementation_Notes/Vehicle_Models

//8 December 2015 : Updated with gap making example
Please run the sinPlatoon example with ./run -u Cmdenv -f omnetpp.ini -c Sinusoidal -r 2
At simulation time 40 seconds, it will receive "makeGap" message and the platoon on the rightmost lane will make a gap. If I understood correctlty, the message is just an event in OMNeT++ simulation. Anyway, we can assume that this kind of messages comes from the organizer for starting scenatios and so on.
