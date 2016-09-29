// ListCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "ListCtrlEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx

CListCtrlEx::CListCtrlEx()
{
}

CListCtrlEx::~CListCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
	//{{AFX_MSG_MAP(CListCtrlEx)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_COMMAND(ID_CRACK_DEL, OnCrackDel)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx message handlers

void CListCtrlEx::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// 获得被选中的行的序号
	int nSelected = -1;
	POSITION pos = GetFirstSelectedItemPosition();
	while (pos)
	{
		nSelected = GetNextSelectedItem(pos);
	}

	// 获得主视图指针
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild = (CMDIChildWnd *) pFrame->GetActiveFrame();
	CView *pView = (CView *) pChild->GetActiveView();

	// 向主视图发送消息，更新主视图
	::SendMessage(pView->m_hWnd, WM_USER_NEWIMAGE, nSelected, 0);
	
	*pResult = 0;
}

void CListCtrlEx::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnClick(pNMHDR, pResult);
}

// 删除自动提取的错误裂缝
void CListCtrlEx::OnCrackDel() 
{
	// 获得被选中的行的序号
	int nSelected = -1;
	POSITION pos = GetFirstSelectedItemPosition();
	while (pos)
	{
		nSelected = GetNextSelectedItem(pos);
	}
	if (nSelected < 0)
		return;

	// 获得主视图指针
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild = (CMDIChildWnd *) pFrame->GetActiveFrame();
	CView *pView = (CView *) pChild->GetActiveView();
	
	// 向主视图发送消息，更新主视图
	::SendMessage(pView->m_hWnd, WM_USER_CRACK_DEL, nSelected, 0);

	// 删除该行数据
	DeleteItem(nSelected);
}

void CListCtrlEx::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if (point.x == -1 && point.y == -1)
	{
		//keystroke invocation
		CRect rect;
		GetClientRect(rect);
		ClientToScreen(rect);
		
		point = rect.TopLeft();
		point.Offset(5, 5);
	}

	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_POPUP_CRACK_PROC));
	
	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	CWnd* pWndPopupOwner = this;
	
// 	while (pWndPopupOwner->GetStyle() & WS_CHILD)
// 		pWndPopupOwner = pWndPopupOwner->GetParent();
	
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
		pWndPopupOwner);
}
