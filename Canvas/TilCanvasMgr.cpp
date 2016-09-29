// TilCanvasMgr.cpp: implementation of the CTilCanvasMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TilCanvasMgr.h"
#include "TilComFunction.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define VERSION "1.0.0.1"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTilCanvasMgr::CTilCanvasMgr() : m_ppCanvas(NULL)
{
	m_bModified = FALSE;
	m_nIdx = -1;
	m_nNum = m_nTotalNum = 0;
	m_dbZoomX = m_dbZoomY = 1.0;

	m_bLayerSrcImage = TRUE;
	m_bLayerCrackImage = TRUE;
	m_bLayerCrackObj = TRUE;
}

CTilCanvasMgr::~CTilCanvasMgr()
{
	clear_mem();
}

void CTilCanvasMgr::New()
{
	clear_mem();
	m_bModified = FALSE;
	m_nIdx = -1;
	m_nNum = 0;
	m_nTotalNum = ADD_SIZE;
	m_strProjName.Empty();

	m_ppCanvas = new CTilCanvas* [m_nTotalNum];
	for (int i = 0; i < m_nTotalNum; i ++)
		m_ppCanvas[i] = NULL;
}

BOOL CTilCanvasMgr::Open(const CString& strProjName)
{
	clear_mem();
	m_bModified = FALSE;
	m_nIdx = -1;
	m_strProjName = strProjName;

	// 得到打开文件的路径（不包含文件名）
	CString strPath = get_file_path(strProjName);

	CFile file;
	if (!file.Open(strProjName, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeRead))
		return FALSE;

	m_dbZoomX = 1.0;
	m_dbZoomY = 1.0;

	CArchive ar(&file, CArchive::load);

	TRY 
	{
		// 版本形式如Version=1.0.0.1
		CString strVersion;	// 版本号未作判别使用
		ar >> strVersion;
		
		// 画布数量及总量
		ar >> m_nNum;
		m_nTotalNum = (m_nNum/ADD_SIZE+1)*ADD_SIZE;
		
		m_ppCanvas = new CTilCanvas* [m_nTotalNum];
		for (int i = 0; i < m_nTotalNum; i ++)
			m_ppCanvas[i] = NULL;

		// 建立图形图像层
		CString strImage;
		for (i = 0; i < m_nNum; i ++)
		{
			// 获得不含后缀的图像文件名
			ar >> strImage;

			// 将图像层、裂缝层、目标层等全部打开
			m_ppCanvas[i] = new CTilCanvas;
			m_ppCanvas[i]->Open(strPath, strImage);

			// 读取并设置画布属性（注意：画布属性存储在项目管理文件中）///////////////////
			m_ppCanvas[i]->SetModifiedFlag(FALSE);
		}
	}
	CATCH (CFileException, e)
	{
#ifdef _DEBUG
		afxDump << "Error during reading " << e->m_cause << "\n";
#endif
		AfxMessageBox("Error during reading file project");
		file.Close();
		return FALSE;
	}
	END_CATCH

	ar.Close();
	file.Close();

	return TRUE;
}

BOOL CTilCanvasMgr::Save()
{
	return SaveAs(m_strProjName);
}

BOOL CTilCanvasMgr::SaveAs(const CString& strProjName)
{
	CFile file;
	if (!file.Open(strProjName, CFile::modeCreate | CFile::modeWrite))
		return FALSE;
	
	CArchive ar(&file, CArchive::store);

	TRY 
	{
		// 版本形式如Version=1.0.0.1
		CString strVersion;	// 版本号未作判别使用
		strVersion = VERSION;
		ar << strVersion;
		
		// 画布数量及总量
		ar << m_nNum;

		// 建立图形图像层
		CString strImage;
		for (int i = 0; i < m_nNum; i ++)
		{
			// 保存画布中的原始图像层、裂缝层、目标层文件
			m_ppCanvas[i]->SaveAll();

			// 获得不带后缀的文件名后保存到项目文件中
			strImage = m_ppCanvas[i]->GetFileName();
			
			ar << strImage;

			m_ppCanvas[i]->SetModifiedFlag(FALSE);
		}
	}
	CATCH (CFileException, e)
	{
		#ifdef _DEBUG
				afxDump << "Error during saving " << e->m_cause << "\n";
		#endif
		AfxMessageBox("Error during saving file project");
		file.Close();
		return FALSE;
	}
	END_CATCH

	ar.Close();
	file.Close();

	// 设置当前项目文件路径
	m_bModified = FALSE;
	m_strProjName = strProjName;

	return TRUE;
}

CString CTilCanvasMgr::GetPathName()
{
	return m_strProjName;
}

CTilCanvas* CTilCanvasMgr::GetCanvasByIdx(int nIdx)
{
	ASSERT(nIdx >= 0 && nIdx < m_nNum);
	return m_ppCanvas[nIdx];
}

CTilCanvas* CTilCanvasMgr::GetFirstCanvas()
{
	if (m_nNum == 0)
		return NULL;
	return m_ppCanvas[0];
}

CTilCanvas* CTilCanvasMgr::GetLastCanvas()
{
	if (m_nIdx < 0)
		return NULL;
	return m_ppCanvas[m_nNum-1];
}

CTilCanvas* CTilCanvasMgr::GetPrevCanvas()
{
	if (m_nIdx < 0)
		return NULL;
	else if (m_nIdx == 0)
		return m_ppCanvas[0];
	else
		return m_ppCanvas[m_nIdx-1];
}

CTilCanvas* CTilCanvasMgr::GetCurCanvas()
{
	if (m_nIdx < 0)
		return NULL;
	return m_ppCanvas[m_nIdx];
}

CTilCanvas* CTilCanvasMgr::GetNextCanvas()
{
	if (m_nIdx < 0)
		return NULL;
	else if (m_nIdx+1 >= m_nNum)
		return m_ppCanvas[m_nNum-1];
	else
		return m_ppCanvas[m_nIdx+1];
}

void CTilCanvasMgr::SetCurCanvasIdx(int nIdx)
{
	ASSERT(nIdx >= 0 && nIdx < m_nNum);
	m_nIdx = nIdx;
}

int	 CTilCanvasMgr::GetCurCanvasIdx()
{
	return m_nIdx;
}

void CTilCanvasMgr::SetZoomFactor(long double dbZoomX, long double dbZoomY)
{
	m_dbZoomX = dbZoomX;
	m_dbZoomY = dbZoomY;

	int i;
	for (i = 0; i < m_nNum; i ++)
	{
		m_ppCanvas[i]->SetZoomFactor(dbZoomX, dbZoomY);
	}	
}

void CTilCanvasMgr::GetZoomFactor(long double& dbZoomX, long double& dbZoomY)
{
	dbZoomX = m_dbZoomX;
	dbZoomY = m_dbZoomY;
}

int CTilCanvasMgr::Size()
{
	return m_nNum;
}

void CTilCanvasMgr::RemoveAll()
{
	New();
}

BOOL CTilCanvasMgr::RemoveAt(int nIdx)
{
	ASSERT(nIdx >= 0 && nIdx < m_nNum);

	int i;

	// 如果
	if ((m_nNum-1)%ADD_SIZE == 0 && (m_nNum-1)/ADD_SIZE >= 1)
	{
		int nTotalNum = m_nTotalNum-ADD_SIZE;
		CTilCanvas** ppCanvas = new CTilCanvas* [nTotalNum];
		for (i = 0; i < m_nTotalNum; i ++)
			ppCanvas[i] = NULL;

		for(i = 0; i < nIdx; i ++)
			ppCanvas[i] = m_ppCanvas[i];
		delete m_ppCanvas[nIdx];
		for (i = nIdx+1; i < m_nNum; i ++)
			ppCanvas[i-1] = m_ppCanvas[i];

		m_nNum --;
		m_nTotalNum = nTotalNum;
		
		delete [] m_ppCanvas;
		m_ppCanvas = ppCanvas;
	}
	else
	{
		delete m_ppCanvas[nIdx];
		for (i = nIdx+1; i < m_nNum; i ++)
			m_ppCanvas[i-1] = m_ppCanvas[i];
		m_ppCanvas[m_nNum-1] = NULL;

		m_nNum --;
	}

	// 如果当前切片被删除
	if (nIdx == m_nIdx)
	{
		if (nIdx > 0)
			m_nIdx = nIdx-1;
		else
		{
			if (m_nNum > 0)
				m_nIdx = 0;
			else
				m_nIdx = -1;
		}
	}
	else if (nIdx < m_nIdx)
		m_nIdx --;

	m_bModified = TRUE;

	return TRUE;
}

BOOL CTilCanvasMgr::PushFrontCanvas(CTilCanvas* pCanvas)
{
	pCanvas->SetZoomFactor(m_dbZoomX, m_dbZoomY);

	int i;

	if (m_nNum%ADD_SIZE == 0)
	{
		int nTotalNum = m_nTotalNum+ADD_SIZE;
		CTilCanvas** ppCanvas = new CTilCanvas* [nTotalNum];
		for (i = 0; i < m_nNum; i ++)
			ppCanvas[i] = NULL;

		ppCanvas[0] = pCanvas;
		for (i = 0; i < m_nNum; i ++)
			ppCanvas[i+1] = m_ppCanvas[i];

		delete [] m_ppCanvas;
		m_ppCanvas = ppCanvas;

		m_nNum ++;
		m_nTotalNum = nTotalNum;
	}
	else
	{
		for (i = m_nNum; i > 0; i ++)
			m_ppCanvas[i] = m_ppCanvas[i-1];
		m_ppCanvas[0] = pCanvas;

		m_nNum ++;
	}

	m_bModified = TRUE;
	m_nIdx ++;

	return TRUE;
}

BOOL CTilCanvasMgr::PushBackCanvas(CTilCanvas* pCanvas)
{
	pCanvas->SetZoomFactor(m_dbZoomX, m_dbZoomY);

	int i;

	if (m_nNum%ADD_SIZE == 0)
	{
		int nTotalNum = m_nTotalNum+ADD_SIZE;
		CTilCanvas** ppCanvas = new CTilCanvas* [nTotalNum];
		for (i = 0; i < m_nNum; i ++)
			ppCanvas[i] = NULL;

		for (i = 0; i < m_nNum; i ++)
			ppCanvas[i] = m_ppCanvas[i];
		ppCanvas[m_nNum] = pCanvas;

		delete [] m_ppCanvas;
		m_ppCanvas = ppCanvas;

		m_nNum ++;
		m_nTotalNum = nTotalNum;
	}
	else
	{
		m_ppCanvas[m_nNum] = pCanvas;

		m_nNum ++;
	}

	m_bModified = TRUE;

	return TRUE;
}

BOOL CTilCanvasMgr::InsertCanvas(CTilCanvas* pCanvas, int nIdx, BOOL bAfter)
{
	ASSERT(nIdx >= 0 && nIdx < m_nNum);
	pCanvas->SetZoomFactor(m_dbZoomX, m_dbZoomY);

	int i;

	if (m_nNum%ADD_SIZE == 0)
	{
		int nTotalNum = m_nTotalNum+ADD_SIZE;
		CTilCanvas** ppCanvas = new CTilCanvas* [nTotalNum];
		for (i = 0; i < m_nNum; i ++)
			ppCanvas[i] = NULL;

		if (bAfter)	// 插入后面
		{
			for (i = 0; i <= nIdx; i ++)
				ppCanvas[i] = m_ppCanvas[i];
			ppCanvas[nIdx+1] = pCanvas;
			for (i = nIdx+1; i < m_nNum; i ++)
				ppCanvas[i+1] = m_ppCanvas[i];

			if (m_nIdx > nIdx)
				m_nIdx ++;
		}
		else	// 插入前面
		{
			for (i = 0; i < nIdx; i ++)
				ppCanvas[i] = m_ppCanvas[i];
			ppCanvas[nIdx] = pCanvas;
			for (i = nIdx; i < m_nNum; i ++)
				ppCanvas[i+1] = m_ppCanvas[i];

			if (m_nIdx > nIdx)
				m_nIdx ++;
		}
	}

	m_bModified = TRUE;

	return TRUE;
}

void CTilCanvasMgr::SetShowHideLayerSrcImage(BOOL bLayerSrcImage/* = TRUE*/)
{
	m_bLayerSrcImage = bLayerSrcImage;
	int nSize = Size();
	for (int i=0; i<nSize; i++)
	{
		CTilCanvas* pCanvas = GetCanvasByIdx(i);
		pCanvas->SetShowHideLayerSrcImage(bLayerSrcImage);
	}
}

void CTilCanvasMgr::SetShowHideLayerCrackImage(BOOL bLayerCrackImage/* = TRUE*/)
{
	m_bLayerCrackImage = bLayerCrackImage;
	int nSize = Size();
	for (int i=0; i<nSize; i++)
	{
		CTilCanvas* pCanvas = GetCanvasByIdx(i);
		pCanvas->SetShowHideLayerCrackImage(bLayerCrackImage);
	}
}

void CTilCanvasMgr::SetShowHideLayerCrackObj(BOOL bLayerCrackObj/* = TRUE*/)
{
	m_bLayerCrackObj = bLayerCrackObj;
	int nSize = Size();
	for (int i=0; i<nSize; i++)
	{
		CTilCanvas* pCanvas = GetCanvasByIdx(i);
		pCanvas->SetShowHideLayerCrackObj(bLayerCrackObj);
	}	
}


// 将序号为nIdx1的画布放到nIdx2的前面或后面
BOOL CTilCanvasMgr::DragCanvasTo(int nIdx1, int nIdx2, BOOL bAfter)
{
	ASSERT(nIdx1 >= 0 && nIdx1 < m_nNum);
	ASSERT(nIdx2 >= 0 && nIdx2 < m_nNum);
	ASSERT(nIdx1 != nIdx2);

	int i;
	CTilCanvas* pTmp = m_ppCanvas[nIdx1];

	// 如果将第nIdx1张切片拖到第nIdx2张切片后面
	if (bAfter)
	{
		if (nIdx1 < nIdx2)
		{
			for (i = nIdx1+1; i <= nIdx2; i ++)
				m_ppCanvas[i-1] = m_ppCanvas[i];
			m_ppCanvas[i-1] = pTmp;
			m_nIdx = nIdx2;
		}
		else
		{
			for (i = nIdx1; i > nIdx2+1; i --)
				m_ppCanvas[i] = m_ppCanvas[i-1];
			m_ppCanvas[i] = pTmp;
			m_nIdx = nIdx2+1;
		}
	}
	else
	{
		if (nIdx1 < nIdx2)
		{
			for (i = nIdx1+1; i < nIdx2; i ++)
				m_ppCanvas[i-1] = m_ppCanvas[i];
			m_ppCanvas[i-1] = pTmp;
			m_nIdx = nIdx2-1;
		}
		else
		{
			for (i = nIdx1; i > nIdx2; i --)
				m_ppCanvas[i] = m_ppCanvas[i-1];
			m_ppCanvas[i] = pTmp;
			m_nIdx = nIdx2;
		}
	}

	// 顺序交换后，链接目标也将重新设置

	// 将拖动的序号为nIdx1的画布设为当前画布
	m_bModified = TRUE;

	return TRUE;
}

void CTilCanvasMgr::AutoArrayCanvas(BOOL bOrder)
{
	int i, j, nStdNum, nUnStdNum;

	int* pIdx = new int [m_nNum];
	long* pMile = new long [m_nNum];

	CTilCanvas** ppCanvas = new CTilCanvas* [m_nTotalNum];
	for (i = 0; i < m_nTotalNum; i ++)
		ppCanvas[i] = NULL;

	CString strFileName;
	nStdNum = nUnStdNum = 0;
	for (i = 0; i < m_nNum; i ++)
	{
		strFileName = m_ppCanvas[i]->GetFileName();

		// 将非标准名称的切片直接放到画布列表前面
		if (!is_std_mileage(strFileName))
		{
			ppCanvas[nUnStdNum] = m_ppCanvas[i];
			nUnStdNum ++;
		}
		else
		{
			pMile[nStdNum] = get_std_mileage(strFileName);
			pIdx[nStdNum] = i;
			nStdNum ++;
		}
	}

	// 对标准里程文件画布切片进行排序
	int nFlag = bOrder ? 1 : -1;
	for (i = 0; i < nStdNum-1; i ++)
	{
		for (j = i+1; j < nStdNum; j ++)
		{
			if (nFlag*pMile[i] > nFlag*pMile[j])
			{
				int tmp = pIdx[j];
				pIdx[j] = pIdx[i];
				pIdx[i] = tmp;

				long lTmp = pMile[j];
				pMile[j] = pMile[i];
				pMile[i] = lTmp;
			}
		}
	}

	for (i = 0; i < nStdNum; i ++)
	{
		ppCanvas[nUnStdNum] = m_ppCanvas[pIdx[i]];
	 	nUnStdNum ++;
	}

	// 判断是否排序后与排序前有所不同
	for (i = 0; i < m_nNum; i ++)
	{
		if (ppCanvas[i] != m_ppCanvas[i])
			break;
	}
	if (i < m_nNum)
		m_bModified = TRUE;

	delete [] m_ppCanvas;
	m_ppCanvas = ppCanvas;

	delete [] pIdx;
	delete [] pMile;
}

void CTilCanvasMgr::clear_mem()
{
	if (m_ppCanvas)
	{
		for (int i = 0; i < m_nNum; i ++)
			delete m_ppCanvas[i];
		delete [] m_ppCanvas;
		m_ppCanvas = NULL;
	}
}

void CTilCanvasMgr::SetModifiedFlag(BOOL bFlag)
{
	m_bModified = bFlag;
}

BOOL CTilCanvasMgr::IsModified()
{
	// 如果项目文件或画布中掌子面信息被修改，则认为项目被修改
	for (int i=0; i<m_nNum; i++)
	{
		m_bModified = m_bModified || m_ppCanvas[i]->IsModified();
	}

	return m_bModified;
}

void CTilCanvasMgr::SetPrjInfo(PRJCAPTION& prjCap)
{
	m_PrjCap = prjCap;
}

PRJCAPTION CTilCanvasMgr::GetPrjInfo()
{
	return m_PrjCap;
}