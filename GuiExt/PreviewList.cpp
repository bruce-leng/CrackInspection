// PreviewList.cpp : implementation file
//

#include "stdafx.h"
#include "PreviewList.h"


#include <PROCESS.H>
#include "resource.h"
//#include "FolderFind.h"
#include "ximage.h"
#include "TilCanvas.h"
#include "GUIShellManager.h"
#include "BPnetwork.h"
#include "TilComFunction.h"

#include "extlib.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreviewList

//##ModelId=3FFE4BD2005E
CPreviewList::CPreviewList()
{
	m_bTerminate=false;
	m_bRunning=false;
	// Initialize Thread Handle and Create Event Handle for controlling thread
	m_hThread = NULL;
}

//##ModelId=3FFE4BD200EC
CPreviewList::~CPreviewList()
{
	TerminateThread();
}


BEGIN_MESSAGE_MAP(CPreviewList, CListCtrl)
	//{{AFX_MSG_MAP(CPreviewList)
	ON_COMMAND(ID_PREVIEWLIST_ADD_FILE, OnPreviewListAddFile)
	ON_COMMAND(ID_OPEN_DIR, OnOpenDir)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_COMMAND(ID_IMAGE_INFO, OnImageFileInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreviewList message handlers
//##ModelId=3FFE4BD2007D
BOOL CPreviewList::Initialize()
{
	m_ImageListThumb.Create(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT, ILC_COLOR24, 0, 1);
	SetImageList(&m_ImageListThumb, LVSIL_NORMAL);
	m_pfunLog = NULL;
	return TRUE;
}

//##ModelId=3FFE4BD20196
void CPreviewList::SetPathInfo(const CString& strCurrentDirectory, vector<CString>* vFileName)
{
//	m_strCurrentDirectory = strCurrentDirectory;
//	m_vFileName = vFileName;
}

//##ModelId=3FFE4BD2011C
void CPreviewList::SelectDirectory(LPCSTR pstr)
{
	m_strCurrentDirectory=pstr;
	if(m_strCurrentDirectory.Right(1) != '\\')
		m_strCurrentDirectory += "\\";

	m_vFileName.clear();

	CFileFind finder;
	CString strWildCard(m_strCurrentDirectory);
	strWildCard+="*.*";

	BOOL bWorking=finder.FindFile(strWildCard);

	while(bWorking)
	{
		bWorking=finder.FindNextFile();

		if(finder.IsDots() || finder.IsDirectory())
			continue;
		else
		{
			CString filePath=finder.GetFileName();

			// Get Image File Name List
			if (get_file_type(filePath) != CXIMAGE_FORMAT_UNKNOWN)
			{
				// Make Lower for Sorting
				filePath.MakeLower();
				m_vFileName.push_back(filePath);
			}
		}
	}
	finder.Close();

	DrawNail();
}

//##ModelId=3FFE4BD200EA
void CPreviewList::DrawNail()
{
	LoadThumbImages();
}

//##ModelId=3FFE4BD20157
unsigned __stdcall CPreviewList::LoadThumbNail(LPVOID lpParam)
{
	CPreviewList* ListCtrl=(CPreviewList*)lpParam;
	CImageList* pImgList=&(ListCtrl->m_ImageListThumb);

	// reset our image list
	for(int i=0; i<pImgList->GetImageCount(); i++)
		pImgList->Remove(i);	

	// remove all items from list view
	ListCtrl->DeleteAllItems();

	pImgList->SetImageCount(ListCtrl->m_vFileName.size());

	char path[MAX_PATH];
	vector<CString>::iterator iter;
	
	// Set redraw to FALSE to avoid flickering during adding new items
	ListCtrl->SetRedraw(FALSE);
	int nIndex=0;
	for(iter=ListCtrl->m_vFileName.begin(); iter!=ListCtrl->m_vFileName.end() && ListCtrl->m_bTerminate!=true; iter++, nIndex++)
	{
		ListCtrl->InsertItem(nIndex, *iter, nIndex);
	}

	ListCtrl->SetRedraw(TRUE);
	ListCtrl->Invalidate();

	// Create Brushes for Border and BackGround
	HBRUSH hBrushBorder=::CreateSolidBrush(RGB(192, 192, 192));
	HBRUSH hBrushBk=::CreateSolidBrush(RGB(255, 255, 255));

	// Border Size
	RECT rcBorder;
	rcBorder.left=rcBorder.top=0;
	rcBorder.right=THUMBNAIL_WIDTH;
	rcBorder.bottom=THUMBNAIL_HEIGHT;

	const float fRatio=(float)THUMBNAIL_HEIGHT/THUMBNAIL_WIDTH;

	int XDest, YDest, nDestWidth, nDestHeight;
	nIndex=0;
//	LogInfo("Reading...");

	if(ListCtrl->m_pfunLog)
		ListCtrl->m_pfunLog("Reading...");
	for(iter=ListCtrl->m_vFileName.begin(); iter!=ListCtrl->m_vFileName.end() && ListCtrl->m_bTerminate!=true; iter++, nIndex++)
	{
		// Load Image File
		sprintf(path, "%s\\%s", ListCtrl->m_strCurrentDirectory, *iter);
//		LogInfo(path);
		if(ListCtrl->m_pfunLog)
			ListCtrl->m_pfunLog(path);

		int nImageType = get_file_type(path);
		if (nImageType == CXIMAGE_FORMAT_UNKNOWN)
			continue;

//		CxImage image((*iter).pBuf, (*iter).lSize, 0);
		CxImage image(path, get_file_type(path));

		if(image.IsValid()==false)
			continue;

		// Calculate Rect to fit to canvas
		const float fImgRatio=(float)image.GetHeight()/image.GetWidth();
		if(fImgRatio > fRatio)
		{
			nDestWidth=THUMBNAIL_HEIGHT/fImgRatio;
			XDest=(THUMBNAIL_WIDTH-nDestWidth)/2;
			YDest=0;
			nDestHeight=THUMBNAIL_HEIGHT;
		}
		else
		{
			XDest=0;
			nDestWidth=THUMBNAIL_WIDTH;
			nDestHeight=THUMBNAIL_WIDTH*fImgRatio;
			YDest=(THUMBNAIL_HEIGHT-nDestHeight)/2;
		}

		CClientDC cdc(ListCtrl);
		HDC hDC=::CreateCompatibleDC(cdc.m_hDC);
		HBITMAP bm = CreateCompatibleBitmap(cdc.m_hDC, THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT);
		HBITMAP pOldBitmapImage = (HBITMAP)SelectObject(hDC,bm);
		// Draw Background
		::FillRect(hDC, &rcBorder, hBrushBk);

		// Draw Image
		image.Stretch(hDC, XDest, YDest, nDestWidth, nDestHeight);

		// Draw Border
		::FrameRect(hDC, &rcBorder, hBrushBorder);

		SelectObject(hDC, pOldBitmapImage);

		// Attach to Bitmap and Replace image in CImageList
		CBitmap bitmap;
		bitmap.Attach(bm);
		pImgList->Replace(nIndex, &bitmap, NULL);

		// Redraw only a current item for removing flickering and fast speed.
		ListCtrl->RedrawItems(nIndex, nIndex);

		// Release used DC and Object
		DeleteDC(hDC);
		DeleteObject(bm);
	}
	DeleteObject(hBrushBorder);
	DeleteObject(hBrushBk);

	ListCtrl->Invalidate();
	ListCtrl->m_bRunning=false;
	ListCtrl->m_bTerminate=false;
//	LogInfo("Done.");
	if(ListCtrl->m_pfunLog)
		ListCtrl->m_pfunLog("Done!");

	_endthreadex( 0 );


	return 0;
}

//##ModelId=3FFE4BD200DC
void CPreviewList::LoadThumbImages()
{
//	CThumbViewerDoc* pDoc = GetDocument();
//	ASSERT_VALID(pDoc);

//	CListCtrl& ListCtrl=GetListCtrl();

	TerminateThread();
//	LoadThumbNail((LPVOID)this);
	m_hThread = (HANDLE)_beginthreadex(NULL,0, LoadThumbNail,(LPVOID)this,0/* CREATE_SUSPENDED*/,&m_dwThreadID); 

	m_bRunning=true;
}

//##ModelId=3FFE4BD200EB
void CPreviewList::OpenNewDir()
{
	DeleteAllItems();

	// reset our image list
	for(int i=0; i<m_ImageListThumb.GetImageCount(); i++)
		m_ImageListThumb.Remove(i);
	m_ImageListThumb.SetImageCount(0);
}
//##ModelId=3FFE4BD200FA
void CPreviewList::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
//	CThumbViewerDoc* pDoc = GetDocument();
//	ASSERT_VALID(pDoc);

//	pDoc->SelectItem(pNMListView->iItem);
	*pResult = 0;
}

//##ModelId=3FFE4BD20129
BOOL CPreviewList::TerminateThread()
{
	if(m_bRunning==false)
		return TRUE;
	m_bTerminate=true;
	HANDLE hTemp = CreateEvent(NULL, FALSE, FALSE, NULL);

	::WaitForSingleObject(m_hThread, 2000);

	CloseHandle(hTemp);
	::CloseHandle(m_hThread);
	return TRUE;
}

//##ModelId=3FFE4BD200FE
void CPreviewList::OnPreviewListAddFile()
{
	if(AfxMessageBox("�Ƿ���ӵ���Ŀ����", MB_ICONQUESTION | MB_YESNO) == IDNO)
		return;
	
	HWND hWnd = ((CFrameWnd*)AfxGetMainWnd())->m_hWnd;//GetActiveView()->m_hWnd;
	
	int nIdx;
	POSITION pos = GetFirstSelectedItemPosition();
	
	while(pos!=NULL)   //CCP ADD LOOP
	{
		if (pos == NULL)
			TRACE0("No items were selected!\n");
		else
		{
			nIdx = GetNextSelectedItem(pos);
		}
		vector<CString>::iterator it;
		int n;
		for(it = m_vFileName.begin(),n=0; it != m_vFileName.end(),n < nIdx; ++it,n++);
		
		::SendMessage(AfxGetMainWnd()->m_hWnd, m_dwRevMessage,WPARAM(&(*it)),LPARAM(&m_strCurrentDirectory));
	}
}

//##ModelId=3FFE4BD2010A
void CPreviewList::OnOpenDir()//CCP ADD
{
//    CFolderFind dlg;
//	if(dlg.DoModal() == IDCANCEL)
//		return;

	CString strInit;
	strInit = AfxGetApp()->GetProfileString("Dir", "Browser", "C:\\");

//	static char BASED_CODE szFilter[] = "supported files|*.jpeg;*.jpg;*.bmp|Jpeg Files (*.jpeg,*.jpg)|*.jpeg;*.jpg|Bmp Files (*.bmp)|*.bmp|All Files (*.*)|*.*||";
//	CFileDialog dlg(TRUE, "*.*", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_FILEMUSTEXIST, szFilter, NULL);

	CGUIShellManager dirmgr;
	if(!dirmgr.BrowseForFolder (strInit,GetParent(),strInit,"���Ŀ¼"))
		return;

	AfxGetApp()->WriteProfileString("Dir", "Browser", strInit);
	SelectDirectory(strInit);
	//SelectDirectory(dlg.m_treeDir.GetSelectedDirectory());
}
///////////////////////////////////////////////////////////////

//##ModelId=3FFE4BD2010C
void CPreviewList::OnRclick(NMHDR* pNMHDR, LRESULT* pResult)
{
		CMenu menu;
		if(GetFirstSelectedItemPosition())
			VERIFY(menu.LoadMenu(IDR_POPUP_PREVIEW_LIST_EDIT_INFO));
		else
			VERIFY(menu.LoadMenu(CG_IDR_POPUP_PREVIEW_LIST));
		
		LPNMITEMACTIVATE lpnmmouse= (LPNMITEMACTIVATE) pNMHDR;
		CMenu* pPopup = menu.GetSubMenu(0);
		ASSERT(pPopup != NULL);
		CWnd* pWndPopupOwner = this;
		CPoint point = lpnmmouse->ptAction;
		ClientToScreen(&point);
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
			pWndPopupOwner);
}

void CPreviewList::OnImageFileInfo()
{
	AfxMessageBox("ͼ���ļ�����");
}

// ����ͼ��ָ��
void CPreviewList::AddImagePtr(CString strImageName, BYTE* pBuf, long lSize)
{
	IMAGELIST imageList;
	imageList.strImageName = strImageName;
	imageList.pBuf = pBuf;
	imageList.lSize = lSize;
	m_vImageList.push_back(imageList);
}

void CPreviewList::DelImagePtrAll()
{
	for (int i=0; i<m_vImageList.size(); i++)
	{
		delete [] m_vImageList[i].pBuf;
	}
	m_vImageList.clear();
}

