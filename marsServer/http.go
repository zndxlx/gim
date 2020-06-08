package main

import (
    "net/http"
    "net"
    //   "time"
    //   "encoding/json"
    "log"
    "time"
    "io/ioutil"
    "marsServer/proto"
    pb "github.com/golang/protobuf/proto"
    "fmt"
    "math/rand"
)

func StartHTTPServer() (err error) {
    httpServeMux := http.NewServeMux()
    httpServeMux.HandleFunc("/mars/hello", Hello)
    httpServeMux.HandleFunc("/mars/hello2", Hello2)
    httpServeMux.HandleFunc("/mars/getconvlist", GetConvList)
    
    httpServeMux.HandleFunc("/mars/sendmessage", SendMessage)
    
    log.Printf("start http listen:\"%s\"", gConf.HTTPAddr)
    
    httpListen(httpServeMux, "tcp", gConf.HTTPAddr)
    
    return
}

func httpListen(mux *http.ServeMux, network, addr string) {
    httpServer := &http.Server{Handler: mux, ReadTimeout: time.Second * 5, WriteTimeout: time.Second * 5}
    httpServer.SetKeepAlivesEnabled(true)
    l, err := net.Listen(network, addr)
    if err != nil {
        log.Printf("net.Listen(\"%s\", \"%s\") error(%v)", network, addr, err)
        panic(err)
    }
    if err := httpServer.Serve(l); err != nil {
        log.Printf("server.Serve() error(%v)", err)
        panic(err)
    }
}

func Hello(w http.ResponseWriter, r *http.Request) {
    var (
        // body      string
        bodyBytes []byte
        err       error
        req       *proto.HelloRequest
        rsp       *proto.HelloResponse
    )
    defer func() {
        data, err := pb.Marshal(rsp)
        if err != nil {
            log.Printf("marshaling error: ", err)
            return
        }
        
        if _, err := w.Write(data); err != nil {
            log.Printf("w.Write error(%v)", err)
        }
        // log.Printf("path=%s, reqBody=%s, rspBody=%s", r.URL.String(), body, rsp)
        log.Printf("path=%s, req=%v, rsp=%v", r.URL.String(), req, rsp)
    }()
    var errMsg string
    var retCode int32 = 0
    rsp = &proto.HelloResponse{
        Errmsg:  &errMsg,
        Retcode: &retCode,
    }
    
    if bodyBytes, err = ioutil.ReadAll(r.Body); err != nil {
        log.Printf("ioutil.ReadAll() failed (%s)", err)
        retCode = -1
        return
    }
    req = &proto.HelloRequest{}
    err = pb.Unmarshal(bodyBytes, req)
    if err != nil {
        log.Printf("unmarshaling error: ", err)
        return
    }
    
    errMsg = fmt.Sprintf("congratulations, %v", *(req.User))

    return
}

func Hello2(w http.ResponseWriter, r *http.Request) {
    var (
        // body      string
        bodyBytes []byte
        err       error
        req       *proto.HelloRequest
        rsp       *proto.HelloResponse
    )
    defer func() {
        data, err := pb.Marshal(rsp)
        if err != nil {
            log.Printf("marshaling error: ", err)
            return
        }
        
        if _, err := w.Write(data); err != nil {
            log.Printf("w.Write error(%v)", err)
        }
        // log.Printf("path=%s, reqBody=%s, rspBody=%s", r.URL.String(), body, rsp)
        log.Printf("path=%s, req=%v, rsp=%v", r.URL.String(), req, rsp)
    }()
    var errMsg string
    var retCode int32 = 0
    rsp = &proto.HelloResponse{
        Errmsg:  &errMsg,
        Retcode: &retCode,
    }
    
    if bodyBytes, err = ioutil.ReadAll(r.Body); err != nil {
        log.Printf("ioutil.ReadAll() failed (%s)", err)
        retCode = -1
        return
    }
    req = &proto.HelloRequest{}
    err = pb.Unmarshal(bodyBytes, req)
    if err != nil {
        log.Printf("unmarshaling error: ", err)
        return
    }
    
    errMsg = fmt.Sprintf("congratulations, %v", *(req.User))
    
    size := 0
    if req.GetText() == "64KB" {
        size = 64 *1024
    }
    if req.GetText() == "128KB" {
        size = 128 *1024
    }
    
    if size > 0 {
        rsp.DumpContent = make([]byte, size)
        rand.Read(rsp.DumpContent)
    }
    return
}


func GetConvList(w http.ResponseWriter, r *http.Request) {
    var (
        bodyBytes []byte
        err       error
        req       *proto.ConversationListRequest
        rsp       *proto.ConversationListResponse
    )
    defer func() {
        data, err := pb.Marshal(rsp)
        if err != nil {
            log.Printf("marshaling error: ", err)
            return
        }
        
        if _, err := w.Write(data); err != nil {
            log.Printf("w.Write error(%v)", err)
        }
        // log.Printf("path=%s, reqBody=%s, rspBody=%s", r.URL.String(), body, rsp)
        log.Printf("path=%s, req=%v, rsp=%v", r.URL.String(), req, rsp)
    }()

    rsp = &proto.ConversationListResponse{}
    
    if bodyBytes, err = ioutil.ReadAll(r.Body); err != nil {
        log.Printf("ioutil.ReadAll() failed (%s)", err)
        return
    }
    req = &proto.ConversationListRequest{}
    err = pb.Unmarshal(bodyBytes, req)
    if err != nil {
        log.Printf("unmarshaling error: ", err)
        return
    }
    
    // conDetails := [][]string{
    //     {"Mars", "0", "STN Discuss"},
    //     {"Mars", "1", "Xlog Discuss"},
    //     {"Mars", "1", "Xlog Discuss"},
    // }
    // converList := []*proto.Conversation
    if (req.GetType() == int32(proto.ConversationListRequest_DEFAULT)) {
        for index, _ := range gConDetails {
            conv := &proto.Conversation{
                Name:   &(gConDetails[index][0]),
                Topic:  &(gConDetails[index][1]),
                Notice: &(gConDetails[index][2]),
            }
            rsp.List = append(rsp.List, conv)
        }
    }
    return
}

func SendMessage(w http.ResponseWriter, r *http.Request) {
    var (
        // body      string
        bodyBytes []byte
        err       error
        req       *proto.SendMessageRequest
        rsp       *proto.SendMessageResponse
    )
    defer func() {
        data, err := pb.Marshal(rsp)
        if err != nil {
            log.Printf("marshaling error: ", err)
            return
        }
        
        if _, err := w.Write(data); err != nil {
            log.Printf("w.Write error(%v)", err)
        }
        log.Printf("path=%s, req=%v, rsp=%v", r.URL.String(), req, rsp)
    }()
    
    if bodyBytes, err = ioutil.ReadAll(r.Body); err != nil {
        log.Printf("ioutil.ReadAll() failed (%s)", err)
        return
    }
    req = &proto.SendMessageRequest{}
    err = pb.Unmarshal(bodyBytes, req)
    if err != nil {
        log.Printf("unmarshaling error: ", err)
        return
    }
    errMsg := "congratulations, " + req.GetFrom();
    rsp = &proto.SendMessageResponse{
        ErrCode: pb.Int32(0),
        From:    pb.String(req.GetFrom()),
        ErrMsg:  &errMsg,
        Text:    pb.String(req.GetText()),
        Topic:   pb.String(req.GetTopic()),
    }
    
    return
}
