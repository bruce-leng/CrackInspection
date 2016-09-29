// TilImage.h: interface for the CTilCanvas class.
//
//////////////////////////////////////////////////////////////////////

/************************************
  REVISION LOG ENTRY
  ά����Ա: Leng
  ά��ʱ�� 2014-6-5 7:36:44
  ˵��: ����������������һ��ԭʼͼ����һ��ͼ��Ŀ��㣬
        ��ͼ��Ŀ��㲻����ʱ��ֻ��ʾԭʼͼ��㣬����ͼ��Ŀ�����ʾ�������档
		��������ɺ�ͼ��Ŀ���Ϊ��ɸѡ�γɵ��γ̱����ֵͼ��
 ************************************/


#if !defined(AFX_TILIMAGE_H__7D71C271_2D20_4A26_938B_241593EB1C7A__INCLUDED_)
#define AFX_TILIMAGE_H__7D71C271_2D20_4A26_938B_241593EB1C7A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Image.h"
#include "FindCrack.h"
#include <vector>
using namespace std;

ENUM_CXIMAGE_FORMATS AFX_EXT_CLASS get_file_type(const CString& strPathName);

class CImage;
class AFX_EXT_CLASS CTilCanvas
{
public:
	CTilCanvas();
	CTilCanvas(CTilCanvas& canvas);
	virtual ~CTilCanvas();

	void ClearMem();

	// ���ļ�����ԭʼͼ�񣬴������ѷ�ͼ��������ѷ���γ̱����ļ���
	BOOL Open(const CString& strFilePath, const CString& strFileName);
	BOOL Open(CTilCanvas& canvas);
	void SaveAll();
	BOOL ReadCrackObject(CString strPathFile);
	BOOL SaveCrackObject(CString strPathFile);

	// ���������ļ�·��
	void SetFilePath(CString strFilePath) {  m_strFilePath = strFilePath; }
	CString GetFilePath() { return m_strFilePath; }

	// ���������ļ���
	void SetFileName(CString strFileName) { m_strFileName = strFileName; }
	CString GetFileName() { return m_strFileName; }

	void SetModifiedFlag(BOOL bFlag = TRUE);
	BOOL IsModified() const;

 	void SetZoomFactor(long double dbZoomX, long double dbZoomY);
 	void GetZoomFactor(long double& dbZoomX, long double& dbZoomY);

	// ������ͼ�����ʾ������
	void SetShowHideLayerSrcImage(BOOL bLayerSrcImage = TRUE) { m_bLayerSrcImage = bLayerSrcImage; }
	void SetShowHideLayerCrackImage(BOOL bLayerCrackImage = TRUE) { m_bLayerCrackImage = bLayerCrackImage; }
	void SetShowHideLayerCrackObj(BOOL bLayerCrackObj = TRUE)	{ m_bLayerCrackObj = bLayerCrackObj; }

	BOOL IsShowLayerSrcImage() { return m_bLayerSrcImage; }
	BOOL IsShowLayerCrackImage() { return m_LayerCrackImage.size()>0 && m_bLayerCrackImage; }
	BOOL IsShowLayerCrackObj() { return m_pCracksObject!=NULL && m_bLayerCrackObj; }

	// ��õ�ǰͼ�����(0: ԭʼͼ��㣬1: �ѷ�ͼ���  2: �ѷ�Ŀ���)
	int GetCurLayerIdx() { return m_nMark; }

	// ���õ�ǰ��m_nMark��m_nIdx
	void SetCurLayerIdx(BOOL bNext = TRUE);

	// ��õ�ǰ��ʾ�����ͼ�㣨������Ŀ��㣩
	CImage* GetCurImage();

	void Draw(CDC* pDC);

	// ѹ��ԭʼ��ͼ��
	void PushSrcImage(CImage* pImage);
	// ѹ���ѷ��ͼ��
	void PushCrackImage(CImage* pImage);
	// ����ԭʼ��ͼ��
	void PopSrcImage(int nBackNum = 1);
	// ����Ŀ���ͼ��
	void PopCrackImage(int nBackNum = 1);
	// �������ԭʼ��ͼ��
	CImage* GetLastSrcImage();
	// �������Ŀ����ѷ�ͼ��
	CImage* GetLastCrackImage();

	// ���ظ����롰������
	void Undo();
	BOOL CanUndo() { return m_bCanUndo; }
	void Redo();
	BOOL CanRedo() { return m_bCanRedo; }

	int GetWidth();
	int GetHeight();
	int GetDepth();

	// ʶ���ѷ�
	void FindCrack();
	// �Ѿ�ʶ����ѷ�����
	BOOL IsFindCrack() { return m_pCracksObject != NULL; }
	// ���Ŀ���γ̱���
	ObjRunLenCode* GetCrackObject() { return m_pCracksObject; }
	// �����ʶ����ѷ������
	CRACK_MGR* GetCrackCharacter() { return m_pCracksCharacter; }
	// ���ñ�ѡ�е��ѷ�Ŀ��
	void SetSelectedObject(int nSelected) { m_nSelected = nSelected; }
	// ɾ���ѷ�
	void DelSelectedObject();

	// ɾ����ǰm_nMark��m_nIdx�����ͼ��Ŀ��㣨��Undo���ٴ�����ͼ��ʱ���в�����
	void DeleteFilesAfterCurLayerIdx();
	// ɾ������Ŀ���
	void DeleteObjLayer();
	// ɾ���ѷ�ͼ���
	void DeleteCrackLayer();

private:
	//////////////////////////////////////////////////////////////////////////
	// ���������������ڿ��ƶ�ͼ�������ġ��������͡��ظ�������
	// ��ǰ�������ս�������ǡ�ԭʼͼ�񡱡����ѷ�Ŀ�ꡱ�������յġ��ѷ�ʶ�������ı��
	// 0:ԭʼͼ��1:�ѷ�Ŀ�꣬2:�ѷ�ʶ����
	int m_nMark;
	// ��ǰ��ʾ��ͼ�����(����m_nMark�����ǡ�ԭʼͼ�񡱡����ѷ�Ŀ�ꡱ���ǡ��ѷ�ʶ�������ĵ�ǰ��ʾ���)
	int m_nIdx;
	// ����Ƿ��ܽ���Undo����
	BOOL m_bCanUndo;
	// ����Ƿ��ܽ���Redo����
	BOOL m_bCanRedo;
	//////////////////////////////////////////////////////////////////////////

 	long double m_dbZoomX;		// ����ͼ���ˮƽ������
 	long double m_dbZoomY;		// ����ͼ��Ĵ�ֱ������
	BOOL m_bModified;			// ͼ���޸ĵı��
	BOOL m_bLayerSrcImage;		// ԭʼͼ����ʾ���
	BOOL m_bLayerCrackImage;	// �ѷ�ͼ����ʾ���
	BOOL m_bLayerCrackObj;		// �ѷ�Ŀ�����ʾ���
	CString m_strFilePath;		// �ļ�·�����������һ���»���'\'��
	CString m_strFileName;		// �ļ����ƣ�������չ����

	// �ѷ�ʶ����
	ObjRunLenCode* m_pCracksObject;
	// �ѷ�ʶ�����б�ѡ�е��ѷ�Ŀ��
	int m_nSelected;
	// ���в��ҵ����ѷ�����
	CRACK_MGR* m_pCracksCharacter;
	// ͼ�����У����ڴ洢�����Ԥ����ͼ��
	vector<CImage*> m_LayerSrcImage;
	// ͼ�����У����ڴ洢����Ķ�ֵ�ָʶ���Ŀ��ͼ��
	vector<CImage*> m_LayerCrackImage;
};

#endif // !defined(AFX_TILIMAGE_H__7D71C271_2D20_4A26_938B_241593EB1C7A__INCLUDED_)
