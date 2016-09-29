// GuiCrackCaption.cpp: implementation of the CGuiCrackCaption class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tilsys.h"
#include "GuiCrackCaption.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CComboListCtrl CGuiCrackCaption::m_CrackCaption;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGuiCrackCaption::CGuiCrackCaption()
{

}

CGuiCrackCaption::~CGuiCrackCaption()
{

}

BEGIN_MESSAGE_MAP(CGuiCrackCaption, CGuiControlBar)
	ON_WM_CREATE()
END_MESSAGE_MAP()

int CGuiCrackCaption::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CGuiControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_TabSolExplorer.Create(WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,0x9999))
		return -1;

	DWORD dwStyleReport = LVS_REPORT|LVS_SHOWSELALWAYS|LVS_ALIGNTOP|LVS_SINGLESEL|LVS_AUTOARRANGE|LVS_EDITLABELS;
	if (!m_CrackCaption.Create(dwStyleReport,CRect(0,0,0,0), &m_TabSolExplorer, 2))
		return -1;

	m_TabSolExplorer.SetImageList(IDB_DBPROJECTS, 16,8, RGB (255, 0, 0));
	m_TabSolExplorer.Addtab(&m_CrackCaption,"�ѷ�����",1);

	// �����ѷ�����
	m_CrackCaption.InsertColumn(0,"��������");
	m_CrackCaption.InsertColumn(1,"����ֵ");
	m_CrackCaption.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	m_CrackCaption.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);

	CString strCaptionName[] = {"����","����","����","�����","��Ҫ������н�","��ע"};
	for (int i=0; i<6; i++)
	{
		m_CrackCaption.InsertItem(i,strCaptionName[i]);
	}

	return 0;
}
