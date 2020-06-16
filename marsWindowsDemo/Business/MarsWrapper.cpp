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
*  MarsWrapper.cpp
*
*  Created on: 2017-7-7
*      Author: chenzihao
*/

#include "MarsWrapper.h"
#include "Wrapper/NetworkService.h"
#include "ChatCGITask.h"

#ifdef __APPLE__
#include "mars/xlog/xlogger.h"
#else
//#include "mars/comm/xlogger/xlogger.h"
#include "mars/xlog/xlogger.h"

#endif


//#include "mars/comm/xlogger/xloggerbase.h"
#include "mars/xlog/xloggerbase.h"

#include "mars/xlog/appender.h"

//static const char* g_host = "marsopen.cn";
static const char* g_host = "im.hpplay.cn";

MarsWrapper& MarsWrapper::Instance()
{
	static MarsWrapper instance_;
	return instance_;
}

MarsWrapper::MarsWrapper()
	: chat_msg_observer_(nullptr)
{
	std::string logPath = "Log"; //use your log path
	std::string pubKey = ""; //use you pubkey for log encrypt

#if _DEBUG
	xlogger_SetLevel(kLevelDebug);
	appender_set_console_log(true);
#else
	xlogger_SetLevel(kLevelInfo);
	appender_set_console_log(false);
#endif
	appender_open(kAppednerAsync, logPath.c_str(), "Sample", pubKey.c_str());
}

void MarsWrapper::OnPush(uint64_t _channel_id, uint32_t _cmdid, uint32_t _taskid, const AutoBuffer& _body, const AutoBuffer& _extend)
{
	if (chat_msg_observer_)
	{
		ChatMsg chat_msg;
		chat_msg.content_ = std::string((const char *)(_body.Ptr()),_body.Length());
		chat_msg_observer_->OnRecvChatMsg(chat_msg);
	}

}

void MarsWrapper::start()
{
	NetworkService::Instance().setClientVersion(200);
	NetworkService::Instance().setShortLinkPort(7172);
	NetworkService::Instance().setLongLinkAddress(g_host, 8080, "");
	NetworkService::Instance().start();	

	NetworkService::Instance().setPushObserver(5, this);  //5为推送消息
}


void MarsWrapper::setChatMsgObserver(ChatMsgObserver* _observer)
{
	chat_msg_observer_ = _observer;
}

void MarsWrapper::sendChatMsg(const ChatMsg& _chat_msg, std::string tuid)
{
   
	ChatCGITask* task = new ChatCGITask();
	task->channel_select_ = ChannelType_ShortConn;
	task->cgi_ = "/1/push?uid="+ tuid;
	task->host_ = g_host;
	task->text_ = _chat_msg.content_;
	task->access_token_ = "123456";

	//xinfo2("111111111 sendChatMsg:cgi_=%s, text_=%s", task->cgi_.c_str(), task->text_.c_str());

	NetworkService::Instance().startTask(task);
}
