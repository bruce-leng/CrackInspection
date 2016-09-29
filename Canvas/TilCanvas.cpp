// TilImage.cpp: implementation of the CTilCanvas class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TilCanvas.h"

#include "TilComFunction.h"
#include "MathComput.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const double PI =  3.14159265359;

ENUM_CXIMAGE_FORMATS get_file_type(const CString& strPathName)
{
	CString strExt = get_file_ext(strPathName);
	strExt.MakeLower();

	if (strExt == "bmp" || strExt == EXT_IMAGE)
#if CXIMAGE_SUPPORT_BMP
		return CXIMAGE_FORMAT_BMP;
#endif
#if CXIMAGE_SUPPORT_GIF
	else if(strExt == "gif")
		return CXIMAGE_FORMAT_GIF;
#endif
#if CXIMAGE_SUPPORT_JPG
	else if(strExt == "jpg")
		return CXIMAGE_FORMAT_JPG;
#endif
	else
		return CXIMAGE_FORMAT_UNKNOWN;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTilCanvas::CTilCanvas()
{
	m_nMark = 0;
	m_nIdx = -1;
	m_bModified = FALSE;
	m_bLayerSrcImage = TRUE;
	m_bLayerCrackImage = TRUE;
	m_bLayerCrackObj = TRUE;
 	m_dbZoomX = m_dbZoomY = 1.0;
	m_pCracksCharacter = NULL;
	m_pCracksObject = NULL;
	m_nSelected = -1;
	m_bCanUndo = FALSE;
	m_bCanRedo = FALSE;
}

CTilCanvas::CTilCanvas(CTilCanvas& canvas)
{
	Open(canvas);
}

CTilCanvas::~CTilCanvas()
{
	ClearMem();
}

void CTilCanvas::ClearMem()
{
	int i, nSize;
	nSize = m_LayerSrcImage.size();
	for (i = 0; i < nSize; i ++)
		delete m_LayerSrcImage.at(i);
	m_LayerSrcImage.clear();
	
	nSize = m_LayerCrackImage.size();
	for (i = 0; i < nSize; i++)
		delete m_LayerCrackImage.at(i);
	m_LayerCrackImage.clear();
	
	if (m_pCracksObject != NULL)
		delete m_pCracksObject;

	if (m_pCracksCharacter != NULL)
		delete m_pCracksCharacter;
}

// const CString& strFilePath : �ļ�·��
// const CString& strFileName : �ļ�����������׺��
BOOL CTilCanvas::Open(const CString& strFilePath, const CString& strFileName)
{
	ASSERT(!strFilePath.IsEmpty());

	m_strFilePath = strFilePath;
	m_strFileName = strFileName;

	CFileFind ff;

	// ��ȡԭʼͼ���ļ�
	CString strImageFile = strFilePath + '\\' + strFileName + ".src";
	// ���ԭʼ.srcͼ�񲻴���,��ֱ�Ӵ�jpg�ļ�
	if (!ff.FindFile(strImageFile))
		strImageFile = strFilePath + '\\' + strFileName + ".jpg";
	CImage* pSrc = new CImage(strImageFile, CXIMAGE_FORMAT_JPG);
	PushSrcImage(pSrc);
	m_nMark = 0;
	m_nIdx = 0;

	// �Ȳ鿴��û���ѷ�Ŀ���ļ����������ȡ��ȡ������ȡ�ѷ�ͼ���ļ�
	CString strCrackObj = strFilePath + '\\' + strFileName + ".obj";
	if (ff.FindFile(strCrackObj))
	{
		if (ReadCrackObject(strCrackObj))
		{
			m_nMark = 2;
			m_nIdx = 0;
		}
	}
	else
	{
		// ���ѷ�Ŀ���ļ������ڣ����ȡ�ѷ�ͼ���ļ�
		CString strCrackFile = strFilePath + '\\' + strFileName + ".crk";
		if (ff.FindFile(strCrackFile))
		{
			CImage* pCrack = new CImage(strCrackFile, CXIMAGE_FORMAT_BMP);
			PushCrackImage(pCrack);
			
			m_nMark = 1;
			m_nIdx = 0;
		}
	}

	return TRUE;
}

BOOL CTilCanvas::Open(CTilCanvas& canvas)
{
	m_nIdx = canvas.m_nIdx;
	m_bModified = canvas.m_bModified;
	m_bLayerSrcImage = canvas.m_bLayerSrcImage;
	m_bLayerCrackImage = canvas.m_bLayerCrackImage;
	m_bLayerCrackObj = canvas.m_bLayerCrackObj;
	m_dbZoomX = canvas.m_dbZoomX;
	m_dbZoomY = canvas.m_dbZoomY;

	// ��ԭʼͼ��㿽��������������m_LayerSrcImage�����һ�����ݣ�������Ϊԭʼͼ��
	int nSize = canvas.m_LayerSrcImage.size();
	if (nSize > 0)
	{
		CImage* pImage = (CImage*)canvas.m_LayerSrcImage.at(nSize-1);
		CImage* pTemp = new CImage;
		pTemp->Copy(*pImage);
		
		long double dbZoomX, dbZoomY;
		pImage->GetZoomFactor(dbZoomX, dbZoomY);
		pTemp->SetZoomFactor(dbZoomX, dbZoomY);
		
		m_LayerSrcImage.push_back(pTemp);
	}

	// ���ѷ�ͼ��㿽��������������m_LayerCrackImage�����һ�����ݣ�������Ϊԭʼͼ��
	nSize = canvas.m_LayerCrackImage.size();
	if (nSize > 0)
	{
		CImage* pImage = (CImage*)canvas.m_LayerCrackImage.at(nSize-1);
		CImage* pTemp = new CImage;
		pTemp->Copy(*pImage);
		
		long double dbZoomX, dbZoomY;
		pImage->GetZoomFactor(dbZoomX, dbZoomY);
		pTemp->SetZoomFactor(dbZoomX, dbZoomY);
		
		m_LayerSrcImage.push_back(pTemp);
	}

	// ��Ŀ��ͼ�㿽������
	if (canvas.m_pCracksObject != NULL)
	{
		m_pCracksObject = new ObjRunLenCode(*m_pCracksObject);
	}

	return TRUE;
}

void CTilCanvas::SaveAll()
{
	CString strPathFile = m_strFilePath + '\\' + m_strFileName;

	// �����ǰ��ʾͼ��Ϊԭʼͼ���
	if (m_nMark == 0)
	{
		// ������m_LayerSrcImage�ĵ�m_nIdx�����ݣ�������Ϊԭʼͼ��
		CImage* pSrc = (CImage*)m_LayerSrcImage.at(m_nIdx);
		// if (pSrc->IsModified())
		pSrc->Save(strPathFile + ".src", CXIMAGE_FORMAT_JPG);
	}
	else if (m_nMark == 1)
	{
		int nSize = m_LayerSrcImage.size();
		CImage* pSrc = (CImage*)m_LayerSrcImage.at(nSize-1);
		// if (pSrc->IsModified())
		pSrc->Save(strPathFile + ".src", CXIMAGE_FORMAT_JPG);

		nSize = m_LayerCrackImage.size();
		CImage* pCrack = (CImage*)m_LayerCrackImage.at(m_nIdx);
		pCrack->Save(strPathFile + ".crk", CXIMAGE_FORMAT_BMP);
	}
	else
	{
		int nSize = m_LayerSrcImage.size();
		CImage* pSrc = (CImage*)m_LayerSrcImage.at(nSize-1);
		// if (pSrc->IsModified())
		pSrc->Save(strPathFile + ".src", CXIMAGE_FORMAT_JPG);

		// ���ԭ�����ѷ�ͼ��㣬����Ȼ���棬ֻ���´�ֻ���ѷ�Ŀ��㣬�������ѷ�ͼ���
		nSize = m_LayerCrackImage.size();
		if (nSize > 0)
		{
			CImage* pCrack = (CImage*)m_LayerCrackImage.at(nSize-1);
			pCrack->Save(strPathFile + ".crk", CXIMAGE_SUPPORT_BMP);
		}
		
		SaveCrackObject(strPathFile + ".obj");
	}
}

BOOL CTilCanvas::ReadCrackObject(CString strPathFile)
{
	CFile file;
	if (file.Open(strPathFile, CFile::modeRead))
	{
		CArchive ar(&file, CArchive::load);

		// ��ȡ�ѷ�ͼ������
		if (m_pCracksObject != NULL)
			delete m_pCracksObject;
		m_pCracksObject = new ObjRunLenCode;
		m_pCracksObject->Serial(ar);

		// ��ȡ�ѷ���������
		int nNum = m_pCracksObject->GetObjNum();
		if (m_pCracksCharacter != NULL)
			delete [] (char*)m_pCracksCharacter;
		m_pCracksCharacter = (CRACK_MGR*)new char [sizeof(int) + nNum * sizeof(CRACK)];
		m_pCracksCharacter->nNum = nNum;
		CRACK* pCrack = m_pCracksCharacter->elem;
		for (int i=0; i<nNum; i++)
		{
			ar >> pCrack[i].fWmax >> pCrack[i].fLength >> pCrack[i].fArea;
			ar >> pCrack[i].fGave >> pCrack[i].fGvari;
		}

		ar.Close();
		file.Close();

		return TRUE;
	}

	return FALSE;
}

BOOL CTilCanvas::SaveCrackObject(CString strPathFile)
{
	if (m_pCracksObject == NULL)
		return FALSE;

	CFile file;
	if (file.Open(strPathFile, CFile::modeCreate | CFile::modeWrite))
	{
		CArchive ar(&file, CArchive::store);

		// �����ѷ�ͼ������
		m_pCracksObject->Serial(ar);
		
		// �����ѷ���������
		if (m_pCracksCharacter)
		{
			int nNum = m_pCracksCharacter->nNum;
			CRACK* pCrack = m_pCracksCharacter->elem;
			for (int i=0; i<nNum; i++)
			{
				ar << pCrack[i].fWmax << pCrack[i].fLength << pCrack[i].fArea;
				ar << pCrack[i].fGave << pCrack[i].fGvari;
			}
		}

		ar.Close();
		file.Close();

		return TRUE;
	}

	return FALSE;
}

void CTilCanvas::SetModifiedFlag(BOOL bFlag)
{
	m_bModified = bFlag;
}

BOOL CTilCanvas::IsModified()  const
{
	return m_bModified;
}

void CTilCanvas::Draw(CDC* pDC)
{
	if (m_nMark == 0)	// �����ǰ���Ϊԭʼͼ���
	{
		// ԭʼͼ������
		if (m_bLayerSrcImage)
		{
			// ���ԭʼͼ���
			CImage* pImage = (CImage*)m_LayerSrcImage.at(m_nIdx);
			pImage->Draw(pDC);
		}
	}
	else if (m_nMark == 1)	// �����ǰ���Ϊ�ѷ촦���
	{
		if (m_bLayerCrackImage)
		{
			CImage* pCrackImage = (CImage*)m_LayerCrackImage.at(m_nIdx);
			pCrackImage->Draw(pDC);
		}
		else if (m_bLayerSrcImage)
		{// ���ѷ�ͼ�㲻����ʱ���ж��Ƿ���ʾԭʼͼ���
			int nSize = m_LayerSrcImage.size();
			CImage* pImage = (CImage*)m_LayerSrcImage.at(nSize-1);
			pImage->Draw(pDC);
		}
	}
	else // if (m_nMark == 2)	// �����ǰ���ΪĿ���ѷ��
	{
		if (m_bLayerSrcImage)
		{
			// ���ԭʼͼ���
			int nSize = m_LayerSrcImage.size();
			CImage* pImage = (CImage*)m_LayerSrcImage.at(nSize-1);
			pImage->Draw(pDC);
		}

		// ����ѷ�Ŀ���
		if (m_bLayerCrackObj)
		{
			m_pCracksObject->Draw(pDC, GetHeight());
			m_pCracksObject->SetObjColor(m_nSelected, pDC, GetHeight(), RGB(0, 255, 255));
		}
	}
}

// ѹ��ԭʼ��ͼ��
void CTilCanvas::PushSrcImage(CImage* pImage)
{
	// ѹ��ͼ���ǰ��ɾ��m_nIdx�����ͼ������
	DeleteFilesAfterCurLayerIdx();
	// ѹͼ��ͼ������
	m_LayerSrcImage.push_back(pImage);
	// �������õ�ǰ��ʾ��ͼ���m_nIdx
	SetCurLayerIdx(TRUE);
	// �����޸�����
	m_bModified = TRUE;
	// ����Undo��Redo״̬
	if (m_LayerSrcImage.size() > 1) // ��һ��ѹ��ԭʼͼ��ʱ��Undo״̬��ΪFALSE
		m_bCanUndo = TRUE;
	m_bCanRedo = FALSE;
}

// ѹ���ѷ��ͼ��
void CTilCanvas::PushCrackImage(CImage* pImage)
{
	// ѹ��ͼ���ǰ��ɾ��m_nIdx�����ͼ������
	DeleteFilesAfterCurLayerIdx();
	// ѹ�ѷ�ͼ������
	m_LayerCrackImage.push_back(pImage);
	// �������õ�ǰ��ʾ��ͼ���m_nIdx
	SetCurLayerIdx(TRUE);
	// �����޸�����
	m_bModified = TRUE;
	// ����Undo��Redo״̬
	m_bCanUndo = TRUE;
	m_bCanRedo = FALSE;
}

// ����ԭʼ��ͼ��ɾ��
void CTilCanvas::PopSrcImage(int nPopCount/* = 1*/)
{
	int nSize = m_LayerSrcImage.size();
	ASSERT(nPopCount > 0 && nPopCount <= nSize);

	for (int i=nSize-1; i>=nSize-nPopCount; i--)
	{
		delete (CImage*)m_LayerSrcImage.at(i);
		m_LayerSrcImage.pop_back();
	}

	// �������õ�ǰ��ʾ��ͼ���m_nIdx
	SetCurLayerIdx(FALSE);
	// �����޸�����
	m_bModified = TRUE;
	// ����Undo��Redo״̬
	if (nSize-nPopCount <= 1)
		m_bCanUndo = FALSE;
	else
		m_bCanUndo = TRUE;
	m_bCanRedo = FALSE;
}

// ����Ŀ���ͼ��ɾ��
void CTilCanvas::PopCrackImage(int nPopCount/* = 1*/)
{
	int nSize = m_LayerCrackImage.size();
	ASSERT(nPopCount > 0 && nPopCount <= nSize);

	for (int i=nSize-1; i>=nSize-nPopCount; i--)
	{
		delete (CImage*)m_LayerCrackImage.at(i);
		m_LayerCrackImage.pop_back();
	}

	// �������õ�ǰ��ʾ��ͼ���m_nIdx
	SetCurLayerIdx(FALSE);
	// �����޸�����
	m_bModified = TRUE;
	// ����Undo��Redo״̬
	if (nSize-nPopCount <= 1)
		m_bCanUndo = FALSE;
	else
		m_bCanUndo = TRUE;
	m_bCanRedo = FALSE;
}

// �������ԭʼ��ͼ��
CImage* CTilCanvas::GetLastSrcImage()
{
	if (m_LayerSrcImage.size() == 0)
		return NULL;
	else
		return (CImage*)m_LayerSrcImage.at(m_LayerSrcImage.size()-1); 
}
// �������Ŀ����ѷ�ͼ��
CImage* CTilCanvas::GetLastCrackImage()
{
	if (m_LayerCrackImage.size() == 0)
		return NULL;
	else
		return (CImage*)m_LayerCrackImage.at(m_LayerCrackImage.size()-1); 
}

// ��Undo(����)������
void CTilCanvas::Undo()
{
	if (m_nMark == 0)	// ���ֻ�Ǵ���ԭʼͼ��
	{
		if (m_nIdx > 0)
		{
			m_nIdx --;

			if (m_nIdx == 0)
				m_bCanUndo = FALSE;
		}

		m_bCanRedo = TRUE;
	}
	else if (m_nMark == 1)	// ������ѷ�Ŀ��ͼ��
	{
		if (m_nIdx > 0)
		{
			m_nIdx --;
		}
		else
		{
			if (m_LayerSrcImage.size() > 0)
			{
				m_nMark = 0;
				m_nIdx = m_LayerSrcImage.size()-1;

				if (m_nIdx == 0)
					m_bCanUndo = FALSE;
			}
		}
		m_bCanRedo = TRUE;
	}
	else
	{
		// ����ѷ�ͼ������Ϊ0��˵����ǰ��Ŀ��ͼ�񾭹����ѷ���ȡ�����
		if (m_LayerCrackImage.size() > 0)
		{
			m_nMark = 1;
			m_nIdx = m_LayerCrackImage.size()-1;
		}
		else
		{
			m_nMark = 0;
			m_nIdx = m_LayerSrcImage.size()-1;	// ԭʼͼ���ͼ��������Ϊ1
			
			if (m_nIdx == 0)
				m_bCanUndo = FALSE;
		}
		m_bCanRedo = TRUE;
	}
}

// ��Redo(�ظ�)������
void CTilCanvas::Redo()
{
	if (m_nMark == 0)	// �����ǰ��ʾͼ��Ϊԭʼͼ���
	{
		if (m_nIdx < m_LayerSrcImage.size()-1)
		{
			m_nIdx ++;

			if (m_nIdx+1 == m_LayerSrcImage.size() && m_LayerCrackImage.size() == 0 && m_pCracksObject == NULL)
				m_bCanRedo = FALSE;
		}
		else
		{
			if (m_LayerCrackImage.size() > 0)
			{
				m_nMark = 1;
				m_nIdx = 0;

				if (m_LayerCrackImage.size() == 1 && m_pCracksObject == NULL)
					m_bCanUndo = FALSE;
			}
			else
			{
				m_nMark = 2;
				m_nIdx = 0;

				// ����ܹ�Redo���ѷ�ͼ��㲻���ڣ�˵��ֱ��Redo���ѷ�Ŀ������ʾ�������һ�㣬�ʲ�����Redo
				m_bCanRedo = FALSE;
			}
		}

		m_bCanUndo = TRUE;
	}
	else if (m_nMark == 1)	// �����ǰ��ʾͼ��Ϊ�ѷ�ͼ���
	{
		if (m_nIdx < m_LayerCrackImage.size()-1)
		{
			m_nIdx ++;

			if (m_nIdx + 1 == m_LayerCrackImage.size() && m_pCracksObject == NULL)
				m_bCanRedo = FALSE;
		}
		else
		{
			if (m_pCracksObject != NULL)
			{
				m_nMark = 2;
				m_nIdx = 0;

				m_bCanRedo = FALSE;
			}
		}
		m_bCanUndo = TRUE;
	}
}

void CTilCanvas::SetZoomFactor(long double dbZoomX, long double dbZoomY)
{
	m_dbZoomX = dbZoomX;
	m_dbZoomY = dbZoomY;

	// �豸��ԭʼͼ�����
	int nSize = m_LayerSrcImage.size();
	for (int i=0; i<nSize; i++)
	{
		CImage* pImage = (CImage*)m_LayerSrcImage.at(i);
		pImage->SetZoomFactor(dbZoomX, dbZoomY);
	}

	// ����ѷ�ͼ�����
	nSize = m_LayerCrackImage.size();
	for (i=0; i<nSize; i++)
	{
		CImage* pImage = (CImage*)m_LayerCrackImage.at(i);
		pImage->SetZoomFactor(dbZoomX, dbZoomY);
	}

	// �����ѷ�Ŀ������
	if (m_pCracksObject)
		m_pCracksObject->SetZoomFactor(dbZoomX, dbZoomY);
}

void CTilCanvas::GetZoomFactor(long double& dbZoomX, long double& dbZoomY)
{
	dbZoomX = m_dbZoomX;
	dbZoomY = m_dbZoomY;
}

// ���õ�ǰ��m_nMark��m_nIdx
// BOOL bNext: ����ǰ�����������
void CTilCanvas::SetCurLayerIdx(BOOL bNext)
{
	if (bNext)
	{
		if (m_nMark == 0)
		{
			if (m_nIdx == m_LayerSrcImage.size()-1 && m_LayerCrackImage.size() > 0)
			{
				m_nMark = 1;
				m_nIdx = 0;
			}
			else
				m_nIdx ++;
		}
		else if (m_nMark == 1)
		{
			if (m_nIdx == m_LayerCrackImage.size()-1 && m_pCracksObject != NULL)
			{
				m_nMark = 2;
				m_nIdx = 0;
			}
			else
				m_nIdx ++;
		}
	}
	else
	{
		if (m_nMark == 0)
		{
			if (m_nIdx > 0)
				m_nIdx --;
		}
		else if (m_nMark == 1)
		{
			if (m_nIdx > 0)
				m_nIdx --;
			else
			{
				m_nMark = 0;
				m_nIdx = m_LayerSrcImage.size()-1;
			}
		}
		else 
		{
			int nSize = m_LayerCrackImage.size();
			if (nSize > 0)
			{
				m_nMark = 1;
				m_nIdx = nSize-1;
			}
			else
			{
				nSize = m_LayerSrcImage.size();
				m_nMark = 0;
				m_nIdx = nSize-1;
			}
		}
	}
}

// ��õ�ǰ��ʾ�����ͼ�㣨������Ŀ��㣩
CImage* CTilCanvas::GetCurImage()
{
	CImage *pImage = NULL;
	if (m_nMark == 0)
	{
		pImage = (CImage *)m_LayerSrcImage.at(m_nIdx);
	}
	else if (m_nMark == 1)
	{
		int nSize = m_LayerCrackImage.size();
		pImage = (CImage *)m_LayerCrackImage.at(m_nIdx);
	}
	else	// m_nMark == 2
	{
		int nSize = m_LayerCrackImage.size();
		if (nSize > 0) // ������ѷ�ͼ��
			pImage = (CImage *)m_LayerCrackImage.at(nSize-1);
		else
		{
			nSize = m_LayerSrcImage.size();
			pImage = (CImage *)m_LayerSrcImage.at(nSize-1);
		}
	}

	return pImage;
}

int CTilCanvas::GetWidth()
{
	CImage* pImage = GetLastSrcImage();
	return pImage->GetWidth()/**m_dbZoomX*/;
}

int CTilCanvas::GetHeight()
{
	CImage* pImage = GetLastSrcImage();
	return pImage->GetHeight()/**m_dbZoomY*/;
}

int CTilCanvas::GetDepth()
{
	CImage* pImage = GetLastSrcImage();
	return pImage->GetBpp()/8;
}

// ʶ���ѷ첢�����ѷ�ͼ��
void CTilCanvas::FindCrack()
{
	// ��ȡ���µ�ԭʼͼ��㡢�����ѷ�Ŀ���,����Ϊ�ѷ���ҵ�ͼ����Դ
	CImage* pSrcImage = (CImage*)m_LayerSrcImage.at(m_LayerSrcImage.size()-1);
	CImage* pObjImage = (CImage*)m_LayerCrackImage.at(m_LayerCrackImage.size()-1);

	// �ѷ�ʶ��
	CFindCrack findCrack(pSrcImage, pObjImage);
	findCrack.FindCrack(30, 100, (float)0.05, 1800, 16000, &m_pCracksObject, &m_pCracksCharacter);

	// �����ѷ����ű����뻭����ͬ
	m_pCracksObject->SetZoomFactor(m_dbZoomX, m_dbZoomY);

	SetCurLayerIdx(TRUE);
	m_bModified = TRUE;
	m_bLayerCrackObj = TRUE;
}

// ɾ���ѷ�
void CTilCanvas::DelSelectedObject()
{
	if (m_pCracksObject->ObjDel(m_nSelected))
	{
		CRACK* pCracks = m_pCracksCharacter->elem;
		int nObjNum = m_pCracksCharacter->nNum;
		for (int i=m_nSelected; i<nObjNum-1; i++)
		{
			pCracks[i] = pCracks[i+1];
		}
		m_pCracksCharacter->nNum = nObjNum-1;
	}

	m_nSelected = -1;
	m_bModified = TRUE;
}

// ɾ����ǰm_nMark��m_nIdx�����ͼ��Ŀ��㣨��Undo���ٴ�����ͼ��ʱ���в�����
void CTilCanvas::DeleteFilesAfterCurLayerIdx()
{
	if (m_nMark == 0)
	{
		// ɾ��ԭʼͼ�������
		int nSize = m_LayerSrcImage.size();
		for (int i=nSize-1; i>=m_nIdx+1; i--)
		{
			delete m_LayerSrcImage.at(i);
			m_LayerSrcImage.pop_back();
		}
		
		// ɾ���ѷ�ͼ�������
		nSize = m_LayerCrackImage.size();
		for (i=nSize-1; i>=0; i--)
		{
			delete m_LayerCrackImage.at(i);
			m_LayerCrackImage.pop_back();
		}
		
		// ɾ��Ŀ��ͼ������
		if (m_pCracksObject != NULL)
		{
			delete m_pCracksObject;
			m_pCracksObject = NULL;
			
			delete m_pCracksCharacter;
			m_pCracksCharacter = NULL;
		}
	}
	else if (m_nMark == 1)
	{
		// ɾ���ѷ�ͼ�������
		int nSize = m_LayerCrackImage.size();
		for (int i=nSize-1; i>=m_nIdx+1; i--)
		{
			delete m_LayerCrackImage.at(i);
			m_LayerCrackImage.pop_back();
		}
		
		// ɾ��Ŀ��ͼ������
		if (m_pCracksObject != NULL)
		{
			delete m_pCracksObject;
			m_pCracksObject = NULL;
			
			delete m_pCracksCharacter;
			m_pCracksCharacter = NULL;
		}
	}
}

// ɾ������Ŀ���
void CTilCanvas::DeleteObjLayer()
{
	if (m_pCracksObject != NULL)
	{
		delete m_pCracksObject;
		m_pCracksObject = NULL;

		delete m_pCracksCharacter;
		m_pCracksCharacter = NULL;

		// �Ա��Ϊ2�������������
		if (m_nMark == 2)
		{
			if (m_LayerCrackImage.size() > 0)
			{
				m_nMark = 1;
				m_nIdx = m_LayerCrackImage.size()-1;
			}
		}
	}
}

// ɾ���ѷ�ͼ���
void CTilCanvas::DeleteCrackLayer()
{
	int nSize = m_LayerCrackImage.size();
	if (nSize > 0)
	{
		for (int i=nSize-1; i>=0; i--)
		{
			delete m_LayerCrackImage.at(i);
			m_LayerCrackImage.pop_back();
		}

		// �Ա��Ϊ1�������������
		if (m_nMark == 1)
		{
			if (m_LayerSrcImage.size() > 0)
			{
				m_nMark = 0;
				m_nIdx = m_LayerSrcImage.size()-1;
			}
		}
	}
}
