// FindCrack.h: interface for the CFindCrack class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FINDCRACK_H__D4532C51_D43E_4F46_B29A_01B1A8FD8376__INCLUDED_)
#define AFX_FINDCRACK_H__D4532C51_D43E_4F46_B29A_01B1A8FD8376__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Image.h"

// �ѷ������
typedef struct __tagCRACK
{
	float fWmax;	// �ѷ�����ȣ�Ԥ��������λ��mm��
	float fLength;	// �ѷ쳤�ȣ���λ��mm��
	float fArea;	// �ѷ��������λ��mm2��
	float fL2A;		// �ѷ��ܳ������֮��(fCircLen/fArea)��Ԥ����
	float fGave;	// �ѷ�����ҶȾ�ֵ
	float fGvari;	// �ѷ�����Ҷȷ���
} CRACK, *PCRACK, **PPCRACK;
// �����ѷ�����
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
	// �ѷ���������
	// �����ѷ�����ʶ���ѷ�
	void CrackExtraction(float fCircLenT, float fAreaT, float fL2AT, float fGaveT, float fGvariT,
						ObjRunLenCode** ppCrackObj, CRACK_MGR** ppCrackMgr);
	void FindCrack(float fCircLenT, float fAreaT, float fL2AT, float fGaveT, float fGvariT,
						ObjRunLenCode** ppCrackObj, CRACK_MGR** ppCrackMgr);

private:
	// ԭʼͼ��
	CImage* m_pSrcImage;
	// ԭʼͼ�񾭴�����γɵĶ�ֵͼ��
	CImage* m_pBinImage;


	//////////////////////////////////////////////////////////////////////////
	// �������ڴ������ؿ�߽�ͼ��
	// �������ڴ洢ͼ������
	typedef struct __tagMATRIX
	{
		int nWidth, nHeight;
		BYTE elem[1];
	} MATRIX, *PMATRIX, **PPMATRIX;
	// �߶ζ˵㼰��б�Ƕ�
	typedef struct __tagLINECAP
	{
		POINT pt1, pt2;	// �˵�
		float fAngle;		// ��б�Ƕ�
	} LINECAP, *PLINECAP, **PPLINECAP;
	// �����߶�֮��������
	typedef struct __tagRELATION
	{
		int nLine1, nEnd1;	// ��1���߶��ϵĶ˵����
		int nLine2, nEnd2;	// ��2���߶��ϵĶ˵����
		float fRelation;	// �����ֵ
	} RELATION, *PRELATION, **PPRELATION;
	// ��Ŀ�꿽�����ڴ���
	void CopyBuffer(PRUNLENGTHCODE_MGR pRLCMgr, MATRIX** ppMatrix);
	// �Ծ����е�Ŀ����ж�ֵϸ��
	void MorphThining(MATRIX* pMatrix);
	// ��������ؿ�߽��ֵͼ���еĵ���Ŀ�������б߽��ߵķֲ��
	void ClearNode(MATRIX* pMatrix);
	// ����Ŀ��߽����ϵĵ�
	void FindEdgePoints(MATRIX* pMatrix, vector<deque<POINT>*>& vdPoints);
	// �Զ����ɵĶ���߽���������ϣ������ض���߳���
	float LinesFit(const deque<POINT>& dPoints, const float& fDist, vector<POINT>& vLines);
	void LinesFit(const deque<POINT>& dPoints, const float& fDist, vector<LINECAP>& vLines);
	// �ж��Ƿ��Ƿ�ձ߽��ߣ������ǣ���ȡ��һ���߽�˵�
	int IsClosed(MATRIX* pMatrix, POINT& ptEnd);
	// ��������ϵı߽��߽��з���,��ͬ����һ���߽��ߵĸ��߶ν������ӹ�ϵ
	void LinkSimilarLines(vector< vector< LINECAP> >& lineArray);

	// �����ֱ�߼�н�
	float GetAngle(const LINECAP& line1, const LINECAP& line2);
	// �����ֱ�߼�˵㵽��һֱ�ߵľ����
	float GetDistSum(const LINECAP& line1, const int& nIdx1, const LINECAP& line2, const int& nIdx2);
	// �ж���ֱ�ߵĶ˵㵽��һ���߶εĴ����Ƿ����߶���
	BOOL IsVertPtInLine(const LINECAP& line1, const LINECAP& line2);
	// ������߶ζ˵�����С���룬����¼��С�����
	float GetMinDist(const LINECAP& line1, const LINECAP& line2, float& fMinDist, int& nIdx1, int& nIdx2);
	// �����������������߽��ߵ�����������
	BOOL BuildAdjacent(const vector<LINECAP>& lineCaps1,
		const vector<LINECAP>& lineCaps2, 
		const float& fDsumT, const float& fDminT,
		const float& fAngleT, const float& fLengthT,
		int& nEnd1, int& nEnd2, float& fRelation);
	// �����뵱ǰ�߽�����������һ���߽���
	BOOL FindNextNeighborEdge(
		const vector< vector< RELATION* > >& vvRelation, 
		const int& nLine1, const int& nEnd1,
		int& nLine2, int& nEnd2);
	// ������С���˷��Զ���߽����������
	void LineFit(vector<LINECAP>& vLine, LINECAP& line);
	// ͳ�ƽǶȷ�Χֵ
	void StatAngles(const vector<vector<LINECAP> >& vvLines, int* pNum);
	// ͳ�Ʊ߽���
	void StatLines(const vector<vector<LINECAP> >& vvLines, vector<int>& vSum);

	// �ض˵㴦���߽��ߵ���߽��߼н�45�ȷ��򽫸�Ŀ�������Ϊ������
	BOOL SharpenEnds(CImage* pBinImage, vector<POINT>& vPoints);
	// ��þ�����45�ȵ��ѷ�������
	float GetSharpenLineCrackMaxWidth(CImage* pBinImage, const vector<POINT>& vPoints, POINT& mwPoint);
	// ��þ�����45�ȵ��ѷ�����
	float GetSharpenLineCrackArea(CImage* pBinImage, POINT & ptSeed);
	
	// �����ptCenterΪԲ��,fRadiusΪ�뾶��Բ�߽��ϵ����е������
	void GetCirclePoints(POINT ptCenter, float fRadius, POINT** ppCirclePts, int& nPtNum);
	// ��ù�pt�㡢�Ƕ�ΪfAngle��ֱ���ϵ���ptΪԲ�㣬��[0,180)��Χ�ڵĵ�i�����[180,360)��Χ�ڵĵ�i��
	POINT GetLinePoint(POINT pt, float fAngle, BOOL bUp, int nPtIdx);
};

#endif // !defined(AFX_FINDCRACK_H__D4532C51_D43E_4F46_B29A_01B1A8FD8376__INCLUDED_)
