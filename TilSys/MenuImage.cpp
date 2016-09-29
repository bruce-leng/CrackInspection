#include "StdAfx.h"
#include "TilSys.h"
#include "TilSysDoc.h"
#include "TilCanvas.h"
#include "Image.h"
#include "DlgThreshold.h"
#include "DlgPointStre.h"
#include "ximage.h"
#include "TilComFunction.h"
#include "FindCrack.h"
#include "TilSysView.h"
#include <process.h>

// 图像阈值分割
void CTilSysDoc::OnImageThreshold() 
{
	BeginWaitCursor();

	// 添加一个新裂缝图像层用于预览显示
	CTilCanvas* pCanvas = GetCanvasMgr()->GetCurCanvas();
	CImage* pCurImage = pCanvas->GetCurImage();
	CImage* pNewImage = new CImage(*pCurImage);
	pCanvas->PushCrackImage(pNewImage);

	DlgThreshold dlg;
	dlg.m_pDoc = this;
	if (dlg.DoModal() == IDCANCEL)
	{
		pCanvas->PopCrackImage();
		UpdateAllViews(NULL);
	}

	EndWaitCursor();
}

void CTilSysDoc::OnImageLighten() 
{
	m_lMenuCommand = ID_IMAGE_LIGHTEN;
	m_hThread = (HANDLE)_beginthread(RunImageThread, 0, this);
}

void CTilSysDoc::OnImageDarken() 
{
	m_lMenuCommand = ID_IMAGE_DARKEN;
	m_hThread = (HANDLE)_beginthread(RunImageThread, 0, this);
}

void CTilSysDoc::OnImageContrast() 
{
	m_lMenuCommand = ID_IMAGE_CONTRAST;
	m_hThread = (HANDLE)_beginthread(RunImageThread, 0, this);
}

void CTilSysDoc::OnImageLesscontrast() 
{
	m_lMenuCommand = ID_IMAGE_LESSCONTRAST;
	m_hThread = (HANDLE)_beginthread(RunImageThread, 0, this);
}

void CTilSysDoc::OnImageColorize() 
{
	m_lMenuCommand = ID_IMAGE_COLORIZE;
	m_hThread = (HANDLE)_beginthread(RunImageThread, 0, this);
}

void CTilSysDoc::OnImageNormalize() 
{
	m_lMenuCommand = ID_IMAGE_NORMALIZE;
	m_hThread = (HANDLE)_beginthread(RunImageThread, 0, this);
}

void CTilSysDoc::OnImageStretch() 
{
	// TODO: Add your command handler code here
	// 循环变量
	int i;
	
	// 在原始图像堆栈中新增一个原始图像层用于显示灰度折线变换效果，若后面取消灰度折线变换处理结果，则弹出该新增图
	CTilCanvas* pCanvas = GetCanvasMgr()->GetCurCanvas();
	CImage* pCurImage = pCanvas->GetCurImage();
	CImage* pNewImage = new CImage(*pCurImage);
	pCanvas->PushSrcImage(pNewImage);
	
	// 各颜色分量的灰度分布密度
	long lGray[256];
	pNewImage->Histogram(NULL,NULL,NULL,lGray,0);
	
	long lArea = pNewImage->GetWidth()*pNewImage->GetHeight();
	
	// 创建对话框
	CDlgPointStre dlg;
	dlg.m_pDoc = this;
	
	// 传递灰度分布密度数据给面板类
	for (i = 0; i <256; i++)
		dlg.m_fIntensity[i] = lGray[i]*1.0/lArea;
	
	// 显示对话框，由用户进行灰度折线变换
	if (dlg.DoModal() == IDCANCEL)
	{
		pCanvas->PopSrcImage();
		UpdateAllViews(NULL);
	}
}

void CTilSysDoc::OnImageMedian() 
{
	m_lMenuCommand = ID_IMAGE_MEDIAN;
	m_hThread = (HANDLE)_beginthread(RunImageThread, 0, this);
}

void CTilSysDoc::OnImageBlur() 
{
	m_lMenuCommand = ID_IMAGE_BLUR;
	m_hThread = (HANDLE)_beginthread(RunImageThread, 0, this);
}

void CTilSysDoc::OnImageSoften() 
{
	m_lMenuCommand = ID_IMAGE_SOFTEN;
	m_hThread = (HANDLE)_beginthread(RunImageThread, 0, this);
}

void CTilSysDoc::OnImageGaussian3x3() 
{
	m_lMenuCommand = ID_IMAGE_GAUSSIAN3X3;
	m_hThread = (HANDLE)_beginthread(RunImageThread, 0, this);
}

void CTilSysDoc::OnImageGaussian5x5() 
{
	m_lMenuCommand = ID_IMAGE_GAUSSIAN5X5;
	m_hThread = (HANDLE)_beginthread(RunImageThread, 0, this);
}

void CTilSysDoc::OnImageSharpen() 
{
	m_lMenuCommand = ID_IMAGE_SHARPEN;
	m_hThread = (HANDLE)_beginthread(RunImageThread, 0, this);
}

void CTilSysDoc::OnImageUsm() 
{
	m_lMenuCommand = ID_IMAGE_USM;
	m_hThread = (HANDLE)_beginthread(RunImageThread, 0, this);
}

void CTilSysDoc::OnImageNegative() 
{
	m_lMenuCommand = ID_IMAGE_NEGATIVE;
	m_hThread = (HANDLE)_beginthread(RunImageThread, 0, this);
}

void CTilSysDoc::OnImageErode() 
{
	m_lMenuCommand = ID_IMAGE_ERODE;
	m_hThread = (HANDLE)_beginthread(RunImageThread, 0, this);
}

// 图像腐蚀
void CTilSysDoc::OnImageDilate() 
{
	m_lMenuCommand = ID_IMAGE_DILATE;
	m_hThread = (HANDLE)_beginthread(RunImageThread, 0, this);
}

void CTilSysDoc::OnImageThin() 
{
	m_lMenuCommand = ID_IMAGE_THIN;
	m_hThread = (HANDLE)_beginthread(RunImageThread, 0, this);
}


// 识别裂缝并裂缝特征放于列表中
void CTilSysDoc::OnImageFindCracks() 
{
	m_lMenuCommand = ID_IMAGE_FIND_CRACKS;
	m_hThread = (HANDLE)_beginthread(RunImageThread, 0, this);
}



void CTilSysDoc::OnUpdateImageLighten(CCmdUI* pCmdUI) 
{
	if (GetCanvasMgr()->GetCurCanvas() == NULL || m_hThread
		|| GetCanvasMgr()->GetCurCanvas()->GetCurLayerIdx() > 0)
		pCmdUI->Enable(0);
}

void CTilSysDoc::OnUpdateImageDarken(CCmdUI* pCmdUI) 
{
	if (GetCanvasMgr()->GetCurCanvas() == NULL || m_hThread
		|| GetCanvasMgr()->GetCurCanvas()->GetCurLayerIdx() > 0)
		pCmdUI->Enable(0);
}

void CTilSysDoc::OnUpdateImageContrast(CCmdUI* pCmdUI) 
{
	if (GetCanvasMgr()->GetCurCanvas() == NULL || m_hThread
		|| GetCanvasMgr()->GetCurCanvas()->GetCurLayerIdx() > 0)
		pCmdUI->Enable(0);
}

void CTilSysDoc::OnUpdateImageLesscontrast(CCmdUI* pCmdUI) 
{
	if (GetCanvasMgr()->GetCurCanvas() == NULL || m_hThread
		|| GetCanvasMgr()->GetCurCanvas()->GetCurLayerIdx() > 0)
		pCmdUI->Enable(0);
}

void CTilSysDoc::OnUpdateImageColorize(CCmdUI* pCmdUI) 
{
	if (GetCanvasMgr()->GetCurCanvas() == NULL || m_hThread
		|| GetCanvasMgr()->GetCurCanvas()->GetCurLayerIdx() > 0)
		pCmdUI->Enable(0);
}

void CTilSysDoc::OnUpdateImageNormalize(CCmdUI* pCmdUI) 
{
	if (GetCanvasMgr()->GetCurCanvas() == NULL || m_hThread
		|| GetCanvasMgr()->GetCurCanvas()->GetCurLayerIdx() > 0)
		pCmdUI->Enable(0);
}

void CTilSysDoc::OnUpdateImageStretch(CCmdUI* pCmdUI) 
{
	if (GetCanvasMgr()->GetCurCanvas() == NULL || m_hThread
		|| GetCanvasMgr()->GetCurCanvas()->GetCurLayerIdx() > 0)
		pCmdUI->Enable(0);
}

void CTilSysDoc::OnUpdateImageMedian(CCmdUI* pCmdUI) 
{
	if (GetCanvasMgr()->GetCurCanvas() == NULL || m_hThread
		|| GetCanvasMgr()->GetCurCanvas()->GetCurLayerIdx() > 0)
		pCmdUI->Enable(0);
}

void CTilSysDoc::OnUpdateImageBlur(CCmdUI* pCmdUI) 
{
	if (GetCanvasMgr()->GetCurCanvas() == NULL || m_hThread
		|| GetCanvasMgr()->GetCurCanvas()->GetCurLayerIdx() > 0)
		pCmdUI->Enable(0);
}

void CTilSysDoc::OnUpdateImageSoften(CCmdUI* pCmdUI) 
{
	if (GetCanvasMgr()->GetCurCanvas() == NULL || m_hThread
		|| GetCanvasMgr()->GetCurCanvas()->GetCurLayerIdx() > 0)
		pCmdUI->Enable(0);
}

void CTilSysDoc::OnUpdateImageGaussian3x3(CCmdUI* pCmdUI) 
{
	if (GetCanvasMgr()->GetCurCanvas() == NULL || m_hThread
		|| GetCanvasMgr()->GetCurCanvas()->GetCurLayerIdx() > 0)
		pCmdUI->Enable(0);
}

void CTilSysDoc::OnUpdateImageGaussian5x5(CCmdUI* pCmdUI) 
{
	if (GetCanvasMgr()->GetCurCanvas() == NULL || m_hThread
		|| GetCanvasMgr()->GetCurCanvas()->GetCurLayerIdx() > 0)
		pCmdUI->Enable(0);
}

void CTilSysDoc::OnUpdateImageSharpen(CCmdUI* pCmdUI) 
{
	if (GetCanvasMgr()->GetCurCanvas() == NULL || m_hThread
		|| GetCanvasMgr()->GetCurCanvas()->GetCurLayerIdx() > 0)
		pCmdUI->Enable(0);
}

void CTilSysDoc::OnUpdateImageUsm(CCmdUI* pCmdUI) 
{
	if (GetCanvasMgr()->GetCurCanvas() == NULL || m_hThread
		|| GetCanvasMgr()->GetCurCanvas()->GetCurLayerIdx() == 2)
		pCmdUI->Enable(0);
}

void CTilSysDoc::OnUpdateImageNegative(CCmdUI* pCmdUI) 
{
	if (GetCanvasMgr()->GetCurCanvas() == NULL || m_hThread
		|| GetCanvasMgr()->GetCurCanvas()->GetCurLayerIdx() == 2)
		pCmdUI->Enable(0);
}

void CTilSysDoc::OnUpdateImageErode(CCmdUI* pCmdUI) 
{
	if (GetCanvasMgr()->GetCurCanvas() == NULL || m_hThread
		|| GetCanvasMgr()->GetCurCanvas()->GetCurLayerIdx() == 2)
		pCmdUI->Enable(0);
}

void CTilSysDoc::OnUpdateImageDilate(CCmdUI* pCmdUI) 
{
	if (GetCanvasMgr()->GetCurCanvas() == NULL || m_hThread
		|| GetCanvasMgr()->GetCurCanvas()->GetCurLayerIdx() == 2)
		pCmdUI->Enable(0);
}

void CTilSysDoc::OnUpdateImageThreshold(CCmdUI* pCmdUI) 
{
	if (GetCanvasMgr()->GetCurCanvas() == NULL || m_hThread
		|| GetCanvasMgr()->GetCurCanvas()->GetCurLayerIdx() == 2)
		pCmdUI->Enable(0);
}

void CTilSysDoc::OnUpdateImageThin(CCmdUI* pCmdUI) 
{
	if (GetCanvasMgr()->GetCurCanvas() == NULL || m_hThread
		|| GetCanvasMgr()->GetCurCanvas()->GetCurLayerIdx() == 2)
		pCmdUI->Enable(0);
}

void CTilSysDoc::OnUpdateImageFindCracks(CCmdUI* pCmdUI) 
{
	if (GetCanvasMgr()->GetCurCanvas() == NULL || m_hThread
		|| GetCanvasMgr()->GetCurCanvas()->GetCurLayerIdx() == 2)
		pCmdUI->Enable(0);
}
