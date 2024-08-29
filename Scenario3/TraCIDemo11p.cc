

#include "veins/modules/application/traci/TraCIDemo11p.h"

#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"

#include "veins/modules/messages/ControlCongestionMsg_m.h"

using namespace veins;

Define_Module(veins::TraCIDemo11p);

void TraCIDemo11p::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        // Schedule the first congestion status broadcast
        scheduleAt(simTime() + 1.0, new cMessage("TrafficStatus"));
    }
}



void TraCIDemo11p::onWSM(BaseFrame1609_4* frame)
{
    ControlCongestionMsg* congestionMsg = dynamic_cast<ControlCongestionMsg*>(frame);
    if (congestionMsg) {

        EV << " congestion info from " << congestionMsg->getAddress()
           << ": Speed = " << congestionMsg->getSenderSpeed()
           << ", Position = " << congestionMsg->getSenderPos()
           << ", Is Congested = " << congestionMsg->isCongested() << endl;

        // Implement logic to adjust routes or notify the driver
        if (congestionMsg->isCongested()) {

            Coord Position = mobility->getPositionAt(simTime());
            Coord destination = getVehicleDestination();

            // Check if an alternative route is available
            std::vector<Coord> OtherRoute = findAlternativeRoute(Position, destination);

            if (!OtherRoute.empty()) {
                // Set the vehicle's route
                mobility->setRoute(OtherRoute);
                EV << "Adjusting route to avoid congestion." << endl;
            } else {
                notifyDriver("You will face traffic congestion .");
            }

        }

    } else {
        DemoBaseApplLayer::onWSM(frame);
    }

}

void TraCIDemo11p::handleSelfMsg(cMessage* msg)
{
    if (strcmp(msg->getName(), "TrafficStatus") == 0) {
        ControlCongestionMsg* congestionMsg = new ControlCongestionMsg();
        congestionMsg->setAddress(mobility->getIndex());
        congestionMsg->setSenderSpeed(mobility->getSpeed());
        congestionMsg->setSenderPos(mobility->getPositionAt(simTime()));
        congestionMsg->setIsCongested(mobility->getSpeed() < 10.0);

        // Broadcast the congestion status
        sendDown(congestionMsg);

        // Reschedule the next broadcast
        scheduleAt(simTime() + 2.0, new cMessage("TrafficStatus"));
    } else {
        DemoBaseApplLayer::handleSelfMsg(msg);
    }

}

