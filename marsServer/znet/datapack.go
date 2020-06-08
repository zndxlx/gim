package znet

import (
	"bytes"
	"encoding/binary"
	"errors"
)

//封包拆包类实例，暂时不需要成员
type DataPack struct{}

//封包拆包实例初始化方法
func NewDataPack() *DataPack {
	return &DataPack{}
}

//获取包头长度方法
func (dp *DataPack) GetHeadLen() uint32 {
	return 20
}

//封包方法(压缩数据)
func (dp *DataPack) Pack(msg *Message) ([]byte, error) {
	//创建一个存放bytes字节的缓冲
	dataBuff := bytes.NewBuffer([]byte{})
	
	if err := binary.Write(dataBuff, binary.BigEndian, msg.HeadLen); err != nil {
		return nil, err
	}
	
	if err := binary.Write(dataBuff, binary.BigEndian, msg.Ver); err != nil {
		return nil, err
	}
	
	if err := binary.Write(dataBuff, binary.BigEndian, msg.Cmd); err != nil {
		return nil, err
	}
	
	if err := binary.Write(dataBuff, binary.BigEndian, msg.Seq); err != nil {
		return nil, err
	}
	
	if err := binary.Write(dataBuff, binary.BigEndian, msg.BodyLen); err != nil {
		return nil, err
	}
	
	//写data数据
	if err := binary.Write(dataBuff, binary.BigEndian, msg.Data); err != nil {
		return nil, err
	}

	return dataBuff.Bytes(), nil
}

//拆包方法(解压数据)
func (dp *DataPack) Unpack(binaryData []byte) (*Message, error) {
	//创建一个从输入二进制数据的ioReader
	dataBuff := bytes.NewReader(binaryData)

	//只解压head的信息，得到dataLen和msgID
	msg := &Message{}

	//读dataLen
	if err := binary.Read(dataBuff, binary.BigEndian, &msg.HeadLen); err != nil {
		return nil, err
	}
	
	if err := binary.Read(dataBuff, binary.BigEndian, &msg.Ver); err != nil {
		return nil, err
	}
	
	if err := binary.Read(dataBuff, binary.BigEndian, &msg.Cmd); err != nil {
		return nil, err
	}
	
	if err := binary.Read(dataBuff, binary.BigEndian, &msg.Seq); err != nil {
		return nil, err
	}
	
	if err := binary.Read(dataBuff, binary.BigEndian, &msg.BodyLen); err != nil {
		return nil, err
	}

	
	//判断dataLen的长度是否超出我们允许的最大包长度
	if msg.BodyLen > 1024*16 {
		return nil, errors.New("too large msg data received")
	}

	//这里只需要把head的数据拆包出来就可以了，然后再通过head的长度，再从conn读取一次数据
	return msg, nil
}
