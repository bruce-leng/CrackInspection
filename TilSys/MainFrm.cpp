// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "TilSys.h"
#include "MainFrm.h"
#include "Splash.h"
#include "TilSysDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_WORKSPACE, OnWorkspace)
	ON_COMMAND(ID_CRACKCAPTION, OnCrackCaption)
	ON_COMMAND(ID_OUTPUT, OnOutput)
	ON_COMMAND(ID_CLASSVIEW, OnClassview)
	ON_COMMAND(ID_UNDO, OnUndo)
	ON_COMMAND(ID_REDO, OnRedo)
	ON_COMMAND(ID_VIEW_TOOLBAR, OnViewToolbar)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR, OnUpdateViewToolbar)
	ON_UPDATE_COMMAND_UI(ID_WORKSPACE, OnUpdateWorkspace)
	ON_UPDATE_COMMAND_UI(ID_CRACKCAPTION, OnUpdateCrackcaption)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_COORD,			// 逻辑坐标
	ID_INDICATOR_RGBA,			// RGBA颜色灰度值
//	ID_INDICATOR_CAPS,
//	ID_INDICATOR_NUM,
//	ID_INDICATOR_SCRL,
};

static UINT Toolbars[] = {
	IDR_MAINFRAME,
	IDR_TOOLBARS/*,
	IDR_NEWITEM*/
	
};
/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

void CMainFrame::OnUndo() {}
void CMainFrame::OnRedo() {}
void CMainFrame::OnClassview() {}

void CMainFrame::OnViewToolbar() { ShowHideBar(&m_wndToolBar); }
void CMainFrame::OnWorkspace() { ShowHideBar(&m_WorkSpaceBar); }
void CMainFrame::OnCrackCaption() { ShowHideBar(&m_CrackCaption); }
void CMainFrame::OnOutput() {	ShowHideBar(&m_OutPut); }

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CSplashWnd::ShowSplashScreen(this);

	if (CGuiMDIFrame::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	InitMenu(IDR_MAINFRAME);
	InitToolBar(IDR_MAINFRAME);
	InitStatusBar(indicators,sizeof(indicators)/sizeof(UINT));

	// TODO: Delete these three lines if you don't want the toolbar to
	if (!m_wndTextBar.Create(this, WS_CHILD | CBRS_ALIGN_TOP |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, IDR_STYLE_BAR))
	{
		TRACE0("Failed to create text stylebar\n");
		return -1;
	}
	//  be dockable
	if (!m_WorkSpaceBar.Create(_T("工作台"),WS_CHILD | WS_VISIBLE, this, 0x999))
	{
		TRACE0("Failed to create m_WorkSpaceBar\n");
		return -1;      
	}
	if (!m_CrackCaption.Create(_T("裂缝属性"),WS_CHILD | WS_VISIBLE, this, 0x998))
	{
		TRACE0("Failed to create m_WorkSpaceBar\n");
		return -1;     
	}
	if (!m_OutPut.Create(_T("状态信息"),WS_CHILD | WS_VISIBLE, this, 0x997))
	{
		TRACE0("Failed to create m_WorkSpaceBar\n");
		return -1;      
	}

	m_wndToolBar.SetWindowText("工具条");
	m_wndTextBar.SetWindowText("文字工具条");

	const DWORD dwStyle =  LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL ;
	m_UndoTree.Create (dwStyle, CRect(0,0,100,100), this, 1);
	m_UndoTree.Test(); // 测试"重复"工具条按钮

	// 设置工具条上的下拉菜单
	m_wndToolBar.SetButtonDropDown(ID_CLASSVIEW, 7, IDR_MENUTOOLBARS);

	m_WorkSpaceBar.SetBarStyle(m_WorkSpaceBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_CrackCaption.SetBarStyle(m_CrackCaption.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_OutPut.SetBarStyle(m_OutPut.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	EnableDocking(CBRS_ALIGN_ANY);

	m_wndTextBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_LEFT);
	m_WorkSpaceBar.EnableDocking(CBRS_ALIGN_ANY);
	m_CrackCaption.EnableDocking(CBRS_ALIGN_ANY);
	m_OutPut.EnableDocking(CBRS_ALIGN_BOTTOM);

	m_wndToolBar.SetMenuContext(IDR_MENUTOOLBARS);
	m_WorkSpaceBar.SetMenuContext(IDR_MENUTOOLBARS);
	m_CrackCaption.SetMenuContext(IDR_MENUTOOLBARS);

	// 工具条及子窗口停靠位置
	DockControlBar(&m_wndToolBar,m_dockTop);
	DockControlBar(&m_wndTextBar, m_dockTop);
	DockControlBar(&m_WorkSpaceBar, AFX_IDW_DOCKBAR_LEFT);
	DockControlBar(&m_CrackCaption, AFX_IDW_DOCKBAR_RIGHT);
	DockControlBar(&m_OutPut, AFX_IDW_DOCKBAR_BOTTOM);

	FloatControlBar(&m_wndTextBar, CPoint(300,10));

	sProfile = _T("TilSys");
	LoadBars();
	InitMDITabbed();
	m_MdiTabbed.SetTabsMenu(IDR_MENUTABS);
	m_MdiTabbed.SetTabMenu(0,0);

	// 显示工具条上的组合下拉菜单各项的左边的小图
	#define _countof(array) (sizeof(array)/sizeof(array[0]))
	m_wndCool.LoadToolbars(Toolbars, _countof(Toolbars));
	RecalcLayout();

	SetTimer(1, 1000, NULL);
	
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
  cs.cy = ::GetSystemMetrics(SM_CYSCREEN); 
  cs.cx = ::GetSystemMetrics(SM_CXSCREEN); 
  cs.y = ::GetSystemMetrics(SM_CYSCREEN); 
  cs.x = -::GetSystemMetrics(SM_CYSCREEN);
	if( !CGuiMDIFrame::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CGuiMDIFrame::PreTranslateMessage(pMsg);
}

// 设置当前图形层
void CMainFrame::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	CTilCanvasMgr* pMgr = GetCanvasMgr();

	int nRet1, nRet2;

	// 保存所有图形、图像文件
	for (int i = 0; i < pMgr->Size(); i ++)
	{
		CTilCanvas* pCanvas = pMgr->GetCanvasByIdx(i);
		if (pCanvas->IsModified())
		{
			nRet1 = AfxMessageBox("是否保存所有文件?", MB_YESNOCANCEL);
			if (nRet1 == IDYES)
			{
				((CTilSysApp*)AfxGetApp())->OnFileSaveAll();
				break;
			}
			else if (nRet1 == IDNO)
				break;
			else if (nRet1 == IDCANCEL)
				return;
		}
	}

	// 保存项目文件
	if (pMgr->IsModified())
	{
		nRet2 = AfxMessageBox("是否保存项目?", MB_YESNOCANCEL);
		if (nRet2 == IDYES)
			((CTilSysApp*)AfxGetApp())->OnProjectSave();
		else if (nRet2 == IDCANCEL)
			return;
	}

	CMDIFrameWnd::OnClose();
}

void CMainFrame::SetMousePosTextColor(CPoint Logpoint, RGBQUAD color)
{
	CString strText;
	strText.Format("逻辑坐标:(%d,%d)",Logpoint.x,Logpoint.y);
	int nIndex=m_wndStatusBar.CommandToIndex(ID_INDICATOR_COORD);
	m_wndStatusBar.SetPaneText(nIndex,strText);
	
	CWindowDC dc(&m_wndStatusBar);
	CSize sizeText=dc.GetTextExtent(strText);
	m_wndStatusBar.SetPaneInfo(nIndex,ID_INDICATOR_COORD,SBPS_NORMAL,sizeText.cx);

	strText.Format("指针处颜色(R:G:B:A):(%d,%d,%d,%d)",
		color.rgbRed, color.rgbGreen, color.rgbBlue, color.rgbReserved);
	nIndex = m_wndStatusBar.CommandToIndex(ID_INDICATOR_RGBA);
	m_wndStatusBar.SetPaneText(nIndex,strText);
}

void CMainFrame::ShowFontBar(BOOL isShow)
{
	if ((!isShow&&m_wndTextBar.IsWindowVisible())
		|| (isShow&&!m_wndTextBar.IsWindowVisible()))
		OnBarCheck(IDR_STYLE_BAR);
}

void CMainFrame::ShowHistoBar(BOOL isShow)
{
//	if (isShow)
//		m_HistoBar.ShowWindow(SW_SHOW);
//	else
//		m_HistoBar.ShowWindow(SW_HIDE);
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{
	static BOOL bShow=false;
	if(!bShow)
	{  
		int cscy = ::GetSystemMetrics(SM_CYSCREEN) / 4*3; 
		int cscx = ::GetSystemMetrics(SM_CXSCREEN) / 4*3; 
		int csy = cscy/6; 
		int csx = cscx/6;
		CRect rect;
	}
	bShow=true;

	CGuiMDIFrame::OnTimer(nIDEvent);
}

void CMainFrame::OnUpdateViewToolbar(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_wndToolBar.IsWindowVisible());
}

void CMainFrame::OnUpdateWorkspace(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_WorkSpaceBar.IsWindowVisible());
}

void CMainFrame::OnUpdateCrackcaption(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_CrackCaption.IsWindowVisible());
}
