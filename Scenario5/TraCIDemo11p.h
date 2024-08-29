

#pragma once

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"

#include "veins/modules/messages/TrafficInfoMsg_m.h"

#include "veins/modules/mac/ieee80211p/Mac80211pToPhy11pInterface.h"





namespace veins {

/**
 * @brief
 * A tutorial demo for TraCI. When the car is stopped for longer than 10 seconds
 * it will send a message out to other cars containing the blocked road id.
 * Receiving cars will then trigger a reroute via TraCI.
 * When channel switching between SCH and CCH is enabled on the MAC, the message is
 * instead send out on a service channel following a Service Advertisement
 * on the CCH.
 *
 * @author Christoph Sommer : initial DemoApp
 * @author David Eckhoff : rewriting, moving functionality to DemoBaseApplLayer, adding WSA
 *
 */

class VEINS_API TraCIDemo11p : public DemoBaseApplLayer {
public:
    void initialize(int stage) override;

protected:
    simtime_t lastDroveAt;
    bool sentMessage;
    int currentSubscribedServiceId;

protected:
    void onWSM(BaseFrame1609_4* wsm) override;

    void handleSelfMsg(cMessage* msg) override;


    void switching_channel(bool toServiceChannel);
    void ChangingSpeed(double speed);




};

} // namespace veins
