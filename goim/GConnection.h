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
            kConnected = 3,
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
        const char *StatusToString() const;

        Status status() const {
            return status_;
        }
        bool IsConnected() const {
            return status_ == kConnected;
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
    private:
        Option option_;
        Status status_;
        evpp::EventLoop* loop_;
        evpp::TCPClientPtr tcp_client_;
    };
}



#endif //APP_GCONNECTION_H
