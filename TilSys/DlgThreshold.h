#if !defined(AFX_DLGTHRESHOLD_H__FCFC4DC2_FB70_11D5_8DD7_00E07D8144D0__INCLUDED_)
#define AFX_DLGTHRESHOLD_H__FCFC4DC2_FB70_11D5_8DD7_00E07D8144D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgThreshold.h : header file
//

#include "Image.h"
#include "TilSysDoc.h"
/////////////////////////////////////////////////////////////////////////////
// DlgThreshold dialog
class DlgThreshold : public CDialog
{
// Construction
public:
	DlgThreshold(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(DlgThreshold)
	enum { IDD = IDD_IMAGE_THRESHOLD };
	CButton	m_ok;
	CButton	m_canc;
	BYTE	m_level;
	BOOL	m_bPreview;
	//}}AFX_DATA

	CTilSysDoc* m_pDoc;

protected:
	CImage* m_pCurImage;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DlgThreshold)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DlgThreshold)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnThresholdMinus();
	afx_msg void OnThresholdPlus();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnReleasedcaptureSlider2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawSlider2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCheck1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTHRESHOLD_H__FCFC4DC2_FB70_11D5_8DD7_00E07D8144D0__INCLUDED_)