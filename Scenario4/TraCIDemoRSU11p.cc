

#include "veins/modules/application/traci/TraCIDemoRSU11p.h"

#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"

#include "veins/modules/messages/TrafficInfoMsg_m.h"




using namespace veins;

Define_Module(veins::TraCIDemoRSU11p);



void TraCIDemoRSU11p::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        // Schedule the first traffic information broadcast
        scheduleAt(simTime() + 1.0, new cMessage("TrafficInfo"));
    }
}

void TraCIDemoRSU11p::handleSelfMsg(cMessage* msg)
{
    if (strcmp(msg->getName(), "TrafficInfo") == 0) {
        TrafficInfoMsg* trafficMsg = new TrafficInfoMsg();
        trafficMsg->setIsCongested(false);
        trafficMsg->setAvrgSpeed(40.0); //Average speed on the road
        trafficMsg->setRoadCondition("Clear"); // Example: Heavy , Clear, Snowy ,...

        sendDown(trafficMsg);

        // Reschedule the next broadcast
        scheduleAt(simTime() + 5.0, new cMessage("TrafficInfo"));
    } else {
        DemoBaseApplLayer::handleSelfMsg(msg);
    }
}
