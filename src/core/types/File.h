#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "Message.h"
#include "core/Multipart.h"

namespace fb {

class File : protected Message {
    friend class ::FastBot2;

   public:
    enum class Type : uint8_t {
        photo,
        audio,
        document,
        video,
        animation,
        voice,
        video_note,
    };

#ifdef FB_ESP_BUILD
    File(const su::Text& name, Type type, ::File& file) : File(name, type, file, false) {}
#endif
    File(const su::Text& name, Type type, uint8_t* bytes, size_t length) : File(name, type, bytes, length, false) {}

    // document by url - GIF, PDF and ZIP
    // https://core.telegram.org/bots/api#sending-files
    File(const su::Text& name, Type type, const su::Text& urlid) : File(name, type, urlid, false) {}

    using Message::chatID;
    using Message::mode;
    using Message::notification;
    using Message::protect;
    using Message::removeMenu;
    using Message::reply;
    using Message::setMenu;
    using Message::threadID;

    // заголовок
    String caption;

    Multipart multipart;

   protected:
    void makePacket(fb::Packet& p) const {
        if (multipart.isFile()) {
            Message::makeQS(p);
            if (caption.length()) p.addQS(fbapi::caption(), caption);
        } else {
            Message::makePacket(p);
            p.addString(multipart.getType(), multipart.getUrlid());
            if (caption.length()) p.addStringEsc(fbapi::caption(), caption);
        }
    }

#ifdef FB_ESP_BUILD
    File(const su::Text& name, Type type, ::File& file, bool edit) : multipart(name, (Multipart::Type)type, file, edit) {}
#endif
    File(const su::Text& name, Type type, uint8_t* bytes, size_t length, bool edit) : multipart(name, (Multipart::Type)type, bytes, length, edit) {}
    File(const su::Text& name, Type type, const su::Text& urlid, bool edit) : multipart(name, (Multipart::Type)type, urlid, edit) {}
};

// https://core.telegram.org/bots/api#editmessagemedia
class FileEdit : protected File {
    friend class ::FastBot2;

   public:
#ifdef FB_ESP_BUILD
    FileEdit(const su::Text& name, Type type, ::File& file) : File(name, type, file, true) {}
#endif
    FileEdit(const su::Text& name, Type type, uint8_t* bytes, size_t length) : File(name, type, bytes, length, true) {}

    // document by url - GIF, PDF and ZIP
    // https://core.telegram.org/bots/api#sending-files
    FileEdit(const su::Text& name, Type type, const su::Text& urlid) : File(name, type, urlid, true) {}

    // id сообщения
    uint32_t messageID;

    using File::caption;
    using File::chatID;
    using File::multipart;
    using File::removeMenu;
    using File::setMenu;

   protected:
    void makePacket(fb::Packet& p) const {
        if (multipart.isFile()) {
            File::Message::makeQS(p);
            p.addQS(fbapi::message_id(), messageID);
            {
                p.beginQS(fbapi::media());
                p.beginObj();
                p.addString(fbapi::type(), multipart.getType());
                p.addString(fbapi::media(), multipart.getAttachName());
                if (caption.length()) p.addStringEsc(fbapi::caption(), caption);
                p.endObj();
                p.end();
            }
        } else {
            File::Message::makePacket(p);
            p.addInt(fbapi::message_id(), messageID);
            {
                p.beginObj(fbapi::media());
                p.addString(fbapi::type(), multipart.getType());
                p.addString(fbapi::media(), multipart.getUrlid());
                if (caption.length()) p.addStringEsc(fbapi::caption(), caption);
                p.endObj();
            }
        }
    }
};

}  // namespace fb