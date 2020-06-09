package main

import (
    "bufio"
    "encoding/binary"
    "net"
    "flag"
    "fmt"
    "log"
    "marsServer/znet"

    "marsServer/proto"
    pb "github.com/golang/protobuf/proto"
    // "marsServer/util"
    "github.com/abiosoft/ishell"
)

var (
    gUid      int64
    gServerAddr    string
)

func init() {
    flag.Int64Var(&gUid, "uid", 2000, "uid")
    flag.StringVar(&gServerAddr, "server", "127.0.0.1:8081", "server ip")
    log.SetFlags(log.Ldate | log.Lmicroseconds | log.Llongfile)
}

func main() {
    flag.Parse()
    shell := ishell.New()
    conn, err := net.Dial("tcp", gServerAddr)
    if err != nil {
        log.Printf("net.Dial(\"%s\") error(%v)", gServerAddr, err)
        return
    }
    seqId := int32(0)
    wr := bufio.NewWriter(conn)
    rd := bufio.NewReader(conn)
    
    var user string = "laden"
    var topic string = "1"

    go func() {
        for {
            tcpReadMsg(rd)
        }
    }()
    
    shell.AddCmd(&ishell.Cmd{
        Name: "user",
        Help: "change user",
        Func: func(c *ishell.Context) {
            if len(c.Args) == 0 {
                c.Printf("please input user name\n")
                return
            }
            user = c.Args[0]
        },
    })
    
    shell.AddCmd(&ishell.Cmd{
        Name: "topic",
        Help: "change topic",
        Func: func(c *ishell.Context) {
            if len(c.Args) == 0 {
                c.Printf("please input topic\n")
                return
            }
            topic = c.Args[0]
        },
    })
    
    shell.AddCmd(&ishell.Cmd{
        Name: "hello",
        Help: "send hello",
        Func: func(c *ishell.Context) {
            req := &proto.HelloRequest{
                User: pb.String(user),
                Text: pb.String("hello"),
            }
            tcpWriteMsg(wr, int32(proto.CmdID_CMD_ID_HELLO), seqId, req)
            seqId++
        },
    })
    
    shell.AddCmd(&ishell.Cmd{
        Name: "sendMsg",
        Help: "send message",
        Func: func(c *ishell.Context) {
            if len(c.Args) == 0 {
                c.Printf("please input message")
                return
            }
            req := &proto.SendMessageRequest{
                Topic: pb.String(topic),
                Text: pb.String(c.Args[0]),
                From: pb.String(user),
                To: pb.String(user+"-to"),
                AccessToken: pb.String("test_token"),
            }
            tcpWriteMsg(wr, int32(proto.CmdID_CMD_ID_SEND_MESSAGE), seqId, req)
            seqId++
        },
    })
    
    shell.Run()
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
    
    log.Printf("write msg , cmd=%d, seq:%d, req=%v", msg.Cmd, msg.Seq, req)
    return
}

func tcpReadMsg(rd *bufio.Reader) ( msg *znet.Message, rspPb pb.Message, err error) {
    msg = &znet.Message{}
    dp := znet.NewDataPack()
    //log.Printf("tcpReadMsg enter")
    //读取客户端的Msg head
    headData := make([]byte, dp.GetHeadLen())
    if _, err = rd.Read(headData); err != nil {
        log.Printf("err %v", err)
        return
    }
    msg, err = dp.Unpack(headData)
    if err != nil {
        log.Printf("unpack error ", err)
        return
    }
    //log.Printf("tcpReadMsg enter 2222")
    //根据 dataLen 读取 data，放在msg.Data中
    var data []byte
    if msg.BodyLen > 0 {
        data = make([]byte, msg.BodyLen)
        if _, err = rd.Read(data); err != nil {
            log.Printf("read msg data error ", err)
            return
        }
    }
    //log.Printf("tcpReadMsg enter 33333")
    msg.Data = data

    switch msg.Cmd {
    case int32(proto.CmdID_CMD_ID_HELLO), int32(proto.CmdID_CMD_ID_HELLO2):
        rsp := &proto.HelloResponse{}
        pb.Unmarshal(msg.Data, rsp)
        rspPb = rsp
    case int32(proto.CmdID_CMD_ID_SEND_MESSAGE):
        rsp := &proto.SendMessageResponse{}
        pb.Unmarshal(msg.Data, rsp)
        rspPb = rsp
    case 10001:
        rsp := &proto.MessagePush{}
        pb.Unmarshal(msg.Data, rsp)
        rspPb = rsp
    default:
        log.Printf("unkonw cmd %d", msg.Cmd)
        return
    }
    
   // log.Printf("tcpReadMsg enter 4444")
    log.Printf("recv msg, cmd=%d, seq:%d, rspPb=%v", msg.Cmd, msg.Seq, rspPb)
    return
}

