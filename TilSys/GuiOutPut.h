#pragma once
#include "GUIExtStd.h"
#include "guicontrolbar.h"
#include "GuiTabWnd.h"
#include "LogEdit.h"
#include "ComboListCtrl.h"
#include "previewlist.h"
#include "FindCrack.h"
//#include "SortListCtrl.h"
#include "ListCtrlEx.h"
#include <vector>

using namespace::std;

// 设置“状态信息”分页窗口需要显示的打开文件相关信息
void LogInfo(const CString& str);
// 设置“图像属性”分页窗口需要显示的图像信息
void ImageInfo(int nWidth, int nHeight, int nDepth, const CString& strFileName);
// 设置“裂缝特征”子窗口中需要显示的裂缝特征信息
void CrackInfo(CRACK_MGR* pCrackMgr);

class TILEXTCLASS CGuiOutPut :	public CGuiControlBar
{
protected:
	// 分页子窗口窗口
	CGuiTabWnd	m_TabSolExplorer;
	//
	CEdit		m_EditFind;
	// 图元模式
	CPreviewList m_Pattern;	
	// "状态信息"子窗口
	static CLogInfo	m_EditOutput;
	// "图像属性"子窗口
	static CListCtrl m_ImgCaption;
	// "裂缝特征"子窗口
	static CListCtrlEx m_CrackInfo;

public:
	CGuiOutPut(void);
	virtual ~CGuiOutPut(void);

	// 设置“状态信息”分页窗口中打开文件时需要显示的相关信息
	friend void LogInfo(const CString& str);
	// 设置“图像属性”分页窗口中的需要显示的信息
	friend void ImageInfo(int nWidth, int nHeight, int nDepth, const CString& strFileName);
	// 设置“裂缝特征”子窗口中需要显示的裂缝特征信息
	friend void CrackInfo(CRACK_MGR* pCrackMgr);

	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

protected:
	// 设置子窗口的Tab页
	void set_tab_sol_explorer_info(void);
	// 设置图像子窗口的显示属性
	void set_image_caption_info(void);
	// 设置子窗口的Tab页中的"裂缝特征"显示
	void set_crack_character_info();
};
