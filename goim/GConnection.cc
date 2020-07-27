//
// Created by lixin on 2020/7/22.
//

#include <memory>
#include "GConnection.h"


namespace goim {
    void gOnSyncRsp(uint32_t seq, int code, SyncRsp &rsp){
        XINFO("seq=%d, code=%d, rsp=%s", seq, code, rsp.ToString().c_str());
        //CommandPtr command = std::make_shared<AckCommand>(msgIDs);
        // LaunchCommand(command);
    }


    GConnection::GConnection(evpp::EventLoop *loop, const Option &ops):
      loop_(loop)
    , option_(ops)
    , status_(kDisconnected) {

    }

    GConnection::~GConnection() {
        ;
    }

    void GConnection::Connect() {
        XINFO("start connect %s", option_.serverAddr.c_str());
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
        XINFO("status %s, TCPConn=%p, remote_addr=%s", StatusToString(), conn.get(),
                conn->remote_addr().c_str());
        if (conn->IsConnected()) {
            if (status_ == kConnecting) {
                XINFO("TCPConn[%p], remote_addr[%s], is connected", conn.get(),
                      conn->remote_addr().c_str());
                Authenticate();
            } else {
                // Maybe the user layer has close this NSQConn and then the underlying TCPConn established a connection with NSQD to invoke this callback
                //assert(status_ == kDisconnecting);
            }
        } else {
            XINFO("TCPConn[%p], remote_addr[%s], disconncet", conn.get(),
                  conn->remote_addr().c_str());
            SetStatus(kDisconnected);
//        if (conn_fn_) {
//            conn_fn_(shared_from_this());
//        }
        }
    }

    void GConnection::OnRecv(const evpp::TCPConnPtr &conn, evpp::Buffer *buf) {
        Message m;  //todo proc multi msg
        int ret;
        ret = m.ReadBuf(buf);
        if (ret == 0) {
            XINFO("resv msg, %s", m.ToString().c_str());
            if (status_ == kAuthenticating) {
                if (m.op_ != Message::OpAuthReply) {
                    XWARN("need auth reply");
                    return;
                }else{
                    SetStatus(kReady);
                    //发送sync
                    AuthRsp rsp;
                    rsp.ParseRsp(m.body_);
                    lastAck_ = rsp.lastAck_;
                    lastMsg_ = rsp.lastMsg_;
                    if (lastAck_ != lastMsg_ ) {
                        XINFO("1111111111");
                        SendSync(lastAck_);
                    }
                }
            }else if (status_ == kReady){
                //log 消息
               // XINFO("resv msg, %s", m.ToString().c_str());
                //process cmd
                switch (m.op_) {
                    case Message::OpHeartbeat:
                        XINFO("resv heart beat rsp");
                        break;
                    case Message::OpSendMsgReply:
                        XINFO("resv sms");
                        break;
                    case Message::OpSyncMsgReply:
                        //处理cmd;
                        ProcessCmdRsp(m);
                        break;
                    case Message::OpMsgNotify:
                        XINFO("resv msg notify");
                        MsgNotify notify;
                        notify.ParseRsp(m.body_);
                        SendSync(lastAck_);
                        break;
                }
            }
        }else if (ret == -1) {
            Close();
        }
    }

    void GConnection::Authenticate() {
        Message m;
        m.op_ = Message::OpAuth;
        m.seq_ = 3;
        m.body_ = "10011;{\"ver\":\"2.0\"};14";

        WriteMessage(m);
        SetStatus(kAuthenticating);
    }

    void GConnection::WriteMessage(const Message& m) {
        evpp::Buffer buf;
        m.WriteBuf(&buf);
        tcp_client_->conn()->Send(&buf);
        return;
    }

//    CommandPtr GConnection::PopRunningCommand() {
//        if (running_command_.empty()) {
//            return CommandPtr();
//        }
//
//        CommandPtr command(running_command_.front());
//        running_command_.pop();
//        return command;
//    }

    void GConnection::PushRunningCommand(CommandPtr& cmd) {
        running_command_.emplace_back(cmd);
        XINFO("PushRunningCommand 44444444");
        if (timer_canceled_) {
            XINFO("PushRunningCommand 555555555");
            //cmd_timer_ = loop_->RunAfter(evpp::Duration(5.0), std::bind(&GConnection::OnPacketTimeout, shared_from_this(), cmd->req_.seq_));
            timer_canceled_ = false;
        }
    }

    void GConnection::OnPacketTimeout(int32_t cmd_id) {
//        timer_canceled_ = true;
//        if (!running_command_.empty()) {
//            CommandPtr cmd(running_command_.front());
//            if (LIKELY(cmd->id() != cmd_id)) {
//                cmd_timer_bakup_.swap(cmd_timer_);
//                cmd_timer_ = exec_loop_->RunAfter(timeout_, std::bind(&MemcacheClient::OnPacketTimeout, shared_from_this(), cmd->id()));
//                timer_canceled_ = false;
//                return;
//            }
//        } else {
//            return;
//        }
//
//        LOG_DEBUG << "InvokeTimer triggered for " << cmd_id << " " << conn()->remote_addr();
//
//        while (!running_command_.empty()) {
//            CommandPtr cmd(running_command_.front());
//            running_command_.pop();
//
//            if (mc_pool_ && cmd->ShouldRetry()) {
//                cmd->set_id(0);
//                cmd->set_server_id(cmd->server_id());
//                exec_loop()->RunInLoop(std::bind(&MemcacheClientBase::LaunchCommand, mc_pool_, cmd));
//            } else {
//                cmd->OnError(ERR_CODE_TIMEOUT);
//            }
//
//            if (cmd->id() == cmd_id) {
//                break;
//            }
//        }
//        LOG_ERROR << "OnPacketTimeout post, waiting=" << waiting_command_.size()
//                  << " running=" << running_command_.size();
    }

    void GConnection::SendSync(const std::string& msgId) {
        XINFO("vvvvvvvv");
        CommandPtr command = std::make_shared<SyncCommand>(msgId, gOnSyncRsp);
        XINFO("222222");
        LaunchCommand(command);
    }

    void GConnection::SendAck(const std::vector<std::string> &msgIDs){
        CommandPtr command = std::make_shared<AckCommand>(msgIDs);
        LaunchCommand(command);
    }

    void GConnection::OnSyncRsp(uint32_t seq, int code, SyncRsp &rsp){
        XINFO("seq=%d, code=%d, rsp=%s", seq, code, rsp.ToString().c_str());
        //CommandPtr command = std::make_shared<AckCommand>(msgIDs);
       // LaunchCommand(command);
    }

    void GConnection::LaunchCommand(CommandPtr& command) {
        if (IsReady()) {
            XINFO("33333");
            PushRunningCommand(command);
            XINFO("4444");
            WriteMessage(command->req_);
            XINFO("55555");
            return;
        }else {
            command->OnError(-1);
        }
    }

    void GConnection::ProcessCmdRsp(Message &msg){
        for (auto it=running_command_.begin(); it !=running_command_.end();) {
            if ((*it)->seq_ == msg.seq_) {
                (*it)->OnCommandDone(msg);
                //删除cmd
                running_command_.erase(it);
                break;
            }
        }
    }

    const char *GConnection::StatusToString() const {
        switch (status_) {
            case kDisconnected:
                return "kDisconnected";
            case kConnecting:
                return "kConnecting";
            case kAuthenticating:
                return "kAuthenticating";
            case kReady:
                return "kReady";
            case kDisconnecting:
                return "kDisconnecting";
            default:
                return "unknow";
        }

    }
}