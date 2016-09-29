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

// �ѷ�����
typedef struct __tagCRACKCAPTION
{
	float fMaxAxis;		// ����
	float fMinAxis;		// ����
	float fLength;		// ����
	float fMaxWidth;	// �ѷ������
	float fMaxAxisAngle;// ����������н�
	CString strDesc;	// ��ע
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
