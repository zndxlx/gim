//
// Created by lixin on 2020/7/22.
//

#include "GConnection.h"
namespace goim {
    GConnection::GConnection(evpp::EventLoop *loop, const Option &ops):
      loop_(loop)
    , option_(ops)
    , status_(kDisconnected) {

    }

    GConnection::~GConnection() {
        ;
    }

    void GConnection::Connect() {
        //DLOG_TRACE << " remote_addr=" << option_.serverAddr;
        tcp_client_ = evpp::TCPClientPtr(new evpp::TCPClient(loop_, option_.serverAddr, std::string("GoimClient-") + option_.serverAddr));
        SetStatus(kConnecting);
        tcp_client_->SetConnectionCallback(std::bind(&GConnection::OnTCPConnectionEvent, this, std::placeholders::_1));
        tcp_client_->SetMessageCallback(
                std::bind(&GConnection::OnRecv, this, std::placeholders::_1, std::placeholders::_2));
        tcp_client_->Connect();
    }

    void GConnection::Close() {
//        LOG_WARN << "NSQConn::Close() this=" << this << " status=" << StatusToString();
        SetStatus(kDisconnecting);
        tcp_client_->Disconnect();
    }

    void GConnection::Reconnect() {
//        LOG_WARN << "NSQConn::Close() this=" << this << " status=" << StatusToString() << " serverAddr="
//                 << tcp_client_->remote_addr();
        tcp_client_->Disconnect();
        Connect();
    }

    void GConnection::SetStatus(Status status) {
        status_ = status;
    }

    void GConnection::OnTCPConnectionEvent(const evpp::TCPConnPtr &conn) {
        //DLOG_TRACE << "status=" << StatusToString() << " TCPConn=" << conn.get() << " remote_addr="
         //          << conn->remote_addr() << std::endl;
        XINFO("111111111111111111111111111111111111111 %s", conn->remote_addr().c_str());
        if (conn->IsConnected()) {
            if (status_ == kConnecting) {
                std::cout << "1111 connected" << std::endl;
                //Identify();  //
            } else {
                // Maybe the user layer has close this NSQConn and then the underlying TCPConn established a connection with NSQD to invoke this callback
                //assert(status_ == kDisconnecting);
            }
        } else {
            std::cout << "1111 disconncet" << std::endl;
            SetStatus(kDisconnected);
//        if (conn_fn_) {
//            conn_fn_(shared_from_this());
//        }
        }
    }

    void GConnection::OnRecv(const evpp::TCPConnPtr &conn, evpp::Buffer *buf) {
        while (buf->size() > 4) {
            return;
        }
    }

    const char *GConnection::StatusToString() const {
        switch (status_) {
            case kDisconnected:
                return "kDisconnected";
            case kConnecting:
                return "kConnecting";
            default:
                return "unknow";
        }

    }
}