// TilSysDoc.h : interface of the CTilSysDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TILSYSDOC_H__53A6FDC2_6CD0_4DC3_AB11_AE07DD16BEF7__INCLUDED_)
#define AFX_TILSYSDOC_H__53A6FDC2_6CD0_4DC3_AB11_AE07DD16BEF7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// 图像预处理线程
void RunImageThread(void* pParam);

class CTilSysDoc : public CDocument
{
protected: // create from serialization only
	CTilSysDoc();
	DECLARE_DYNCREATE(CTilSysDoc)

// Operations
public:
	HANDLE	m_hThread;		// 执行菜单命令的线程
	long	m_lMenuCommand;	// 菜单命令ID
	int		m_nMenuCol;		// 标记调用的是哪一列的菜单命令
	HWND	m_hWnd;			// 视图指针句柄

	void UpdateDrawTree();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTilSysDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTilSysDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CTilSysDoc)
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnImageDilate();
	afx_msg void OnImageThreshold();
	afx_msg void OnUpdateImageDilate(CCmdUI* pCmdUI);
	afx_msg void OnUpdateImageThreshold(CCmdUI* pCmdUI);
	afx_msg void OnViewZoomin();
	afx_msg void OnViewZoomout();
	afx_msg void OnImageMirror();
	afx_msg void OnImageFlip();
	afx_msg void OnImageNegative();
	afx_msg void OnImageRotatel();
	afx_msg void OnImageRotater();
	afx_msg void OnUpdateViewZoomin(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewZoomout(CCmdUI* pCmdUI);
	afx_msg void OnUpdateImageMirror(CCmdUI* pCmdUI);
	afx_msg void OnUpdateImageFlip(CCmdUI* pCmdUI);
	afx_msg void OnUpdateImageNegative(CCmdUI* pCmdUI);
	afx_msg void OnUpdateImageRotatel(CCmdUI* pCmdUI);
	afx_msg void OnUpdateImageRotater(CCmdUI* pCmdUI);
	afx_msg void OnImageColorize();
	afx_msg void OnImageLighten();
	afx_msg void OnImageDarken();
	afx_msg void OnImageContrast();
	afx_msg void OnImageLesscontrast();
	afx_msg void OnImageMedian();
	afx_msg void OnImageErode();
	afx_msg void OnImageBlur();
	afx_msg void OnImageSoften();
	afx_msg void OnImageGaussian3x3();
	afx_msg void OnImageGaussian5x5();
	afx_msg void OnImageSharpen();
	afx_msg void OnUpdateImageColorize(CCmdUI* pCmdUI);
	afx_msg void OnUpdateImageLighten(CCmdUI* pCmdUI);
	afx_msg void OnUpdateImageDarken(CCmdUI* pCmdUI);
	afx_msg void OnUpdateImageContrast(CCmdUI* pCmdUI);
	afx_msg void OnUpdateImageLesscontrast(CCmdUI* pCmdUI);
	afx_msg void OnUpdateImageMedian(CCmdUI* pCmdUI);
	afx_msg void OnUpdateImageErode(CCmdUI* pCmdUI);
	afx_msg void OnUpdateImageBlur(CCmdUI* pCmdUI);
	afx_msg void OnUpdateImageSoften(CCmdUI* pCmdUI);
	afx_msg void OnUpdateImageGaussian3x3(CCmdUI* pCmdUI);
	afx_msg void OnUpdateImageGaussian5x5(CCmdUI* pCmdUI);
	afx_msg void OnUpdateImageSharpen(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewZoomnormal(CCmdUI* pCmdUI);
	afx_msg void OnViewZoomnormal();
	afx_msg void OnViewGraphLayer(UINT nID);
	afx_msg void OnUpdateViewGraphLayer(CCmdUI* pCmdUI);
	afx_msg void OnFileResume();
	afx_msg void OnUpdateFileResume(CCmdUI* pCmdUI);
	afx_msg void OnImageStretch();
	afx_msg void OnUpdateImageStretch(CCmdUI* pCmdUI);
	afx_msg void OnImageNormalize();
	afx_msg void OnUpdateImageNormalize(CCmdUI* pCmdUI);
	afx_msg void OnImageLog();
	afx_msg void OnUpdateImageLog(CCmdUI* pCmdUI);
	afx_msg void OnGeoPredict();
	afx_msg void OnImageUsm();
	afx_msg void OnUpdateImageUsm(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnImageFindCracks();
	afx_msg void OnUpdateImageFindCracks(CCmdUI* pCmdUI);
	afx_msg void OnViewLayerSrcImage();
	afx_msg void OnUpdateViewLayerSrcImage(CCmdUI* pCmdUI);
	afx_msg void OnViewLayerCrackImage();
	afx_msg void OnUpdateViewLayerCrackImage(CCmdUI* pCmdUI);
	afx_msg void OnViewLayerCrackObj();
	afx_msg void OnUpdateViewLayerCrackObj(CCmdUI* pCmdUI);
	afx_msg void OnImageThin();
	afx_msg void OnUpdateImageThin(CCmdUI* pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	// 识别掌子面岩体岩性
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TILSYSDOC_H__53A6FDC2_6CD0_4DC3_AB11_AE07DD16BEF7__INCLUDED_)
