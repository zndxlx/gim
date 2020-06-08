package main

import (
    "marsServer/util"
    "marsServer/proto"
    pb "github.com/golang/protobuf/proto"
)

import (
    "log"
)


func testHello() {
    req := &proto.HelloRequest{
        User: pb.String("laden"),
        Text: pb.String("test hello"),
    }
    data, err := pb.Marshal(req)
    if err != nil {
        log.Printf("marshaling error: ", err)
        return
    }
    
    code, rspBody, err := util.HttpPost("http://127.0.0.1:8080/mars/hello", string(data))
    
    rsp := &proto.HelloResponse{}
    err = pb.Unmarshal(rspBody, rsp)
    if err != nil {
        log.Printf("unmarshaling error: ", err)
        return
    }
    log.Printf("testHello code=%v, req=%v, rsp=%v, err=%v", code, req, rsp, err)
    
}



func testHello2() {
    req := &proto.HelloRequest{
        User: pb.String("laden2"),
        Text: pb.String("64KB"),
    }
    data, err := pb.Marshal(req)
    if err != nil {
        log.Printf("marshaling error: ", err)
        return
    }
    
    code, rspBody, err := util.HttpPost("http://127.0.0.1:8080/mars/hello2", string(data))
    
    rsp := &proto.HelloResponse{}
    err = pb.Unmarshal(rspBody, rsp)
    if err != nil {
        log.Printf("unmarshaling error: ", err)
        return
    }
    log.Printf("testHello code=%v, req=%v, rsp=%v, err=%v", code, req, rsp, err)
    
}


func testGetconvlist() {
    req := &proto.ConversationListRequest{
        Type:        pb.Int32(0),
        AccessToken: pb.String("test_token"),
    }
    data, err := pb.Marshal(req)
    if err != nil {
        log.Printf("marshaling error: ", err)
        return
    }
    
    code, rspBody, err := util.HttpPost("http://127.0.0.1:8080/mars/getconvlist", string(data))
    
    rsp := &proto.ConversationListResponse{}
    err = pb.Unmarshal(rspBody, rsp)
    if err != nil {
        log.Printf("unmarshaling error: ", err)
        return
    }
    log.Printf("testGetconvlist code=%v, req=%v, rsp=%v, err=%v", code, req, rsp, err)
}

func testSendMsg() {
    req := &proto.SendMessageRequest{
        From:        pb.String("test_from"),
        To:          pb.String("test_to"),
        Text:        pb.String("test_text"),
        Topic:       pb.String("test_topic"),
        AccessToken: pb.String("test_token"),
    }
    data, err := pb.Marshal(req)
    if err != nil {
        log.Printf("marshaling error: ", err)
        return
    }
    
    code, rspBody, err := util.HttpPost("http://127.0.0.1:8080/mars/sendmessage", string(data))
    
    rsp := &proto.SendMessageResponse{}
    err = pb.Unmarshal(rspBody, rsp)
    if err != nil {
        log.Printf("unmarshaling error: ", err)
        return
    }
    log.Printf("testSendMsg code=%v, req=%v, rsp=%v, err=%v", code, req, rsp, err)
}

func main() {
    //util.InitHttpClient()
    testHello()
   // testHello2()
   //  testGetconvlist()
   // testSendMsg()
}
