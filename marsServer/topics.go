package main

import (
    "marsServer/znet"
    "marsServer/proto"
    pb "github.com/golang/protobuf/proto"
    "marsServer/util"
)

var gConDetails [][]string = [][]string{
    {"Mars", "0", "STN Discuss"},
    {"Mars", "1", "Xlog Discuss"},
    {"Mars", "2", "Xlog Discuss"},
}

type topics struct {
     conns    map[string][]znet.Connection
}

type TopicJoiners map[string]map[*znet.Connection]struct{}

var topicJoiners *TopicJoiners

func init() {
    var t TopicJoiners = make(map[string]map[*znet.Connection]struct{})
    for i := 0; i < len(gConDetails); i++ {
        t[gConDetails[i][1]] = make(map[*znet.Connection]struct{})
    }
    topicJoiners = &t
}

func (topic *TopicJoiners)JoinTopic(conn *znet.Connection){
    for k, _ := range *topic {
        (*topicJoiners)[k][conn] = struct{}{}
    }
}

func  (topic *TopicJoiners)LeftTopic(conn *znet.Connection){
    for k, _ := range *topic {
        delete((*topicJoiners)[k], conn)
    }
}

func  (topic *TopicJoiners)PushMessage(topicName string, content string, from string, conn *znet.Connection) {
    connections := (*topic)[topicName]
    
    msg := &proto.MessagePush{
        Content:pb.String(content),
        From:pb.String(from),
        Topic: pb.String(topicName),
    }
    
    for c,_ := range connections {
        if c == conn {
            continue
        }
        util.SendPbMsg(c,  10001,0, msg)
    }


}