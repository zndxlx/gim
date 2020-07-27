//
// Created by lixin on 2020/7/23.
//

#ifndef APP_COMMAND_H
#define APP_COMMAND_H

#include <vector>
#include "message.h"


namespace goim{
    struct RMessage{
        std::string msgID_;
        std::string sendTime_;
        std::string msgBody_;
    };

    class SyncRsp {
    public:
        SyncRsp(){};
        int ParseRsp(std::string &rsp);
        std::string ToString() const;

        int ret_;
        std::vector<RMessage> msgs_;
    };

    typedef std::function<void(uint32_t seq, int err, SyncRsp &rsp)> SyncCallback;

    class Command {
    public:
        Command() {
        }

        virtual void OnError(int code) = 0;
        virtual void OnCommandDone(Message &rsp) = 0;
        static  uint32_t nextID() {
            return ++id_seq_;
        }
    public:
        bool sendOnly_;
        //int needReTryTimes_;
        //int reTryTimes_;
        Message req_;
        static uint32_t id_seq_;
        int seq_;
        //Message rsp_;
    };

    typedef std::shared_ptr<Command> CommandPtr;

    class SyncCommand : public Command {
    public:
        SyncCommand(const std::string& msgID, SyncCallback callback);
        virtual void OnCommandDone(Message &rsp);
        virtual void OnError(int code);
    private:
        SyncCallback syncCallback_;
    };

    class AckCommand : public Command {
    public:
        AckCommand(const std::vector<std::string> &msgIDs);
        virtual void OnCommandDone(Message &rsp){};
        virtual void OnError(int code) {};
    private:
        std::string BuildBody(const std::vector<std::string> &msgIDs);
    };

//    class AuthCommand : public Command {
//    public:
//        AuthCommand();
//        virtual void OnCommandDone(Message &rsp){};
//        virtual void OnError(int code) { };
//        std::string ParseRsp();
//    private:
//        std::string BuildBody(const std::vector<std::string> &msgIDs);
//    };

    class AuthRsp {
    public:
        AuthRsp(){};
        int ParseRsp(std::string &rsp);

        std::string lastAck_;
        std::string lastMsg_;
    };

    class MsgNotify {
    public:
        MsgNotify(){};
        int ParseRsp(std::string &rsp);

        std::string msgId_;
    };



}

#endif //APP_COMMAND_H
