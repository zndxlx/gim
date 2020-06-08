package main

type Config struct  {
    HTTPAddr string
    TCPServerAddr  string
}

var gConf *Config

func init(){
    gConf = &Config{
        HTTPAddr: "127.0.0.1:8080",
        TCPServerAddr: "127.0.0.1:8081",
    }
}
