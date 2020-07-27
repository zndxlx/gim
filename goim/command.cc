//
// Created by lixin on 2020/7/23.
//

#include "command.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include <iostream>
#include <string>

namespace goim {
    uint32_t Command::id_seq_ = 0;

    SyncCommand::SyncCommand(const std::string &msgID, SyncCallback callback) {
        XINFO("SyncCommand 5555555555");
        req_.op_ = Message::OpSyncMsg;
        req_.body_ = "{\"msgID\":\"";
        req_.body_ += msgID;
        req_.body_ += "\"}";
        req_.seq_ = nextID();
        sendOnly_ = false;
        syncCallback_ = callback;
        seq_ = req_.seq_;
    }

    void SyncCommand::OnCommandDone(Message &rsp) {
        XINFO("rsp=%s", rsp.ToString().c_str());
        SyncRsp syncRsp;
        syncRsp.ParseRsp(rsp.body_);
        syncCallback_(seq_, 0, syncRsp);
        return;
    }

    void SyncCommand::OnError(int code) {
        SyncRsp syncRsp;
        syncCallback_(seq_, code, syncRsp);
    }

    AckCommand::AckCommand(const std::vector<std::string> &msgIDs) {
        req_.op_ = Message::OpSyncMsg;
        req_.body_ = BuildBody(msgIDs);
        req_.seq_ = nextID();
        sendOnly_ = true;
    }

    std::string AckCommand::BuildBody(const std::vector<std::string> &msgIDs) {
        rapidjson::StringBuffer strBuf;
        rapidjson::Writer<rapidjson::StringBuffer> writer(strBuf);

        writer.StartObject();

        writer.Key("msgIDs");

        writer.StartArray();
        writer.String("one");
        for (auto v:msgIDs) {
            writer.String(v.c_str());
        }
        writer.EndArray();

        writer.EndObject();

        std::string data = strBuf.GetString();
        return data;
    }

    int AuthRsp::ParseRsp(std::string &rsp) {
        if (rsp.size() == 0) {
            return 0;
        }
        rapidjson::Document doc;
        if (!doc.Parse(rsp.c_str()).HasParseError()) {
            if (doc.HasMember("lastAck") && doc["lastAck"].IsString()) {
                lastAck_ = doc["lastAck"].GetString();
            }
            if (doc.HasMember("lastMsg") && doc["lastMsg"].IsString()) {
                lastMsg_ = doc["lastMsg"].GetString();
            }
        } else {
            XERROR("json parse error, str=%s", rsp.c_str());
            return -1;
        }
    }


    int MsgNotify::ParseRsp(std::string &rsp) {
        rapidjson::Document doc;
        if (!doc.Parse(rsp.c_str()).HasParseError()) {
            if (doc.HasMember("msgID") && doc["msgID"].IsString()) {
                msgId_ = doc["msgID"].GetString();
                return 0;
            } else {
                XERROR("no msgID or not string");
                return -1;
            }
        } else {
            XERROR("json parse error, str=%s", rsp.c_str());
            return -1;
        }
    }

    std::string SyncRsp::ToString() const {
        std::string log = "ret=";
        log += std::to_string(ret_);
        log += ", ";
        for (auto m:msgs_) {
            log += "msgid:";
            log += m.msgID_;
            log += ",sendTime:";
            log += m.sendTime_,
            log += ",body:";
            log += m.msgBody_;
            log += ",";
        }
        return log;

    }


    int SyncRsp::ParseRsp(std::string &rsp) {
        rapidjson::Document doc;
        if (!doc.Parse(rsp.c_str()).HasParseError()) {
            if (doc.HasMember("ret") && doc["ret"].IsInt()) {
                ret_ = doc["ret"].GetInt();
            }
            if (doc.HasMember("msgs") && doc["msgs"].IsArray()) {
                const rapidjson::Value &array = doc["msgs"];
                size_t len = array.Size();

                msgs_.resize(len);

                for (size_t i = 0; i < len; i++) {
                    const rapidjson::Value &object = array[i];
                    if (object.IsObject()) {
                        std::cout << "ObjectArray[" << i << "]: ";
                        if (object.HasMember("msgID") && object["msgID"].IsString()) {
                            msgs_[i].msgID_ = object["msgID"].GetString();
                        }
                        if (object.HasMember("sendTime") && object["sendTime"].IsString()) {
                            msgs_[i].sendTime_ = object["sendTime"].GetString();
                        }
                        if (object.HasMember("msgBody") && object["msgBody"].IsString()) {
                            msgs_[i].msgBody_ = object["msgBody"].GetString();
                        }

                    }
                }
            }
            return 0;
        }else {
            return -1;
        }

    }
}