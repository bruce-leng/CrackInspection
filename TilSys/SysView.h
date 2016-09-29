// TilSysView.h : interface of the CSysView class
//
/////////////////////////////////////////////////////////////////////////////

/************************************
  REVISION LOG ENTRY
  维护人员: Leng
  维护时间 2008-9-6 12:34:10
  说明: 将该类作为CTilSysView类的基类
 ************************************/

#if !defined(SYSVIEW_H)
#define SYSVIEW_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TilPoint.h"
#include "TilSysDoc.h"

class CSysView : public CScrollView
{
protected: // create from serialization only
	CSysView();
	DECLARE_DYNCREATE(CSysView)

// Attributes
public:
	CTilSysDoc* GetDocument();

// Operations
public:
	POINT2DF GetSrcPoint(const CPoint& point);	// 取得缩放前的点
	CPoint GetDstPoint(const POINT2DF& point);	// 取得缩放后的点
	void SetMousePosTextColor(CPoint Logpoint);

	void UpdateRulersInfo(int nMessage, CPoint ScrollPos, CPoint Pos = CPoint(0, 0));

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSysView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSysView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSysView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnImageConcat();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
	afx_msg LRESULT OnChangeItem(WPARAM,LPARAM);
	afx_msg LRESULT OnSelChanged(WPARAM,LPARAM);
	afx_msg LRESULT OnSelDeleted(WPARAM,LPARAM);
	afx_msg LRESULT OnLogInfo(WPARAM, LPARAM);
	afx_msg LRESULT OnNewImage(WPARAM, LPARAM);
	afx_msg LRESULT OnCrackDel(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in TilSysView.cpp
inline CTilSysDoc* CSysView::GetDocument()
   { return (CTilSysDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TILSYSVIEW_H__81A627F9_2DE2_434A_80DF_AD96683EB026__INCLUDED_)
