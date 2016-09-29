// TilCanvasMgr.h: interface for the CTilCanvasMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TILCANVASMGR_H__2459C0F4_34AB_47A5_A2C9_F5D9D3EEAB0C__INCLUDED_)
#define AFX_TILCANVASMGR_H__2459C0F4_34AB_47A5_A2C9_F5D9D3EEAB0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// 项目名称
typedef struct __tagPRJCAPTION
{
	CString strLineName;	// 线路名称
	CString strTunName;		// 隧道名称
	CString strPartName;	// 洞室名称
	CString strPartIdx;		// 洞室编号
} PRJCAPTION;

#include "TilCanvas.h"
class AFX_EXT_CLASS CTilCanvasMgr  
{
public:
	enum { ADD_SIZE = 100};
	CTilCanvasMgr();
	virtual ~CTilCanvasMgr();

	void New();
	BOOL Open(const CString& strProjName);

	// 只保存项目文件
	BOOL Save();
	BOOL SaveAs(const CString& strProjName);

	CString GetPathName();

	CTilCanvas* GetCanvasByIdx(int nIdx);
	CTilCanvas* GetFirstCanvas();
	CTilCanvas* GetLastCanvas();
	CTilCanvas* GetPrevCanvas();
	CTilCanvas* GetCurCanvas();
	CTilCanvas* GetNextCanvas();

	void SetCurCanvasIdx(int nIdx);
	int	 GetCurCanvasIdx();

	void SetZoomFactor(long double dbZoomX, long double dbZoomY);
	void GetZoomFactor(long double& dbZoomX, long double& dbZoomY);

	int Size();

	void SetModifiedFlag(BOOL bFlag = TRUE);
	BOOL IsModified();

	void RemoveAll();
	BOOL RemoveAt(int nIdx);

	BOOL PushFrontCanvas(CTilCanvas* pCanvas);
	BOOL PushBackCanvas(CTilCanvas* pCanvas);
	BOOL InsertCanvas(CTilCanvas* pCanvas, int nIdx, BOOL bAfter = TRUE);

	void SetShowHideLayerSrcImage(BOOL bLayerSrcImage = TRUE);
	void SetShowHideLayerCrackImage(BOOL bLayerCrackImage = TRUE);
	void SetShowHideLayerCrackObj(BOOL bLayerCrackObj = TRUE);

	BOOL IsShowLayerSrcImage()	{ return m_bLayerSrcImage; }
	BOOL IsShowLayerCrackImage(){ return m_bLayerCrackImage; }
	BOOL IsShowLayerCrackObj()	{ return m_bLayerCrackObj; }

	// 将序号为nIdx的画布放到nIdx2的前面或后面
	BOOL DragCanvasTo(int nIdx1, int nIdx2, BOOL bAfter=TRUE);
//	BOOL DragCanvasTo(int nIdx1, int nIdx2);

	// 按照里程大小自动排列画布
	void AutoArrayCanvas(BOOL bOrder);

public:
	// 当前隧道名称等信息
	PRJCAPTION m_PrjCap;	
	// 设置隧道属性
	void SetPrjInfo(PRJCAPTION& prjCap);
	// 获得隧道属性
	PRJCAPTION GetPrjInfo();
	
private:
	// 各图层的显示与隐藏标记
	BOOL m_bLayerSrcImage;		// 原始图像层
	BOOL m_bLayerCrackImage;	// 裂缝图像层
	BOOL m_bLayerCrackObj;		// 裂缝目标层

	BOOL m_bModified;		// 修改状态
	long double m_dbZoomX;	// 水平缩放比例尺
	long double m_dbZoomY;	// 垂直缩放比例尺
	int m_nIdx;			// 当前显示的画布序号
	int m_nNum;			// 画布数
	int m_nTotalNum;	// 分配的指针总数
	CTilCanvas** m_ppCanvas;

	CString m_strProjName;

	void clear_mem();
};

#endif // !defined(AFX_TILCANVASMGR_H__2459C0F4_34AB_47A5_A2C9_F5D9D3EEAB0C__INCLUDED_)
