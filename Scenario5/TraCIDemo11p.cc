

#include "veins/modules/application/traci/TraCIDemo11p.h"

#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"

#include "veins/modules/messages/TrafficInfoMsg_m.h"

#include "veins/modules/mac/ieee80211p/Mac1609_4.h"

#include "veins/modules/mac/ieee80211p/Mac80211pToPhy11pInterface.h"




using namespace veins;

Define_Module(veins::TraCIDemo11p);

void TraCIDemo11p::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        // Initialize variables
        sentMessage = false;
        lastDroveAt = simTime();
        currentSubscribedServiceId = -1;

        scheduleAt(simTime() + 1.0, new cMessage("TrafficInfo"));
    }


}


void TraCIDemo11p::onWSM(BaseFrame1609_4* frame)
{
    TrafficInfoMsg* trafficMsg = dynamic_cast<TrafficInfoMsg*>(frame);
    if (trafficMsg) {
        EV << "Traffic information: "
           << "Congested = " << trafficMsg->isCongested()
           << ", Average Speed = " << trafficMsg->getAvrgSpeed()
           << ", Road Condition = " << trafficMsg->getRoadCondition() << endl;

        // Checking congestion of road
        if (trafficMsg->isCongested()) {
            // Switch to Service Channel
            switching_channel(true);

            // Current speed
            TraCICommandInterface::Vehicle* traciVehicle = mobility->getVehicleCommandInterface();
            double currentSpeed =traciVehicle->getSpeed();
            ChangingSpeed(currentSpeed);
        }
    } else {
        DemoBaseApplLayer::onWSM(frame);
    }

}




void TraCIDemo11p::handleSelfMsg(cMessage* msg)
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



void TraCIDemo11p::switching_channel(bool toServiceChannel) {
    cModule* macModule = getParentModule()->getSubmodule("mac1609_4");
    Mac1609_4* mac1609_4 = dynamic_cast<Mac1609_4*>(macModule);

    if (!mac1609_4) {
        EV << "Error: MAC module not found!" << endl;
        return;
    }

    // Check if it is switching to the service channel
    if (toServiceChannel) {
        EV << "Switching to Service Channel." << endl;

        // Set the active channel to the service channel
        mac1609_4->setActiveChannel(ChannelType::service);

        // Changing listening channel of the physical layer  to the service channel
        mac1609_4->phy11p->changeListeningChannel(mac1609_4->mySCH);
    } else {
        EV << "Switching to Control Channel." << endl;

        // Set the active channel back to the control channel
        mac1609_4->setActiveChannel(ChannelType::control);

        // Changing listening channel of the physical layer to the control channel
        mac1609_4->phy11p->changeListeningChannel(Channel::cch);
    }
}


void TraCIDemo11p::ChangingSpeed(double speed)
{
    // Reduce speed by 50%
    double newSpeed = speed * 0.5;
    traciVehicle->setSpeed(newSpeed);

}
