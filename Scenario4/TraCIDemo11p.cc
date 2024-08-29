

#include "veins/modules/application/traci/TraCIDemo11p.h"

#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"

#include "veins/modules/messages/TrafficInfoMsg_m.h"

using namespace veins;

Define_Module(veins::TraCIDemo11p);

void TraCIDemo11p::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        sentMessage = false;
        lastDroveAt = simTime();
        currentSubscribedServiceId = -1;
    }
}

void TraCIDemo11p::onWSM(BaseFrame1609_4* frame)
{
    TrafficInfoMsg* trafficMsg = dynamic_cast<TrafficInfoMsg*>(frame);
    if (trafficMsg) {
        EV << "traffic information: "
           << "Congested = " << trafficMsg->isCongested()
           << ", Average Speed = " << trafficMsg->getAvrgSpeed()
           << ", Road Condition = " << trafficMsg->getRoadCondition() << endl;

        // set new speed based on traffic info
        if (trafficMsg->isCongested()) {

            // current speed
            TraCICommandInterface::Vehicle* traciVehicle = mobility->getVehicleCommandInterface();
            double currentSpeed =traciVehicle->getSpeed();
            ChangingSpeed(currentSpeed);

        }
    } else {
        DemoBaseApplLayer::onWSM(frame);
    }
}


void TraCIDemo11p::ChangingSpeed(double speed)
{
    // Reduce speed by 50%
    double newSpeed = speed * 0.5;
    traciVehicle->setSpeed(newSpeed);

}
