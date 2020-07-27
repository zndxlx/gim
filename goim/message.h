//
// Created by lixin on 2020/7/23.
//

#ifndef APP_MESSAGE_H
#define APP_MESSAGE_H

#include <stdint.h>
#include "evpp/buffer.h"

namespace goim {

class Message {
public:
    int ReadBuf(evpp::Buffer *buf); // < 0 err, 0, success, 1 need more data
    void WriteBuf(evpp::Buffer* buf) const;
    std::string ToString() const;
public:
    uint16_t version_{200};
    int32_t op_;
    int32_t seq_{0};
    std::string body_{""};
public:
    static const int16_t RawHeaderSize = 16;
    static const int16_t MaxPackSize = (1 << 12);
    enum {
        OpHeartbeat = 2,
        OpHeartbeatReply = 3,
        OpSendMsg = 4,
        OpSendMsgReply = 5,
        OpAuth = 7,
        OpAuthReply = 8,
        OpSyncMsg       = 17,
        OpSyncMsgReply  = 18,
        OpAckMsg        = 19,
        OpMsgNotify     = 20,
    };
};

}


#endif //APP_MESSAGE_H
