// FindCrack.cpp: implementation of the CFindCrack class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FindCrack.h"
#include "MathComput.h"
#include "TilCore.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// ��180�ȷ�ΪANGLE_NUM��
#define ANGLE_NUM 18

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFindCrack::CFindCrack(CImage* pSrcImage, CImage* pBinImage)
{
	m_pSrcImage = pSrcImage;
	m_pBinImage = pBinImage;
}

CFindCrack::~CFindCrack()
{
}

/* ==================================================================
 * �������ƣ�  CFindCrack::CrackExtraction()
 * ���ߣ�      [Leng]
 * ������:     [2014-6-2 14:03:20]
 * �����б� 
               [float fLenT] - [In] �߽��ܳ���ֵ
               [ float fAreaT] - [In] �߽������ֵ
               [ float fL2AT] - [In] �߽糤�����������ֵ
               [ float fGaveT] - [In] �߽�ҶȾ�ֵ��ֵ
               [ float fGvariT] - [In] �߽�ҶȾ�������ֵ
			   [ ObjRunLenCode** ppCrackObj] - [Out] ��ȡ�����ѷ���γ̱���
			   [ CRACK_MGR** ppCrackMgr] - [Out] ʶ����ѷ����������
ע�� :	�����ѷ�����ʶ���ѷ�
		������Ŀ��ͼ��Ҫ���Ƕ�ֵͼ�񣬷���������ܳ���		
==================================================================*/
void CFindCrack::CrackExtraction(float fCircLenT, float fAreaT, float fL2AT, float fGaveT, float fGvariT,
								 ObjRunLenCode** ppCrackObj, CRACK_MGR** ppCrackMgr)
{
	int i,j,k;

	// �õ���ֵͼ��������Ŀ���γ̱���
	if (*ppCrackObj != NULL)
		delete (*ppCrackObj);
	*ppCrackObj = new ObjRunLenCode;
	m_pBinImage->ObjAllRLC(*ppCrackObj, 0);

	// ���ϸС��Ŀ��

	// �õ���ֵͼ���и�Ŀ��ı߽��߳���(���ڱ߽�)
// 	CImage* pEdgeImage = CImage(*m_pBinImage);
// 	pEdgeImage->MorphThining();
	ObjEdgeList* pObjEdgeLst = new ObjEdgeList;
	m_pBinImage->ObjAllEdgeLst(pObjEdgeLst, 0);
	int* pLength = new int[pObjEdgeLst->GetObjNum()];
	pObjEdgeLst->GetObjEdgeLength(&pLength);

	// ������Ŀ����з���
	// ��������Ŀ����������ԭʼͼ���еĻҶȾ�ֵ������
	float objArea, objGave, objGvari;
	vector< vector< LINECAP> > lineArray;

	int nObjNum = (*ppCrackObj)->GetObjNum();
	ASSERT(pObjEdgeLst->GetObjNum() == nObjNum);

	// �����ڴ����ڴ洢�ѷ�������Ϣ
	if (*ppCrackMgr != NULL)
		delete [] (char*)ppCrackMgr;
	*ppCrackMgr = (CRACK_MGR*)new char [sizeof(int)+sizeof(CRACK)*nObjNum];
	CRACK* pCracks = (*ppCrackMgr)->elem;
	memset(pCracks, NULL, sizeof(CRACK)*nObjNum);

	// ɸѡ�ѷ�Ŀ��
	int nPixelNum;
	int nCrackObjNum = 0;
	PRUNLENGTHCODE_MGR pObjRLC = NULL;
	for (i=nObjNum-1; i>=0; i--)
	{
		// 1.������ֵɸѡ��Ŀ��ı߽糤�ȡ�
		if (pLength[i] < fCircLenT)
			goto deleteCrack;
			
		// 2.������ֵɸѡ��Ŀ�������
		objArea = (*ppCrackObj)->ObjArea(i);
		if (objArea < fAreaT)
			goto deleteCrack;

		// 3.������ֵɸѡ��Ŀ��߽糤�������֮�ȡ�
		if (pLength[i]/objArea < fL2AT)
			goto deleteCrack;

		// 4.������ֵɸѡ��Ŀ��ҶȾ�ֵ��
		objGave = 0;
		pObjRLC = (*ppCrackObj)->GetObj(i);
		for (j=0; j<pObjRLC->nRunNum; j++)
		{
			int y=pObjRLC->RC[j].y;
			int xStart = pObjRLC->RC[j].xStart;
			int xEnd = pObjRLC->RC[j].xEnd;
			for (k=xStart; k<=xEnd; k++)
				objGave += m_pSrcImage->GetPixelGray(k,y);
		}
		objGave /= objArea;
		if (objGave > fGaveT)
			goto deleteCrack;

		// 5.������ֵɸѡ��Ŀ��Ҷȷ��
		nPixelNum = 0;
		objGvari = 0.0;
		BYTE gray; 
		for (j=0; j<pObjRLC->nRunNum; j++)
		{
			int y=pObjRLC->RC[j].y;
			int xStart = pObjRLC->RC[j].xStart;
			int xEnd = pObjRLC->RC[j].xEnd;
			for (k=xStart; k<=xEnd; k++)
			{
				gray = m_pSrcImage->GetPixelGray(k,y);
				objGvari += (gray-objGave)*(gray-objGave);
			}
			nPixelNum += xEnd-xStart+1;
		}
		objGvari = objGvari/nPixelNum;
		if (objGvari > fGvariT)
			goto deleteCrack;

		// 6.�����ѷ�Ŀ�꼰�����Բ���
		pCracks[nCrackObjNum].fLength = pLength[i];
		pCracks[nCrackObjNum].fArea = objArea;
		pCracks[nCrackObjNum].fGave = objGave;
		pCracks[nCrackObjNum].fGvari = objGvari;
		nCrackObjNum ++;
		continue;

deleteCrack:
		// �����ǰĿ�겻����������ɸѡ������ɾ��Ŀ��
		(*ppCrackObj)->ObjDel(i);
	}

	(*ppCrackMgr)->nNum = nCrackObjNum;

	delete [] pLength;
	delete pObjEdgeLst;
}

/* ==================================================================
 * �������ƣ�  CFindCrack::FindCrack()
 * ���ߣ�      [Leng]
 * ������:     [2014-7-29 21:19:57]
 * �����б� 
			 [float fLengthT] - [In] �߽��ܳ���ֵ
			 [ float fAreaT] - [In] �߽������ֵ
			 [ float fL2AT] - [In] �߽糤�����������ֵ
			 [ float fGaveT] - [In] �߽�ҶȾ�ֵ��ֵ
			 [ float fGvariT] - [In] �߽�ҶȾ�������ֵ
			 [ ObjRunLenCode** ppCrackObj] - [Out] ��ȡ�����ѷ���γ̱���
			 [ CRACK_MGR** ppCrackMgr] - [Out] ʶ����ѷ����������
 ע�� :	�����ѷ�����ʶ���ѷ죨�ѷ�Ҷ�ֵΪ0�������Ҷ�ֵΪ255��
        ������Ŀ��ͼ��Ҫ���Ƕ�ֵͼ�񣬷���������ܳ���		
==================================================================*/
void CFindCrack::FindCrack(float fLengthT, float fAreaT, float fL2AT, float fGaveT, float fGvariT,
			   ObjRunLenCode** ppCrackObj, CRACK_MGR** ppCrackMgr)
{
	int i, j, k;
	
	// �õ���ֵͼ��������Ŀ���γ̱���
	if (*ppCrackObj != NULL)
		delete (*ppCrackObj);
	*ppCrackObj = new ObjRunLenCode;

	// �γ̶�Ӧ���ѷ�����
	vector<CRACK> vCracks;

	// ��ֵϸ��
	CImage* pThinImage = new CImage(*m_pBinImage);
	pThinImage->MorphThining();

	// ���ϸ��ͼ���е�Ŀ���γ̱���
	ObjRunLenCode thinAllObjRLC;
	pThinImage->ObjAllRLC(&thinAllObjRLC, 0);
	delete pThinImage;

	// ����ϸ������ɫ�����ں�������ʱ��������ϸ�����ཻ
	// ���ƶ�ֵͼ�����ں�������
	CImage* pBinImage = new CImage(*m_pBinImage);
	pBinImage->SetObjColor(&thinAllObjRLC, RGB(128,128,128));

	// ��ÿһ��Ŀ����з���
	PPRUNLENGTHCODE_MGR ppRLCMgr = thinAllObjRLC.GetObjAll(); 
	for (i=0; i<thinAllObjRLC.GetObjNum(); i++)
	{
		long lLength, lArea;
		float objGave, objGvari, fMaxWidth = 0.0;

		// 1. ����ϸ���߽���������൱��ϸ���߽��߳��ȣ�ȷ���Ƿ��������
		lLength = thinAllObjRLC.ObjArea(i);
		if (lLength < fLengthT)
			continue;

		// 2. ����ԭ��ֵͼ���ж�Ӧ���ѷ��������з���
		POINT pt;
		pt.x = thinAllObjRLC.GetObj(i)->RC[0].xStart;
		pt.y = thinAllObjRLC.GetObj(i)->RC[0].y;
		ObjRunLenCode binObjRLC; 
		m_pBinImage->ObjRLC(&binObjRLC, pt, 0);
		lArea = binObjRLC.ObjArea(0);
		if (lArea < fAreaT)
			continue;

		// 3.������ֵɸѡ��Ŀ��߽糤�������֮�ȡ�
		if (lLength*1.0/lArea < fL2AT)
			continue;
		
		// 4.������ֵɸѡ��Ŀ��ҶȾ�ֵ��
		objGave = 0;
		PRUNLENGTHCODE_MGR pObjRLC = binObjRLC.GetObj(0);
		for (j=0; j<pObjRLC->nRunNum; j++)
		{
			int y=pObjRLC->RC[j].y;
			int xStart = pObjRLC->RC[j].xStart;
			int xEnd = pObjRLC->RC[j].xEnd;
			for (int x=xStart; x<=xEnd; x++)
				objGave += m_pSrcImage->GetPixelGray(x,y);
		}
		objGave /= lArea;
		if (objGave > fGaveT)
			continue;
		
		// 5.������ֵɸѡ��Ŀ��Ҷȷ��
		objGvari = 0.0;
		BYTE gray; 
		for (j=0; j<pObjRLC->nRunNum; j++)
		{
			int y=pObjRLC->RC[j].y;
			int xStart = pObjRLC->RC[j].xStart;
			int xEnd = pObjRLC->RC[j].xEnd;
			for (int x=xStart; x<=xEnd; x++)
			{
				gray = m_pSrcImage->GetPixelGray(x,y);
				objGvari += (gray-objGave)*(gray-objGave);
			}
		}
		objGvari = objGvari/lArea;
		if (objGvari > fGvariT)
			continue;

		// 6.��ȡ����Ŀ���ϸ���߽��ߣ���������з���
		MATRIX* pMatrix = NULL;
		CopyBuffer(ppRLCMgr[i], &pMatrix);	// ������Ŀ��ϸ���߽������ݷ��������з���
		ClearNode(pMatrix);					// ��������
		vector<deque<POINT>*> vdPoints;		// ���ñ������ڱ�����ѷ��ϸ�����ϵĵ�
		FindEdgePoints(pMatrix, vdPoints);	// �ҵ�Ŀ���ϸ�ϸ���߽����ϵĵ�
		delete pMatrix;

		// 5. ���ݸ�ϸ���߽��ߵĶ˵㼰��ϸ���߽��߶�ԭʼ��ֵͼ����з���
		// 5.1 �ض˵㴦���߽��ߵ���߽��߼н�45�ȷ��򽫸�Ŀ�������Ϊ������
		// 5.2 ȥ��ϸ�������˸������˵�󣬷��������ϸ���ߴ�ֱ�����ֱ�߱�ԭʼ��ֵͼ����Ŀ�����и��ĳ��ȣ�
		//     ��Ϊ�ô����ѷ��ȣ��ݴ��������ѷ���
		// 5.3 ����ȥ��ϸ�������˸������˵����ѷ�����

		CRect rect;
		thinAllObjRLC.ObjRect(i, rect);
		vector<POINT> vPoints;
		int nSize = vdPoints.size();
		for (j=0; j<nSize; j++)
		{
			// ��ϸ�����ϵĵ���С��5ʱ��������������
			int nNum = vdPoints[j]->size();
			if (nNum < 5)	continue;	

			// ���ȫ��ͼ���е�ϸ���߽��
			vPoints.clear();
			for (k=0; k<nNum; k++)
			{
				POINT pt = vdPoints[j]->at(k);
				pt.x = pt.x-1+rect.left;
				pt.y = pt.y-1+rect.top;
				
				vPoints.push_back(pt);
				//pBinImage->SetPixelColor(pt.x, pt.y, RGB(128,128,128)); // ���ϸ������ɫ
			}
			//CString str;
			//str.Format("d:\\%d-%d.bmp", i, j);
			//m_pBinImage->Save(str, CXIMAGE_FORMAT_BMP);

			// �ض˵㴦���߽��ߵ���߽��߼н�45�ȷ��򽫸�Ŀ�������Ϊ������
			//if (!SharpenEnds(pBinImage, vPoints))
			//	continue;
			//str.Format("d:\\%d-%d-sharpen.bmp", i, j);
			//pBinImage->Save(str, CXIMAGE_FORMAT_BMP);

			// ���vPoints��������ѷ������
			POINT maxwidthPoint;
			float fWidth = GetSharpenLineCrackMaxWidth(pBinImage, vPoints, maxwidthPoint);
			if (fMaxWidth < fWidth)
				fMaxWidth = fWidth;
			
			/*
			// ��ĸ����൱�ڳ���
			lLength = vPoints.size();

			// �����
			ObjRunLenCode* pObjRLC = new ObjRunLenCode;
			m_pBinImage->ObjRLC(pObjRLC, maxwidthPoint, 0);
			lArea = pObjRLC->ObjArea(0);

			// ��ҶȾ�ֵ
			objGave = 0.0;
			PRUNLENGTHCODE_MGR pObj = pObjRLC->GetObj(0);
			for (k=0; k<pObj->nRunNum; k++)
			{
				int y=pObj->RC[j].y;
				int xStart = pObj->RC[j].xStart;
				int xEnd = pObj->RC[j].xEnd;
				for (int x=xStart; x<=xEnd; x++)
					objGave += m_pSrcImage->GetPixelGray(x,y);
			}
			objGave = objGave/(lArea*1.0);

			// ��Ŀ��Ҷȷ��
			objGvari = 0.0;
			for (k=0; k<pObj->nRunNum; k++)
			{
				int y=pObj->RC[k].y;
				int xStart = pObj->RC[k].xStart;
				int xEnd = pObj->RC[k].xEnd;
				for (int x=xStart; x<=xEnd; x++)
				{
					gray = m_pSrcImage->GetPixelGray(x,y);
					objGvari += (gray-objGave)*(gray-objGave);
				}
			}
			objGvari = objGvari/(lArea*1.0);
			*/
		}

		// �����Ӧ���ѷ�
		(*ppCrackObj)->ObjAdd(pObjRLC);
		PPRUNLENGTHCODE_MGR ppObjAllRLC = binObjRLC.GetObjAll();
		ppObjAllRLC[0] = NULL;
		//delete pObjRLC;

		// ���������������ѷ켰�ѷ�����
		CRACK crack;
		crack.fArea = lArea;
		crack.fWmax = fMaxWidth;
		crack.fLength = lLength;
		crack.fL2A = lLength/lArea;
		crack.fGave = objGave;
		crack.fGvari = objGvari;
		vCracks.push_back(crack);

		// ɾ������
		for (j=0; j<vdPoints.size(); j++)
			delete vdPoints[j];
	}

	delete pBinImage;

	// ���ѷ����±���
	if (*ppCrackMgr != NULL)
		delete [] *ppCrackMgr;
	*ppCrackMgr = (CRACK_MGR*)new char[sizeof(int)+vCracks.size()*sizeof(CRACK)];
	(*ppCrackMgr)->nNum = vCracks.size();
	for (i=0; i<(*ppCrackMgr)->nNum; i++)
		(*ppCrackMgr)->elem[i] = vCracks[i];
}

// ��Ŀ�꿽�����ڴ���
void CFindCrack::CopyBuffer(PRUNLENGTHCODE_MGR pRLCMgr, MATRIX** ppMatrix)
{
	int i, j;
	
	// ȡ����Ӿ���
	CRect rect;
	rect.left = pRLCMgr->RC[0].xStart;
	rect.right = pRLCMgr->RC[0].xEnd;
	rect.top = pRLCMgr->RC[0].y;
	rect.bottom = pRLCMgr->RC[pRLCMgr->nRunNum-1].y;
	for (i=0; i<pRLCMgr->nRunNum; i++)
	{
		if (rect.left > pRLCMgr->RC[i].xStart)
			rect.left = pRLCMgr->RC[i].xStart;
		if (rect.right < pRLCMgr->RC[i].xEnd)
			rect.right = pRLCMgr->RC[i].xEnd;
	}
	rect.right ++;
	rect.bottom ++;
	
	// ��չһ���߽粢�����ڴ�
	int nWidth, nHeight;
	nWidth = rect.Width()+2;
	nHeight = rect.Height()+2;
	ppMatrix[0] = (MATRIX*)new BYTE [sizeof(int)*2+sizeof(BYTE)*nWidth*nHeight];
	ppMatrix[0]->nWidth = nWidth;
	ppMatrix[0]->nHeight = nHeight;
	memset(ppMatrix[0]->elem, 255, sizeof(BYTE)*nWidth*nHeight);
	
	int xStart, xEnd, y;
	for (i=0; i<pRLCMgr->nRunNum; i++)
	{
		xStart = pRLCMgr->RC[i].xStart;
		xEnd = pRLCMgr->RC[i].xEnd;
		y = pRLCMgr->RC[i].y;
		
		for (j=xStart; j<=xEnd; j++)
		{
			ppMatrix[0]->elem[(y-rect.top+1)*nWidth+(j-rect.left+1)] = 0;
		}	
	}
}

// �Զ�ֵͼ��������ϸ��
void CFindCrack::MorphThining(MATRIX* pMatrix)
{
	BYTE	rgb;
	LONG    x,y;
	int		num;
	BOOL    Finished;
	int     nw,n,ne,w,e,sw,s,se;
	static int erasetable[256]={
				0,0,1,1,0,0,1,1,
				1,1,0,1,1,1,0,1,
				1,1,0,0,1,1,1,1,
				0,0,0,0,0,0,0,1,
				
				0,0,1,1,0,0,1,1,
				1,1,0,1,1,1,0,1,
				1,1,0,0,1,1,1,1,
				0,0,0,0,0,0,0,1,
				
				1,1,0,0,1,1,0,0,
				0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,
				
				1,1,0,0,1,1,0,0,
				1,1,0,1,1,1,0,1,
				0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,

				0,0,1,1,0,0,1,1,
				1,1,0,1,1,1,0,1,
				1,1,0,0,1,1,1,1,
				0,0,0,0,0,0,0,1,
				
				0,0,1,1,0,0,1,1,
				1,1,0,1,1,1,0,1,
				1,1,0,0,1,1,1,1,
				0,0,0,0,0,0,0,0,
				
				1,1,0,0,1,1,0,0,
				0,0,0,0,0,0,0,0,
				1,1,0,0,1,1,1,1,
				0,0,0,0,0,0,0,0,

				1,1,0,0,1,1,0,0,
				1,1,0,1,1,1,0,0,
				1,1,0,0,1,1,1,0,
				1,1,0,0,1,0,0,0
		  };

	long x1, y1, x2, y2;

	// ͼ��ɫ�����߽��߱�ɺ�ɫ
	int nWidth = pMatrix->nWidth;
	int nHeight = pMatrix->nHeight;
	BYTE* pElem = pMatrix->elem;
	for (y=0; y<nHeight; y++)
	{
		for (x=0; x<nWidth; x++)
		{
			pElem[y*nWidth+x] = 255-pElem[y*nWidth+x];
		}
	}

	// ����ͼ�����
	MATRIX* pMatrixNew = (MATRIX*)new char [sizeof(pMatrix)];
	memcpy(pMatrixNew, pMatrix, sizeof(pMatrix));
	BYTE* pElemNew = pMatrixNew->elem;
	
	Finished=FALSE;
	while (!Finished)
	{
		Finished = TRUE;
		for (y=1; y<nHeight-1; y++)
		{
			x1 = 0; y1 = nHeight-y-1;
			x2 = x1; y2 = y1;
			x=1; 
			while(x<nWidth-1)
			{
				rgb = pElem[y*nWidth+(x1+x)];//GetPixelColor(x1+x, y);
				if(rgb==0)
				{
					w = (unsigned char)pElem[y*nWidth+(x1+x-1)];
					e = (unsigned char)pElem[y*nWidth+(x1+x+1)];//GetPixelColor(x1+x+1, y).rgbBlue;
					
					if( (w==255)|| (e==255))
					{
						nw = (unsigned char)pElem[(y+1)*nWidth+(x1+x-1)];//GetPixelColor(x1+x-1, y+1).rgbBlue;
						n  = (unsigned char)pElem[(y+1)*nWidth+(x1+x)];//GetPixelColor(x1+x  , y+1).rgbBlue;
						ne = (unsigned char)pElem[(y+1)*nWidth+(x1+x+1)];//GetPixelColor(x1+x+1, y+1).rgbBlue;
						sw = (unsigned char)pElem[(y-1)*nWidth+(x1+x-1)];//GetPixelColor(x1+x-1, y-1).rgbBlue;
						s  = (unsigned char)pElem[(y-1)*nWidth+(x1+x)];//GetPixelColor(x1+x  , y-1).rgbBlue;
						se = (unsigned char)pElem[(y-1)*nWidth+(x1+x+1)];//GetPixelColor(x1+x+1, y-1).rgbBlue;
						
						num=nw/255+n/255*2+ne/255*4+w/255*8+e/255*16+sw/255*32+s/255*64+se/255*128;
						if(erasetable[num]==1)
						{
							pElem[y*nWidth+(x1+x)] = 255;
							pElemNew[y*nWidth+(x2+x)] = 255;
							//SetPixelColor(x1+x, y, RGB(255, 255, 255));
							//image.SetPixelColor(x2+x, y, RGB(255, 255, 255));
							Finished=FALSE;
							x++;
						}
					}
				}
				x++;
			}
		}
		
		for (x=1; x<nWidth-1; x++)
		{ 
			y=1;
			while(y<nHeight-1)
			{
				x1 = 0;		y1 = nHeight-y-1;
				x2 = x1;	y2 = y1;
				
				rgb = pElem[y*nWidth+(x1+x)];//GetPixelColor(x1+x, y);
				if(rgb==0)
				{
					n = (unsigned char)pElem[(y+1)*nWidth+(x1+x)];//GetPixelColor(x1+x, y+1).rgbBlue;
					s = (unsigned char)pElem[(y-1)*nWidth+(x1+x)];//GetPixelColor(x1+x, y-1).rgbBlue;
					
					if( (n==255)|| (s==255))
					{
						nw = (unsigned char)pElem[(y+1)*nWidth+(x1+x-1)];//GetPixelColor(x1+x-1, y+1).rgbBlue;
						ne = (unsigned char)pElem[(y+1)*nWidth+(x1+x+1)];//GetPixelColor(x1+x+1, y+1).rgbBlue;
						w  = (unsigned char)pElem[y*nWidth+(x1+x-1)];//GetPixelColor(x1+x-1, y  ).rgbBlue;
						e  = (unsigned char)pElem[y*nWidth+(x1+x+1)];//GetPixelColor(x1+x+1, y  ).rgbBlue;
						sw = (unsigned char)pElem[(y-1)*nWidth+(x1+x-1)];//GetPixelColor(x1+x-1, y-1).rgbBlue;
						se = (unsigned char)pElem[(y-1)*nWidth+(x1+x+1)];//GetPixelColor(x1+x+1, y-1).rgbBlue;
						
						num=nw/255+n/255*2+ne/255*4+w/255*8+e/255*16+sw/255*32+s/255*64+se/255*128;
						if(erasetable[num]==1)
						{
							pElem[y*nWidth+(x1+x)] = 255;
							pElemNew[y*nWidth+(x2+x)] = 255;
							//SetPixelColor(x1+x, y, RGB(255, 255, 255));
							//image.SetPixelColor(x2+x, y, RGB(255, 255, 255));
							Finished=FALSE;
							y++;
						}
					}
				}
				y++;
			}
		} 
	}

	// ���߽��߷�ɫ�ɰ�ɫ
	for (y=0; y<nHeight; y++)
	{
		for (x=0; x<nWidth; x++)
		{
			pElemNew[y*nWidth+x] = 255-pElemNew[y*nWidth+x];
		}
	}
	
	// ����ͼ���������ͼ����
	memcpy(pMatrix, pMatrixNew, sizeof(pMatrix));

	// �����ʱ�����ڴ�
	delete [] (char*)pMatrixNew;
}

// ��������ؿ�߽��ֵͼ���еĵ���Ŀ�������б߽��ߵķֲ��
void CFindCrack::ClearNode(MATRIX* pMatrix)
{
	// �������Ŀ��ɫ��
	int i,j,m,n;
	BYTE gray;
	int nCount;
	int nWidth = pMatrix->nWidth;
	int nHeight = pMatrix->nHeight;

	// ���Ŀ��߽��еķֲ��
	vector<POINT> vKeyPts;
	POINT pt;
	for (i=1; i<nWidth-1; i++)
	{
		for (j=1; j<nHeight-1; j++)
		{
			gray = pMatrix->elem[j*nWidth+i];
			if (gray != 0)
				continue;
			
			nCount = 0;
			for (m=-1; m<=1; m++)
			{
				for (n=-1; n<=1; n++)
				{
					gray = pMatrix->elem[(j+n)*nWidth+(i+m)];
					if (gray == 0)
						nCount ++;
				}
			}
			
			// ����ǰ����8������3��Ŀ�����ص����Ŀ�����ص�ʱ��ֱ�������
			if (nCount>=4)	// nCount���������ĵ�
			{
				pt.x = i;
				pt.y = j;
				vKeyPts.push_back(pt);
			}
		}
	}

	// �����ҵ��Ľڵ��Ϊ������
	for (i=0; i<vKeyPts.size(); i++)
	{
		pt = vKeyPts[i];
		pMatrix->elem[pt.y*nWidth+pt.x] = 255;
	}
}

// ����Ŀ��߽����ϵĵ�
void CFindCrack::FindEdgePoints(MATRIX* pMatrix, vector<deque<POINT>*>& vdPoints)
{
	int i;
	int nWidth = pMatrix->nWidth;
	int nHeight = pMatrix->nHeight;
	BYTE* pElem = pMatrix->elem;

	//////////////////////////////////////////////////////////////////////////
	// ����8�����ƽ������
	CPoint move8[8];
	move8[0].x = 1, move8[0].y = 0;	// ��1��
	move8[1].x = 1, move8[1].y =-1;	// ��2��
	move8[2].x = 0, move8[2].y =-1;	// ��3��
	move8[3].x =-1, move8[3].y =-1;	// ��4��
	move8[4].x =-1, move8[4].y = 0;	// ��5��
	move8[5].x =-1, move8[5].y = 1;	// ��6��
	move8[6].x = 0, move8[6].y = 1;	// ��7��
	move8[7].x = 1, move8[7].y = 1;	// ��8��

	// �����ҵ���ǰ��ʱ�����ҵ�ǰ���8�����4��������ʼ�������·�ʽ����
	int nPrevOri8[] = {7,7,1,1,3,3,5,5};

	CPoint ptEnd, prevPt, curPt, nextPt;
	int nPrevOri, nNextOri;

	while(TRUE)
	{
		// ��������ֱ�߻������ߣ�������ѭ��
		int nClosed = IsClosed(pMatrix, ptEnd);
		if (nClosed < 0)
			break;

		deque<POINT>* pPoints = new deque<POINT>;

		// ��ǵ�һ��
		prevPt = ptEnd;
		pPoints->push_back(prevPt);
		
		// ����һ���㼰����
		nNextOri = -1;
		for (i=5; i<5+8; i++)
		{
			nextPt = prevPt+move8[i%8];
			if (pElem[nextPt.y*nWidth+nextPt.x] == 0)
			{
				nNextOri = i%8;
				pPoints->push_back(nextPt);
				break;
			}
		}
		if (nNextOri < 0) // ���С��0��˵��Ϊһ������
		{
			vdPoints.push_back(pPoints);
			goto setLineColor;
		}

		if (nClosed == 1)	// ���Ϊ�������
		{
			while (TRUE)
			{
				nPrevOri = nPrevOri8[nNextOri];
				
				// �ӵ�nPrevOri����ʼ����8�����
				for (i=nPrevOri; i<nPrevOri+8; i++)
				{
					nextPt = prevPt+move8[i%8];
					if (pElem[nextPt.y*nWidth+nextPt.x] == 0)
					{
						nNextOri = i%8;
						pPoints->push_back(nextPt);
						break;
					}
				}
				
				// ����һ��͵�һ����ͬʱ��˵���ѱ���һ��
				if (nextPt == ptEnd)
				{
					pPoints->pop_back();
					vdPoints.push_back(pPoints);
					break;
				}
				
				// 
				prevPt = nextPt;
			}
		}
		else	// ������Ƿ������
		{
			curPt = nextPt;
			
			while (TRUE)
			{
				nPrevOri = nPrevOri8[nNextOri];
				
				// �ӵ�nPrevOri����ʼ����8�����
				for (i=nPrevOri; i<nPrevOri+8; i++)
				{
					nextPt = curPt+move8[i%8];
					if (pElem[nextPt.y*nWidth+nextPt.x] == 0)
					{
						nNextOri = i%8;
						pPoints->push_back(nextPt);
						break;
					}
				}
				
				// ����һ��͵�һ����ͬʱ��˵���ѱ�������һ���˵�
				if (nextPt == prevPt)
				{
					pPoints->pop_back();
					vdPoints.push_back(pPoints);
					break;
				}
				
				// ��������
				prevPt = curPt;
				curPt = nextPt;
			}
		}

setLineColor:
		// ���Ѿ������ĵ�����Ϊ����ɫ
		int nSize = pPoints->size();
		for (i=0; i<nSize; i++)
		{
			POINT pt = (*pPoints)[i];
			pMatrix->elem[pt.y*nWidth+pt.x] = 255;
		}
	}
}

// �Զ����ɵĶ���߽���������ϣ������ض���߳���
float CFindCrack::LinesFit(const deque<POINT>& dPoints, const float& fDist, vector<POINT>& vLines)
{
	float fLength = 0.0;

	int nIdx1, nIdx2;
	POINT pt, pt1, pt2;
	int nSum = dPoints.size();
	nIdx1 = 0;
	nIdx2 = 1;
	pt1 = dPoints[0];
	vLines.push_back(pt1);
	while (nIdx2<nSum)
	{
		pt2 = dPoints[nIdx2];
		
		for (int i=nIdx1+1; i<=nIdx2-1; i++)
		{
			pt = dPoints[i];
			if (point_line_dist(pt, pt1, pt2) > fDist)
			{
				vLines.push_back(dPoints[nIdx2-1]);
				fLength += get_distance(pt1, dPoints[nIdx2-1]);

				pt1 = dPoints[nIdx2-1];
				nIdx1 = nIdx2-1;
				break;
			}
		}
		
		nIdx2++;
	}
	
	// ���һ��
	vLines.push_back(dPoints[nIdx2-1]);
	fLength += get_distance(pt1, dPoints[nIdx2-1]);

	return fLength;
}

// �Զ����ɵĶ�ν����������
void CFindCrack::LinesFit(const deque<POINT>& dPoints, const float& fDist, vector<LINECAP>& vLines)
{
	int nIdx1, nIdx2;
	POINT pt, pt1, pt2;
	LINECAP line;
	int nSum = dPoints.size();
	nIdx1 = 0;
	nIdx2 = 1;
	pt1 = dPoints[0];
	line.pt1 = pt1;
	while (nIdx2<nSum)
	{
		pt2 = dPoints[nIdx2];
		
		for (int i=nIdx1+1; i<=nIdx2-1; i++)
		{
			pt = dPoints[i];
			if (point_line_dist(pt, pt1, pt2) > fDist)
			{
				line.pt2 = dPoints[nIdx2-1];
				line.fAngle = get_angle(line.pt1, line.pt2);
				vLines.push_back(line);
				
				line.pt1 = line.pt2;
				pt1 = line.pt1;
				nIdx1 = nIdx2-1;
				break;
			}
		}
		
		nIdx2++;
	}
	
	// ���һ��
	line.pt2 = dPoints[nIdx2-1];
	line.fAngle = get_angle(line.pt1, line.pt2);
	vLines.push_back(line);
}

// �ж��Ƿ��Ƿ�ձ߽��ߣ������ǣ���ȡ��һ���߽�˵�
// ����ֵ��-1: MATRIX��Ŀ���; 0: ��Ŀ��߽�,��δ���; 1: ��Ŀ��߽�,���ѷ��
int CFindCrack::IsClosed(MATRIX* pMatrix, POINT& ptEnd)
{
	int i, j;
	CPoint prevPt, curPt, nextPt, startPt;
	int nPrevOri, nNextOri;

	int nWidth = pMatrix->nWidth;
	int nHeight = pMatrix->nHeight;
	BYTE* pElem = pMatrix->elem;

	// ����8�����ƽ������
	CPoint move8[8];
	move8[0].x = 1, move8[0].y = 0;	// ��1��
	move8[1].x = 1, move8[1].y =-1;	// ��2��
	move8[2].x = 0, move8[2].y =-1;	// ��3��
	move8[3].x =-1, move8[3].y =-1;	// ��4��
	move8[4].x =-1, move8[4].y = 0;	// ��5��
	move8[5].x =-1, move8[5].y = 1;	// ��6��
	move8[6].x = 0, move8[6].y = 1;	// ��7��
	move8[7].x = 1, move8[7].y = 1;	// ��8��
	
	// �����ҵ���ǰ��ʱ�����ҵ�ǰ���8�����4��������ʼ�������·�ʽ����
	int nPrevOri8[] = {7,7,1,1,3,3,5,5};
	
	// ��ǲ��ҵ��ĵ�һ��Ŀ�����ص�
	for (i=1; i<nWidth-1; i++)
	{
		for (j=1; j<nHeight-1; j++)
		{
			if (pElem[j*nWidth+i] == 0)
			{
				startPt.x = i;
				startPt.y = j;
				break;
			}	
		}
		if (j < nHeight-1)
			break;
	}

	// �������������������˵����Ŀ��㣬ֱ�ӷ��أ�1
	if (i>=nWidth-1)
		return -1;

	// ����1�����긳�����ص������ֵ
	prevPt = startPt;

	// ����1�����8�����Ƿ���1��Ŀ������Ŀ��㣬���ǣ���õ�Ϊ�˵㣬ֱ���˳�
	int nNum = 0;
	for (i=0; i<8; i++)
	{
		CPoint ptTmp = prevPt+move8[i];
		if (pElem[ptTmp.y*nWidth+ptTmp.x] == 0)
			nNum ++;
	}	
	if (nNum == 1 || nNum == 0)
	{
		ptEnd = startPt;
		return 0;
	}
	
	// �ӵ�4�㿪ʼ���ҵ�point����һ��8�������ɫΪuObjClr�ĵ�
	for (i=5; i<5+8; i++)
	{
		curPt = prevPt+move8[i%8];
		if (pElem[curPt.y*nWidth+curPt.x] == 0)
		{
			nNextOri = i%8;
			break;
		}
	}
	
	while (TRUE)
	{
		nPrevOri = nPrevOri8[nNextOri];
		
		// �ӵ�nPrevOri����ʼ����8�����
		for (i=nPrevOri; i<nPrevOri+8; i++)
		{
			nextPt = curPt+move8[i%8];
			if (pElem[nextPt.y*nWidth+nextPt.x] == 0)
			{
				nNextOri = i%8;
				break;
			}
		}

		// ��ǰһ��ͺ�һ����ͬʱ��˵����ǰ��Ϊ�˵�
		if (nextPt == prevPt)
		{
			ptEnd = curPt;
			return 0;
		}
		else if (nextPt == startPt)	// �����һ�����һ����ͬ����Ϊ�������
		{
			ptEnd = nextPt;
			return 1;
		}	
		
		// 
		prevPt = curPt;
		curPt = nextPt;
	}
	
}

// ��������ϵı߽��߽��з���,��ͬ����һ���߽��ߵĸ��߶ν������ӹ�ϵ
void CFindCrack::LinkSimilarLines(vector< vector< LINECAP> >& lineArray)
{
	int i, j, m, n;

	// ��������Ŀ����бȽϷ���
	float fRelation;
	int nEnd1, nEnd2, nSum, nSum1, nSum2;
	vector< vector< RELATION* > > vvRelation;
	nSum = lineArray.size();
	for (i=0; i<nSum; i++)
	{
		vector< RELATION* > vRelation;
		vvRelation.push_back(vRelation);
	}

	float fDsumT, fDminT, fAngleT, fLengthT;
	fDsumT = 40;	fDminT = 50;	fAngleT = 30;	fLengthT = 20;
	for (i=0; i<nSum-1; i++)
	{
		vector<LINECAP> lineCaps1 = lineArray[i];
		for (j=i+1; j<nSum; j++)
		{
			vector<LINECAP> lineCaps2 = lineArray[j];
			if (BuildAdjacent(lineCaps1, lineCaps2, fDsumT, fDminT,	
							fAngleT, fLengthT, nEnd1, nEnd2, fRelation))
			{
				// ��¼�໥��ϵ
				PRELATION pRelation = new RELATION;
				pRelation->fRelation = fRelation;
				pRelation->nLine1 = i;
				pRelation->nEnd1 = nEnd1;
				pRelation->nLine2 = j;
				pRelation->nEnd2 = nEnd2;
				vvRelation[i].push_back(pRelation);

				PRELATION pRelation1 = new RELATION;
				pRelation1->fRelation = fRelation;
				pRelation1->nLine1 = j;
				pRelation1->nEnd1 = nEnd2;
				pRelation1->nLine2 = i;
				pRelation1->nEnd2 = nEnd1;
				vvRelation[j].push_back(pRelation1);
			}
		} 
	}

	// ���߽��߸�������Ӧ��������͹���
	PRELATION pTmp;
	for (i=0; i<nSum; i++)
	{
		nSum1 = vvRelation[i].size();

		// ���������ֵ���Ӵ�С����
		for (m=0; m<nSum1-1; m++)
		{
			for (n=m+1; n<nSum1; n++)
			{
				if (vvRelation[i][m]->fRelation < vvRelation[i][n]->fRelation)
				{
					pTmp			 = vvRelation[i][m];
					vvRelation[i][m] = vvRelation[i][n];
					vvRelation[i][n] = pTmp;
				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// ȷ��ͬһ�߽����ϸ������ڱ߽��ߵĶ�Ӧ��ϵ
	BOOL* pMark = new BOOL [nSum];
	memset(pMark, 0, sizeof(BOOL)*nSum);
	vector< deque< POINT > > vdPoints;
	for (i=0; i<nSum; i++)
	{
		// �����ǰ�߽��߱���ǹ���û����֮���Ӧ�ı߽磬�������һ���߽���
		if (pMark[i])
			continue;
		pMark[i] = TRUE;

		deque< POINT > dPoints;
		int nCurLine, nCurEnd, nLine1, nEnd1, nLine2, nEnd2;

		// �����ǰ�߽��������߽粻��������ֱ���˳�
		if (vvRelation[i].size() == 0)
		{
			dPoints.push_back(lineArray[i][0].pt1);
			for (j=0; j<lineArray[i].size(); j++)
			{
				dPoints.push_back(lineArray[i][j].pt2);
			}
			vdPoints.push_back(dPoints);
			continue;
		}

		// ���ҵ�i���߽��ߵ�����һ���˵����Ӧ�ı߽���
 		nCurLine = vvRelation[i][0]->nLine1;
		nCurEnd  = vvRelation[i][0]->nEnd1;
		nLine1 = nCurLine;
		nEnd1  = nCurEnd;

		// �洢��ǰ�߽��ߵĶ˵�
		if (nEnd1 == 0)	// �߽���ͷ
		{
			dPoints.push_front(lineArray[i][0].pt1);
			for (j=0; j<lineArray[i].size(); j++)
			{
				dPoints.push_front(lineArray[i][j].pt2);
			}
		}
		else // �߽���β
		{
			dPoints.push_back(lineArray[i][0].pt1);
			for (j=0; j<lineArray[i].size(); j++)
			{
				dPoints.push_back(lineArray[i][j].pt2);
			}
		}

		// �����뵱ǰ�߽��߶˵�nCurEnd���ڵ���һ�߽��ߣ�ֱ��û�����ڱ߽�
		while (FindNextNeighborEdge(vvRelation, nLine1, nEnd1, nLine2, nEnd2)
			&& !pMark[nLine2])
		{
			pMark[nLine2] = TRUE;

			if (nEnd2 == 0)	// �߽���ͷ
			{
				dPoints.push_back(lineArray[nLine2][0].pt1);
				for (j=0; j<lineArray[nLine2].size(); j++)
				{
					dPoints.push_back(lineArray[nLine2][j].pt2);
				}
			}
			else	// �߽���β
			{
				nSum2 = lineArray[nLine2].size();
				dPoints.push_back(lineArray[nLine2][nSum2-1].pt2);
				for (j=nSum2-1; j>=0; j--)
				{
					dPoints.push_back(lineArray[nLine2][j].pt1);
				}
			}

			nLine1 = nLine2;
			nEnd1  = 1-nEnd2;
		}

		// �����뵱ǰ�߽��߶˵�1-nCurEnd���ڵ���һ�߽��ߣ�ֱ��û�����ڱ߽�
		nLine1 = nCurLine;
		nEnd1  = 1-nCurEnd;
		while (FindNextNeighborEdge(vvRelation, nLine1, nEnd1, nLine2, nEnd2)
			&& !pMark[nLine2])
		{
			pMark[nLine2] = TRUE;
			
			if (nEnd2 == 0)	// �߽���ͷ
			{
				dPoints.push_front(lineArray[nLine2][0].pt1);
				for (j=0; j<lineArray[nLine2].size(); j++)
				{
					dPoints.push_front(lineArray[nLine2][j].pt2);
				}
			}
			else	// �߽���β
			{
				nSum2 = lineArray[nLine2].size();
				dPoints.push_front(lineArray[nLine2][nSum2-1].pt2);
				for (j=nSum2-1; j>=0; j--)
				{
					dPoints.push_front(lineArray[nLine2][j].pt1);
				}
			}

			nLine1 = nLine2;
			nEnd1  = 1-nEnd2;
		}

		vdPoints.push_back(dPoints);
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// �Ա߽��߽���ɸѡ���ҵ�����Ҫ��ı߽���
	// ���ݱ߽��߳��Ƚ���ɸѡ
	nSum = vdPoints.size();
	float fLength;
	vector< int > vBad;
	for (i=0; i<nSum; i++)
	{
		fLength = 0;
		nSum1 = vdPoints[i].size();
		for (j=0; j<nSum1-1; j++)
		{
			fLength += get_distance(vdPoints[i][j], vdPoints[i][j+1]);
		}

		// ���ݳ�����ֵ�ж��Ƿ����߽���,��¼���豣���߽�����
		if (fLength <= fLengthT)
			vBad.push_back(i);
	}

	// ɾ������Ҫ�����ı߽���
	nSum = vBad.size();
	for (i=nSum-1; i>=0;  i--)
	{
		vdPoints.erase(vdPoints.begin()+vBad[i]);
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// ���߽����ٽ���������Ϻ�
	POINT2DF pt;
	vector<vector<LINECAP> > vvLinesFit;

	nSum = vdPoints.size();
	for (i=0; i<nSum; i++)
	{
		vector<LINECAP> vLines;
		LinesFit(vdPoints[i], 3, vLines);
		vvLinesFit.push_back(vLines);
	}

	//////////////////////////////////////////////////////////////////////////
	// ͳ�ƽǶȷ�Χ
	int nNum[ANGLE_NUM];
	StatAngles(vvLinesFit, nNum);
	vector<int> vSum;
	StatLines(vvLinesFit, vSum);
	int nNumber = vSum.size();
	int* pNum = new int [nNumber];
	for (i=0; i<nNumber; i++)
	{
		pNum[i] = vSum[i];
		TRACE("\n%d", pNum[i]);
	}
	delete [] pNum;
	//////////////////////////////////////////////////////////////////////////

	// ɾ��ָ��ָ����ڴ�
	for (i=0; i<vvRelation.size(); i++)
	{
		for (j=0; j<vvRelation[i].size(); j++)
		{
			delete vvRelation[i][j];
		}
	}
	delete [] pMark;
}

// �����ֱ�߼�н�
float CFindCrack::GetAngle(const LINECAP& line1, const LINECAP& line2)
{
	float fRetAngle;
	if (fabs(line1.fAngle-line2.fAngle) < 90)
		fRetAngle = fabs(line1.fAngle-line2.fAngle);
	else
		fRetAngle = 180-fabs(line1.fAngle-line2.fAngle);

	return fRetAngle;
}

// �����ֱ�߼�˵㵽��һֱ�ߵľ����
float CFindCrack::GetDistSum(const LINECAP& line1, const int& nIdx1,
									const LINECAP& line2, const int& nIdx2)
{
	float fD1, fD2;
	if (nIdx1==0)
		fD1 = point_line_dist(line1.pt1, line2.pt1, line2.pt2);
	else
		fD1 = point_line_dist(line1.pt2, line2.pt1, line2.pt2);

	if (nIdx2==0)
		fD2 = point_line_dist(line2.pt1, line1.pt1, line1.pt2);
	else
		fD2 = point_line_dist(line2.pt2, line1.pt1, line1.pt2);

	return (fD1+fD2);
}

// �ж���ֱ�ߵ�����һ���Ķ˵㵽��һ���߶εĴ����Ƿ����߶���
BOOL CFindCrack::IsVertPtInLine(const LINECAP& line1, const LINECAP& line2)
{
	POINT2DF pt1_1, pt1_2, pt2_1, pt2_2;
	pt1_1.x = line1.pt1.x;	pt1_1.y = line1.pt1.y;
	pt1_2.x = line1.pt2.x;	pt1_2.y = line1.pt2.y;
	pt2_1.x = line2.pt1.x;	pt2_1.y = line2.pt1.y;
	pt2_2.x = line2.pt2.x;	pt2_2.y = line2.pt2.y;
	
	// �ж�L1���˵�a1,a2��L2�ϵĴ����Ƿ���L2�ϼ�L2���˵㵽L1�Ĵ����Ƿ���L1��
	POINT2DF ptVert;
	get_vert_point_in_line(pt1_1, pt2_1, pt2_2, ptVert);
	if (point_in_line(ptVert, pt2_1, pt2_2) == POINT_IN_LINE)
			return TRUE;

	get_vert_point_in_line(pt1_2, pt2_1, pt2_2, ptVert);
	if (point_in_line(ptVert, pt2_1, pt2_2) == POINT_IN_LINE)
		return TRUE;

	get_vert_point_in_line(pt2_1, pt1_1, pt1_2, ptVert);
	if (point_in_line(ptVert, pt1_1, pt1_2) == POINT_IN_LINE)
		return TRUE;

	get_vert_point_in_line(pt2_2, pt1_1, pt1_2, ptVert);
	if (point_in_line(ptVert, pt1_1, pt1_2) == POINT_IN_LINE)
		return TRUE;

	return FALSE;
}

// ������߶ζ˵�����С���룬����¼��С�����
// float& fMinDist: ������С����
// int& nIdx1, int& nIdx2: ��С��������˵�,0��ʾ��1���˵�,1��ʾ��2���˵�
float CFindCrack::GetMinDist(const LINECAP& line1, const LINECAP& line2,
														float& fMinDist, int& nIdx1, int& nIdx2)
{
	float fD1, fD2, fD3, fD4;
	// ��L1��L2���߶ξ�����������˵��ľ���
	fD1 = get_distance(line1.pt1, line2.pt1);
	fD2 = get_distance(line1.pt1, line2.pt2);
	fD3 = get_distance(line1.pt2, line2.pt1);
	fD4 = get_distance(line1.pt2, line2.pt2);

	fMinDist = fD1;
	nIdx1 = 0;
	nIdx2 = 0;
	if (fMinDist>fD2)
	{
		fMinDist = fD2;
		nIdx1 = 0;
		nIdx2 = 1;
	}
	if (fMinDist>fD3)
	{
		fMinDist = fD3;
		nIdx1 = 1;
		nIdx2 = 0;
	}
	if (fMinDist>fD4)
	{
		fMinDist = fD4;
		nIdx1 = 1;
		nIdx2 = 1;
	}

	return fMinDist;
}

// �����������������߽��ߵ�����������
// const vector<LINECAP>& lineCaps1: �߽���1
// const vector<LINECAP>& lineCaps2: �߽���2
// const float& fDsumT, const float& fDminT: �����ж�������������ֵ
// const float& fAngleT, const float& fLengthT: �����ж�������������ֵ
// int& nEnd1, int&nEnd2: ���ر߽���1��2�Ķ˵����,0Ϊ��1���˵�,1Ϊ���һ���˵�
// float& fRelation: �����
// ���������ڵ�ɹ�����TRUE�����򷵻�FALSE
BOOL CFindCrack::BuildAdjacent(
	   const vector<LINECAP>& lineCaps1,
	   const vector<LINECAP>& lineCaps2, 
	   const float& fDsumT, const float& fDminT,
	   const float& fAngleT, const float& fLengthT,
	   int& nEnd1, int& nEnd2, float& fRelation)
{
	int i, j, nIdx1, nIdx2;
	int nSegNum1, nSegNum2;
	float fDmin, fLen1, fLen2, fDsum, fAngle;
	nSegNum1 = lineCaps1.size();
	nSegNum2 = lineCaps2.size();

	int nNum1, nNum2;
	nNum1 = (nSegNum1>1)?2:1;
	nNum2 = (nSegNum2>1)?2:1;
	for (i=0; i<nNum1; i++)
	{
		nIdx1 = (i<1)?0:nSegNum1-1;
		LINECAP line1 = lineCaps1[nIdx1];
		for (j=0; j<nNum2; j++)
		{
			nIdx2 = (j<1)?0:nSegNum2-1;
			LINECAP line2 = lineCaps2[nIdx2];

			// �������߶ζ˵��ľ��룬�ҳ���С����Ͷ˵�
			GetMinDist(line1, line2, fDmin, nEnd1, nEnd2);
			fLen1 = get_distance(line1.pt1, line1.pt2);
			fLen2 = get_distance(line2.pt1, line2.pt2);
			// ��fDmin���ڸ�����ֵ�����line1��line2�ĳ���,�򷵻�
			if (fDmin>fDminT || (fDmin>0.75*fLen1 && fDmin>0.75*fLen2))
				return FALSE;

			if (nNum1 > 1)	// ����1������߲�ֹ1��
			{
				if (i == 0)
				{
					if (nNum2 > 1)
					{
						if (j == 0)
						{
							if (nEnd1!=0 || nEnd2!=0)
								continue;
						}
						else
						{
							if (nEnd1!=0 || nEnd2!=1)
								continue;
						}
					}
					else
					{
						if (nEnd1!=0)
							continue;
					}
				}
				else	// i==1
				{
					if (nNum2 > 1)
					{
						if (j == 0)
						{
							if (nEnd1!=1 || nEnd2!=0)
								continue;
						}
						else
						{
							if (nEnd1!=1 || nEnd2!=1)
								continue;
						}
					}
					else
					{
						if (nEnd1!=1)
							continue;
					}
				}
			}
			else	// nNum==1
			{
				if (nNum2 > 1)
				{
					if (j == 0)
					{
						if (nEnd2 != 0)
							continue;
					}
					else
					{
						if (nEnd2 != 1)
							continue;
					}
				}
			}

			// ������ֱ�߼�һ��ֱ�����˵㵽��һ��ֱ�ߵľ���
			fDsum = GetDistSum(line1, nEnd1, line2, nEnd2);
			// ��fDsumС�ڸ�����ֵ�������
			if (fDsum > fDsumT)
				return FALSE;

			// �������߶μ�ļн�
			fAngle = GetAngle(line1, line2);
			// ��fAС�ڸ�����ֵ�������
			if (fAngle > fAngleT)
				return FALSE;

			// ����line1�Ķ˵�nEnd1��line2�˵�nEnd2��������line1��ļн�
#if 0
			POINT pt0, pt1, pt2;
			pt0 = (nEnd1==0)?line1.pt2:line1.pt1;
			pt1 = (nEnd1==0)?line1.pt1:line1.pt2;
			pt2 = (nEnd2==0)?line2.pt1:line2.pt2;
			fAngle1 = get_angle(pt0,pt1,pt2);
			fAngle1 = 180-fAngle1*180/PI;
			if (fAngle1 > 45)
				return FALSE;
#endif
			
			// �ж�line1�Ƿ���line2���ӳ����ϣ�ͬʱline2�Ƿ���line2���ӳ�����
			if (IsVertPtInLine(line1, line2))
				return FALSE;
			
			// �������߽��ߵ�����ԣ�ϵ��ѡ��������д���֤
			fRelation = 2/fDsum+1/fDmin+1/(fAngle+10);
			
			return TRUE;
		}
	}	

	return FALSE;
}

// �����뵱ǰ�߽�����������һ���߽���
// const CArray<LINECAP, LINECAP>& lineCaps : ԭʼ�߽���
// const vector< vector< Relation* > >& vvRelation : �߽��߼���
// const int& nLine1, const int& nEnd1 : �Ѿ��߽��߼��˵�
// int& nLine2, int& nEnd2 : ��������֪�߽��������ı߽��ߺͶ˵�
BOOL CFindCrack::FindNextNeighborEdge( 
				  const vector< vector< RELATION* > >& vvRelation, 
				  const int& nLine1, const int& nEnd1,
				  int& nLine2, int& nEnd2)
{
	int i, nSum1, nSum2, nEndIdx;

	// ���ҵ�1��������nEnd1��������һ���߽���
	nSum1 = vvRelation[nLine1].size();
	for (i=0; i<nSum1; i++)
	{
		if (vvRelation[nLine1][i]->nEnd1 == nEnd1)
		{
			nLine2 = vvRelation[nLine1][i]->nLine2;
			nEnd2  = vvRelation[nLine1][i]->nEnd2;
			break;
		}
	}
	if (i == nSum1)
		return FALSE;

	// ����nLine2�ĵ�һ���˵��Ƿ���nLine1����
	nEndIdx = vvRelation[nLine2][0]->nEnd2;
	nSum2 = vvRelation[nLine2].size();
	if ((vvRelation[nLine2][0]->nLine2==nLine1)	&& (nEndIdx==nEnd1))
	{
		return TRUE;
	}
	else
	{
		// �����һ����¼������nLine1����һ�£��п�������ΪnLine2�ĵ�һ��
		// ��¼�洢������һ���˵��������߽��������ӵ���Ϣ���ʲ��Һ������
		// ��һ���˵㲻ͬ�ļ�¼�����Ƿ�����һ�����߽������ӵ���Ϣ
		nEndIdx = vvRelation[nLine2][0]->nEnd1;

		for (i=1; i<nSum2; i++)
		{
			// ��������¼�ĵ�nLine2�Ķ˵����1����¼�Ķ˵���ͬ�������
			if (vvRelation[nLine2][i]->nEnd1 == nEndIdx)
				continue;
			
			if ((vvRelation[nLine2][i]->nLine2==nLine1)
				&& (vvRelation[nLine2][i]->nEnd2==nEnd1))
			{
				return TRUE;
			}
			else // ������Ӧ��һ���߽磬������ѭ��
			{
				break;
			}
		}
		// �������һ���߽���������������ѭ��
		return FALSE;
	}
}

// ������С���˷��Զ���߽����������
void CFindCrack::LineFit(vector<LINECAP>& vLine, LINECAP& line)
{
	int i, nSum;
	float f_xSum, f_ySum, f_xxSum, f_xySum;

	// �ṹ�����������������
	nSum = vLine.size();
	f_xSum = f_ySum = f_xySum = f_xxSum = 0.0;
	f_xSum = vLine[0].pt1.x;
	f_ySum = vLine[0].pt1.y;
	f_xxSum = vLine[0].pt1.x*vLine[0].pt1.x;
	f_xySum = vLine[0].pt1.x*vLine[0].pt1.y;
	for (i=0; i<nSum; i++)
	{
		f_xSum += vLine[i].pt2.x;
		f_ySum += vLine[i].pt2.y;
		f_xySum += vLine[i].pt2.x*vLine[i].pt2.y;
		f_xxSum += vLine[i].pt2.x*vLine[i].pt2.x;
	}

	if (abs(f_xSum*f_xSum - nSum*f_xxSum) > 0.000001)
	{
		// �ṹ����������Է���б��
		float fK = (f_xSum*f_ySum - nSum*f_xySum)/(f_xSum*f_xSum - nSum*f_xxSum);

		// �ṹ�����			
		line.fAngle = atan(fK)*180/PI;
	}
	// �ṹ�����
	else
		line.fAngle = 90;

	// ע�⣺�˴�ֱ��ȡ�����˵���Ϊ������Ϻ�Ķ˵�
	line.pt1.x = vLine[0].pt1.x;
	line.pt1.y = vLine[0].pt1.y;
	line.pt2.x = vLine[nSum-1].pt2.x;
	line.pt2.y = vLine[nSum-1].pt2.y;
}

// ͳ�ƽǶȷ�Χֵ
// const vector<vector<LINECAP>>& vvLines: �߽���
// int* pNum: ���ֵ����18��Ԫ�أ���Ϊ��180��ƽ��Ϊ18�ݣ�ÿ��10��
// ͳ��ÿ10�ȷ�Χ�ڵı߽�������
void CFindCrack::StatAngles(const vector<vector<LINECAP> >& vvLines, int* pNum)
{
	memset(pNum, 0, sizeof(int)*ANGLE_NUM);

	int i, j, nSum, nSum1;
	nSum = vvLines.size();

	for (i=0; i<nSum; i++)
	{
		nSum1 = vvLines[i].size();
		for (j=0; j<nSum1; j++)
		{
			LINECAP line = vvLines[i][j];
			pNum[int(line.fAngle/(180/ANGLE_NUM))] ++;
		}
	}
}

// ͳ�Ʊ߽���
void CFindCrack::StatLines(const vector<vector<LINECAP> >& vvLines, vector<int>& vSum)
{
	int i, j, k;
	int nLines, nLinePts;
	float fLength;

	nLines = vvLines.size();
	for (i=0; i<nLines; i++)
	{
		fLength = 0;
		nLinePts = vvLines[i].size();
		for (j=0; j<nLinePts; j++)
		{
			fLength += get_distance(vvLines[i][j].pt1, vvLines[i][j].pt2);
		}

		// �ж��Ƿ�û�е�ǰfLength������ͳ��ֵ
		k = fLength/10;
		if (vSum.size()==k)
		{
			vSum.push_back(1);
		}
		else if (vSum.size()<k)
		{
			for (int m=vSum.size(); m<k; m++)
				vSum.push_back(0);
			vSum.push_back(1);
		}
		else
			vSum[k] ++;
	}
}

/* ==================================================================
 * �������ƣ�  CFindCrack::GetCirclePoints()
 * ���ߣ�      [Leng]
 * ������:     [2014-7-30 18:31:25]
 * �����б� 
               [POINT ptCenter] - [In]��Բ�ĵ�����
               [ float fRadius] - [In]  �뾶
               [ POINT** ppCirclePts] - [In] �����Բ�ϵ������
               [ int& nPtNum] - [In] �����Բ�ϵ������ĸ���
ע�� :	�е�Բ�����㷨 ��Բ�������ptCenterΪԲ��,fRadiusΪ�뾶��Բ�߽��ϵ����е������
==================================================================*/
void CFindCrack::GetCirclePoints(POINT ptCenter, float fRadius, POINT** ppCirclePts, int& nPtNum)
{
	/*
	int x=0;
	float y = fRadius;
	
	int d=1-fRadius; //���(0,R),��һ���е�(1,R-0.5),d=1*1+(R-0.5)*(R-0.5)-R*R=1.25-R,dֻ�����������㣬����С�����ֿ�ʡ��
	
	while(y>x) //y>x����һ���޵ĵ�2���˷�Բ
	{
		pDC->SetPixel(x+ptCenter.x, y+ptCenter.y,RGB(255,0,0)); //Բ��(x1,y1),����ʱֱ�����ƽ��,��2��
		pDC->SetPixel(y+ptCenter.x,x+ptCenter.y,RGB(255,0,0)); //��1��
		pDC->SetPixel(-x+ptCenter.x,y+ptCenter.y,RGB(255,0,0)); //��3��
		pDC->SetPixel(-y+ptCenter.x,x+ptCenter.y,RGB(255,0,0)); //��4��
		pDC->SetPixel(-x+ptCenter.x,-y+ptCenter.y,RGB(255,0,0)); //��5��
		pDC->SetPixel(-y+ptCenter.x,-x+ptCenter.y,RGB(255,0,0)); //��6��
		pDC->SetPixel(x+ptCenter.x,-y+ptCenter.y,RGB(255,0,0)); //��7��
		pDC->SetPixel(y+ptCenter.x,-x+ptCenter.y,RGB(255,0,0)); //��8��
		
		if(d<0)
			d=d+2*x+3; //d�ı仯
		else
		{
			d=d+2*(x-y)+5; //d<=0ʱ,d�ı仯
			y--; //y�����1
		}
		
		x++; //x�����1
	}*/
}

/* ==================================================================
 * �������ƣ�  CFindCrack::GetLinePoint()
 * ���ߣ�      [Leng]
 * ������:     [2014-7-30 18:33:10]
 * ����ֵ��    [POINT] - ��pt�㡢�Ƕ�ΪfAngle��ֱ���ϵ���ptΪԲ�㣬
		������������������[0,180)��Χ�ڵĵ�i�����[180,360)��Χ�ڵĵ�i��
 * �����б� 
               [POINT pt] - [In] ������ֱ���ϵĵ�
               [ float fAngle] - [In] ֱ�ߵĽǶ�
               [ BOOL bUp] - [In]��TRUE:��ptΪԲ���[0,180)��Χ��FALSE:��ptΪԲ���[180,360)��Χ
               [ int nPtIdx] - [In] ���������
ע�� :	��ù�pt�㡢�Ƕ�ΪfAngle��ֱ���ϵ���ptΪԲ�㣬
		��[0,180)��Χ�ڵĵ�i�����[180,360)��Χ�ڵĵ�i��	
==================================================================*/
POINT CFindCrack::GetLinePoint(POINT pt, float fAngle, BOOL bUp, int nPtIdx)
{
	int   dx,dy,n,k,i,f; 
	POINT ptRet;

	// ��ֱ���ϵľ�pt��Զ��ĳһ��
	POINT pt1; 
	if (fabs(fAngle/90) < 0.00001 || fabs(fAngle/270) < 0.00001)
	{
		pt1.x = pt.x;
		if (bUp)
			pt1.y = pt.y + 10000;
		else
			pt1.y = pt.y - 10000;
	}
	else
	{
		if (bUp)
			pt1.y = pt.y + 10000;
		else
			pt1.y = pt.y - 10000;
		pt1.x = 10000 / tan(fAngle/180*PI) + pt.x;
	}

	dx = abs(pt.x - pt1.x); 
	dy = abs(pt.y - pt1.y); 
	n = dx + dy; 
	if(pt.x >= pt1.x) 
	{ 
		k = (pt.y >= pt1.y) ? 1 : 4; 
		ptRet.x = pt1.x; 
		ptRet.y = pt1.y;                         
	} 
	else 
	{ 
		k = (pt.y >= pt1.y) ? 2 : 4; 
		ptRet.x = pt1.x; 
		ptRet.y = pt1.y; 
	} 

	for(i=0,f=0; i<=nPtIdx; i++) 
	{
		if(f >= 0) 
		{
			switch(k) 
			{ 
			case 1: ptRet.x++; f-=dy; break; 
			case 2: ptRet.y++; f-=dx; break; 
			case 3: ptRet.x--; f-=dy; break; 
			case 4: ptRet.y--; f-=dx; break; 
			} 
		}
		else 
		{
			switch(k) 
			{ 
			case 1: ptRet.y++; f+=dx; break; 
			case 2: ptRet.x--; f+=dy; break; 
			case 3: ptRet.y--; f+=dx; break; 
			case 4: ptRet.x++; f+=dy; break; 
			}
		}
	}

	return ptRet;
}

// �ض˵㴦���߽��ߵ���߽��߼н�45�ȷ��򽫸�Ŀ�������Ϊ������
// CImage* pBinImage: ��ֵͼ���ѷ�Ҷ�ֵΪ0
// vector<POINT>& vPoints: һ��ϸ���߽����ϵĵ㣬�޷ֲ�
BOOL CFindCrack::SharpenEnds(CImage* pBinImage, vector<POINT>& vPoints)
{
	int i, k, l, m, n;
	POINT pt1, pt2, pt, ptD, ptDs, ptDe, ptD1, ptD2;
	vector<POINT> vBlade1, vBlade2, *pBlade;
	int nEnd1Idx = -1, nEnd2Idx = -1, nMark = 0;

	POINT move[] = {{1,0}, {1,1}, {0,1}, {-1,1}, {-1,0}, {-1,-1}, {0,-1},{1,-1}};
	int nSize = vPoints.size();
	for (k=0; k<2; k++)
	{// k=0ʱ�����߽���ͷ��k=1ʱ�����߽���β
		for (i=0; i<nSize-1; i++)
		{
			if (k==0)
			{
				m = i;
				n = m+1;
				pBlade = &vBlade1;

				ptD.x = ptDs.x = vPoints[n].x-vPoints[m].x;
				ptD.y = ptDs.y = vPoints[n].y-vPoints[m].y;
			}
			else
			{
				m = nSize-1-i;
				n = m-1;
				pBlade = &vBlade2;

				ptD.x = ptDe.x = vPoints[n].x-vPoints[m].x;
				ptD.y = ptDe.y = vPoints[n].y-vPoints[m].y;
			}

			// ����vPoints[j]��vPoints[i]��8�����е���һ����
			for (l=0; l<8; l++)
			{
				if (move[l].x==ptD.x && move[l].y==ptD.y)
					break;
			}
			// ���ݷ�������8�����еĵ�������ǰһ���ͺ�һ��8�����
			ptD1 = move[(l-1+8)%8];
			ptD2 = move[(l+1+8)%8];

			// ���˵���Ϊ��һ����ѹ��
			pt = vPoints[m];
			pBlade->push_back(vPoints[m]);
			int nEndSize = pBlade->size();

			// ��һ��45�ȽǱ�Ե���ж೤
			pt1.x = vPoints[m].x+ptD1.x;
			pt1.y = vPoints[m].y+ptD1.y;
			BYTE bGray = pBinImage->GetPixelGray(pt1.x, pt1.y);
			while (bGray == 0)
			{
				pBlade->push_back(pt1);
				pt1.x = pt1.x+ptD1.x;
				pt1.y = pt1.y+ptD1.y;
				bGray = pBinImage->GetPixelGray(pt1.x, pt1.y);
			}
			if (pBlade->size() > 10)
			{
				pBlade->erase(pBlade->begin()+nEndSize, pBlade->end());
				continue;
			}

			// ����һ��45�ȽǱ�Ե���ж೤
			pt2.x = vPoints[m].x+ptD2.x;
			pt2.y = vPoints[m].y+ptD2.y;
			bGray = pBinImage->GetPixelGray(pt2.x, pt2.y);
			while (bGray == 0)
			{
				pBlade->push_back(pt2);
				pt2.x = pt2.x+ptD2.x;
				pt2.y = pt2.y+ptD2.y;
				bGray = pBinImage->GetPixelGray(pt2.x, pt2.y);
			}
			if (pBlade->size() > 10)
			{// ɾ����Ч�ĵ㣬����ϸ�����ϵĵ�
				pBlade->erase(pBlade->begin()+nEndSize, pBlade->end());
				continue;
			}

			// �������ʱ�漰��ϸ�����ϵĵ�
			if (k==0)
			{
				nEnd1Idx = n;
				nMark = 1;
				for (l=0; l<n+1; l++)
					pBlade->push_back(vPoints[l]);
			}
			else
			{
				nEnd2Idx = n;
				nMark += 1;
				for (l=nSize-1; l>=n; l--)
					pBlade->push_back(vPoints[l]);
			}

			goto changeEnd;
		} // for (i=...)
changeEnd:
		;
	} // for (k=...)

	// �����߶˵㲻����������δ��ȫ���д������һ�˵���Ų�С�ڵڶ��˵���ţ��򷵻�FALSE
	if (nMark < 2 || (nEnd1Idx < 0 || nEnd2Idx < 0 && nEnd1Idx >= nEnd2Idx))
		return FALSE;

	// ����õĵ�����Ϊ����ɫ
	for (k=0; k<2; k++)
	{
		if (k==0)
		{
			pBlade = &vBlade1;
			ptD.x = ptDs.x;
			ptD.y = ptDs.y;
		}
		else
		{
			pBlade = &vBlade2;
			ptD.x = ptDe.x;
			ptD.y = ptDe.y;
		}

		// ������45�ȽǱ�Ե�߾���������ʱ����45�ȽǱ�Ե���ϵĵ�����Ϊ����ɫ
		for (l=0; l<pBlade->size(); l++)
		{
			pt = (*pBlade)[l];
			pBinImage->SetPixelColor(pt.x, pt.y, RGB(255,255,255));
		}

		// ������˵�ĵ㰴��ͬ�������д���
		for (l=0; l<pBlade->size(); l++)
		{
			pt.x = (*pBlade)[l].x+ptD.x;
			pt.y = (*pBlade)[l].y+ptD.y;
			pBinImage->SetPixelColor(pt.x, pt.y, RGB(255,255,255));
		}
	}
	
	return TRUE;
}

// ��þ�����45�ȵ��ѷ�Ŀ�ȼ����
/* ==================================================================
 * �������ƣ�  CFindCrack::GetSharpenLineCrackMaxWidth()
 * ���ߣ�      [Leng]
 * ������:     [2014-7-31 16:43:47]
 * ����ֵ��    [float] - ��������ѷ���
  * �����б� 
               [CImage* pBinImage] - [In] ԭʼ��ֵͼ��
               [ const vector<POINT>& vPoints] - [In] �ѷ�ϸ���߽����ϵĵ�
               [ POINT& mwPoint] - [Out]���������ѷ��ȴ���ϸ�����ϵĵ�
ע�� : ��þ�����45�ȵ��ѷ�Ŀ�ȼ����
==================================================================*/
float CFindCrack::GetSharpenLineCrackMaxWidth(CImage* pBinImage, const vector<POINT>& vPoints, POINT& mwPoint)
{
	float fMaxWidth = 0.0;

	BOOL bFindSeed = TRUE;
	int i, j;
	POINT pt, pt1, pt2,  ptD, ptD1, ptD2;
	POINT move[] = {{1,0}, {1,1}, {0,1}, {-1,1}, {-1,0}, {-1,-1}, {0,-1},{1,-1}};
	int nSize = vPoints.size();
	for (i=0; i<nSize-1; i++)
	{
		BYTE bGray = pBinImage->GetPixelGray(vPoints[i].x, vPoints[i].y);
		if (bGray == 255)
			continue;

		POINT ptCenter = vPoints[i];

		ptD.x = vPoints[i+1].x-vPoints[i].x;
		ptD.y = vPoints[i+1].y-vPoints[i].y;
		
		// ����vPoints[j]��vPoints[i]��8�����е���һ����
		for (j=0; j<8; j++)
		{
			if (move[j].x==ptD.x && move[j].y==ptD.y)
				break;
		}
		// ���ݷ�������8�����еĵ�������ֱ�������������8�����
		ptD1 = move[(j-2+8)%8];
		ptD2 = move[(j+2+8)%8];

		// ���i����ptD1����ֱ���ѷ�ߵĽ���
		pt.x = vPoints[i].x+ptD1.x;
		pt.y = vPoints[i].y+ptD1.y;
		bGray = pBinImage->GetPixelGray(pt.x, pt.y);
		if (bGray == 255)
			pt1 = vPoints[i];
		else
		{
			while (bGray == 0)
			{
				pt1 = pt;
				pt.x += ptD1.x;
				pt.y += ptD1.y;
				bGray = pBinImage->GetPixelGray(pt.x, pt.y);
			}
			if (bGray == 128) // 128Ϊϸ���߱�ǣ������ȣ�˵����ϸ�����ཻ�ˣ�������Ҫ��
				continue;
		}

		// ���i����ptD2����ֱ���ѷ�ߵĽ���
		pt.x = vPoints[i].x+ptD2.x;
		pt.y = vPoints[i].y+ptD2.y;
		bGray = pBinImage->GetPixelGray(pt.x, pt.y);
		if (bGray == 255)
			pt2 = vPoints[i];
		else
		{
			while (bGray == 0)
			{
				pt2 = pt;
				pt.x += ptD2.x;
				pt.y += ptD2.y;
				bGray = pBinImage->GetPixelGray(pt.x, pt.y);
			}
			if (bGray == 128)
				continue;
		}

		// ��pt1��pt2�ľ��룬���ѷ���
		float fDist = get_distance(pt1, pt2);
		float fDist1 = get_distance(ptCenter, pt1);
		float fDist2 = get_distance(ptCenter, pt2);
		// ��fDist1��fDist2�Ĳ�ֵ��С��˵���ڼ���ptCenter�����ѷ���ʱ��û���������ѷ��֧�ཻ������Ϊ��ʵ�ѷ���
		if (fMaxWidth < fDist && fabs(fDist1 - fDist2) < 5)
		{
			fMaxWidth = fDist;
			mwPoint = vPoints[i];
		}
	}

	return fMaxWidth;
}

// ��þ�����45�ȵ��ѷ�����
float CFindCrack::GetSharpenLineCrackArea(CImage* pBinImage, POINT & ptSeed)
{
	ObjRunLenCode objRLC;
	pBinImage->ObjRLC(&objRLC, ptSeed, 0);
	return objRLC.ObjArea(0);
}
