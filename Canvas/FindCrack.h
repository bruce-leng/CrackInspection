// FindCrack.h: interface for the CFindCrack class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FINDCRACK_H__D4532C51_D43E_4F46_B29A_01B1A8FD8376__INCLUDED_)
#define AFX_FINDCRACK_H__D4532C51_D43E_4F46_B29A_01B1A8FD8376__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Image.h"

// 裂缝的特征
typedef struct __tagCRACK
{
	float fWmax;	// 裂缝最大宽度（预留）（单位：mm）
	float fLength;	// 裂缝长度（单位：mm）
	float fArea;	// 裂缝面积（单位：mm2）
	float fL2A;		// 裂缝周长与面积之比(fCircLen/fArea)（预留）
	float fGave;	// 裂缝区域灰度均值
	float fGvari;	// 裂缝区域灰度方差
} CRACK, *PCRACK, **PPCRACK;
// 所有裂缝特征
typedef struct __tagCRACKMGR
{
	int nNum;
	CRACK elem[1];
} CRACK_MGR, *PCRACK_MGR, **PPCRACK_MGR;

class CFindCrack  
{
public:
	CFindCrack(CImage* pSrcImage, CImage* pBinImage);
	virtual ~CFindCrack();

	//////////////////////////////////////////////////////////////////////////
	// 裂缝特征分析
	// 根据裂缝特征识别裂缝
	void CrackExtraction(float fCircLenT, float fAreaT, float fL2AT, float fGaveT, float fGvariT,
						ObjRunLenCode** ppCrackObj, CRACK_MGR** ppCrackMgr);
	void FindCrack(float fCircLenT, float fAreaT, float fL2AT, float fGaveT, float fGvariT,
						ObjRunLenCode** ppCrackObj, CRACK_MGR** ppCrackMgr);

private:
	// 原始图像
	CImage* m_pSrcImage;
	// 原始图像经处理后形成的二值图像
	CImage* m_pBinImage;


	//////////////////////////////////////////////////////////////////////////
	// 以下用于处理单像素宽边界图像
	// 矩阵，用于存储图像数据
	typedef struct __tagMATRIX
	{
		int nWidth, nHeight;
		BYTE elem[1];
	} MATRIX, *PMATRIX, **PPMATRIX;
	// 线段端点及倾斜角度
	typedef struct __tagLINECAP
	{
		POINT pt1, pt2;	// 端点
		float fAngle;		// 倾斜角度
	} LINECAP, *PLINECAP, **PPLINECAP;
	// 两条线段之间的相关性
	typedef struct __tagRELATION
	{
		int nLine1, nEnd1;	// 第1条线段上的端点序号
		int nLine2, nEnd2;	// 第2条线段上的端点序号
		float fRelation;	// 相关性值
	} RELATION, *PRELATION, **PPRELATION;
	// 将目标拷贝至内存中
	void CopyBuffer(PRUNLENGTHCODE_MGR pRLCMgr, MATRIX** ppMatrix);
	// 对矩阵中的目标进行二值细化
	void MorphThining(MATRIX* pMatrix);
	// 清除单像素宽边界二值图像中的单个目标点或者有边界线的分叉点
	void ClearNode(MATRIX* pMatrix);
	// 查找目标边界线上的点
	void FindEdgePoints(MATRIX* pMatrix, vector<deque<POINT>*>& vdPoints);
	// 对多点组成的多段线进行线性拟合，并返回多段线长度
	float LinesFit(const deque<POINT>& dPoints, const float& fDist, vector<POINT>& vLines);
	void LinesFit(const deque<POINT>& dPoints, const float& fDist, vector<LINECAP>& vLines);
	// 判断是否是封闭边界线，若不是，则取得一个边界端点
	int IsClosed(MATRIX* pMatrix, POINT& ptEnd);
	// 对线性拟合的边界线进行分析,将同属于一条边界线的各线段建立连接关系
	void LinkSimilarLines(vector< vector< LINECAP> >& lineArray);

	// 获得两直线间夹角
	float GetAngle(const LINECAP& line1, const LINECAP& line2);
	// 获得两直线间端点到另一直线的距离和
	float GetDistSum(const LINECAP& line1, const int& nIdx1, const LINECAP& line2, const int& nIdx2);
	// 判断两直线的端点到另一条线段的垂足是否在线段上
	BOOL IsVertPtInLine(const LINECAP& line1, const LINECAP& line2);
	// 获得两线段端点间的最小距离，并记录最小距离点
	float GetMinDist(const LINECAP& line1, const LINECAP& line2, float& fMinDist, int& nIdx1, int& nIdx2);
	// 查找满足条件的两边界线的相连的两点
	BOOL BuildAdjacent(const vector<LINECAP>& lineCaps1,
		const vector<LINECAP>& lineCaps2, 
		const float& fDsumT, const float& fDminT,
		const float& fAngleT, const float& fLengthT,
		int& nEnd1, int& nEnd2, float& fRelation);
	// 查找与当前边界线相连的下一条边界线
	BOOL FindNextNeighborEdge(
		const vector< vector< RELATION* > >& vvRelation, 
		const int& nLine1, const int& nEnd1,
		int& nLine2, int& nEnd2);
	// 利用最小二乘法对多段线进行线性拟合
	void LineFit(vector<LINECAP>& vLine, LINECAP& line);
	// 统计角度范围值
	void StatAngles(const vector<vector<LINECAP> >& vvLines, int* pNum);
	// 统计边界线
	void StatLines(const vector<vector<LINECAP> >& vvLines, vector<int>& vSum);

	// 沿端点处两边界线点与边界线夹角45度方向将各目标点设置为背景点
	BOOL SharpenEnds(CImage* pBinImage, vector<POINT>& vPoints);
	// 获得经削尖45度的裂缝的最大宽度
	float GetSharpenLineCrackMaxWidth(CImage* pBinImage, const vector<POINT>& vPoints, POINT& mwPoint);
	// 获得经削尖45度的裂缝的面积
	float GetSharpenLineCrackArea(CImage* pBinImage, POINT & ptSeed);
	
	// 获得以ptCenter为圆心,fRadius为半径的圆边界上的所有点的坐标
	void GetCirclePoints(POINT ptCenter, float fRadius, POINT** ppCirclePts, int& nPtNum);
	// 获得过pt点、角度为fAngle的直线上的以pt为圆点，在[0,180)范围内的第i点或在[180,360)范围内的第i点
	POINT GetLinePoint(POINT pt, float fAngle, BOOL bUp, int nPtIdx);
};

#endif // !defined(AFX_FINDCRACK_H__D4532C51_D43E_4F46_B29A_01B1A8FD8376__INCLUDED_)
