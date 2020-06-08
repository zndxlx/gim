package util

import (
    "marsServer/znet"
    "log"
    pb "github.com/golang/protobuf/proto"
)

func SendPbMsg(c *znet.Connection, cmd int32, seq int32, msg pb.Message) error {
    data, err := pb.Marshal(msg)
    if err != nil {
        log.Printf("marshaling error: ", err)
        return err
    }
    return c.SendMsg(cmd, seq, data)
}
