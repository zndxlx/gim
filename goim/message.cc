//
// Created by lixin on 2020/7/23.
//

#include "Message.h"
#include "evpp/logging.h"

namespace goim {
    int Message::ReadBuf(evpp::Buffer *buf){
        if (buf->size() < RawHeaderSize) {
            return 1;
        }
        int32_t packLen = buf->PeekInt32();
        if (packLen > MaxPackSize) {
            XERROR("pack length error, packLen=%d", packLen);
            return -1;
        }
        if (buf->size() >= (uint32_t)packLen) {
            buf->Skip(sizeof packLen);
            int16_t headLen = buf->ReadInt16();
            if (headLen != RawHeaderSize) {
                XERROR("header length error, headLen=%d", headLen);
                return -1;
            }
            version_ = buf->ReadInt16();
            op_ = buf->ReadInt32();
            seq_ = buf->ReadInt32();
            body_ = buf->NextString(packLen - RawHeaderSize);
            return 0;
        } else {
            XINFO("need more data");
            return 1;
        }
    }

    void Message::WriteBuf(evpp::Buffer* buf) const {
        int32_t packLen = RawHeaderSize + body_.size();
        buf->AppendInt32(packLen);
        buf->AppendInt16(RawHeaderSize);
        buf->AppendInt16(version_);
        buf->AppendInt32(op_);
        buf->AppendInt32(seq_);
        buf->Append(body_);
        return;
    }

    std::string Message::ToString() const {
        char data[MaxPackSize];
        sprintf_s(data, MaxPackSize,"op[%d],seq[%d],body[%s]", op_, seq_, body_.c_str());
        return std::string(data);
    }

}