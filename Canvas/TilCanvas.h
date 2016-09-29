// TilImage.h: interface for the CTilCanvas class.
//
//////////////////////////////////////////////////////////////////////

/************************************
  REVISION LOG ENTRY
  维护人员: Leng
  维护时间 2014-6-5 7:36:44
  说明: 处理单个画布，包含一个原始图像层和一个图像目标层，
        当图像目标层不存在时，只显示原始图像层，否则将图像目标层显示在最上面。
		当处理完成后，图像目标层为经筛选形成的游程编码二值图像
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

	// 打开文件（含原始图像，处理后的裂缝图像或最终裂缝的游程编码文件）
	BOOL Open(const CString& strFilePath, const CString& strFileName);
	BOOL Open(CTilCanvas& canvas);
	void SaveAll();
	BOOL ReadCrackObject(CString strPathFile);
	BOOL SaveCrackObject(CString strPathFile);

	// 设置与获得文件路径
	void SetFilePath(CString strFilePath) {  m_strFilePath = strFilePath; }
	CString GetFilePath() { return m_strFilePath; }

	// 设置与获得文件名
	void SetFileName(CString strFileName) { m_strFileName = strFileName; }
	CString GetFileName() { return m_strFileName; }

	void SetModifiedFlag(BOOL bFlag = TRUE);
	BOOL IsModified() const;

 	void SetZoomFactor(long double dbZoomX, long double dbZoomY);
 	void GetZoomFactor(long double& dbZoomX, long double& dbZoomY);

	// 设置两图层的显示或隐藏
	void SetShowHideLayerSrcImage(BOOL bLayerSrcImage = TRUE) { m_bLayerSrcImage = bLayerSrcImage; }
	void SetShowHideLayerCrackImage(BOOL bLayerCrackImage = TRUE) { m_bLayerCrackImage = bLayerCrackImage; }
	void SetShowHideLayerCrackObj(BOOL bLayerCrackObj = TRUE)	{ m_bLayerCrackObj = bLayerCrackObj; }

	BOOL IsShowLayerSrcImage() { return m_bLayerSrcImage; }
	BOOL IsShowLayerCrackImage() { return m_LayerCrackImage.size()>0 && m_bLayerCrackImage; }
	BOOL IsShowLayerCrackObj() { return m_pCracksObject!=NULL && m_bLayerCrackObj; }

	// 获得当前图层序号(0: 原始图像层，1: 裂缝图像层  2: 裂缝目标层)
	int GetCurLayerIdx() { return m_nMark; }

	// 设置当前的m_nMark和m_nIdx
	void SetCurLayerIdx(BOOL bNext = TRUE);

	// 获得当前显示的最顶层图层（不包含目标层）
	CImage* GetCurImage();

	void Draw(CDC* pDC);

	// 压入原始层图像
	void PushSrcImage(CImage* pImage);
	// 压入裂缝层图像
	void PushCrackImage(CImage* pImage);
	// 弹出原始层图像
	void PopSrcImage(int nBackNum = 1);
	// 弹出目标层图像
	void PopCrackImage(int nBackNum = 1);
	// 获得最新原始层图像
	CImage* GetLastSrcImage();
	// 获得最新目标层裂缝图像
	CImage* GetLastCrackImage();

	// “重复”与“撤销”
	void Undo();
	BOOL CanUndo() { return m_bCanUndo; }
	void Redo();
	BOOL CanRedo() { return m_bCanRedo; }

	int GetWidth();
	int GetHeight();
	int GetDepth();

	// 识别裂缝
	void FindCrack();
	// 已经识别过裂缝了吗
	BOOL IsFindCrack() { return m_pCracksObject != NULL; }
	// 获得目标游程编码
	ObjRunLenCode* GetCrackObject() { return m_pCracksObject; }
	// 获得已识别的裂缝的特征
	CRACK_MGR* GetCrackCharacter() { return m_pCracksCharacter; }
	// 设置被选中的裂缝目标
	void SetSelectedObject(int nSelected) { m_nSelected = nSelected; }
	// 删除裂缝
	void DelSelectedObject();

	// 删除当前m_nMark和m_nIdx后面的图像及目标层（供Undo后再处理了图像时进行操作）
	void DeleteFilesAfterCurLayerIdx();
	// 删除最终目标层
	void DeleteObjLayer();
	// 删除裂缝图像层
	void DeleteCrackLayer();

private:
	//////////////////////////////////////////////////////////////////////////
	// 以下两个参数用于控制对图像处理结果的“撤销”和“重复”操作
	// 当前处理最终结果究竟是“原始图像”、“裂缝目标”还是最终的“裂缝识别结果”的标记
	// 0:原始图像，1:裂缝目标，2:裂缝识别结果
	int m_nMark;
	// 当前显示的图像序号(根据m_nMark决定是“原始图像”、“裂缝目标”还是“裂缝识别结果”的当前显示序号)
	int m_nIdx;
	// 标记是否能进行Undo操作
	BOOL m_bCanUndo;
	// 标记是否能进行Redo操作
	BOOL m_bCanRedo;
	//////////////////////////////////////////////////////////////////////////

 	long double m_dbZoomX;		// 所有图像的水平缩放率
 	long double m_dbZoomY;		// 所有图像的垂直缩放率
	BOOL m_bModified;			// 图像被修改的标记
	BOOL m_bLayerSrcImage;		// 原始图层显示标记
	BOOL m_bLayerCrackImage;	// 裂缝图层显示标记
	BOOL m_bLayerCrackObj;		// 裂缝目标层显示标记
	CString m_strFilePath;		// 文件路径（不含最后一条下划线'\'）
	CString m_strFileName;		// 文件名称（不含扩展名）

	// 裂缝识别结果
	ObjRunLenCode* m_pCracksObject;
	// 裂缝识别结果中被选中的裂缝目标
	int m_nSelected;
	// 所有查找到的裂缝特征
	CRACK_MGR* m_pCracksCharacter;
	// 图像序列，用于存储最近的预处理图像
	vector<CImage*> m_LayerSrcImage;
	// 图像序列，用于存储最近的二值分割及识别的目标图像
	vector<CImage*> m_LayerCrackImage;
};

#endif // !defined(AFX_TILIMAGE_H__7D71C271_2D20_4A26_938B_241593EB1C7A__INCLUDED_)
