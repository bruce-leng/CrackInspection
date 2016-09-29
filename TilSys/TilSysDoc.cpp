// TilSysDoc.cpp : implementation of the CTilSysDoc class
//

#include "stdafx.h"
#include "TilSys.h"
#include "TilSysDoc.h"
#include "MainFrm.h"
#include <process.h>

#include "TilCanvas.h"
#include "TilComFunction.h"
#include "Image.h"
#include "MathComput.h"
#include "TilSysView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 图像预处理线程
void /*unsigned long _stdcall*/ RunImageThread(void* pParam)
{
	CTilSysDoc* pDoc = (CTilSysDoc*)pParam;

	CTilCanvasMgr* pMgr = GetCanvasMgr();
	CTilCanvas* pCanvas = pMgr->GetCurCanvas();
	CImage* pImage = NULL;
	if (pDoc->m_lMenuCommand >= ID_IMAGE_LIGHTEN && pDoc->m_lMenuCommand <= ID_IMAGE_THIN)
		pImage = new CImage(*pCanvas->GetCurImage());
	else
		pImage = pCanvas->GetCurImage();
	
	switch (pDoc->m_lMenuCommand)
	{
	case ID_VIEW_LAYER_SRC_IMAGE:
	case ID_VIEW_LAYER_CRACK_IMAGE:
	case ID_VIEW_LAYER_CRACK_OBJ:
		break;
	case ID_VIEW_ZOOMIN: // 放大
		{
			long double dbZoomX, dbZoomY;
			pMgr->GetZoomFactor(dbZoomX, dbZoomY);
			pMgr->SetZoomFactor(dbZoomX*1.25, dbZoomY*1.25);
			
			// 设置滑动条范围
			SendMessage(pDoc->m_hWnd, WM_SIZE, 0, 0);
			break;
		}
	case ID_VIEW_ZOOMOUT: // 缩小
		{
			long double dbZoomX, dbZoomY;
			pMgr->GetZoomFactor(dbZoomX, dbZoomY);
			pMgr->SetZoomFactor(dbZoomX*0.8, dbZoomY*0.8);
			
			// 设置滑动条范围
			SendMessage(pDoc->m_hWnd, WM_SIZE, 0, 0);
			break;
		}
	case ID_VIEW_ZOOMNORMAL: // 原始大小
		pMgr->SetZoomFactor(1.0, 1.0);
		
		// 设置滑动条范围
		SendMessage(pDoc->m_hWnd, WM_SIZE, 0, 0);
		break;
	case ID_IMAGE_LIGHTEN:	
		pImage->Light(20);
		break;
	case ID_IMAGE_DARKEN:
		pImage->Light(-20);
		break;
	case ID_IMAGE_CONTRAST:
		pImage->Light(0, 25);
		break;
	case ID_IMAGE_LESSCONTRAST:
		pImage->Light(0, -25);
		break;
	case ID_IMAGE_COLORIZE:
		pImage->HistogramEqualize();
		break;
	case ID_IMAGE_NORMALIZE:
		pImage->HistogramNormalize();
		break;
	// case ID_IMAGE_STRETCH:
	case ID_IMAGE_MEDIAN:
		pImage->Median();
		break;
	case ID_IMAGE_BLUR:
		{
			long kernel[]={1,1,1,1,8,1,1,1,1};
			pImage->Filter(kernel,3,16,0);
			break;
		}
	case ID_IMAGE_SOFTEN:
		{
			long kernel[]={1,1,1,1,1,1,1,1,1};
			pImage->Filter(kernel,3,9,0);
			break;
		}
	case ID_IMAGE_GAUSSIAN3X3:
		{
			long kernel[]={1,2,1,2,4,2,1,2,1};
			pImage->Filter(kernel,3,16,0);
			break;
		}
	case ID_IMAGE_GAUSSIAN5X5:
		{
			long kernel[]={0,1,2,1,0,1,3,4,3,1,2,4,8,4,2,1,3,4,3,1,0,1,2,1,0};
			pImage->Filter(kernel,5,52,0);
			break;
		}
	case ID_IMAGE_SHARPEN:
		{
			long kernel[]={-1,-1,-1,-1,9,-1,-1,-1,-1};
			pImage->Filter(kernel,3,1,0);
			break;
		}
	case ID_IMAGE_USM:
		pImage->UnsharpMask();
		break;
	case ID_IMAGE_NEGATIVE:
		pImage->Negative();
		break;
	case ID_IMAGE_ERODE:
		pImage->Erode();
		break;
	case ID_IMAGE_DILATE:
		pImage->Dilate();
		break;
	//case ID_IMAGE_THRESHOLD:
	case ID_IMAGE_THIN:
		pImage->MorphThining();
		break;
	case ID_IMAGE_FIND_CRACKS:
		pCanvas->FindCrack();
		CrackInfo(pCanvas->GetCrackCharacter());
		break;
	}

	if (pDoc->m_lMenuCommand >= ID_IMAGE_LIGHTEN && pDoc->m_lMenuCommand <= ID_IMAGE_SHARPEN)
		pCanvas->PushSrcImage(pImage);
	else if (pDoc->m_lMenuCommand >= ID_IMAGE_USM && pDoc->m_lMenuCommand <= ID_IMAGE_THIN)
		pCanvas->PushCrackImage(pImage);

	// -1表示无被选中的裂缝目标
	SendMessage(pDoc->m_hWnd, WM_USER_NEWIMAGE,-1,0);

	pDoc->m_hThread=0;
	_endthread();
}

/////////////////////////////////////////////////////////////////////////////
// CTilSysDoc

IMPLEMENT_DYNCREATE(CTilSysDoc, CDocument)

BEGIN_MESSAGE_MAP(CTilSysDoc, CDocument)
	//{{AFX_MSG_MAP(CTilSysDoc)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_IMAGE_DILATE, OnImageDilate)
	ON_COMMAND(ID_IMAGE_THRESHOLD, OnImageThreshold)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_DILATE, OnUpdateImageDilate)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_THRESHOLD, OnUpdateImageThreshold)
	ON_COMMAND(ID_VIEW_ZOOMIN, OnViewZoomin)
	ON_COMMAND(ID_VIEW_ZOOMOUT, OnViewZoomout)
	ON_COMMAND(ID_IMAGE_NEGATIVE, OnImageNegative)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMIN, OnUpdateViewZoomin)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMOUT, OnUpdateViewZoomout)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_NEGATIVE, OnUpdateImageNegative)
	ON_COMMAND(ID_IMAGE_COLORIZE, OnImageColorize)
	ON_COMMAND(ID_IMAGE_LIGHTEN, OnImageLighten)
	ON_COMMAND(ID_IMAGE_DARKEN, OnImageDarken)
	ON_COMMAND(ID_IMAGE_CONTRAST, OnImageContrast)
	ON_COMMAND(ID_IMAGE_LESSCONTRAST, OnImageLesscontrast)
	ON_COMMAND(ID_IMAGE_MEDIAN, OnImageMedian)
	ON_COMMAND(ID_IMAGE_ERODE, OnImageErode)
	ON_COMMAND(ID_IMAGE_BLUR, OnImageBlur)
	ON_COMMAND(ID_IMAGE_SOFTEN, OnImageSoften)
	ON_COMMAND(ID_IMAGE_GAUSSIAN3X3, OnImageGaussian3x3)
	ON_COMMAND(ID_IMAGE_GAUSSIAN5X5, OnImageGaussian5x5)
	ON_COMMAND(ID_IMAGE_SHARPEN, OnImageSharpen)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_COLORIZE, OnUpdateImageColorize)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_LIGHTEN, OnUpdateImageLighten)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_DARKEN, OnUpdateImageDarken)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_CONTRAST, OnUpdateImageContrast)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_LESSCONTRAST, OnUpdateImageLesscontrast)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_MEDIAN, OnUpdateImageMedian)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_ERODE, OnUpdateImageErode)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_BLUR, OnUpdateImageBlur)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_SOFTEN, OnUpdateImageSoften)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_GAUSSIAN3X3, OnUpdateImageGaussian3x3)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_GAUSSIAN5X5, OnUpdateImageGaussian5x5)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_SHARPEN, OnUpdateImageSharpen)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMNORMAL, OnUpdateViewZoomnormal)
	ON_COMMAND(ID_VIEW_ZOOMNORMAL, OnViewZoomnormal)
	ON_COMMAND(ID_FILE_RESUME, OnFileResume)
	ON_UPDATE_COMMAND_UI(ID_FILE_RESUME, OnUpdateFileResume)
	ON_COMMAND(ID_IMAGE_STRETCH, OnImageStretch)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_STRETCH, OnUpdateImageStretch)
	ON_COMMAND(ID_IMAGE_NORMALIZE, OnImageNormalize)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_NORMALIZE, OnUpdateImageNormalize)
	ON_COMMAND(ID_IMAGE_USM, OnImageUsm)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_USM, OnUpdateImageUsm)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_COMMAND(ID_IMAGE_FIND_CRACKS, OnImageFindCracks)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_FIND_CRACKS, OnUpdateImageFindCracks)
	ON_COMMAND(ID_VIEW_LAYER_SRC_IMAGE, OnViewLayerSrcImage)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LAYER_SRC_IMAGE, OnUpdateViewLayerSrcImage)
	ON_COMMAND(ID_VIEW_LAYER_CRACK_IMAGE, OnViewLayerCrackImage)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LAYER_CRACK_IMAGE, OnUpdateViewLayerCrackImage)
	ON_COMMAND(ID_VIEW_LAYER_CRACK_OBJ, OnViewLayerCrackObj)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LAYER_CRACK_OBJ, OnUpdateViewLayerCrackObj)
	ON_COMMAND(ID_IMAGE_THIN, OnImageThin)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_THIN, OnUpdateImageThin)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTilSysDoc construction/destruction

CTilSysDoc::CTilSysDoc()
{
	m_hThread = NULL;
	m_lMenuCommand = 0;
	m_hWnd = NULL;
}

CTilSysDoc::~CTilSysDoc()
{
	//INVALID_HANDLE_VALUE
	if (m_hThread)
	{
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTilSysDoc serialization

void CTilSysDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTilSysDoc diagnostics

#ifdef _DEBUG
void CTilSysDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTilSysDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTilSysDoc commands


// 打开多个文件并添加到当前项目中
void CTilSysDoc::OnFileOpen() 
{
	CFileDialog dlgFile(TRUE);
	CString title="打开切片文件";
	CString strInitialDir = AfxGetApp()->GetProfileString("Dir", "Browser");
	CString strFilter = ((CTilSysApp*)AfxGetApp())->GetFileTypes(TRUE);

	char buf[100000];
	memset(buf,NULL,100000);

	dlgFile.m_ofn.Flags |= OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT;
	dlgFile.m_ofn.nFilterIndex = 0;
	dlgFile.m_ofn.lpstrTitle = title;
	dlgFile.m_ofn.lpstrInitialDir = strInitialDir;
	dlgFile.m_ofn.lpstrFilter = strFilter;
	dlgFile.m_ofn.nMaxFile = 100000;
	dlgFile.m_ofn.lpstrFile = buf;

	BOOL bRet = FALSE;;
	if (dlgFile.DoModal() == IDOK)
		bRet = TRUE;

	CTilCanvasMgr* pMgr = GetCanvasMgr();
	if (bRet)
	{
		POSITION pos = dlgFile.GetStartPosition(); 
		while (pos != NULL) 
		{
			// 如果原来的项目中有相同的文件，则不添加该文件
			CString str = dlgFile.GetNextPathName(pos);
			for (int i = 0; i < pMgr->Size(); i ++)
			{
				CString strFileName = pMgr->GetCanvasByIdx(i)->GetFileName();
				if (get_file_name(str) == strFileName)
					break;
			}
			if (i < pMgr->Size())
			{
				LogInfo("切片文件 "+str+" 已经存在!");
				continue;
			}
			else
			{
				CTilCanvas* pCanvas = new CTilCanvas();
				pCanvas->Open(get_file_path(str), get_file_name(str));
				if (pMgr->PushBackCanvas(pCanvas))
				{
					LogInfo("装载图像文件 "+str+" 成功!");
				}
			}
		}
	}

	AfxGetApp()->WriteProfileString("Dir", "Browser", dlgFile.GetPathName());

	UpdateDrawTree();
}

// 将文件还原到上次保存状态
void CTilSysDoc::OnFileResume() 
{
	// TODO: Add your command handler code here
	BeginWaitCursor();

	CTilCanvas* pCanvas = GetCanvasMgr()->GetCurCanvas();
	long double dbRatioX;
	pCanvas->GetZoomFactor(dbRatioX, dbRatioX);

// 	CString strImage = pCanvas->GetImage()->GetPathName();
// 	CString strGraph = pCanvas->GetGraph()->GetPathName();
// 
// 	pCanvas->Open(strImage, strGraph);
// 	pCanvas->SetZoomFactor(dbRatioX, dbRatioX);
// 	pCanvas->SetModifiedFlag(FALSE);

	UpdateAllViews(NULL);
	EndWaitCursor();
}

void CTilSysDoc::OnUpdateFileResume(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CTilCanvas* pCanvas = GetCanvasMgr()->GetCurCanvas();
	if (pCanvas == NULL)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(pCanvas->IsModified());
}

// 保存当前切片的图形和图像文件
void CTilSysDoc::OnFileSave() 
{
	CTilCanvas* pCanvas = GetCanvasMgr()->GetCurCanvas();
	if (!pCanvas)
		return;

	if (pCanvas->IsModified())
		pCanvas->SaveAll();

	pCanvas->SetModifiedFlag(FALSE);
/*
	CFileDialog dlgFile(FALSE);
	CString title;
	CString strInitialDir = AfxGetApp()->GetProfileString("Dir", "Browser");
	CString strFilter = ((CTilSysApp*)AfxGetApp())->GetFileTypes(FALSE);

	dlgFile.m_ofn.Flags |= OFN_OVERWRITEPROMPT;
	dlgFile.m_ofn.nFilterIndex = 0;
	dlgFile.m_ofn.lpstrInitialDir = strInitialDir;
	dlgFile.m_ofn.lpstrFilter = strFilter;

	// 保存图像文件
	title="保存切片图像文件";
	dlgFile.m_ofn.lpstrTitle = title;
	if (pCanvas->GetImage()->GetPathName().IsEmpty())
	{
		if (dlgFile.DoModal() == IDOK)
		{
			CString strPathName = dlgFile.GetPathName();
			pCanvas->GetImage()->SaveAs(strPathName);
			AfxGetApp()->WriteProfileString("Dir", "Browser", strPathName);
		}
	}
	else
		pCanvas->GetImage()->Save();

	// 保存图形文件
	strInitialDir = AfxGetApp()->GetProfileString("Dir", "Browser");
	dlgFile.m_ofn.lpstrInitialDir = strInitialDir;
	title = "保存切片图形文件";
	dlgFile.m_ofn.lpstrTitle = title;
	if (pCanvas->GetGraph()->GetPathName().IsEmpty())
	{
		if (dlgFile.DoModal() == IDOK)
		{
			CString strPathName = dlgFile.GetPathName();
			pCanvas->GetGraph()->SaveAs(strPathName);
			AfxGetApp()->WriteProfileString("Dir", "Browser", strPathName);
		}
	}
	else
		pCanvas->GetGraph()->Save();
*/
}

void CTilSysDoc::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CTilCanvas* pCanvas = GetCanvasMgr()->GetCurCanvas();
	if (pCanvas != NULL)
	{
		pCmdUI->Enable(pCanvas->IsModified());
	}
	else
		pCmdUI->Enable(FALSE);
}

BOOL CTilSysDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	GetCanvasMgr()->RemoveAll();
	UpdateDrawTree();

	SetTitle(_T("未命名"));
//	// (SDI documents will reuse this document)
//	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
//	pFrame->InsertProjToSolExplorer(GetTitle());

	return TRUE;
}

// 打开"*.stu"项目文件
BOOL CTilSysDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	CString strFileInfo;
	strFileInfo.Format("正在读取文件 %s", lpszPathName);
	LogInfo(strFileInfo);

	// 打开项目文件并装载切片列表
	LogInfo("装载文件列表...");
	GetCanvasMgr()->Open(lpszPathName);
	LogInfo("文件列表装载完成...");

	// 设置当前默认图形层为节理层
//	((CMainFrame*)AfxGetMainWnd())->m_ComboCurLayer.SetCurSel(GRAPH_LAYER_JOINT);
//	GetCanvasMgr()->SetZoomFactor(2.0,2.0);
	CTilCanvasMgr* pMgr = GetCanvasMgr();

	UpdateDrawTree();

	SetModifiedFlag(FALSE);     // start off with unmodified

	return TRUE;
}

void CTilSysDoc::UpdateDrawTree()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->InsertProjToSolExplorer(GetCanvasMgr());
}

void CTilSysDoc::OnEditUndo() 
{
	// TODO: Add your command handler code here
	GetCanvasMgr()->GetCurCanvas()->Undo();
	UpdateAllViews(NULL);
}

void CTilSysDoc::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
	CTilCanvas* pCanvas = GetCanvasMgr()->GetCurCanvas();
	if (pCanvas == NULL || m_hThread || !pCanvas->CanUndo())
		pCmdUI->Enable(0);
}

void CTilSysDoc::OnEditRedo() 
{
	// TODO: Add your command handler code here
	GetCanvasMgr()->GetCurCanvas()->Redo();
	UpdateAllViews(NULL);
}

void CTilSysDoc::OnUpdateEditRedo(CCmdUI* pCmdUI) 
{
	CTilCanvas* pCanvas = GetCanvasMgr()->GetCurCanvas();
	if (pCanvas == NULL || m_hThread || !pCanvas->CanRedo())
		pCmdUI->Enable(0);
}

void CTilSysDoc::OnViewLayerSrcImage() 
{
	// TODO: Add your command handler code here
	CTilCanvasMgr* pMgr = GetCanvasMgr();
	BOOL bShowHide = pMgr->GetCurCanvas()->IsShowLayerSrcImage();
	pMgr->SetShowHideLayerSrcImage(!bShowHide);	

	UpdateAllViews(NULL);
}

void CTilSysDoc::OnUpdateViewLayerSrcImage(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CTilCanvasMgr* pMgr = GetCanvasMgr();
	if (pMgr->GetCurCanvas() == NULL || m_hThread)
		pCmdUI->Enable(0);
	else
		pCmdUI->SetCheck(pMgr->IsShowLayerSrcImage());

}

void CTilSysDoc::OnViewLayerCrackImage() 
{
	CTilCanvasMgr* pMgr = GetCanvasMgr();
	BOOL bShowHide = pMgr->GetCurCanvas()->IsShowLayerCrackImage();
	pMgr->SetShowHideLayerCrackImage(!bShowHide);	

	UpdateAllViews(NULL);
}

void CTilSysDoc::OnUpdateViewLayerCrackImage(CCmdUI* pCmdUI) 
{
	CTilCanvasMgr* pMgr = GetCanvasMgr();
	if (pMgr->GetCurCanvas() == NULL || m_hThread)
		pCmdUI->Enable(0);
	else
		pCmdUI->SetCheck(pMgr->IsShowLayerCrackImage());
}

void CTilSysDoc::OnViewLayerCrackObj() 
{
	CTilCanvasMgr* pMgr = GetCanvasMgr();
	BOOL bShowHide = pMgr->GetCurCanvas()->IsShowLayerCrackObj();
	pMgr->SetShowHideLayerCrackObj(!bShowHide);	

	UpdateAllViews(NULL);
}

void CTilSysDoc::OnUpdateViewLayerCrackObj(CCmdUI* pCmdUI) 
{
	CTilCanvasMgr* pMgr = GetCanvasMgr();
	if (pMgr->GetCurCanvas() == NULL || m_hThread)
		pCmdUI->Enable(0);
	else
		pCmdUI->SetCheck(pMgr->IsShowLayerCrackObj());
}
