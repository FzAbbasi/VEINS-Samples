

#pragma once

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"

#include "veins/modules/messages/TrafficInfoMsg_m.h"


namespace veins {

/**
 * Small RSU Demo using 11p
 */
class VEINS_API TraCIDemoRSU11p : public DemoBaseApplLayer {

public:
    void initialize(int stage) override;
protected:
    void handleSelfMsg(cMessage* msg) override;
};

} // namespace veins
