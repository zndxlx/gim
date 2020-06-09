package main

type Config struct  {
    HTTPAddr string
    TCPServerAddr  string
}

var gConf *Config

func init(){
    gConf = &Config{
        HTTPAddr: "0.0.0.0:8080",
        TCPServerAddr: "0.0.0.0:8081",
    }
}
