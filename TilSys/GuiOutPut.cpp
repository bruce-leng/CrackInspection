#include "StdAfx.h"
#include "guioutput.h"
#include "resource.h"


CLogInfo	CGuiOutPut::m_EditOutput;
CListCtrl CGuiOutPut::m_ImgCaption;
CListCtrlEx CGuiOutPut::m_CrackInfo;

CGuiOutPut::CGuiOutPut(void)
{
}

CGuiOutPut::~CGuiOutPut(void)
{
}
BEGIN_MESSAGE_MAP(CGuiOutPut, CGuiControlBar)
	ON_WM_CREATE()
END_MESSAGE_MAP()

int CGuiOutPut::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CGuiControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_TabSolExplorer.Create(WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,0x9999))
		return -1;

	DWORD dwStyleReport = LVS_REPORT|LVS_SHOWSELALWAYS|LVS_ALIGNTOP|LVS_SINGLESEL|LVS_AUTOARRANGE;
	if (!m_ImgCaption.Create(dwStyleReport,CRect(0,0,0,0), &m_TabSolExplorer, 2))
		return -1;

	dwStyleReport |= LVS_EDITLABELS;
	if (!m_CrackInfo.Create(dwStyleReport,CRect(0,0,0,0), &m_TabSolExplorer, 2))
		return -1;

	DWORD dwStyleListBox = LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;
	if (!m_EditOutput.Create(dwStyleListBox,CRect(0,0,0,0), &m_TabSolExplorer, 2))
		return -1;

	/*
	if (!m_EditFind.Create(WS_CHILD|WS_VISIBLE,CRect(0,0,0,0), &m_TabSolExplorer, 2))
		return -1;
	*/

	/*if (!m_Pattern.Create(WS_CHILD|WS_VISIBLE,CRect(0,0,0,0), &m_TabSolExplorer, 2))
		return -1;
	m_Pattern.Initialize();
	*/

	set_tab_sol_explorer_info();
	set_image_caption_info();
	set_crack_character_info();

	return 0;
}

void CGuiOutPut::set_tab_sol_explorer_info(void)
{
	m_TabSolExplorer.SetImageList(IDB_DBPROJECTS, 16,8, RGB (255, 0, 0));
	m_TabSolExplorer.Addtab(&m_EditOutput,"状态信息",1);
	m_TabSolExplorer.Addtab(&m_ImgCaption,"图像属性",2);
	m_TabSolExplorer.Addtab(&m_CrackInfo,"裂缝特征",3);
}

void CGuiOutPut::set_image_caption_info(void)
{
	m_ImgCaption.InsertColumn(0, "属性");
	m_ImgCaption.InsertColumn(1, "参数");
	m_ImgCaption.InsertItem(0, "宽度");
	m_ImgCaption.InsertItem(1, "高度");
	m_ImgCaption.InsertItem(2, "颜色");
	m_ImgCaption.InsertItem(3, "文件名");

	m_ImgCaption.SetColumnWidth(0, -1);
	m_ImgCaption.SetColumnWidth(1, -1);
}

void CGuiOutPut::set_crack_character_info()
{
	m_CrackInfo.InsertColumn(0,"裂缝编号", LVCFMT_CENTER, 80);
//	m_CrackInfo.InsertColumn(1,"裂缝颜色", LVCFMT_CENTER, 100);
	m_CrackInfo.InsertColumn(2,"最大宽度(mm)", LVCFMT_CENTER, 100);
	m_CrackInfo.InsertColumn(3,"长度(mm)", LVCFMT_CENTER, 100);
	m_CrackInfo.InsertColumn(4,"面积(mm2)", LVCFMT_CENTER, 100);
	m_CrackInfo.InsertColumn(5,"长度/面积", LVCFMT_CENTER, 100);
	m_CrackInfo.InsertColumn(6,"裂缝灰度均值", LVCFMT_CENTER, 100);
	m_CrackInfo.InsertColumn(7,"裂缝灰度方差", LVCFMT_CENTER, 100);

	m_CrackInfo.SetExtendedStyle(LVS_EX_FULLROWSELECT);
}

// 设置“状态信息”分页窗口需要显示的打开文件相关信息
void LogInfo(const CString& str)
{
	CGuiOutPut::m_EditOutput.AddString(str);
	int nCount = CGuiOutPut::m_EditOutput.GetCount();
	if (nCount > 0)
		CGuiOutPut::m_EditOutput.SetCurSel(nCount-1);
}

// 设置“图像属性”分页窗口需要显示的图像信息
void ImageInfo(int nWidth, int nHeight, int nDepth, const CString& strFileName)
{
	CString str;
	str.Format("%d", nWidth);
	CGuiOutPut::m_ImgCaption.SetItemText(0, 1, str);

	str.Format("%d", nHeight);
	CGuiOutPut::m_ImgCaption.SetItemText(1, 1, str);

	str.Format("%d位", nDepth);
	CGuiOutPut::m_ImgCaption.SetItemText(2, 1, str);

	CGuiOutPut::m_ImgCaption.SetItemText(3, 1, strFileName);

	CGuiOutPut::m_ImgCaption.SetColumnWidth(0, -1);
	CGuiOutPut::m_ImgCaption.SetColumnWidth(1, -1);
}

// 设置“裂缝特征”分页窗口中需要显示的裂缝特征信息
void CrackInfo(CRACK_MGR* pCrackMgr)
{
	// 清除以前显示的所有内容
	CGuiOutPut::m_CrackInfo.DeleteAllItems();
	
	if (pCrackMgr == NULL)
		return;

	CString str;

	int nNum = pCrackMgr->nNum;
	CRACK* pCrack = pCrackMgr->elem;
	for (int i=0; i<nNum; i++)
	{
		str.Format("%d", i+1);
		CGuiOutPut::m_CrackInfo.InsertItem(i, str);
// 		// 裂缝颜色
// 		str = "------";
// 		CGuiOutPut::m_CrackInfo.SetItemText(i, 1, str);
		// 裂缝最大宽度（预留）（单位：mm）
		str.Format("%4.1f", pCrack[i].fWmax);
		CGuiOutPut::m_CrackInfo.SetItemText(i, 1, str);
		// 裂缝长度（单位：mm）
		str.Format("%6.1f", pCrack[i].fLength);
		CGuiOutPut::m_CrackInfo.SetItemText(i, 2, str);
		// 裂缝面积（单位：mm2）
		str.Format("%8.1f", pCrack[i].fArea);
		CGuiOutPut::m_CrackInfo.SetItemText(i, 3, str);
		// 裂缝长度与面积之比(fCircLen/fArea)
		if (pCrack[i].fArea < 0.0000001)
			str = "不存在";
		else
			str.Format("%3.1f", pCrack[i].fLength/pCrack[i].fArea);
		CGuiOutPut::m_CrackInfo.SetItemText(i, 4, str);
		// 裂缝区域灰度均值
		str.Format("%4.1f", pCrack[i].fGave);
		CGuiOutPut::m_CrackInfo.SetItemText(i, 5, str);
		// 裂缝区域灰度方差
		str.Format("%5.1f", pCrack[i].fGvari);
		CGuiOutPut::m_CrackInfo.SetItemText(i, 6, str);
	}
}
