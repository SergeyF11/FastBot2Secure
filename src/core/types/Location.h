#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "Message.h"
#include "core/api.h"

// https://core.telegram.org/bots/api#sendlocation
namespace fb {

class Location : private Message {
    friend class ::FastBot2;

   public:
    using Message::chatID;
    using Message::notification;
    using Message::protect;
    using Message::removeMenu;
    using Message::reply;
    using Message::setMenu;
    using Message::threadID;

    // широта
    float latitude;

    // долгота
    float longitude;

    // точность в метрах, 0-1500
    float horizontalAccuracy = NAN;

    // период обновления локации в секундах 60.. 86400
    uint32_t livePeriod = 0;

    // направление в градусах, 1-360
    uint16_t heading = 0;

    // Максимальное расстояние в метрах для оповещений о приближении к другому участнику чата
    uint32_t proximityAlertRadius = 0;

   protected:
    void makePacket(fb::Packet& p) {
        Message::makePacket(p);
        p.addFloat(fbapi::latitude(), latitude, 6);
        p.addFloat(fbapi::longitude(), longitude, 6);
        if (!isnan(horizontalAccuracy)) p.addFloat(fbapi::horizontal_accuracy(), horizontalAccuracy, 1);
        if (livePeriod) p.addInt(fbapi::live_period(), livePeriod);
        if (heading) p.addInt(fbapi::heading(), heading);
        if (proximityAlertRadius) p.addInt(fbapi::proximity_alert_radius(), proximityAlertRadius);
    }
};

}  // namespace fb