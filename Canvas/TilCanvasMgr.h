// TilCanvasMgr.h: interface for the CTilCanvasMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TILCANVASMGR_H__2459C0F4_34AB_47A5_A2C9_F5D9D3EEAB0C__INCLUDED_)
#define AFX_TILCANVASMGR_H__2459C0F4_34AB_47A5_A2C9_F5D9D3EEAB0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// ��Ŀ����
typedef struct __tagPRJCAPTION
{
	CString strLineName;	// ��·����
	CString strTunName;		// �������
	CString strPartName;	// ��������
	CString strPartIdx;		// ���ұ��
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

	// ֻ������Ŀ�ļ�
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

	// �����ΪnIdx�Ļ����ŵ�nIdx2��ǰ������
	BOOL DragCanvasTo(int nIdx1, int nIdx2, BOOL bAfter=TRUE);
//	BOOL DragCanvasTo(int nIdx1, int nIdx2);

	// ������̴�С�Զ����л���
	void AutoArrayCanvas(BOOL bOrder);

public:
	// ��ǰ������Ƶ���Ϣ
	PRJCAPTION m_PrjCap;	
	// �����������
	void SetPrjInfo(PRJCAPTION& prjCap);
	// ����������
	PRJCAPTION GetPrjInfo();
	
private:
	// ��ͼ�����ʾ�����ر��
	BOOL m_bLayerSrcImage;		// ԭʼͼ���
	BOOL m_bLayerCrackImage;	// �ѷ�ͼ���
	BOOL m_bLayerCrackObj;		// �ѷ�Ŀ���

	BOOL m_bModified;		// �޸�״̬
	long double m_dbZoomX;	// ˮƽ���ű�����
	long double m_dbZoomY;	// ��ֱ���ű�����
	int m_nIdx;			// ��ǰ��ʾ�Ļ������
	int m_nNum;			// ������
	int m_nTotalNum;	// �����ָ������
	CTilCanvas** m_ppCanvas;

	CString m_strProjName;

	void clear_mem();
};

#endif // !defined(AFX_TILCANVASMGR_H__2459C0F4_34AB_47A5_A2C9_F5D9D3EEAB0C__INCLUDED_)
