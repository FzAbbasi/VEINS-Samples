

#pragma once

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"

#include "veins/modules/messages/TrafficInfoMsg_m.h"



namespace veins {



class VEINS_API TraCIDemo11p : public DemoBaseApplLayer {
public:
    void initialize(int stage) override;

protected:
    simtime_t lastDroveAt;
    bool sentMessage;
    int currentSubscribedServiceId;

protected:
    void onWSM(BaseFrame1609_4* frame) override;

    void ChangingSpeed(double speed);

};

} // namespace veins
