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

// ���á�״̬��Ϣ����ҳ������Ҫ��ʾ�Ĵ��ļ������Ϣ
void LogInfo(const CString& str);
// ���á�ͼ�����ԡ���ҳ������Ҫ��ʾ��ͼ����Ϣ
void ImageInfo(int nWidth, int nHeight, int nDepth, const CString& strFileName);
// ���á��ѷ��������Ӵ�������Ҫ��ʾ���ѷ�������Ϣ
void CrackInfo(CRACK_MGR* pCrackMgr);

class TILEXTCLASS CGuiOutPut :	public CGuiControlBar
{
protected:
	// ��ҳ�Ӵ��ڴ���
	CGuiTabWnd	m_TabSolExplorer;
	//
	CEdit		m_EditFind;
	// ͼԪģʽ
	CPreviewList m_Pattern;	
	// "״̬��Ϣ"�Ӵ���
	static CLogInfo	m_EditOutput;
	// "ͼ������"�Ӵ���
	static CListCtrl m_ImgCaption;
	// "�ѷ�����"�Ӵ���
	static CListCtrlEx m_CrackInfo;

public:
	CGuiOutPut(void);
	virtual ~CGuiOutPut(void);

	// ���á�״̬��Ϣ����ҳ�����д��ļ�ʱ��Ҫ��ʾ�������Ϣ
	friend void LogInfo(const CString& str);
	// ���á�ͼ�����ԡ���ҳ�����е���Ҫ��ʾ����Ϣ
	friend void ImageInfo(int nWidth, int nHeight, int nDepth, const CString& strFileName);
	// ���á��ѷ��������Ӵ�������Ҫ��ʾ���ѷ�������Ϣ
	friend void CrackInfo(CRACK_MGR* pCrackMgr);

	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

protected:
	// �����Ӵ��ڵ�Tabҳ
	void set_tab_sol_explorer_info(void);
	// ����ͼ���Ӵ��ڵ���ʾ����
	void set_image_caption_info(void);
	// �����Ӵ��ڵ�Tabҳ�е�"�ѷ�����"��ʾ
	void set_crack_character_info();
};
