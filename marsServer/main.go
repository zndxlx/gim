package main

import (
    // "encoding/json"
    // "time"
    // "net/http"
    // "net"
    "log"
)

func init(){
    log.SetFlags(log.Ldate | log.Lmicroseconds | log.Llongfile)
}

func main() {
    log.Printf("hhhh\n")
    StartTCPServer()
    StartHTTPServer()
    return
}

