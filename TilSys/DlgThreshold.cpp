// DlgThreshold.cpp : implementation file
//

#include "stdafx.h"
#include "TilSys.h"
#include "DlgThreshold.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// DlgThreshold dialog


DlgThreshold::DlgThreshold(CWnd* pParent /*=NULL*/)
	: CDialog(DlgThreshold::IDD, pParent)
{
	//{{AFX_DATA_INIT(DlgThreshold)
	m_level = 128;
	m_bPreview = FALSE;
	//}}AFX_DATA_INIT
}

void DlgThreshold::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DlgThreshold)
	DDX_Control(pDX, IDOK, m_ok);
	DDX_Control(pDX, IDCANCEL, m_canc);
	DDX_Text(pDX, IDC_EDIT1, m_level);
	DDV_MinMaxByte(pDX, m_level, 0, 255);
	DDX_Check(pDX, IDC_CHECK1, m_bPreview);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DlgThreshold, CDialog)
	//{{AFX_MSG_MAP(DlgThreshold)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_THRESHOLD_MINUS, OnThresholdMinus)
	ON_BN_CLICKED(IDC_THRESHOLD_PLUS, OnThresholdPlus)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER2, OnReleasedcaptureSlider2)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER2, OnCustomdrawSlider2)
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgThreshold message handlers

BOOL DlgThreshold::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER2);
	pSlider->SetRange(0, 255);
	pSlider->SetPos(128);

	m_pCurImage = new CImage(*GetCanvasMgr()->GetCurCanvas()->GetCurImage());
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void DlgThreshold::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
}

void DlgThreshold::OnThresholdMinus() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_level --;
	if (m_level == 254)
		GetDlgItem(IDC_THRESHOLD_PLUS)->EnableWindow(TRUE);
	else if (m_level <= 0)
		GetDlgItem(IDC_THRESHOLD_MINUS)->EnableWindow(FALSE);

	UpdateData(FALSE);

	if (m_bPreview)
	{
		// 更新主视图中的显示
		CImage* pImage = GetCanvasMgr()->GetCurCanvas()->GetCurImage();
		pImage->Copy(*m_pCurImage);
		pImage->ThresholdRGB(m_level);
		
		m_pDoc->SetModifiedFlag(TRUE);
		m_pDoc->UpdateAllViews(NULL);
	}
}

void DlgThreshold::OnThresholdPlus() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_level ++;
	if (m_level == 1)
		GetDlgItem(IDC_THRESHOLD_MINUS)->EnableWindow(TRUE);
	else if (m_level >= 255)
		GetDlgItem(IDC_THRESHOLD_PLUS)->EnableWindow(FALSE);

	UpdateData(FALSE);

	if (m_bPreview)
	{
		// 更新主视图中的显示
		CImage* pImage = GetCanvasMgr()->GetCurCanvas()->GetCurImage();
		pImage->Copy(*m_pCurImage);
		pImage->ThresholdRGB(m_level);
		
		m_pDoc->SetModifiedFlag(TRUE);
		m_pDoc->UpdateAllViews(NULL);
	}
}

void DlgThreshold::OnOK() 
{
	CTilCanvas* pCanvas = GetCanvasMgr()->GetCurCanvas();
	pCanvas->SetModifiedFlag(TRUE);

	// 更新主视图中的显示
	if (!m_bPreview)
	{
		CImage* pImage = pCanvas->GetCurImage();
		pImage->ThresholdRGB(m_level);
		
		m_pDoc->SetModifiedFlag(TRUE);
		m_pDoc->UpdateAllViews(NULL);
	}

	delete m_pCurImage;

	CDialog::OnOK();
}

void DlgThreshold::OnCancel() 
{
	// TODO: Add extra cleanup here
	CImage* pImage = GetCanvasMgr()->GetCurCanvas()->GetCurImage();
	pImage->Copy(*m_pCurImage);

// 	m_pDoc->SetModifiedFlag(FALSE);
	m_pDoc->UpdateAllViews(NULL);

	delete m_pCurImage;
	
	CDialog::OnCancel();
}

void DlgThreshold::OnReleasedcaptureSlider2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER2);
	m_level = pSlider->GetPos();

// 	if (m_bPreview)
// 	{
// 		// 更新主视图中的显示
// 		CImage* pImage = GetCanvasMgr()->GetCurCanvas()->GetCurImage();
// 		pImage->Copy(*m_pCurImage);
// 		pImage->ThresholdRGB(m_level);
// 		
// 		m_pDoc->SetModifiedFlag(TRUE);
// 		m_pDoc->UpdateAllViews(FALSE);
// 	}
	

	*pResult = 0;
}

void DlgThreshold::OnCustomdrawSlider2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER2);
	m_level = pSlider->GetPos();
	
	UpdateData(FALSE);
	
	if (m_bPreview)
	{
		// 更新主视图中的显示
		CImage* pImage = GetCanvasMgr()->GetCurCanvas()->GetCurImage();
		pImage->Copy(*m_pCurImage);
		pImage->ThresholdRGB(m_level);
		
		m_pDoc->SetModifiedFlag(TRUE);
		m_pDoc->UpdateAllViews(NULL);
	}

	*pResult = 0;
}

void DlgThreshold::OnCheck1() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	CImage* pImage = GetCanvasMgr()->GetCurCanvas()->GetCurImage();
	pImage->Copy(*m_pCurImage);

	if (m_bPreview)
	{
		// 更新主视图中的显示
		pImage->ThresholdRGB(m_level);
	}

	m_pDoc->SetModifiedFlag(TRUE);
	m_pDoc->UpdateAllViews(NULL);
}
