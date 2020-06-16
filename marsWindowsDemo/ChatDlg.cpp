// PingServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ChatDlg.h"
#include "Business/MarsWrapper.h"
#include <comutil.h>

CChatDlg::CChatDlg()
{
	::LoadLibrary(CRichEditCtrl::GetLibraryName());
}
CChatDlg::~CChatDlg()
{
	MarsWrapper::Instance().setChatMsgObserver(NULL);
}
BOOL CChatDlg::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

BOOL CChatDlg::OnIdle()
{
	UIUpdateChildWindows();
	return FALSE;
}
LRESULT CChatDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_editMsg = GetDlgItem(IDC_EDITMSG);
	m_editHistory = GetDlgItem(IDC_EDITHISTORY);
	//m_editTuid = GetDlgItem(IDC_EDITTUID);
	return true;
}


LRESULT CChatDlg::OnBnClickedBtnsendmsg(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int len = m_editMsg.GetWindowTextLength();
	wchar_t* buf = new wchar_t[len + 3 ];
	m_editMsg.GetWindowText(buf, len + 1);
	//buf[len] = L'\r';
	//buf[len + 1] = L'\n';
	buf[len] = L'\0';

    std::string tuid = GetTuid();
    std::string info = std::string("Send Msg To ") + tuid + ":";
	m_editHistory.AppendText((wchar_t*)(_bstr_t)info.c_str());
	m_editHistory.AppendText(buf);
	m_editHistory.AppendText(L"\r\n");
	_bstr_t bstr_buf = (buf);
	ChatMsg msg;

	msg.content_ = (char*)bstr_buf;
	MarsWrapper::Instance().sendChatMsg(msg, tuid);


	delete[]buf;

	m_editMsg.SetWindowText(L"");

	return 0;
}
void CChatDlg::OnRecvChatMsg(const ChatMsg& msg)
{
    std::wstring info = std::wstring(L"Recv Msg: ") + (wchar_t*)(_bstr_t)msg.content_.c_str() + L"\r\n";
	m_editHistory.AppendText(info.c_str());
}

std::string CChatDlg::GetTuid()
{
	wchar_t tuid[1024] = L"\0";
	GetDlgItemText(IDC_EDITTUID, tuid, sizeof(tuid) / sizeof(tuid[0]));
	std::wstring strTuid = tuid;
	if (strTuid.empty())strTuid = L"10000";

    _bstr_t t = strTuid.c_str();  
    char* pchar = (char*)t;  
    std::string result = pchar;  
    return result;

}

void CChatDlg::SetChatInfo(const std::string& name, const std::string& topic)
{
	m_topic = topic;
	m_name = name;
}