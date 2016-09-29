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

// const CString& strFilePath : 文件路径
// const CString& strFileName : 文件名（不带后缀）
BOOL CTilCanvas::Open(const CString& strFilePath, const CString& strFileName)
{
	ASSERT(!strFilePath.IsEmpty());

	m_strFilePath = strFilePath;
	m_strFileName = strFileName;

	CFileFind ff;

	// 读取原始图像文件
	CString strImageFile = strFilePath + '\\' + strFileName + ".src";
	// 如果原始.src图像不存在,则直接打开jpg文件
	if (!ff.FindFile(strImageFile))
		strImageFile = strFilePath + '\\' + strFileName + ".jpg";
	CImage* pSrc = new CImage(strImageFile, CXIMAGE_FORMAT_JPG);
	PushSrcImage(pSrc);
	m_nMark = 0;
	m_nIdx = 0;

	// 先查看有没有裂缝目标文件，若有则读取读取，不读取裂缝图像文件
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
		// 若裂缝目标文件不存在，则读取裂缝图像文件
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

	// 将原始图像层拷贝过来，仅考虑m_LayerSrcImage的最后一个数据，此数据为原始图像
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

	// 将裂缝图像层拷贝过来，仅考虑m_LayerCrackImage的最后一个数据，此数据为原始图像
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

	// 将目标图层拷贝过来
	if (canvas.m_pCracksObject != NULL)
	{
		m_pCracksObject = new ObjRunLenCode(*m_pCracksObject);
	}

	return TRUE;
}

void CTilCanvas::SaveAll()
{
	CString strPathFile = m_strFilePath + '\\' + m_strFileName;

	// 如果当前显示图层为原始图像层
	if (m_nMark == 0)
	{
		// 仅保存m_LayerSrcImage的第m_nIdx个数据，此数据为原始图像
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

		// 如果原来有裂缝图像层，则依然保存，只是下次只打开裂缝目标层，而不打开裂缝图像层
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

		// 读取裂缝图像数据
		if (m_pCracksObject != NULL)
			delete m_pCracksObject;
		m_pCracksObject = new ObjRunLenCode;
		m_pCracksObject->Serial(ar);

		// 读取裂缝特征数据
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

		// 保存裂缝图像数据
		m_pCracksObject->Serial(ar);
		
		// 保存裂缝特征数据
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
	if (m_nMark == 0)	// 如果当前标记为原始图像层
	{
		// 原始图像层绘制
		if (m_bLayerSrcImage)
		{
			// 绘出原始图像层
			CImage* pImage = (CImage*)m_LayerSrcImage.at(m_nIdx);
			pImage->Draw(pDC);
		}
	}
	else if (m_nMark == 1)	// 如果当前标记为裂缝处理层
	{
		if (m_bLayerCrackImage)
		{
			CImage* pCrackImage = (CImage*)m_LayerCrackImage.at(m_nIdx);
			pCrackImage->Draw(pDC);
		}
		else if (m_bLayerSrcImage)
		{// 当裂缝图层不存在时，判断是否显示原始图像层
			int nSize = m_LayerSrcImage.size();
			CImage* pImage = (CImage*)m_LayerSrcImage.at(nSize-1);
			pImage->Draw(pDC);
		}
	}
	else // if (m_nMark == 2)	// 如果当前标记为目标裂缝层
	{
		if (m_bLayerSrcImage)
		{
			// 绘出原始图像层
			int nSize = m_LayerSrcImage.size();
			CImage* pImage = (CImage*)m_LayerSrcImage.at(nSize-1);
			pImage->Draw(pDC);
		}

		// 绘出裂缝目标层
		if (m_bLayerCrackObj)
		{
			m_pCracksObject->Draw(pDC, GetHeight());
			m_pCracksObject->SetObjColor(m_nSelected, pDC, GetHeight(), RGB(0, 255, 255));
		}
	}
}

// 压入原始层图像
void CTilCanvas::PushSrcImage(CImage* pImage)
{
	// 压入图像层前，删除m_nIdx后面的图像数据
	DeleteFilesAfterCurLayerIdx();
	// 压图像图像数据
	m_LayerSrcImage.push_back(pImage);
	// 重新设置当前显示的图像的m_nIdx
	SetCurLayerIdx(TRUE);
	// 设置修改属性
	m_bModified = TRUE;
	// 设置Undo与Redo状态
	if (m_LayerSrcImage.size() > 1) // 第一次压入原始图像时，Undo状态仍为FALSE
		m_bCanUndo = TRUE;
	m_bCanRedo = FALSE;
}

// 压入裂缝层图像
void CTilCanvas::PushCrackImage(CImage* pImage)
{
	// 压入图像层前，删除m_nIdx后面的图像数据
	DeleteFilesAfterCurLayerIdx();
	// 压裂缝图像数据
	m_LayerCrackImage.push_back(pImage);
	// 重新设置当前显示的图像的m_nIdx
	SetCurLayerIdx(TRUE);
	// 设置修改属性
	m_bModified = TRUE;
	// 设置Undo与Redo状态
	m_bCanUndo = TRUE;
	m_bCanRedo = FALSE;
}

// 弹出原始层图像并删除
void CTilCanvas::PopSrcImage(int nPopCount/* = 1*/)
{
	int nSize = m_LayerSrcImage.size();
	ASSERT(nPopCount > 0 && nPopCount <= nSize);

	for (int i=nSize-1; i>=nSize-nPopCount; i--)
	{
		delete (CImage*)m_LayerSrcImage.at(i);
		m_LayerSrcImage.pop_back();
	}

	// 重新设置当前显示的图像的m_nIdx
	SetCurLayerIdx(FALSE);
	// 设置修改属性
	m_bModified = TRUE;
	// 设置Undo与Redo状态
	if (nSize-nPopCount <= 1)
		m_bCanUndo = FALSE;
	else
		m_bCanUndo = TRUE;
	m_bCanRedo = FALSE;
}

// 弹出目标层图像并删除
void CTilCanvas::PopCrackImage(int nPopCount/* = 1*/)
{
	int nSize = m_LayerCrackImage.size();
	ASSERT(nPopCount > 0 && nPopCount <= nSize);

	for (int i=nSize-1; i>=nSize-nPopCount; i--)
	{
		delete (CImage*)m_LayerCrackImage.at(i);
		m_LayerCrackImage.pop_back();
	}

	// 重新设置当前显示的图像的m_nIdx
	SetCurLayerIdx(FALSE);
	// 设置修改属性
	m_bModified = TRUE;
	// 设置Undo与Redo状态
	if (nSize-nPopCount <= 1)
		m_bCanUndo = FALSE;
	else
		m_bCanUndo = TRUE;
	m_bCanRedo = FALSE;
}

// 获得最新原始层图像
CImage* CTilCanvas::GetLastSrcImage()
{
	if (m_LayerSrcImage.size() == 0)
		return NULL;
	else
		return (CImage*)m_LayerSrcImage.at(m_LayerSrcImage.size()-1); 
}
// 获得最新目标层裂缝图像
CImage* CTilCanvas::GetLastCrackImage()
{
	if (m_LayerCrackImage.size() == 0)
		return NULL;
	else
		return (CImage*)m_LayerCrackImage.at(m_LayerCrackImage.size()-1); 
}

// “Undo(撤销)”操作
void CTilCanvas::Undo()
{
	if (m_nMark == 0)	// 如果只是处理原始图像
	{
		if (m_nIdx > 0)
		{
			m_nIdx --;

			if (m_nIdx == 0)
				m_bCanUndo = FALSE;
		}

		m_bCanRedo = TRUE;
	}
	else if (m_nMark == 1)	// 如果是裂缝目标图像
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
		// 如果裂缝图像数不为0，说明当前项目中图像经过了裂缝提取处理的
		if (m_LayerCrackImage.size() > 0)
		{
			m_nMark = 1;
			m_nIdx = m_LayerCrackImage.size()-1;
		}
		else
		{
			m_nMark = 0;
			m_nIdx = m_LayerSrcImage.size()-1;	// 原始图像层图像数至少为1
			
			if (m_nIdx == 0)
				m_bCanUndo = FALSE;
		}
		m_bCanRedo = TRUE;
	}
}

// “Redo(重复)”操作
void CTilCanvas::Redo()
{
	if (m_nMark == 0)	// 如果当前显示图层为原始图像层
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

				// 如果能够Redo且裂缝图像层不存在，说明直接Redo到裂缝目标层的显示，是最后一层，故不能再Redo
				m_bCanRedo = FALSE;
			}
		}

		m_bCanUndo = TRUE;
	}
	else if (m_nMark == 1)	// 如果当前显示图层为裂缝图像层
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

	// 设备各原始图像比例
	int nSize = m_LayerSrcImage.size();
	for (int i=0; i<nSize; i++)
	{
		CImage* pImage = (CImage*)m_LayerSrcImage.at(i);
		pImage->SetZoomFactor(dbZoomX, dbZoomY);
	}

	// 设各裂缝图像比例
	nSize = m_LayerCrackImage.size();
	for (i=0; i<nSize; i++)
	{
		CImage* pImage = (CImage*)m_LayerCrackImage.at(i);
		pImage->SetZoomFactor(dbZoomX, dbZoomY);
	}

	// 设置裂缝目标层比例
	if (m_pCracksObject)
		m_pCracksObject->SetZoomFactor(dbZoomX, dbZoomY);
}

void CTilCanvas::GetZoomFactor(long double& dbZoomX, long double& dbZoomY)
{
	dbZoomX = m_dbZoomX;
	dbZoomY = m_dbZoomY;
}

// 设置当前的m_nMark和m_nIdx
// BOOL bNext: 是向前还是向后设置
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

// 获得当前显示的最顶层图层（不包含目标层）
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
		if (nSize > 0) // 如果有裂缝图层
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

// 识别裂缝并设置裂缝图层
void CTilCanvas::FindCrack()
{
	// 提取最新的原始图像层、最新裂缝目标层,以作为裂缝查找的图像来源
	CImage* pSrcImage = (CImage*)m_LayerSrcImage.at(m_LayerSrcImage.size()-1);
	CImage* pObjImage = (CImage*)m_LayerCrackImage.at(m_LayerCrackImage.size()-1);

	// 裂缝识别
	CFindCrack findCrack(pSrcImage, pObjImage);
	findCrack.FindCrack(30, 100, (float)0.05, 1800, 16000, &m_pCracksObject, &m_pCracksCharacter);

	// 设置裂缝缩放比例与画布相同
	m_pCracksObject->SetZoomFactor(m_dbZoomX, m_dbZoomY);

	SetCurLayerIdx(TRUE);
	m_bModified = TRUE;
	m_bLayerCrackObj = TRUE;
}

// 删除裂缝
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

// 删除当前m_nMark和m_nIdx后面的图像及目标层（供Undo后再处理了图像时进行操作）
void CTilCanvas::DeleteFilesAfterCurLayerIdx()
{
	if (m_nMark == 0)
	{
		// 删除原始图像层数据
		int nSize = m_LayerSrcImage.size();
		for (int i=nSize-1; i>=m_nIdx+1; i--)
		{
			delete m_LayerSrcImage.at(i);
			m_LayerSrcImage.pop_back();
		}
		
		// 删除裂缝图像层数据
		nSize = m_LayerCrackImage.size();
		for (i=nSize-1; i>=0; i--)
		{
			delete m_LayerCrackImage.at(i);
			m_LayerCrackImage.pop_back();
		}
		
		// 删除目标图像数据
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
		// 删除裂缝图像层数据
		int nSize = m_LayerCrackImage.size();
		for (int i=nSize-1; i>=m_nIdx+1; i--)
		{
			delete m_LayerCrackImage.at(i);
			m_LayerCrackImage.pop_back();
		}
		
		// 删除目标图像数据
		if (m_pCracksObject != NULL)
		{
			delete m_pCracksObject;
			m_pCracksObject = NULL;
			
			delete m_pCracksCharacter;
			m_pCracksCharacter = NULL;
		}
	}
}

// 删除最终目标层
void CTilCanvas::DeleteObjLayer()
{
	if (m_pCracksObject != NULL)
	{
		delete m_pCracksObject;
		m_pCracksObject = NULL;

		delete m_pCracksCharacter;
		m_pCracksCharacter = NULL;

		// 对标记为2的情况重新设置
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

// 删除裂缝图像层
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

		// 对标记为1的情况重新设置
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
