package main

import (
    // "time"
    "marsServer/znet"
    "log"
    // "context"
    "marsServer/proto"
    "marsServer/util"
    pb "github.com/golang/protobuf/proto"
)
var gTcpServer *znet.Server


type handler struct{}

var Handler = new(handler)

func (*handler) OnConnect(c *znet.Connection) {
    log.Printf("new connet c %v", c.ConnID)
    topicJoiners.JoinTopic(c);
}

func (h *handler) OnMessage(c *znet.Connection, msg *znet.Message) {
    log.Printf("new msg cmd=%v", msg.Cmd)
    
    switch msg.Cmd {
    case int32(proto.CmdID_CMD_ID_HELLO):
        log.Printf("recv hello req")
        _, rspBody, err := util.HttpPost("http://127.0.0.1:8080/mars/hello", string(msg.Data))
        if err != nil {
            log.Printf("hello err=%v", err)
            return
        }
        c.SendMsg(msg.Cmd, msg.Seq, rspBody)
        
    case int32(proto.CmdID_CMD_ID_HELLO2):
        log.Printf("recv hello req2")
        _, rspBody, err := util.HttpPost("http://127.0.0.1:8080/mars/hello2", string(msg.Data))
        if err != nil {
            return
        }
        c.SendMsg(msg.Cmd, msg.Seq, rspBody)
        
        
    case int32(proto.CmdID_CMD_ID_SEND_MESSAGE):
        log.Printf("recv send msg")
        _, rspBody, err := util.HttpPost("http://127.0.0.1:8080/mars/sendmessage", string(msg.Data))
        if err != nil {
            return
        }
        rsp := &proto.SendMessageResponse{}
        pb.Unmarshal(rspBody, rsp)
        topicJoiners.PushMessage(rsp.GetTopic(), rsp.GetText(), rsp.GetFrom(), c)
        c.SendMsg(msg.Cmd, msg.Seq, rspBody)
        
    case  6:
        c.SendMsg(msg.Cmd, msg.Seq, msg.Data)
    }
    
    
    
    return
}

func (*handler) OnClose(c *znet.Connection) {
    log.Printf("connect close, c=%v", c.ConnID)
    topicJoiners.LeftTopic(c);
}




func StartTCPServer() {
    gTcpServer = znet.NewServer()
    gTcpServer.SetOnConnStart(Handler.OnConnect)
    gTcpServer.SetOnConnStop(Handler.OnClose)
    gTcpServer.SetOnMsg(Handler.OnMessage)
    gTcpServer.Start()
}
