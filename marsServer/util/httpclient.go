package util

import (
    "context"
    "io/ioutil"
    "net"
    "net/http"
    "strings"
    "time"

    "log"
    "github.com/viki-org/dnscache"
)

var (
    httpClient *http.Client
    resolver   *dnscache.Resolver
    dialer     *net.Dialer
)

func init() {
    resolver = dnscache.New(10 * time.Second)
    dialer = &net.Dialer{
        Timeout: 30 * time.Second,
    }
    httpClient = &http.Client{
        Transport: &http.Transport{
            DialContext: func(ctx context.Context, network string, address string) (net.Conn, error) {
                separator := strings.LastIndex(address, ":")
                ip, _ := resolver.FetchOneString(address[:separator])
                return dialer.DialContext(ctx, network, ip+address[separator:])
            },

            MaxIdleConns:        10,
            MaxIdleConnsPerHost: 10,
            IdleConnTimeout:     90 * time.Second,
        },
        Timeout: 10 * time.Second,
    }

    return
}

func HttpPost(url string, body string) (code int, rspBody []byte, err error) {
    payload := strings.NewReader(body)
    req, err := http.NewRequest("POST", url, payload)
    if err != nil {
        log.Printf("Error Occured. %+v", err)
        return
    }
    //req.Header.Add("Content-Type", "application/x-www-form-urlencoded")
    req.Header.Add("Content-Type", "application/octet-stream")
    response, err := httpClient.Do(req)
    if err != nil && response == nil {
        log.Printf("req_url:%s, body:%s, err:%+v", url, body, err)
        return
    } else {
        defer response.Body.Close()

        rspBody, err = ioutil.ReadAll(response.Body)
        if err != nil {
            log.Printf("Couldn't parse response body:%s. %+v", rspBody, err)
        }
        code = response.StatusCode
        //log.Printf("post url:%s, body:%s StatusCode:%v", url, body, response.StatusCode)
    }
    return
}

func HttpGet(url string) (code int, body []byte, err error) {
    req, err := http.NewRequest("GET", url, nil)
    if err != nil {
        log.Printf("Error Occured. %+v", err)
        return
    }

    response, err := httpClient.Do(req)
    if err != nil && response == nil {
        log.Printf("req_url:%s, body:%s, err:%+v", url, body, err)
        return
    } else {
        defer response.Body.Close()

        body, err = ioutil.ReadAll(response.Body)
        if err != nil {
            log.Printf("Couldn't parse body:%s. %+v", body, err)
            return
        }
        code = response.StatusCode
        log.Printf("get url:%s, body:%s StatusCode:%v", url, body, response.StatusCode)
    }
    return
}
