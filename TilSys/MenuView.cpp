#include "StdAfx.h"
#include "TilSys.h"
#include "TilSysDoc.h"
#include "TilCanvas.h"
#include <process.h>

void CTilSysDoc::OnViewZoomin() 
{
	m_lMenuCommand = ID_VIEW_ZOOMIN;
	m_hThread = (HANDLE)_beginthread(RunImageThread, 0, this);
}

void CTilSysDoc::OnViewZoomout() 
{
	m_lMenuCommand = ID_VIEW_ZOOMOUT;
	m_hThread = (HANDLE)_beginthread(RunImageThread, 0, this);
}

void CTilSysDoc::OnViewZoomnormal() 
{
	m_lMenuCommand = ID_VIEW_ZOOMNORMAL;
	m_hThread = (HANDLE)_beginthread(RunImageThread, 0, this);
}

void CTilSysDoc::OnUpdateViewZoomin(CCmdUI* pCmdUI) 
{
	long double dbZoomX, dbZoomY;
	CTilCanvasMgr* pMgr = GetCanvasMgr();
	pMgr->GetZoomFactor(dbZoomX, dbZoomY);

	if (dbZoomX < Min_Zoom_Factor || dbZoomX > Max_Zoom_Factor
		|| pMgr->GetCurCanvas() == NULL || m_hThread)
		pCmdUI->Enable(0);
}

void CTilSysDoc::OnUpdateViewZoomout(CCmdUI* pCmdUI) 
{
	OnUpdateViewZoomin(pCmdUI);
}

void CTilSysDoc::OnUpdateViewZoomnormal(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	long double dbZoomX, dbZoomY;
	CTilCanvasMgr* pMgr = GetCanvasMgr();
	pMgr->GetZoomFactor(dbZoomX, dbZoomY);
	pCmdUI->Enable((dbZoomX!=1.0) && (dbZoomY!=1.0));
	if ((dbZoomX == 1.0 && dbZoomY == 1.0) ||
		pMgr->GetCurCanvas() == NULL || m_hThread)
		pCmdUI->Enable(0);
}
