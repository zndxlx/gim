package main

import (
    "bufio"
    "encoding/binary"
    "net"
    "time"
    "flag"
    "fmt"
    "log"
    "marsServer/znet"
    // "encoding/json"
    // "sort"
    "marsServer/proto"
    pb "github.com/golang/protobuf/proto"
    // "marsServer/util"
)

var (
    gUid      int64
    gServerAddr    string
)

func init() {
    flag.Int64Var(&gUid, "uid", 2000, "uid")
    flag.StringVar(&gServerAddr, "server", "127.0.0.1:8081", "server ip")
}

func main() {
    flag.Parse()
    conn, err := net.Dial("tcp", gServerAddr)
    if err != nil {
        log.Printf("net.Dial(\"%s\") error(%v)", gServerAddr, err)
        return
    }
    seqId := int32(0)
    wr := bufio.NewWriter(conn)
    rd := bufio.NewReader(conn)
    
    req := &proto.HelloRequest{
        User: pb.String("laden"),
        Text: pb.String("test hello"),
    }

    tcpWriteMsg(wr, int32(proto.CmdID_CMD_ID_HELLO), seqId, req)
    
    if _, _, err := tcpReadMsg(rd); err != nil {
        log.Printf("tcpReadMsg() error(%v)", err)
        return
    }
    time.Sleep(time.Second*3)
}

func tcpWriteMsg(wr *bufio.Writer, cmd int32, seq int32, req pb.Message) (err error) {
    data, err := pb.Marshal(req)
    if err != nil {
        log.Printf("marshaling error: ", err)
        return
    }
    msg := znet.NewMsgPackage(cmd, seq, data)

    dp := znet.NewDataPack()
    buf, err := dp.Pack(msg)
    if err != nil {
        fmt.Printf("Pack error cmd = %d, req=%v", msg.Cmd, req)
        return
    }
    
    if err = binary.Write(wr, binary.BigEndian, buf); err != nil {
        log.Printf("tcpWriteMsg err=%v", err)
        return
    }

    err = wr.Flush()
    
    log.Printf("write msg, cmd=%d, seq:%d, req=%v", msg.Cmd, msg.Seq, req)
    return
}

func tcpReadMsg(rd *bufio.Reader) ( msg *znet.Message, rspPb pb.Message, err error) {
    msg = &znet.Message{}
    dp := znet.NewDataPack()
    
    //读取客户端的Msg head
    headData := make([]byte, dp.GetHeadLen())
    if _, err = rd.Read(headData); err != nil {
        return
    }
    msg, err = dp.Unpack(headData)
    if err != nil {
        fmt.Println("unpack error ", err)
        return
    }
    
    //根据 dataLen 读取 data，放在msg.Data中
    var data []byte
    if msg.BodyLen > 0 {
        data = make([]byte, msg.BodyLen)
        if _, err = rd.Read(data); err != nil {
            fmt.Println("read msg data error ", err)
            return
        }
    }
    msg.Data = data
   // message
    log.Printf("recv msg, cmd=%d, seq:%d", msg.Cmd, msg.Seq)
    
    switch msg.Cmd {
    case int32(proto.CmdID_CMD_ID_HELLO), int32(proto.CmdID_CMD_ID_HELLO2):
        rsp := &proto.HelloResponse{}
        pb.Unmarshal(msg.Data, rsp)
        rspPb = rsp
    case int32(proto.CmdID_CMD_ID_SEND_MESSAGE):
        rsp := &proto.SendMessageResponse{}
        pb.Unmarshal(msg.Data, rsp)
        rspPb = rsp
    default:
        log.Printf("unkonw cmd %d", msg.Cmd)
        return
    }
    log.Printf("recv msg, cmd=%d, seq:%d, rspPb=%v", msg.Cmd, msg.Seq, rspPb)
    return
}

