// GuiCrackCaption.h: interface for the CGuiCrackCaption class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUICRACKCAPTION_H__4137CEE4_8BE7_4E3F_B06E_47FC2D907775__INCLUDED_)
#define AFX_GUICRACKCAPTION_H__4137CEE4_8BE7_4E3F_B06E_47FC2D907775__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GuiControlBar.h"
#include "GuiTabWnd.h"
#include "ComboListCtrl.h"

// 裂缝属性
typedef struct __tagCRACKCAPTION
{
	float fMaxAxis;		// 长轴
	float fMinAxis;		// 短轴
	float fLength;		// 长度
	float fMaxWidth;	// 裂缝最大宽度
	float fMaxAxisAngle;// 长轴沿纵向夹角
	CString strDesc;	// 备注
} CRACK_CAPTION, *PCRACK_CAPTION, **PPCRACK_CAPTION;

class CGuiCrackCaption : public CGuiControlBar  
{
protected:
	CGuiTabWnd	m_TabSolExplorer;

	static CComboListCtrl m_CrackCaption;
public:
	CGuiCrackCaption();
	virtual ~CGuiCrackCaption();

	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

};

#endif // !defined(AFX_GUICRACKCAPTION_H__4137CEE4_8BE7_4E3F_B06E_47FC2D907775__INCLUDED_)
