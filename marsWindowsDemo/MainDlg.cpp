#include "stdafx.h"
#include "MainDlg.h"
#include "AboutDlg.h"
#include "Business/MarsWrapper.h"
#include <comutil.h>


BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnIdle()
{
	UIUpdateChildWindows();
	return FALSE;
}

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

	// set icons
	HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(hIconSmall, FALSE);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);

	RECT rc;
	GetClientRect(&rc);
	m_tabView.Create(m_hWnd, rc, NULL, WS_CHILD | WS_VISIBLE, WS_EX_STATICEDGE);

	m_chatDlg = new CChatDlg();
	m_chatDlg->Create(m_tabView.m_hWnd);
	m_chatDlg->SetChatInfo("test_name", "test_topic");  //topic
	m_tabView.AddPage(m_chatDlg->m_hWnd, (wchar_t*)(_bstr_t)"test_dialog");


	MarsWrapper::Instance().start();

	MarsWrapper::Instance().setChatMsgObserver(this);
	return TRUE;
}

LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);


	MarsWrapper::Instance().setChatMsgObserver(nullptr);
	//m_pingServerDlg->SetHostWnd(nullptr);
	delete m_chatDlg;
	return 0;
}

LRESULT CMainDlg::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add validation code 
	CloseDialog(wID);
	return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CloseDialog(wID);
	return 0;
}

void CMainDlg::CloseDialog(int nVal)
{
	DestroyWindow();
	::PostQuitMessage(nVal);
}



void CMainDlg::OnRecvChatMsg(const ChatMsg& msg)
{
	m_chatDlg->OnRecvChatMsg(msg);
}