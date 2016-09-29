// DlgRulerSet.cpp : implementation file
//

#include "stdafx.h"
#include "tilsys.h"
#include "DlgRulerSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgRulerSet dialog

DlgRulerSet::DlgRulerSet(CWnd* pParent /*=NULL*/)
	: CDialog(DlgRulerSet::IDD, pParent)
{
	//{{AFX_DATA_INIT(DlgRulerSet)
	m_fPixelWidth = 0.0f;
	m_fRealWidth = 0.0f;
	//}}AFX_DATA_INIT
	m_fRealRuler = 1.0f;	// ʵ����Ҫ�Ļ���������
}

void DlgRulerSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DlgRulerSet)
	DDX_Text(pDX, IDC_PIXEL_WIDTH, m_fPixelWidth);
	DDX_Text(pDX, IDC_REAL_WIDTH, m_fRealWidth);
	DDV_MinMaxFloat(pDX, m_fRealWidth, 10.f, 1.e+012f);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(DlgRulerSet, CDialog)
	//{{AFX_MSG_MAP(DlgRulerSet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgRulerSet message handlers

void DlgRulerSet::OnOK() 
{
	// �������õ�ǰͼ������ű����������ź��ͼ����Ϊ��ǰԭʼ�ߴ�
	CTilCanvas* pCanvas = GetCanvasMgr()->GetCurCanvas();
	if (pCanvas != NULL)
	{
		// ��ȡ�Ի�������
		UpdateData(TRUE);
		if (m_fRealWidth < 10)
		{
			GetDlgItem(IDC_REAL_WIDTH)->SetFocus();
			return;
		}
		m_fCurRuler = m_fPixelWidth/m_fRealWidth;

		// ����ͼ�񣬵��Ƕ���ͼ�β����д���
		long imgWidth, imgHeight;
		imgWidth = pCanvas->GetWidth();
		imgHeight = pCanvas->GetHeight();
		CImage* pImage = new CImage(*pCanvas->GetCurImage());
		pImage->Resample(imgWidth/m_fCurRuler,imgHeight/m_fCurRuler);
		pCanvas->PushSrcImage(pImage);

		// ����ͼ����ı��
		pCanvas->SetModifiedFlag(TRUE);

		// ������ͼ
		((CTilSysApp*)AfxGetApp())->GetDocument()->UpdateAllViews(NULL);
	}
	
	CDialog::OnOK();
}
