package znet

type Message struct {
    HeadLen int32  // 消息头长度
    Ver     int32  // 版本号
    Cmd   int32  // 消息类型
    Seq     int32  // 消息的ID
    BodyLen int32 // 消息的长度
    Data    []byte // 消息的内容
}

// 创建一个Message消息包
func NewMsgPackage(cmd int32, seq int32,  data []byte) *Message {
    return &Message{
        HeadLen: 20,
        Ver: 200,
        Cmd: cmd,
        Seq:      seq,
        BodyLen: int32(len(data)),
        Data:    data,
    }
}
//
// // 获取消息数据段长度
// func (msg *Message) GetDataLen() int32 {
//     return msg.BodyLen
// }
//
// // 获取消息ID
// func (msg *Message) GetMsgSeq() int32 {
//     return msg.Seq
// }
//
// // 获取消息内容
// func (msg *Message) GetData() []byte {
//     return msg.Data
// }
//
// // 设置消息数据段长度
// func (msg *Message) SetBodyLen(len int32) {
//     msg.BodyLen = len
// }
//
// // 设计消息Seq
// func (msg *Message) SetMsgSeq(seq int32) {
//     msg.Seq = seq
// }
//
// // 设计消息内容
// func (msg *Message) SetData(data []byte) {
//     msg.Data = data
// }
