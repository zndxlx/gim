// Tencent is pleased to support the open source community by making Mars available.
// Copyright (C) 2017 THL A29 Limited, a Tencent company. All rights reserved.

// Licensed under the MIT License (the "License"); you may not use this file except in 
// compliance with the License. You may obtain a copy of the License at
// http://opensource.org/licenses/MIT

// Unless required by applicable law or agreed to in writing, software distributed under the License is
// distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
// either express or implied. See the License for the specific language governing permissions and
// limitations under the License.

/*
*  longlink_packer.cc
*
*  Created on: 2017-7-7
*      Author: chenzihao
*/

#include "longlink_packer.h"

#ifndef _WIN32
#include <arpa/inet.h>
#endif


#include "mars/xlog/xlogger.h"
#include "mars/comm/autobuffer.h"
#include "mars/stn/stn.h"

static uint32_t sg_client_version = 200;

#pragma pack(push, 1)

struct __STNetMsgXpHeader {
	uint32_t	pack_length; //�ܳ���
    uint16_t    head_length;
    uint16_t    client_version;
    uint32_t    cmdid;
    uint32_t    seq;
};


#pragma pack(pop)

namespace mars {
namespace stn {
longlink_tracker* (*longlink_tracker::Create)()
= []() {
    return new longlink_tracker;
};
    
void SetClientVersion(uint32_t _client_version)  {
    sg_client_version = _client_version;
}


static int __unpack_test(const void* _packed, size_t _packed_len, uint32_t& _cmdid, uint32_t& _seq, size_t& _package_len, size_t& _body_len) {
    __STNetMsgXpHeader st = {0};
    if (_packed_len < sizeof(__STNetMsgXpHeader)) {
        _package_len = 0;
        _body_len = 0;
        return LONGLINK_UNPACK_CONTINUE;
    }
    
    memcpy(&st, _packed, sizeof(__STNetMsgXpHeader));
    
    uint32_t head_len = ntohs(st.head_length);
    uint32_t client_version = ntohs(st.client_version);
	#if 0
    if (client_version != sg_client_version) {
        _package_len = 0;
        _body_len = 0;
    	return LONGLINK_UNPACK_FALSE;
    }
	#endif
    _cmdid = ntohl(st.cmdid);
	_seq = ntohl(st.seq);
	_package_len = ntohl(st.pack_length);
	_body_len = _package_len - head_len;

	xinfo2("11111111 recv _cmdid:%d", _cmdid);

    if (_package_len > 1024*1024) { return LONGLINK_UNPACK_FALSE; }
    if (_package_len > _packed_len) { return LONGLINK_UNPACK_CONTINUE; }
    
    return LONGLINK_UNPACK_OK;
}

void (*longlink_pack)(uint32_t _cmdid, uint32_t _seq, const AutoBuffer& _body, const AutoBuffer& _extension, AutoBuffer& _packed, longlink_tracker* _tracker)
= [](uint32_t _cmdid, uint32_t _seq, const AutoBuffer& _body, const AutoBuffer& _extension, AutoBuffer& _packed, longlink_tracker* _tracker) {
    __STNetMsgXpHeader st = {0};
    st.head_length = htons(sizeof(__STNetMsgXpHeader));
    //st.client_version = htonl(sg_client_version);
	st.client_version = htons(200);
    st.cmdid = htonl(_cmdid);
    st.seq = htonl(_seq);
    st.pack_length = htonl(_body.Length() + sizeof(__STNetMsgXpHeader));

    _packed.AllocWrite(sizeof(__STNetMsgXpHeader) + _body.Length());
    _packed.Write(&st, sizeof(st));
    
    if (NULL != _body.Ptr()) _packed.Write(_body.Ptr(), _body.Length());
    
    _packed.Seek(0, AutoBuffer::ESeekStart);
};


int (*longlink_unpack)(const AutoBuffer& _packed, uint32_t& _cmdid, uint32_t& _seq, size_t& _package_len, AutoBuffer& _body, AutoBuffer& _extension, longlink_tracker* _tracker)
= [](const AutoBuffer& _packed, uint32_t& _cmdid, uint32_t& _seq, size_t& _package_len, AutoBuffer& _body, AutoBuffer& _extension, longlink_tracker* _tracker) {
   size_t body_len = 0;
   int ret = __unpack_test(_packed.Ptr(), _packed.Length(), _cmdid,  _seq, _package_len, body_len);
    
    if (LONGLINK_UNPACK_OK != ret) return ret;
    
    _body.Write(AutoBuffer::ESeekCur, _packed.Ptr(_package_len-body_len), body_len);
    
    return ret;
};


#define NOOP_CMDID 2
#define NOOP_CMDID_RSP 3

#define SIGNALKEEP_CMDID 243
#define PUSH_DATA_TASKID 0

uint32_t (*longlink_noop_cmdid)()
= []() -> uint32_t {
    return NOOP_CMDID;
};

bool  (*longlink_noop_isresp)(uint32_t _taskid, uint32_t _cmdid, uint32_t _recv_seq, const AutoBuffer& _body, const AutoBuffer& _extend)
= [](uint32_t _taskid, uint32_t _cmdid, uint32_t _recv_seq, const AutoBuffer& _body, const AutoBuffer& _extend) {
    return Task::kNoopTaskID == _taskid && NOOP_CMDID_RSP == _cmdid;
};

uint32_t (*signal_keep_cmdid)()
= []() -> uint32_t {
    return SIGNALKEEP_CMDID;
};

void (*longlink_noop_req_body)(AutoBuffer& _body, AutoBuffer& _extend)
= [](AutoBuffer& _body, AutoBuffer& _extend) {
    
};
    
void (*longlink_noop_resp_body)(const AutoBuffer& _body, const AutoBuffer& _extend)
= [](const AutoBuffer& _body, const AutoBuffer& _extend) {
    
};

uint32_t (*longlink_noop_interval)()
= []() -> uint32_t {
	return 0;
};

bool (*longlink_complexconnect_need_verify)()
= []() {
    return false;
};

bool (*longlink_ispush)(uint32_t _cmdid, uint32_t _taskid, const AutoBuffer& _body, const AutoBuffer& _extend)
= [](uint32_t _cmdid, uint32_t _taskid, const AutoBuffer& _body, const AutoBuffer& _extend) {
    return PUSH_DATA_TASKID == _taskid;
};
    
bool (*longlink_identify_isresp)(uint32_t _sent_seq, uint32_t _cmdid, uint32_t _recv_seq, const AutoBuffer& _body, const AutoBuffer& _extend)
= [](uint32_t _sent_seq, uint32_t _cmdid, uint32_t _recv_seq, const AutoBuffer& _body, const AutoBuffer& _extend) {
    return _sent_seq == _recv_seq && 0 != _sent_seq;
};

}
}
