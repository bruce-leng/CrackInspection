// ObjRunLenCode.cpp: implementation of the ObjRunLenCode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "canvas.h"
#include "ObjRunLenCode.h"
#include "TilComFunction.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ObjRunLenCode::ObjRunLenCode() : 
m_uchObjClr(255), 
m_nObjMaxNum(OBJ_MAX_NUM),	// 允许容纳最大目标数设为OBJ_MAX_NUM
m_nObjNum(0)
{
	// 设置OBJ_MAX_NUM个目标指针
	m_ppObjRun = new PRUNLENGTHCODE_MGR [OBJ_MAX_NUM];
	memset(m_ppObjRun, NULL, sizeof(PRUNLENGTHCODE_MGR)*OBJ_MAX_NUM);
	m_dbZoomX = m_dbZoomY = 1.0;
}

ObjRunLenCode::ObjRunLenCode(const CString& strFileName)
{
	CFile file;
	if (file.Open(strFileName, CFile::modeRead))
	{
		CArchive ar(&file, CArchive::load);
		Serial(ar);
		
		ar.Close();
	}
}

ObjRunLenCode::ObjRunLenCode(ObjRunLenCode& objRLC)
{
	m_uchObjClr = objRLC.GetObjColor();
	m_nObjNum = objRLC.GetObjNum();
	m_nObjMaxNum = OBJ_MAX_NUM;
	m_ppObjRun = new PRUNLENGTHCODE_MGR [OBJ_MAX_NUM];
	memset(m_ppObjRun, NULL, sizeof(PRUNLENGTHCODE_MGR)*OBJ_MAX_NUM);

	PPRUNLENGTHCODE_MGR ppRLCMgr = objRLC.GetObjAll();
	for (int i=0; i<m_nObjNum; i++)
	{
		PRUNLENGTHCODE_MGR pObj = objRLC.GetObj(i);
		m_ppObjRun[i] = (PRUNLENGTHCODE_MGR)new char[sizeof(pObj)];
		memcpy(m_ppObjRun[i], pObj, sizeof(pObj));
	}
}

ObjRunLenCode::~ObjRunLenCode()
{
	Clear();
	if (m_ppObjRun)
		delete [] m_ppObjRun;
}

void ObjRunLenCode::Serial(CArchive& ar)
{
	if (ar.IsLoading())
	{
		Clear();

		ar >> m_uchObjClr >> m_nObjMaxNum >> m_nObjNum;
		
		int nRunNum;
		for (int i=0; i<m_nObjNum; i++)
		{
			ar >> nRunNum;
			m_ppObjRun[i] = (PRUNLENGTHCODE_MGR)new char [sizeof(int)+sizeof(RUNLENGTHCODE)*nRunNum];
			m_ppObjRun[i]->nRunNum = nRunNum;
			for (int j=0; j<nRunNum; j++)
			{
				ar >> m_ppObjRun[i]->RC[j].bFlags;
				ar >> m_ppObjRun[i]->RC[j].bObserved;
				ar >> m_ppObjRun[i]->RC[j].xStart;
				ar >> m_ppObjRun[i]->RC[j].xEnd;
				ar >> m_ppObjRun[i]->RC[j].y;
			}
		}
	}
	else
	{
		ar << m_uchObjClr << m_nObjMaxNum << m_nObjNum;
		
		for (int i=0; i<m_nObjNum; i++)
		{
			int nRunNum = m_ppObjRun[i]->nRunNum;
			ar << nRunNum;
			for (int j=0; j<nRunNum; j++)
			{
				ar << m_ppObjRun[i]->RC[j].bFlags;
				ar << m_ppObjRun[i]->RC[j].bObserved;
				ar << m_ppObjRun[i]->RC[j].xStart;
				ar << m_ppObjRun[i]->RC[j].xEnd;
				ar << m_ppObjRun[i]->RC[j].y;
			}
		}
	}
}

// 打开文件
BOOL ObjRunLenCode::Open(const CString& strFileName)
{
	CFile file;
	if (file.Open(strFileName, CFile::modeRead))
	{
		CArchive ar(&file, CArchive::load);
		Serial(ar);
		ar.Close();
		file.Close();

		return TRUE;
	}

	return FALSE;
}

// 保存文件
BOOL ObjRunLenCode::Save(const CString& strFileName)
{
	CFile file;
	if (file.Open(strFileName, CFile::modeWrite|CFile::modeCreate))
	{
		// 计算需要存储的空间
		int nBufSize = sizeof(m_uchObjClr) + sizeof(m_nObjMaxNum) + sizeof(m_nObjNum);
		for (int i=0; i<m_nObjNum; i++)
		{
			int nRunNum = m_ppObjRun[i]->nRunNum;
			nBufSize += sizeof(nRunNum) + nRunNum * sizeof(RUNLENGTHCODE);
		}

		CArchive ar(&file, CArchive::store, nBufSize);
		Serial(ar);
		ar.Close();
		file.Close();

		return TRUE;
	}

	return FALSE;
}

// 清除内存
void ObjRunLenCode::Clear()
{
	for (int i=0; i<OBJ_MAX_NUM; i++)
	{
		if (m_ppObjRun[i] != NULL)
		{
			delete [] (char*)m_ppObjRun[i];
			m_ppObjRun[i] = NULL;
		}
	}
	m_nObjNum = 0;
}

// 设置、获取图像比例
void ObjRunLenCode::SetZoomFactor(long double dbZoomX, long double dbZoomY)
{
	m_dbZoomX = dbZoomX;
	m_dbZoomY = dbZoomY;
}

void ObjRunLenCode::GetZoomFactor(long double& dbZoomX, long double& dbZoomY)
{
	dbZoomX = m_dbZoomX;
	dbZoomY = m_dbZoomY;
}

// 根据游程编码绘图
void ObjRunLenCode::Draw(CDC* pDC, int nImgHeight, COLORREF clr)
{
	for (int i=0; i<m_nObjNum; i++)
	{
		SetObjColor(i, pDC, nImgHeight, clr);
	}
}

/* ==================================================================
 * 函数名称：  ObjRunLenCode::SetObjColor()
 * 作者：      [Leng]
 * 创建于:     [2014-6-4 15:13:59]
 * 参数列表： 
               [int nObjIdx] - [In] 目标序号
               [ CDC* pDC] - [In] 绘图指针
			   [ nImgHeight] - [In] 绘图所在图像区域的高度，用于缩放及坐标点y轴的转换 
               [ COLORREF clr] - [In] 目标绘制颜色
注释 :	绘制目标颜色
==================================================================*/
void ObjRunLenCode::SetObjColor(int nObjIdx, CDC* pDC, int nImgHeight, COLORREF clr)
{
	if (nObjIdx < 0)
		return;

	ASSERT(nObjIdx < m_nObjNum);

	int nRunNum = m_ppObjRun[nObjIdx]->nRunNum;
	for (int i=0; i<nRunNum; i++)
	{
		int xStart = m_ppObjRun[nObjIdx]->RC[i].xStart;
		int xEnd = m_ppObjRun[nObjIdx]->RC[i].xEnd;
		int y = m_ppObjRun[nObjIdx]->RC[i].y;

		xStart = xStart * m_dbZoomX;
		xEnd = xEnd * m_dbZoomX;
		y = (nImgHeight - 1 - y) * m_dbZoomY;
		for (int x=xStart; x<=xEnd; x++)
			pDC->SetPixel(x, y, clr);
	}
}

// 获得种子点ptSeed所在的目标
PRUNLENGTHCODE_MGR  ObjRunLenCode::GetObj(POINT ptSeed)
{
	int i, j;
	for (i=0; i<m_nObjNum; i++)
	{
		for (j=0; j<m_ppObjRun[i]->nRunNum; j++)
		{
			if (ptSeed.y==m_ppObjRun[i]->RC[j].y
			 && ptSeed.x>=m_ppObjRun[i]->RC[j].xStart
			 && ptSeed.x<=m_ppObjRun[i]->RC[j].xEnd)
			{
				return m_ppObjRun[i];
			}
		}
	}

	return NULL;
}

// 添加一个目标
void ObjRunLenCode::ObjAdd(PRUNLENGTHCODE_MGR pObject)
{
	m_ppObjRun[m_nObjNum] = pObject;
	m_nObjNum ++;
}

// 删除第nIdx个目标的指针
BOOL ObjRunLenCode::ObjDel(int nIdx)
{
	ASSERT(nIdx>=0 && nIdx<m_nObjNum);

	delete m_ppObjRun[nIdx];

	// 后继指针前移
	for (int i=nIdx; i<m_nObjNum-1; i++)
	{
		m_ppObjRun[i] = m_ppObjRun[i+1];
	}
	m_ppObjRun[m_nObjNum-1] = NULL;

	// 目标数减1
	m_nObjNum --;

	return TRUE;
}

// 第nIdx个目标的外接矩形
void ObjRunLenCode::ObjRect(int nIdx, CRect& rect)
{
	ASSERT(nIdx >= 0 && nIdx < m_nObjNum);

	rect.left = INT_MAX;
	rect.top = INT_MAX;
	rect.right = INT_MIN;
	rect.bottom = INT_MIN;

	for (int i=0; i<m_ppObjRun[nIdx]->nRunNum; i++)
	{
		rect.left = (m_ppObjRun[nIdx]->RC[i].xStart < rect.left) ? m_ppObjRun[nIdx]->RC[i].xStart : rect.left;
		rect.right = (m_ppObjRun[nIdx]->RC[i].xEnd > rect.right) ? m_ppObjRun[nIdx]->RC[i].xEnd : rect.right;
		rect.top = (m_ppObjRun[nIdx]->RC[i].y < rect.top) ? m_ppObjRun[nIdx]->RC[i].y : rect.top;
		rect.bottom = (m_ppObjRun[nIdx]->RC[i].y > rect.bottom) ? m_ppObjRun[nIdx]->RC[i].y : rect.bottom;
	}
}

// 获得目标pObject的外接矩形
void ObjRunLenCode::ObjRect(PRUNLENGTHCODE_MGR pObject, CRect& rect)
{
	ASSERT(pObject != NULL);

	rect.left = INT_MAX;
	rect.top = INT_MAX;
	rect.right = INT_MIN;
	rect.bottom = INT_MIN;

	for (int i=0; i<pObject->nRunNum; i++)
	{
		rect.left = (pObject->RC[i].xStart < rect.left) ? pObject->RC[i].xStart : rect.left;
		rect.right = (pObject->RC[i].xEnd > rect.right) ? pObject->RC[i].xEnd : rect.right;
		rect.top = (pObject->RC[i].y < rect.top) ? pObject->RC[i].y : rect.top;
		rect.bottom = (pObject->RC[i].y > rect.bottom) ? pObject->RC[i].y : rect.bottom;
	}
}

// 获得第nIdx个目标区域的面积
long ObjRunLenCode::ObjArea(int nIdx)
{
	ASSERT(nIdx>=0 && nIdx<m_nObjNum);

	long lArea = 0;
	for (int i=0; i<m_ppObjRun[nIdx]->nRunNum; i++)
	{
		lArea += m_ppObjRun[nIdx]->RC[i].xEnd-m_ppObjRun[nIdx]->RC[i].xStart+1;
	}

	return lArea;
}

// 获得目标pObject的面积
long ObjRunLenCode::ObjArea(PRUNLENGTHCODE_MGR pObject)
{
	ASSERT(pObject != NULL);

	long lArea = 0;
	for (int i=0; i<pObject->nRunNum; i++)
	{
		lArea += pObject->RC[i].xEnd-pObject->RC[i].xStart+1;
	}

	return lArea;
}

// 获得第nIdx个目标的中心点坐标
void ObjRunLenCode::ObjCenter(int nIdx, POINT2DF& ptCenter)
{
	ASSERT(nIdx>=0 && nIdx<m_nObjNum);

	ptCenter.x = 0;
	ptCenter.y = 0;

	int nArea = 0;
	for (int i=0; i<m_ppObjRun[nIdx]->nRunNum; i++)
	{
		ptCenter.x += (m_ppObjRun[nIdx]->RC[i].xEnd+m_ppObjRun[nIdx]->RC[i].xStart)*
									(m_ppObjRun[nIdx]->RC[i].xEnd-m_ppObjRun[nIdx]->RC[i].xStart+1)/2.0;
		ptCenter.y += (m_ppObjRun[nIdx]->RC[i].xEnd-m_ppObjRun[nIdx]->RC[i].xStart+1)*
									 m_ppObjRun[nIdx]->RC[i].y;
		nArea += m_ppObjRun[nIdx]->RC[i].xEnd-m_ppObjRun[nIdx]->RC[i].xStart+1;
	}

	ptCenter.x /= nArea;
	ptCenter.y /= nArea;
}

// 获得目标pObject的中心点坐标
void ObjRunLenCode::ObjCenter(PRUNLENGTHCODE_MGR pObject, POINT2DF& ptCenter)
{
	ASSERT(pObject != NULL);

	ptCenter.x = 0;
	ptCenter.y = 0;

	int nArea = 0;
	for (int i=0; i<pObject->nRunNum; i++)
	{
		ptCenter.x += (pObject->RC[i].xEnd+pObject->RC[i].xStart)*
									(pObject->RC[i].xEnd-pObject->RC[i].xStart+1)/2.0;
		ptCenter.y += (pObject->RC[i].xEnd-pObject->RC[i].xStart+1)*
									 pObject->RC[i].y;
		nArea += pObject->RC[i].xEnd-pObject->RC[i].xStart+1;
	}

	ptCenter.x /= nArea;
	ptCenter.y /= nArea;
}
