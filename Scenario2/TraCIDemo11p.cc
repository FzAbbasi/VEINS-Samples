
#include "veins/modules/application/traci/TraCIDemo11p.h"

#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"

#include "veins/modules/messages/EmergencyMessage_m.h"


using namespace veins;

Define_Module(veins::TraCIDemo11p);

void TraCIDemo11p::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        // Schedule the first emergency status broadcast
        scheduleAt(simTime() + 1.0, new cMessage("EmergencyStatus"));
    }
}





void TraCIDemo11p::onWSM(BaseFrame1609_4* frame)
{
    EmergencyMessage* emrgcMsg = dynamic_cast<EmergencyMessage*>(frame);
    if (emrgcMsg) {
        // Handle the emergency vehicle message
        EV << "Received emergency  message from " << emrgcMsg->getVehicleAddress()
           << ": Urgency Level = " << emrgcMsg->getUrgencyLevel()
           << ", Position = " << emrgcMsg->getSenderPos() << endl;

        // Yield for the emergency vehicle
        ChangingBehaviorVehicle(emrgcMsg);
    }
    else {
        DemoBaseApplLayer::onWSM(frame);
        }

    }





void TraCIDemo11p::handleSelfMsg(cMessage* msg)
{
    if (strcmp(msg->getName(), "EmergencyStatus") == 0) {
        EmergencyMessage* emrgcMsg = new EmergencyMessage();
        emrgcMsg->setVehicleAddress(mobility->getId()); // Set the vehicle's ID
        emrgcMsg->setUrgencyLevel(8); // Set high urgency for emergency vehicles
        emrgcMsg->setSenderPos(mobility->getPositionAt(simTime())); // Set current position

        // Broadcast the emergency status
        sendDown(emrgcMsg);

        // Reschedule the next broadcast
        scheduleAt(simTime() + 2.0, new cMessage("EmergencyStatus"));
    } else {
        DemoBaseApplLayer::handleSelfMsg(msg);
    }
}




void TraCIDemo11p::ChangingBehaviorVehicle(EmergencyMessage* msg)
{
    EV << "Chenging  behavior of vehicles at Speed: " << endl;

    // Get the current position and speed of the vehicle
    Coord currentPos = mobility->getPositionAt(simTime());
    TraCICommandInterface::Vehicle* traciVehicle = mobility->getVehicleCommandInterface();
    double currentSpeed =traciVehicle->getSpeed();

    // Calculate the distance to the emergency vehicle
    double distance_To_EmergencyVeh = currentPos.distance(msg->getSenderPos());


    if (distance_To_EmergencyVeh < 30.0) { // if distance is less than 30 meters
        // Slow down the vehicle
        double slow_Speed = currentSpeed * 0.5; // Reduce speed by 50%
        traciVehicle->setSpeed(slow_Speed);

    }
}
