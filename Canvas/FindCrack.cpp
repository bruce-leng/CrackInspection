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

// 将180度分为ANGLE_NUM份
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
 * 函数名称：  CFindCrack::CrackExtraction()
 * 作者：      [Leng]
 * 创建于:     [2014-6-2 14:03:20]
 * 参数列表： 
               [float fLenT] - [In] 边界周长阈值
               [ float fAreaT] - [In] 边界面积阈值
               [ float fL2AT] - [In] 边界长度与面积比阈值
               [ float fGaveT] - [In] 边界灰度均值阈值
               [ float fGvariT] - [In] 边界灰度均方差阈值
			   [ ObjRunLenCode** ppCrackObj] - [Out] 提取出的裂缝的游程编码
			   [ CRACK_MGR** ppCrackMgr] - [Out] 识别的裂缝的特征参数
注释 :	根据裂缝特征识别裂缝
		分析的目标图像要求是二值图像，否则分析可能出错		
==================================================================*/
void CFindCrack::CrackExtraction(float fCircLenT, float fAreaT, float fL2AT, float fGaveT, float fGvariT,
								 ObjRunLenCode** ppCrackObj, CRACK_MGR** ppCrackMgr)
{
	int i,j,k;

	// 得到二值图像中所有目标游程编码
	if (*ppCrackObj != NULL)
		delete (*ppCrackObj);
	*ppCrackObj = new ObjRunLenCode;
	m_pBinImage->ObjAllRLC(*ppCrackObj, 0);

	// 清除细小的目标

	// 得到二值图像中各目标的边界线长度(含内边界)
// 	CImage* pEdgeImage = CImage(*m_pBinImage);
// 	pEdgeImage->MorphThining();
	ObjEdgeList* pObjEdgeLst = new ObjEdgeList;
	m_pBinImage->ObjAllEdgeLst(pObjEdgeLst, 0);
	int* pLength = new int[pObjEdgeLst->GetObjNum()];
	pObjEdgeLst->GetObjEdgeLength(&pLength);

	// 对所有目标进行分析
	// 根据所有目标分析面积，原始图像中的灰度均值、方差
	float objArea, objGave, objGvari;
	vector< vector< LINECAP> > lineArray;

	int nObjNum = (*ppCrackObj)->GetObjNum();
	ASSERT(pObjEdgeLst->GetObjNum() == nObjNum);

	// 分配内存用于存储裂缝特征信息
	if (*ppCrackMgr != NULL)
		delete [] (char*)ppCrackMgr;
	*ppCrackMgr = (CRACK_MGR*)new char [sizeof(int)+sizeof(CRACK)*nObjNum];
	CRACK* pCracks = (*ppCrackMgr)->elem;
	memset(pCracks, NULL, sizeof(CRACK)*nObjNum);

	// 筛选裂缝目标
	int nPixelNum;
	int nCrackObjNum = 0;
	PRUNLENGTHCODE_MGR pObjRLC = NULL;
	for (i=nObjNum-1; i>=0; i--)
	{
		// 1.根据阈值筛选“目标的边界长度”
		if (pLength[i] < fCircLenT)
			goto deleteCrack;
			
		// 2.根据阈值筛选“目标面积”
		objArea = (*ppCrackObj)->ObjArea(i);
		if (objArea < fAreaT)
			goto deleteCrack;

		// 3.根据阈值筛选“目标边界长度与面积之比”
		if (pLength[i]/objArea < fL2AT)
			goto deleteCrack;

		// 4.根据阈值筛选“目标灰度均值”
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

		// 5.根据阈值筛选“目标灰度方差”
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

		// 6.保留裂缝目标及各属性参数
		pCracks[nCrackObjNum].fLength = pLength[i];
		pCracks[nCrackObjNum].fArea = objArea;
		pCracks[nCrackObjNum].fGave = objGave;
		pCracks[nCrackObjNum].fGvari = objGvari;
		nCrackObjNum ++;
		continue;

deleteCrack:
		// 如果当前目标不满足条件被筛选掉，则删除目标
		(*ppCrackObj)->ObjDel(i);
	}

	(*ppCrackMgr)->nNum = nCrackObjNum;

	delete [] pLength;
	delete pObjEdgeLst;
}

/* ==================================================================
 * 函数名称：  CFindCrack::FindCrack()
 * 作者：      [Leng]
 * 创建于:     [2014-7-29 21:19:57]
 * 参数列表： 
			 [float fLengthT] - [In] 边界周长阈值
			 [ float fAreaT] - [In] 边界面积阈值
			 [ float fL2AT] - [In] 边界长度与面积比阈值
			 [ float fGaveT] - [In] 边界灰度均值阈值
			 [ float fGvariT] - [In] 边界灰度均方差阈值
			 [ ObjRunLenCode** ppCrackObj] - [Out] 提取出的裂缝的游程编码
			 [ CRACK_MGR** ppCrackMgr] - [Out] 识别的裂缝的特征参数
 注释 :	根据裂缝特征识别裂缝（裂缝灰度值为0，背景灰度值为255）
        分析的目标图像要求是二值图像，否则分析可能出错		
==================================================================*/
void CFindCrack::FindCrack(float fLengthT, float fAreaT, float fL2AT, float fGaveT, float fGvariT,
			   ObjRunLenCode** ppCrackObj, CRACK_MGR** ppCrackMgr)
{
	int i, j, k;
	
	// 得到二值图像中所有目标游程编码
	if (*ppCrackObj != NULL)
		delete (*ppCrackObj);
	*ppCrackObj = new ObjRunLenCode;

	// 游程对应的裂缝特征
	vector<CRACK> vCracks;

	// 二值细化
	CImage* pThinImage = new CImage(*m_pBinImage);
	pThinImage->MorphThining();

	// 获得细化图像中的目标游程编码
	ObjRunLenCode thinAllObjRLC;
	pThinImage->ObjAllRLC(&thinAllObjRLC, 0);
	delete pThinImage;

	// 设置细化线颜色，用于后续分析时不允许与细化线相交
	// 复制二值图像用于后续处理
	CImage* pBinImage = new CImage(*m_pBinImage);
	pBinImage->SetObjColor(&thinAllObjRLC, RGB(128,128,128));

	// 对每一个目标进行分析
	PPRUNLENGTHCODE_MGR ppRLCMgr = thinAllObjRLC.GetObjAll(); 
	for (i=0; i<thinAllObjRLC.GetObjNum(); i++)
	{
		long lLength, lArea;
		float objGave, objGvari, fMaxWidth = 0.0;

		// 1. 根据细化边界线面积（相当于细化边界线长度）确定是否继续分析
		lLength = thinAllObjRLC.ObjArea(i);
		if (lLength < fLengthT)
			continue;

		// 2. 根据原二值图像中对应的裂缝的面积进行分析
		POINT pt;
		pt.x = thinAllObjRLC.GetObj(i)->RC[0].xStart;
		pt.y = thinAllObjRLC.GetObj(i)->RC[0].y;
		ObjRunLenCode binObjRLC; 
		m_pBinImage->ObjRLC(&binObjRLC, pt, 0);
		lArea = binObjRLC.ObjArea(0);
		if (lArea < fAreaT)
			continue;

		// 3.根据阈值筛选“目标边界长度与面积之比”
		if (lLength*1.0/lArea < fL2AT)
			continue;
		
		// 4.根据阈值筛选“目标灰度均值”
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
		
		// 5.根据阈值筛选“目标灰度方差”
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

		// 6.提取单个目标的细化边界线，并对其进行分析
		MATRIX* pMatrix = NULL;
		CopyBuffer(ppRLCMgr[i], &pMatrix);	// 将单个目标细化边界线数据放入矩阵进行分析
		ClearNode(pMatrix);					// 清除三叉点
		vector<deque<POINT>*> vdPoints;		// 设置变量用于保存各裂缝的细化线上的点
		FindEdgePoints(pMatrix, vdPoints);	// 找到目标上各细化边界线上的点
		delete pMatrix;

		// 5. 根据各细化边界线的端点及各细化边界线对原始二值图像进行分析
		// 5.1 沿端点处两边界线点与边界线夹角45度方向将各目标点设置为背景点
		// 5.2 去掉细化线两端各两个端点后，分析其余的细化边垂直方向的直线被原始二值图像中目标线切割后的长度，
		//     作为该处的裂缝宽度，据此求出最大裂缝宽度
		// 5.3 分析去掉细化线两端各两个端点后的裂缝的面积

		CRect rect;
		thinAllObjRLC.ObjRect(i, rect);
		vector<POINT> vPoints;
		int nSize = vdPoints.size();
		for (j=0; j<nSize; j++)
		{
			// 当细化线上的点数小于5时，不分析其特征
			int nNum = vdPoints[j]->size();
			if (nNum < 5)	continue;	

			// 获得全局图像中的细化边界点
			vPoints.clear();
			for (k=0; k<nNum; k++)
			{
				POINT pt = vdPoints[j]->at(k);
				pt.x = pt.x-1+rect.left;
				pt.y = pt.y-1+rect.top;
				
				vPoints.push_back(pt);
				//pBinImage->SetPixelColor(pt.x, pt.y, RGB(128,128,128)); // 标记细化线颜色
			}
			//CString str;
			//str.Format("d:\\%d-%d.bmp", i, j);
			//m_pBinImage->Save(str, CXIMAGE_FORMAT_BMP);

			// 沿端点处两边界线点与边界线夹角45度方向将各目标点设置为背景点
			//if (!SharpenEnds(pBinImage, vPoints))
			//	continue;
			//str.Format("d:\\%d-%d-sharpen.bmp", i, j);
			//pBinImage->Save(str, CXIMAGE_FORMAT_BMP);

			// 获得vPoints所代表的裂缝最大宽度
			POINT maxwidthPoint;
			float fWidth = GetSharpenLineCrackMaxWidth(pBinImage, vPoints, maxwidthPoint);
			if (fMaxWidth < fWidth)
				fMaxWidth = fWidth;
			
			/*
			// 点的个数相当于长度
			lLength = vPoints.size();

			// 求面积
			ObjRunLenCode* pObjRLC = new ObjRunLenCode;
			m_pBinImage->ObjRLC(pObjRLC, maxwidthPoint, 0);
			lArea = pObjRLC->ObjArea(0);

			// 求灰度均值
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

			// “目标灰度方差”
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

		// 保存对应的裂缝
		(*ppCrackObj)->ObjAdd(pObjRLC);
		PPRUNLENGTHCODE_MGR ppObjAllRLC = binObjRLC.GetObjAll();
		ppObjAllRLC[0] = NULL;
		//delete pObjRLC;

		// 保存满足条件的裂缝及裂缝特征
		CRACK crack;
		crack.fArea = lArea;
		crack.fWmax = fMaxWidth;
		crack.fLength = lLength;
		crack.fL2A = lLength/lArea;
		crack.fGave = objGave;
		crack.fGvari = objGvari;
		vCracks.push_back(crack);

		// 删除变量
		for (j=0; j<vdPoints.size(); j++)
			delete vdPoints[j];
	}

	delete pBinImage;

	// 将裂缝重新保存
	if (*ppCrackMgr != NULL)
		delete [] *ppCrackMgr;
	*ppCrackMgr = (CRACK_MGR*)new char[sizeof(int)+vCracks.size()*sizeof(CRACK)];
	(*ppCrackMgr)->nNum = vCracks.size();
	for (i=0; i<(*ppCrackMgr)->nNum; i++)
		(*ppCrackMgr)->elem[i] = vCracks[i];
}

// 将目标拷贝至内存中
void CFindCrack::CopyBuffer(PRUNLENGTHCODE_MGR pRLCMgr, MATRIX** ppMatrix)
{
	int i, j;
	
	// 取得外接矩形
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
	
	// 扩展一个边界并分配内存
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

// 对二值图像矩阵进行细化
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

	// 图像反色，将边界线变成黑色
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

	// 复制图像矩阵
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

	// 将边界线反色成白色
	for (y=0; y<nHeight; y++)
	{
		for (x=0; x<nWidth; x++)
		{
			pElemNew[y*nWidth+x] = 255-pElemNew[y*nWidth+x];
		}
	}
	
	// 复制图像矩阵到输入图像中
	memcpy(pMatrix, pMatrixNew, sizeof(pMatrix));

	// 清除临时分配内存
	delete [] (char*)pMatrixNew;
}

// 清除单像素宽边界二值图像中的单个目标点或者有边界线的分叉点
void CFindCrack::ClearNode(MATRIX* pMatrix)
{
	// 清除单个目标色点
	int i,j,m,n;
	BYTE gray;
	int nCount;
	int nWidth = pMatrix->nWidth;
	int nHeight = pMatrix->nHeight;

	// 清除目标边界中的分叉点
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
			
			// 若当前像素8邻域有3个目标像素点或无目标像素点时，直接清除掉
			if (nCount>=4)	// nCount包含了中心点
			{
				pt.x = i;
				pt.y = j;
				vKeyPts.push_back(pt);
			}
		}
	}

	// 将查找到的节点变为背景点
	for (i=0; i<vKeyPts.size(); i++)
	{
		pt = vKeyPts[i];
		pMatrix->elem[pt.y*nWidth+pt.x] = 255;
	}
}

// 查找目标边界线上的点
void CFindCrack::FindEdgePoints(MATRIX* pMatrix, vector<deque<POINT>*>& vdPoints)
{
	int i;
	int nWidth = pMatrix->nWidth;
	int nHeight = pMatrix->nHeight;
	BYTE* pElem = pMatrix->elem;

	//////////////////////////////////////////////////////////////////////////
	// 定义8邻域的平移坐标
	CPoint move8[8];
	move8[0].x = 1, move8[0].y = 0;	// 第1点
	move8[1].x = 1, move8[1].y =-1;	// 第2点
	move8[2].x = 0, move8[2].y =-1;	// 第3点
	move8[3].x =-1, move8[3].y =-1;	// 第4点
	move8[4].x =-1, move8[4].y = 0;	// 第5点
	move8[5].x =-1, move8[5].y = 1;	// 第6点
	move8[6].x = 0, move8[6].y = 1;	// 第7点
	move8[7].x = 1, move8[7].y = 1;	// 第8点

	// 当查找到当前点时，查找当前点的8邻域和4邻域点的起始方向按如下方式进行
	int nPrevOri8[] = {7,7,1,1,3,3,5,5};

	CPoint ptEnd, prevPt, curPt, nextPt;
	int nPrevOri, nNextOri;

	while(TRUE)
	{
		// 若不存在直线或封闭曲线，则跳出循环
		int nClosed = IsClosed(pMatrix, ptEnd);
		if (nClosed < 0)
			break;

		deque<POINT>* pPoints = new deque<POINT>;

		// 标记第一点
		prevPt = ptEnd;
		pPoints->push_back(prevPt);
		
		// 找下一个点及方向
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
		if (nNextOri < 0) // 如果小于0，说明为一个单点
		{
			vdPoints.push_back(pPoints);
			goto setLineColor;
		}

		if (nClosed == 1)	// 如果为封闭曲线
		{
			while (TRUE)
			{
				nPrevOri = nPrevOri8[nNextOri];
				
				// 从第nPrevOri方向开始查找8邻域点
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
				
				// 当下一点和第一点相同时，说明已遍历一周
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
		else	// 如果不是封闭曲线
		{
			curPt = nextPt;
			
			while (TRUE)
			{
				nPrevOri = nPrevOri8[nNextOri];
				
				// 从第nPrevOri方向开始查找8邻域点
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
				
				// 当下一点和第一点相同时，说明已遍历至后一个端点
				if (nextPt == prevPt)
				{
					pPoints->pop_back();
					vdPoints.push_back(pPoints);
					break;
				}
				
				// 接力传点
				prevPt = curPt;
				curPt = nextPt;
			}
		}

setLineColor:
		// 将已经遍历的点设置为背景色
		int nSize = pPoints->size();
		for (i=0; i<nSize; i++)
		{
			POINT pt = (*pPoints)[i];
			pMatrix->elem[pt.y*nWidth+pt.x] = 255;
		}
	}
}

// 对多点组成的多段线进行线性拟合，并返回多段线长度
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
	
	// 最后一段
	vLines.push_back(dPoints[nIdx2-1]);
	fLength += get_distance(pt1, dPoints[nIdx2-1]);

	return fLength;
}

// 对多点组成的多段进行线性拟合
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
	
	// 最后一段
	line.pt2 = dPoints[nIdx2-1];
	line.fAngle = get_angle(line.pt1, line.pt2);
	vLines.push_back(line);
}

// 判断是否是封闭边界线，若不是，则取得一个边界端点
// 返回值：-1: MATRIX无目标点; 0: 有目标边界,但未封闭; 1: 有目标边界,且已封闭
int CFindCrack::IsClosed(MATRIX* pMatrix, POINT& ptEnd)
{
	int i, j;
	CPoint prevPt, curPt, nextPt, startPt;
	int nPrevOri, nNextOri;

	int nWidth = pMatrix->nWidth;
	int nHeight = pMatrix->nHeight;
	BYTE* pElem = pMatrix->elem;

	// 定义8邻域的平移坐标
	CPoint move8[8];
	move8[0].x = 1, move8[0].y = 0;	// 第1点
	move8[1].x = 1, move8[1].y =-1;	// 第2点
	move8[2].x = 0, move8[2].y =-1;	// 第3点
	move8[3].x =-1, move8[3].y =-1;	// 第4点
	move8[4].x =-1, move8[4].y = 0;	// 第5点
	move8[5].x =-1, move8[5].y = 1;	// 第6点
	move8[6].x = 0, move8[6].y = 1;	// 第7点
	move8[7].x = 1, move8[7].y = 1;	// 第8点
	
	// 当查找到当前点时，查找当前点的8邻域和4邻域点的起始方向按如下方式进行
	int nPrevOri8[] = {7,7,1,1,3,3,5,5};
	
	// 标记查找到的第一个目标像素点
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

	// 如果下面条件成立，则说明无目标点，直接返回－1
	if (i>=nWidth-1)
		return -1;

	// 将第1点坐标赋给返回点的坐标值
	prevPt = startPt;

	// 看第1个点的8邻域是否有1个目标点或无目标点，若是，则该点为端点，直接退出
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
	
	// 从第4点开始查找点point的下一个8邻域的颜色为uObjClr的点
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
		
		// 从第nPrevOri方向开始查找8邻域点
		for (i=nPrevOri; i<nPrevOri+8; i++)
		{
			nextPt = curPt+move8[i%8];
			if (pElem[nextPt.y*nWidth+nextPt.x] == 0)
			{
				nNextOri = i%8;
				break;
			}
		}

		// 当前一点和后一点相同时，说明当前点为端点
		if (nextPt == prevPt)
		{
			ptEnd = curPt;
			return 0;
		}
		else if (nextPt == startPt)	// 如果下一点与第一点相同，则为封闭曲线
		{
			ptEnd = nextPt;
			return 1;
		}	
		
		// 
		prevPt = curPt;
		curPt = nextPt;
	}
	
}

// 对线性拟合的边界线进行分析,将同属于一条边界线的各线段建立连接关系
void CFindCrack::LinkSimilarLines(vector< vector< LINECAP> >& lineArray)
{
	int i, j, m, n;

	// 对所有线目标进行比较分析
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
				// 记录相互关系
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

	// 将边界线根据其相应属性排序和归类
	PRELATION pTmp;
	for (i=0; i<nSum; i++)
	{
		nSum1 = vvRelation[i].size();

		// 根据相关性值按从大到小排序
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
	// 确立同一边界线上各段相邻边界线的对应关系
	BOOL* pMark = new BOOL [nSum];
	memset(pMark, 0, sizeof(BOOL)*nSum);
	vector< deque< POINT > > vdPoints;
	for (i=0; i<nSum; i++)
	{
		// 如果当前边界线被标记过或没有与之相对应的边界，则分析下一条边界线
		if (pMark[i])
			continue;
		pMark[i] = TRUE;

		deque< POINT > dPoints;
		int nCurLine, nCurEnd, nLine1, nEnd1, nLine2, nEnd2;

		// 如果当前边界与其它边界不相连，则直接退出
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

		// 查找第i条边界线的其中一个端点相对应的边界线
 		nCurLine = vvRelation[i][0]->nLine1;
		nCurEnd  = vvRelation[i][0]->nEnd1;
		nLine1 = nCurLine;
		nEnd1  = nCurEnd;

		// 存储当前边界线的端点
		if (nEnd1 == 0)	// 边界线头
		{
			dPoints.push_front(lineArray[i][0].pt1);
			for (j=0; j<lineArray[i].size(); j++)
			{
				dPoints.push_front(lineArray[i][j].pt2);
			}
		}
		else // 边界线尾
		{
			dPoints.push_back(lineArray[i][0].pt1);
			for (j=0; j<lineArray[i].size(); j++)
			{
				dPoints.push_back(lineArray[i][j].pt2);
			}
		}

		// 查找与当前边界线端点nCurEnd相邻的下一边界线，直至没有相邻边界
		while (FindNextNeighborEdge(vvRelation, nLine1, nEnd1, nLine2, nEnd2)
			&& !pMark[nLine2])
		{
			pMark[nLine2] = TRUE;

			if (nEnd2 == 0)	// 边界线头
			{
				dPoints.push_back(lineArray[nLine2][0].pt1);
				for (j=0; j<lineArray[nLine2].size(); j++)
				{
					dPoints.push_back(lineArray[nLine2][j].pt2);
				}
			}
			else	// 边界线尾
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

		// 查找与当前边界线端点1-nCurEnd相邻的下一边界线，直至没有相邻边界
		nLine1 = nCurLine;
		nEnd1  = 1-nCurEnd;
		while (FindNextNeighborEdge(vvRelation, nLine1, nEnd1, nLine2, nEnd2)
			&& !pMark[nLine2])
		{
			pMark[nLine2] = TRUE;
			
			if (nEnd2 == 0)	// 边界线头
			{
				dPoints.push_front(lineArray[nLine2][0].pt1);
				for (j=0; j<lineArray[nLine2].size(); j++)
				{
					dPoints.push_front(lineArray[nLine2][j].pt2);
				}
			}
			else	// 边界线尾
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
	// 对边界线进行筛选，找到符合要求的边界线
	// 根据边界线长度进行筛选
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

		// 根据长度阈值判断是否保留边界线,记录不予保留边界的序号
		if (fLength <= fLengthT)
			vBad.push_back(i);
	}

	// 删除不需要保留的边界线
	nSum = vBad.size();
	for (i=nSum-1; i>=0;  i--)
	{
		vdPoints.erase(vdPoints.begin()+vBad[i]);
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 将边界线再进行线性拟合后
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
	// 统计角度范围
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

	// 删除指针指向的内存
	for (i=0; i<vvRelation.size(); i++)
	{
		for (j=0; j<vvRelation[i].size(); j++)
		{
			delete vvRelation[i][j];
		}
	}
	delete [] pMark;
}

// 获得两直线间夹角
float CFindCrack::GetAngle(const LINECAP& line1, const LINECAP& line2)
{
	float fRetAngle;
	if (fabs(line1.fAngle-line2.fAngle) < 90)
		fRetAngle = fabs(line1.fAngle-line2.fAngle);
	else
		fRetAngle = 180-fabs(line1.fAngle-line2.fAngle);

	return fRetAngle;
}

// 获得两直线间端点到另一直线的距离和
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

// 判断两直线的其中一条的端点到另一条线段的垂足是否在线段上
BOOL CFindCrack::IsVertPtInLine(const LINECAP& line1, const LINECAP& line2)
{
	POINT2DF pt1_1, pt1_2, pt2_1, pt2_2;
	pt1_1.x = line1.pt1.x;	pt1_1.y = line1.pt1.y;
	pt1_2.x = line1.pt2.x;	pt1_2.y = line1.pt2.y;
	pt2_1.x = line2.pt1.x;	pt2_1.y = line2.pt1.y;
	pt2_2.x = line2.pt2.x;	pt2_2.y = line2.pt2.y;
	
	// 判断L1两端点a1,a2到L2上的垂足是否在L2上及L2两端点到L1的垂足是否在L1上
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

// 获得两线段端点间的最小距离，并记录最小距离点
// float& fMinDist: 返回最小距离
// int& nIdx1, int& nIdx2: 最小距离的两端点,0表示第1个端点,1表示第2个端点
float CFindCrack::GetMinDist(const LINECAP& line1, const LINECAP& line2,
														float& fMinDist, int& nIdx1, int& nIdx2)
{
	float fD1, fD2, fD3, fD4;
	// 求L1与L2两线段距离最近的两端点间的距离
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

// 查找满足条件的两边界线的相连的两点
// const vector<LINECAP>& lineCaps1: 边界线1
// const vector<LINECAP>& lineCaps2: 边界线2
// const float& fDsumT, const float& fDminT: 用于判定符合条件的阈值
// const float& fAngleT, const float& fLengthT: 用于判定符合条件的阈值
// int& nEnd1, int&nEnd2: 返回边界线1和2的端点序号,0为第1个端点,1为最后一个端点
// float& fRelation: 相关性
// 若连接相邻点成功返回TRUE，否则返回FALSE
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

			// 计算两线段端点间的距离，找出最小距离和端点
			GetMinDist(line1, line2, fDmin, nEnd1, nEnd2);
			fLen1 = get_distance(line1.pt1, line1.pt2);
			fLen2 = get_distance(line2.pt1, line2.pt2);
			// 若fDmin大于给定阈值或大于line1和line2的长度,则返回
			if (fDmin>fDminT || (fDmin>0.75*fLen1 && fDmin>0.75*fLen2))
				return FALSE;

			if (nNum1 > 1)	// 若第1个多段线不止1段
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

			// 计算两直线间一条直线两端点到另一条直线的距离
			fDsum = GetDistSum(line1, nEnd1, line2, nEnd2);
			// 若fDsum小于给定阈值，则继续
			if (fDsum > fDsumT)
				return FALSE;

			// 计算两线段间的夹角
			fAngle = GetAngle(line1, line2);
			// 若fA小于给定阈值，则继续
			if (fAngle > fAngleT)
				return FALSE;

			// 计算line1的端点nEnd1与line2端点nEnd2的连线与line1间的夹角
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
			
			// 判断line1是否在line2的延长线上，同时line2是否在line2的延长线上
			if (IsVertPtInLine(line1, line2))
				return FALSE;
			
			// 计算两边界线的相关性，系数选择合理性有待验证
			fRelation = 2/fDsum+1/fDmin+1/(fAngle+10);
			
			return TRUE;
		}
	}	

	return FALSE;
}

// 查找与当前边界线相连的下一条边界线
// const CArray<LINECAP, LINECAP>& lineCaps : 原始边界线
// const vector< vector< Relation* > >& vvRelation : 边界线集合
// const int& nLine1, const int& nEnd1 : 已经边界线及端点
// int& nLine2, int& nEnd2 : 待求与已知边界线相连的边界线和端点
BOOL CFindCrack::FindNextNeighborEdge( 
				  const vector< vector< RELATION* > >& vvRelation, 
				  const int& nLine1, const int& nEnd1,
				  int& nLine2, int& nEnd2)
{
	int i, nSum1, nSum2, nEndIdx;

	// 查找第1条可能与nEnd1相连的下一条边界线
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

	// 查找nLine2的第一个端点是否与nLine1相连
	nEndIdx = vvRelation[nLine2][0]->nEnd2;
	nSum2 = vvRelation[nLine2].size();
	if ((vvRelation[nLine2][0]->nLine2==nLine1)	&& (nEndIdx==nEnd1))
	{
		return TRUE;
	}
	else
	{
		// 如果第一条记录不能与nLine1的相一致，有可能是因为nLine2的第一条
		// 记录存储的是另一个端点与其它边界线相连接的信息，故查找后面的与
		// 第一个端点不同的记录，看是否有与一第条边界相连接的信息
		nEndIdx = vvRelation[nLine2][0]->nEnd1;

		for (i=1; i<nSum2; i++)
		{
			// 如果后面记录的第nLine2的端点与第1条记录的端点相同，则继续
			if (vvRelation[nLine2][i]->nEnd1 == nEndIdx)
				continue;
			
			if ((vvRelation[nLine2][i]->nLine2==nLine1)
				&& (vvRelation[nLine2][i]->nEnd2==nEnd1))
			{
				return TRUE;
			}
			else // 若不对应第一条边界，则跳出循环
			{
				break;
			}
		}
		// 若不与第一条边界线相连，则跳出循环
		return FALSE;
	}
}

// 利用最小二乘法对多段线进行线性拟合
void CFindCrack::LineFit(vector<LINECAP>& vLine, LINECAP& line)
{
	int i, nSum;
	float f_xSum, f_ySum, f_xxSum, f_xySum;

	// 结构面线性拟合特征数据
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
		// 结构面所拟合线性方程斜率
		float fK = (f_xSum*f_ySum - nSum*f_xySum)/(f_xSum*f_xSum - nSum*f_xxSum);

		// 结构面倾角			
		line.fAngle = atan(fK)*180/PI;
	}
	// 结构面倾角
	else
		line.fAngle = 90;

	// 注意：此处直接取两个端点作为线性拟合后的端点
	line.pt1.x = vLine[0].pt1.x;
	line.pt1.y = vLine[0].pt1.y;
	line.pt2.x = vLine[nSum-1].pt2.x;
	line.pt2.y = vLine[nSum-1].pt2.y;
}

// 统计角度范围值
// const vector<vector<LINECAP>>& vvLines: 边界线
// int* pNum: 输出值，共18个元素，意为将180度平分为18份，每份10度
// 统计每10度范围内的边界线条数
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

// 统计边界线
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

		// 判断是否没有当前fLength的数量统计值
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
 * 函数名称：  CFindCrack::GetCirclePoints()
 * 作者：      [Leng]
 * 创建于:     [2014-7-30 18:31:25]
 * 参数列表： 
               [POINT ptCenter] - [In]　圆心点坐标
               [ float fRadius] - [In]  半径
               [ POINT** ppCirclePts] - [In] 输出的圆上的坐标点
               [ int& nPtNum] - [In] 输出的圆上的坐标点的个数
注释 :	中点圆整数算法 画圆，获得以ptCenter为圆心,fRadius为半径的圆边界上的所有点的坐标
==================================================================*/
void CFindCrack::GetCirclePoints(POINT ptCenter, float fRadius, POINT** ppCirclePts, int& nPtNum)
{
	/*
	int x=0;
	float y = fRadius;
	
	int d=1-fRadius; //起点(0,R),下一点中点(1,R-0.5),d=1*1+(R-0.5)*(R-0.5)-R*R=1.25-R,d只参与整数运算，所以小数部分可省略
	
	while(y>x) //y>x即第一象限的第2区八分圆
	{
		pDC->SetPixel(x+ptCenter.x, y+ptCenter.y,RGB(255,0,0)); //圆心(x1,y1),画点时直接相加平移,画2区
		pDC->SetPixel(y+ptCenter.x,x+ptCenter.y,RGB(255,0,0)); //画1区
		pDC->SetPixel(-x+ptCenter.x,y+ptCenter.y,RGB(255,0,0)); //画3区
		pDC->SetPixel(-y+ptCenter.x,x+ptCenter.y,RGB(255,0,0)); //画4区
		pDC->SetPixel(-x+ptCenter.x,-y+ptCenter.y,RGB(255,0,0)); //画5区
		pDC->SetPixel(-y+ptCenter.x,-x+ptCenter.y,RGB(255,0,0)); //画6区
		pDC->SetPixel(x+ptCenter.x,-y+ptCenter.y,RGB(255,0,0)); //画7区
		pDC->SetPixel(y+ptCenter.x,-x+ptCenter.y,RGB(255,0,0)); //画8区
		
		if(d<0)
			d=d+2*x+3; //d的变化
		else
		{
			d=d+2*(x-y)+5; //d<=0时,d的变化
			y--; //y坐标减1
		}
		
		x++; //x坐标加1
	}*/
}

/* ==================================================================
 * 函数名称：  CFindCrack::GetLinePoint()
 * 作者：      [Leng]
 * 创建于:     [2014-7-30 18:33:10]
 * 返回值：    [POINT] - 过pt点、角度为fAngle的直线上的以pt为圆点，
		　　　　　　　　在[0,180)范围内的第i点或在[180,360)范围内的第i点
 * 参数列表： 
               [POINT pt] - [In] 过所求直线上的点
               [ float fAngle] - [In] 直线的角度
               [ BOOL bUp] - [In]　TRUE:以pt为圆点的[0,180)范围　FALSE:以pt为圆点的[180,360)范围
               [ int nPtIdx] - [In] 所求点的序号
注释 :	获得过pt点、角度为fAngle的直线上的以pt为圆点，
		在[0,180)范围内的第i点或在[180,360)范围内的第i点	
==================================================================*/
POINT CFindCrack::GetLinePoint(POINT pt, float fAngle, BOOL bUp, int nPtIdx)
{
	int   dx,dy,n,k,i,f; 
	POINT ptRet;

	// 求直线上的距pt较远的某一点
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

// 沿端点处两边界线点与边界线夹角45度方向将各目标点设置为背景点
// CImage* pBinImage: 二值图像，裂缝灰度值为0
// vector<POINT>& vPoints: 一条细化边界线上的点，无分叉
BOOL CFindCrack::SharpenEnds(CImage* pBinImage, vector<POINT>& vPoints)
{
	int i, k, l, m, n;
	POINT pt1, pt2, pt, ptD, ptDs, ptDe, ptD1, ptD2;
	vector<POINT> vBlade1, vBlade2, *pBlade;
	int nEnd1Idx = -1, nEnd2Idx = -1, nMark = 0;

	POINT move[] = {{1,0}, {1,1}, {0,1}, {-1,1}, {-1,0}, {-1,-1}, {0,-1},{1,-1}};
	int nSize = vPoints.size();
	for (k=0; k<2; k++)
	{// k=0时分析边界线头，k=1时分析边界线尾
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

			// 分析vPoints[j]是vPoints[i]的8邻域中的哪一个点
			for (l=0; l<8; l++)
			{
				if (move[l].x==ptD.x && move[l].y==ptD.y)
					break;
			}
			// 根据分析到的8邻域中的点的序号求前一个和后一个8邻域点
			ptD1 = move[(l-1+8)%8];
			ptD2 = move[(l+1+8)%8];

			// 将端点作为第一个点压入
			pt = vPoints[m];
			pBlade->push_back(vPoints[m]);
			int nEndSize = pBlade->size();

			// 看一条45度角边缘线有多长
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

			// 看另一条45度角边缘线有多长
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
			{// 删除无效的点，保留细化线上的点
				pBlade->erase(pBlade->begin()+nEndSize, pBlade->end());
				continue;
			}

			// 计算清除时涉及的细化线上的点
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

	// 若两边端点不满足条件，未完全进行处理；或第一端点序号不小于第二端点序号，则返回FALSE
	if (nMark < 2 || (nEnd1Idx < 0 || nEnd2Idx < 0 && nEnd1Idx >= nEnd2Idx))
		return FALSE;

	// 将获得的点设置为背景色
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

		// 当两条45度角边缘线均满足条件时，将45度角边缘线上的点设置为背景色
		for (l=0; l<pBlade->size(); l++)
		{
			pt = (*pBlade)[l];
			pBinImage->SetPixelColor(pt.x, pt.y, RGB(255,255,255));
		}

		// 将最靠近端点的点按相同方法进行处理
		for (l=0; l<pBlade->size(); l++)
		{
			pt.x = (*pBlade)[l].x+ptD.x;
			pt.y = (*pBlade)[l].y+ptD.y;
			pBinImage->SetPixelColor(pt.x, pt.y, RGB(255,255,255));
		}
	}
	
	return TRUE;
}

// 获得经削尖45度的裂缝的宽度及面积
/* ==================================================================
 * 函数名称：  CFindCrack::GetSharpenLineCrackMaxWidth()
 * 作者：      [Leng]
 * 创建于:     [2014-7-31 16:43:47]
 * 返回值：    [float] - 返回最大裂缝宽度
  * 参数列表： 
               [CImage* pBinImage] - [In] 原始二值图像
               [ const vector<POINT>& vPoints] - [In] 裂缝细化边界线上的点
               [ POINT& mwPoint] - [Out]　输出最大裂缝宽度处的细化线上的点
注释 : 获得经削尖45度的裂缝的宽度及面积
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
		
		// 分析vPoints[j]是vPoints[i]的8邻域中的哪一个点
		for (j=0; j<8; j++)
		{
			if (move[j].x==ptD.x && move[j].y==ptD.y)
				break;
		}
		// 根据分析到的8邻域中的点的序号求垂直方向的上下两个8邻域点
		ptD1 = move[(j-2+8)%8];
		ptD2 = move[(j+2+8)%8];

		// 求第i点沿ptD1方向垂直与裂缝边的交点
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
			if (bGray == 128) // 128为细化线标记，如果相等，说明与细化线相交了，不满足要求
				continue;
		}

		// 求第i点沿ptD2方向垂直与裂缝边的交点
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

		// 求pt1到pt2的距离，即裂缝宽度
		float fDist = get_distance(pt1, pt2);
		float fDist1 = get_distance(ptCenter, pt1);
		float fDist2 = get_distance(ptCenter, pt2);
		// 若fDist1与fDist2的差值较小，说明在计算ptCenter处的裂缝宽度时，没有与其它裂缝分支相交，可作为真实裂缝宽度
		if (fMaxWidth < fDist && fabs(fDist1 - fDist2) < 5)
		{
			fMaxWidth = fDist;
			mwPoint = vPoints[i];
		}
	}

	return fMaxWidth;
}

// 获得经削尖45度的裂缝的面积
float CFindCrack::GetSharpenLineCrackArea(CImage* pBinImage, POINT & ptSeed)
{
	ObjRunLenCode objRLC;
	pBinImage->ObjRLC(&objRLC, ptSeed, 0);
	return objRLC.ObjArea(0);
}
