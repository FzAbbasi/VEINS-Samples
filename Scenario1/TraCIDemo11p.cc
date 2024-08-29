//
// Copyright (C) 2006-2011 Christoph Sommer <christoph.sommer@uibk.ac.at>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "veins/modules/application/traci/TraCIDemo11p.h"

#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"


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

void TraCIDemo11p::onWSA(DemoServiceAdvertisment* wsa)
{
    if (currentSubscribedServiceId == -1) {
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
        currentSubscribedServiceId = wsa->getPsid();
        if (currentOfferedServiceId != wsa->getPsid()) {
            stopService();
            startService(static_cast<Channel>(wsa->getTargetChannel()), wsa->getPsid(), "Mirrored Traffic Service");
        }
    }
}

void TraCIDemo11p::onWSM(BaseFrame1609_4* frame)
{
    TraCIDemo11pMessage* wsm = check_and_cast<TraCIDemo11pMessage*>(frame);

    findHost()->getDisplayString().setTagArg("i", 1, "green");

    if (mobility->getRoadId()[0] != ':') traciVehicle->changeRoute(wsm->getDemoData(), 9999);
    if (!sentMessage) {
        sentMessage = true;
        // repeat the received traffic update once in 2 seconds plus some random delay
        wsm->setSenderAddress(myId);
        wsm->setSerial(3);
        scheduleAt(simTime() + 2 + uniform(0.01, 0.2), wsm->dup());
    }
}






void TraCIDemo11p::onBSM(DemoSafetyMessage* bsm)
{
    // Handle incoming Basic Safety Message (BSM)
    if (bsm) {
        // Process the received BSM
        Id = bsm->getSenderId();
        Speed = bsm->getSenderSpeed();
        Position =  bsm->getSenderPos();

        EV << ": Id =" << Id
           << ": Speed = " << Speed.x << ", " << Speed.y << ", " << Speed.z
           << ": Position = " << Position.x << ", " << Position.y << ", " << Position.z<< endl;

        // Append the received speed and position to the history vectors
        speedHistory.push_back(Speed);
        idHistory.push_back( Id);
        positionHistory.push_back(Position);

    }

}






void TraCIDemo11p::handleSelfMsg(cMessage* msg)
{
    if (strcmp(msg->getName(), "SendStatusUpdate") == 0) {
        // Create a new Base Safety Message (BSM)
        DemoSafetyMessage* bsm = new DemoSafetyMessage();
        bsm->setSenderId(mobility->getId());
        bsm->setSenderSpeed(mobility->getHostSpeed());
        bsm->setSenderPos(mobility->getPositionAt(simTime()));



        // Send the message on the service channel
        sendDown(bsm);

        // Reschedule the next status update
        scheduleAt(simTime() + 2.0, new cMessage("SendStatusUpdate"));
    }
    else {
        DemoBaseApplLayer::handleSelfMsg(msg);
    }

}

void TraCIDemo11p::handlePositionUpdate(cObject* obj)
{
    DemoBaseApplLayer::handlePositionUpdate(obj);

    // stopped for for at least 10s?
    if (mobility->getSpeed() < 1) {
        if (simTime() - lastDroveAt >= 10 && sentMessage == false) {
            findHost()->getDisplayString().setTagArg("i", 1, "red");
            sentMessage = true;

            TraCIDemo11pMessage* wsm = new TraCIDemo11pMessage();
            populateWSM(wsm);
            wsm->setDemoData(mobility->getRoadId().c_str());

            // host is standing still due to crash
            if (dataOnSch) {
                startService(Channel::sch2, 42, "Traffic Information Service");
                // started service and server advertising, schedule message to self to send later
                scheduleAt(computeAsynchronousSendingTime(1, ChannelType::service), wsm);
            }
            else {
                // send right away on CCH, because channel switching is disabled
                sendDown(wsm);
            }
        }
    }
    else {
        lastDroveAt = simTime();
    }
}





void TraCIDemo11p::finish() {
    // Call the base class finish method
    DemoBaseApplLayer::finish();

    std::string resultsPath = getEnvir()->getConfigEx()->getVariable(CFGVAR_RESULTDIR);
    std::string csvFilePath = resultsPath + "/vehicle_data.csv";

    std::ofstream csvFile(csvFilePath);
    if (csvFile.is_open()) {
        csvFile << "Id,Speed_X,Speed_Y,Speed_Z,Position_X,Position_Y,Position_Z" << std::endl;

        EV << "Speed history size: " << speedHistory.size() << endl;
        EV << "Position history size: " << positionHistory.size() << endl;
        EV << "Id history size: " << idHistory.size() << endl;

        for (size_t i = 0; i < speedHistory.size(); i++) {
            csvFile << idHistory[i] << ","
                     << speedHistory[i].x << ","
                     << speedHistory[i].y << ","
                     << speedHistory[i].z << ","
                     << positionHistory[i].x << ","
                     << positionHistory[i].y << ","
                     << positionHistory[i].z << std::endl;
        }

        csvFile.close();
        EV << "Data written to " << csvFilePath << std::endl;
    } else {
        EV << "Failed to open file for writing: " << csvFilePath << std::endl;
    }
}
