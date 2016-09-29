// ObjEdgeList.cpp: implementation of the ObjEdgeList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "canvas.h"
#include "ObjEdgeList.h"
#include "TilComFunction.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ObjEdgeList::ObjEdgeList() :
m_nObjMaxNum(OBJ_MAX_NUM),
m_nObjNum(0)
{
	m_ppEdgeObj = new PEDGE_OBJECT [OBJ_MAX_NUM];
}

ObjEdgeList::~ObjEdgeList()
{
	Clear();
	if (m_ppEdgeObj)
		delete [] m_ppEdgeObj;
}

// ����ڴ�
void ObjEdgeList::Clear()
{
	for (int i=0; i<m_nObjNum; i++)
	{
		if (m_ppEdgeObj[i]->ppEdgeLst)
		{
			for (int j=0; j<m_ppEdgeObj[i]->nEdgeNum; j++)
			{
				if (m_ppEdgeObj[i]->ppEdgeLst[j])
					delete [] (char*)m_ppEdgeObj[i]->ppEdgeLst[j];
			}
			delete [] (char*)m_ppEdgeObj[i]->ppEdgeLst;
		}
		delete m_ppEdgeObj[i];
		m_ppEdgeObj[i] = NULL;
	}

	m_nObjNum = 0;
}

// ��õ�nIdx��Ŀ����������
long ObjEdgeList::ObjArea(int nIdx, int& nArea)
{
	return 0;
}

// ���Ŀ��pObject�����
long ObjEdgeList::ObjArea(PEDGE_OBJECT pObject, int& nArea)
{
	return 0;
}

// ���Ŀ��ı߽��߳���(������Ŀ�����߽���ڱ߽�ĳ����ܺ�)
void ObjEdgeList::GetObjEdgeLength(int** ppLength)
{
	int* pLength = *ppLength;
	if (pLength != NULL)
		delete [] pLength;
	
	// ���Ŀ��߽��߳���
	pLength = new int[m_nObjNum];
	memset(pLength, 0, m_nObjNum);
	for (int i=0; i<m_nObjNum; i++)
	{
		// Ŀ��߽糤�Ȱ�����߽���ڱ߽���ܺ�
		int nLength = 0;
		int nObjEdgeNum = m_ppEdgeObj[i]->nEdgeNum;
		PPEDGE_LIST ppEdgeLst = m_ppEdgeObj[i]->ppEdgeLst;
		for (int j=0; j<nObjEdgeNum; j++)
		{
			int nEdgeLen = ppEdgeLst[j]->nEdgeLen;
			POINT* pPts = ppEdgeLst[j]->edge;
			for (int k=1; k<nEdgeLen; k++)
			{
				nLength += abs(pPts[k].x-pPts[k-1].x) + abs(pPts[k].y-pPts[k-1].y); 
			}
		}
		pLength[i] = nLength;
	}
}
