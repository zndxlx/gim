//
// Created by lixin on 2020/7/22.
//

#ifndef APP_GCONNECTION_H
#define APP_GCONNECTION_H
#include <memory>
#include "evpp/duration.h"
#include <evpp/buffer.h>
#include <evpp/tcp_conn.h>
#include <evpp/tcp_client.h>
#include "message.h"
#include <list>
#include <command.h>

namespace evpp {
    class EventLoop;
    typedef std::shared_ptr<evpp::TCPClient> TCPClientPtr;
    typedef std::shared_ptr<evpp::TCPConn> TCPConnPtr;
}

namespace goim{
    class GConnection: public std::enable_shared_from_this<GConnection> {
    public :
        enum Status {
            kDisconnected = 0,
            kConnecting = 1,
            kAuthenticating = 2,
            kReady = 3,
            kDisconnecting = 4,
        };
        struct Option {
            evpp::Duration dial_timeout{evpp::Duration(1.0)};
            evpp::Duration read_timeout{evpp::Duration(60.0)};
            evpp::Duration write_timeout{evpp::Duration(1.0)};
            std::string serverAddr {"47.112.113.131:8080"};
        };
        typedef std::function<void(const std::shared_ptr<GConnection>& conn)> ConnectionCallback;
    public:
        GConnection(evpp::EventLoop* loop, const Option& ops);
        ~GConnection();
        void Connect();
        void Close();
        //send command
        void SendAck(const std::vector<std::string> &msgIDs);

        const char *StatusToString() const;

        Status status() const {
            return status_;
        }
        bool IsReady() const {
            return status_ == kReady;
        }
        bool IsConnecting() const {
            return status_ == kConnecting;
        }
        bool IsDisconnected() const {
            return status_ == kDisconnected;
        }
        bool IsAuthenticating() const {
            return status_ == kAuthenticating;
        }

    private:
        void SetStatus(Status s);
        void Reconnect();
        void OnTCPConnectionEvent(const evpp::TCPConnPtr& conn);
        void OnRecv(const evpp::TCPConnPtr& conn, evpp::Buffer* buf);
        void Authenticate();
        void WriteMessage(const Message& m);
        void PushRunningCommand(CommandPtr& cmd);
        void OnPacketTimeout(int32_t cmd_id);
        void SendSync(const std::string& msgId);
        //CommandPtr PopRunningCommand();
        void LaunchCommand(CommandPtr& command);
        void OnSyncRsp(uint32_t seq, int code, SyncRsp &rsp);
        void ProcessCmdRsp(Message &msg);
        //CommandPtr FindCommand();
    private:
        Option option_;
        Status status_;
        evpp::EventLoop* loop_;
        evpp::TCPClientPtr tcp_client_;
        std::list<CommandPtr> running_command_;
        evpp::InvokeTimerPtr cmd_timer_;
        bool  timer_canceled_;
        std::string lastAck_;
        std::string lastMsg_;
    };
}



#endif //APP_GCONNECTION_H
