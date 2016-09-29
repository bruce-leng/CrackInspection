// TilSysView.cpp : implementation of the CSysView class
//

#include "stdafx.h"
#include "TilSys.h"
#include <float.h>

#include "TilSysDoc.h"
#include "TilSysView.h"
#include "ChildFrm.h"
#include "MainFrm.h"

#include "GuiOutPut.h"
#include "TilCanvas.h"
#include "Image.h"
#include "TilComFunction.h"
#include "GuiDefine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CSysView

IMPLEMENT_DYNCREATE(CSysView, CScrollView)

BEGIN_MESSAGE_MAP(CSysView, CScrollView)
	//{{AFX_MSG_MAP(CSysView)
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_CONTEXTMENU()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)

	ON_MESSAGE(WM_CHANGE_ITEM, OnChangeItem)
	ON_MESSAGE(WM_SELE_CHANGED, OnSelChanged)
	ON_MESSAGE(WM_SELE_DELETED, OnSelDeleted)
	ON_MESSAGE(WM_USER_NEWIMAGE, OnNewImage)
	ON_MESSAGE(WM_USER_CRACK_DEL, OnCrackDel)
	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSysView construction/destruction

CSysView::CSysView()
{
}

CSysView::~CSysView()
{
}

BOOL CSysView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSysView drawing

void CSysView::OnDraw(CDC* pDC)
{
	CTilSysDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
	CTilCanvas* pCanvas = GetCanvasMgr()->GetCurCanvas();
	if (pCanvas && !pDC->IsPrinting())
	{
		pCanvas->Draw(pDC);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSysView printing

void CSysView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	pDC->SetMapMode(MM_TWIPS);

	CScrollView::OnPrepareDC(pDC, pInfo);
}

BOOL CSysView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	pInfo->SetMaxPage(GetCanvasMgr()->Size());
	pInfo->SetMinPage(1);

	return DoPreparePrinting(pInfo);
}

void CSysView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{

}

#include <afxpriv.h> 
void CSysView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	if (pInfo->m_nCurPage <= 0 || pInfo->m_nCurPage > GetCanvasMgr()->Size())
		return;

	CTilSysDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pDC->SetMapMode(MM_LOMETRIC);

	CScrollView::OnPrint(pDC, pInfo);
}

void CSysView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: add cleanup after printing
	CScrollView::OnEndPrinting(pDC, pInfo);

	// 完成打印预览或打印后，清理打印内容
	CTilSysDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
}

/////////////////////////////////////////////////////////////////////////////
// CSysView diagnostics

#ifdef _DEBUG
void CSysView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CSysView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CTilSysDoc* CSysView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTilSysDoc)));
	return (CTilSysDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSysView message handlers

void CSysView::OnInitialUpdate() 
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	sizeTotal.cx = 100;
	sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);

	// 显示标尺（不完善，暂不显示）
	CChildFrame* pParent = ((CChildFrame*)GetParentFrame());
	pParent->ShowRulers(TRUE);
}

void CSysView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	CMainFrame* pMain = (CMainFrame*) AfxGetMainWnd();
//	if (pMain->m_HistoBar.IsWindowVisible()){
//		CTilSysDoc* pDoc = GetDocument();
//		CTilCanvas* pCanvas = GetCanvasMgr()->GetCurCanvas();
//		if (pCanvas!=NULL)
//		{
//			CImage* pImage = pCanvas->GetImage()->GetCurImage();
////			pDoc->m_hmax=pImage->Histogram(pDoc->m_hr,pDoc->m_hg,pDoc->m_hb,pDoc->m_hgray);
////			pMain->m_HistoBar.Invalidate();
//		}
//	}

	UpdateRulersInfo(RW_POSITION, GetDeviceScrollPosition());
	Invalidate();
}

// 更新标尺
void CSysView::UpdateRulersInfo(int nMessage, CPoint ScrollPos, CPoint Pos)
{
	CChildFrame* pParent = ((CChildFrame*)GetParentFrame());
	if (!pParent)
		return;

	CTilCanvasMgr* pMgr = GetCanvasMgr();
	if (!pMgr->GetCurCanvas())
		return;
	CTilCanvas* pCanvas = pMgr->GetCurCanvas();
	long double dbZoomX, dbZoomY;
	pCanvas->GetZoomFactor(dbZoomX, dbZoomY);

	stRULER_INFO pRulerInfo;
	pRulerInfo.uMessage    = nMessage;
	pRulerInfo.ScrollPos   = ScrollPos;
	pRulerInfo.Pos         = Pos;
	pRulerInfo.DocSize     = CSize(pCanvas->GetWidth(), pCanvas->GetHeight()); 

	// 标尺在原图缩放的基础上再缩放0.5倍,因为图像比例尺按像素:厘米＝2:1设置
	if (nMessage == RW_HSCROLL)
		pRulerInfo.fZoomFactor = dbZoomX;
	else
		pRulerInfo.fZoomFactor = dbZoomY;

	pParent->UpdateRulersInfo(pRulerInfo);
}

// 用鼠标拖动改变文件相互位置
LRESULT CSysView::OnChangeItem(WPARAM wParam,LPARAM lParam)
{
	int nOldPos = (int)wParam;
	int nNewPos = (int)lParam;

	GetCanvasMgr()->DragCanvasTo(nOldPos, nNewPos);
	SendMessage(WM_SIZE);

	UpdateRulersInfo(RW_VSCROLL, GetScrollPosition());
	Invalidate();

	return 0;
}

// 当文件选择改变时
// WPARAM wParam:当前切片序号
LRESULT CSysView::OnSelChanged(WPARAM wParam,LPARAM lParam)
{
	CTilSysDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	CTilCanvasMgr* pMgr = GetCanvasMgr();
	pMgr->SetCurCanvasIdx((int)wParam);
	
	SendMessage(WM_SIZE);
	
	//画出水平和竖直标尺
	UpdateRulersInfo(RW_VSCROLL, GetScrollPosition());
	Invalidate();

	pDoc->m_hWnd = GetSafeHwnd();

	return 0;
}

// 删除选中的图像切片
// WPARAM wParam:当前切片序号
LRESULT CSysView::OnSelDeleted(WPARAM wParam,LPARAM lParam)
{
	int nIdx = (int)wParam;
	GetCanvasMgr()->RemoveAt(nIdx);
	Invalidate();

	return 0;
}

LRESULT CSysView::OnLogInfo(WPARAM wParam, LPARAM lParam)
{
	int nIdx = int(wParam);
	CTilCanvas* pCanvas = GetCanvasMgr()->GetCanvasByIdx(nIdx);
	//LogInfo(pCanvas->GetImage()->GetPathName());
	
	return 0;
}

void CSysView::OnSize(UINT nType, int cx, int cy) 
{
	CScrollView::OnSize(nType, cx, cy);

	CTilSysDoc* pDoc = GetDocument();

	// TODO: Add your message handler code here
	CTilCanvas* pCanvas = GetCanvasMgr()->GetCurCanvas();
	if (!pCanvas)	return;

	long double dbZoomX, dbZoomY;
	pCanvas->GetZoomFactor(dbZoomX, dbZoomY);

	int nZoomWidth = int(pCanvas->GetWidth()*dbZoomX);
	int nZoomHeigh = int(pCanvas->GetHeight()*dbZoomY);
	SetScrollSizes(MM_TEXT, CSize(nZoomWidth, nZoomHeigh));

	// 显示图像属性
	ImageInfo(pCanvas->GetWidth(), pCanvas->GetHeight(),
						pCanvas->GetDepth()*8, "");
	CrackInfo(pCanvas->GetCrackCharacter());
}

void CSysView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CTilCanvas* pCanvas = GetCanvasMgr()->GetCurCanvas();
	if (!pCanvas)
		return;

	CScrollView::OnLButtonDown(nFlags, point);
}

void CSysView::OnMouseMove(UINT nFlags, CPoint point) 
{
	CTilCanvas* pCanvas = GetCanvasMgr()->GetCurCanvas();
	if (!pCanvas)
		return;

	CClientDC dc(this);
	OnPrepareDC(&dc);

	CScrollView::OnMouseMove(nFlags, point);
}

void CSysView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CTilCanvas* pCanvas = GetCanvasMgr()->GetCurCanvas();
	if (!pCanvas)
		return;

	CClientDC dc(this);
	OnPrepareDC(&dc);

	CScrollView::OnLButtonDblClk(nFlags, point);
}


void CSysView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CTilCanvas* pCanvas = GetCanvasMgr()->GetCurCanvas();
	if (!pCanvas)
		return;
	
	CClientDC dc(this);
	OnPrepareDC(&dc);

	CScrollView::OnLButtonUp(nFlags, point);
}

void CSysView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	switch (nChar)
	{
		case VK_HOME:
			OnVScroll(SB_TOP, 0, NULL);
			OnHScroll(SB_LEFT, 0, NULL);
			break;
		case VK_END:
			OnVScroll(SB_BOTTOM, 0, NULL);
			OnHScroll(SB_RIGHT, 0, NULL);
			break;
		case VK_UP:
			OnVScroll(SB_LINEUP, 0, NULL);
			break;
		case VK_DOWN:
			OnVScroll(SB_LINEDOWN, 0, NULL);
			break;
		case VK_PRIOR:
			OnVScroll(SB_PAGEUP, 0, NULL);
			break;
		case VK_NEXT:
			OnVScroll(SB_PAGEDOWN, 0, NULL);
			break;
		case VK_LEFT:
			OnHScroll(SB_LINELEFT, 0, NULL);
			break;
		case VK_RIGHT:
			OnHScroll(SB_LINERIGHT, 0, NULL);
			break;
	}

	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CSysView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	CScrollView::OnKeyUp(nChar, nRepCnt, nFlags);
}

// 设置状态栏信息
void CSysView::SetMousePosTextColor(CPoint Logpoint)
{
// 	POINT2DF point = GetSrcPoint(Logpoint);
// 	Logpoint = CPoint(point.x,point.y);
// 
// 	CTilCanvas* pCanvas = GetCanvasMgr()->GetCurCanvas();
// 	if (!pCanvas)
// 		return;
// 
// 	CImage* pImage = pCanvas->GetImage()->GetCurImage();
// 	RGBQUAD color = pImage->GetPixelColor(Logpoint.x, pImage->GetHeight()-Logpoint.y-1);
// 
// 	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
// 	pFrame->SetMousePosTextColor(Logpoint, color);
}

// 取得缩放前的点
POINT2DF CSysView::GetSrcPoint(const CPoint& point)
{
	POINT2DF pt;
	long double dbZoomX, dbZoomY;

	GetCanvasMgr()->GetZoomFactor(dbZoomX, dbZoomY);
	pt = POINT2DF(point.x/dbZoomX, point.y/dbZoomY);

	return pt;
}

// 取得缩放后的点
CPoint CSysView::GetDstPoint(const POINT2DF& point)
{
	CPoint pt;
	long double dbZoomX, dbZoomY;

	GetCanvasMgr()->GetZoomFactor(dbZoomX, dbZoomY);
	pt = CPoint(long(point.x*dbZoomX+0.5), long(point.y*dbZoomY+0.5));

	return pt;
}

void CSysView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// CG: This block was added by the Pop-up Menu component
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
	VERIFY(menu.LoadMenu(IDR_POPUP_SYS_VIEW));

	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	CWnd* pWndPopupOwner = this;

	while (pWndPopupOwner->GetStyle() & WS_CHILD)
		pWndPopupOwner = pWndPopupOwner->GetParent();

	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
		pWndPopupOwner);
}

void CSysView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// 滑动条水平滑动时，标尺作相应的滑动
	UpdateRulersInfo(RW_HSCROLL, GetScrollPosition(), nPos);
	
	CScrollView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CSysView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// 滑动条垂直滑动时，标尺作相应的滑动
	UpdateRulersInfo(RW_VSCROLL, GetScrollPosition(), nPos);
	
	CScrollView::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CSysView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if (zDelta > 0)
		OnVScroll(SB_LINEUP, 0, NULL);
	else
		OnVScroll(SB_LINEDOWN, 0, NULL);

	return CScrollView::OnMouseWheel(nFlags, zDelta, pt);
}

LRESULT CSysView::OnNewImage(WPARAM wParam, LPARAM lParam)
{
	CTilSysDoc* pDoc = GetDocument();
	int nObjIdx = int(wParam);
	CTilCanvas* pCanvas = GetCanvasMgr()->GetCurCanvas();
	if (pCanvas != NULL)
	{
		pCanvas->SetSelectedObject(nObjIdx);
		pDoc->UpdateAllViews(0, WM_USER_NEWIMAGE);
	}

	return 0;
}

LRESULT CSysView::OnCrackDel(WPARAM wParam, LPARAM lParam)
{
	CTilSysDoc* pDoc = GetDocument();
	int nObjIdx = int(wParam);
	CTilCanvas* pCanvas = GetCanvasMgr()->GetCurCanvas();
	pCanvas->DelSelectedObject();
	
	pDoc->UpdateAllViews(0, WM_USER_CRACK_DEL);
	
	return 0;
}
