; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CListCtrlEx
LastTemplate=CListCtrl
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "guiext.h"
LastPage=0

ClassCount=14
Class1=CCanvasHistogram
Class2=CCanvasSection
Class3=CComboListCtrl
Class4=CInPlaceCombo
Class5=CInPlaceEdit
Class6=CLogEdit
Class7=CLogInfo
Class8=CPreviewList
Class9=CRulerSplitterWnd
Class10=CRulerView
Class11=CRulerCornerView
Class12=CSplashWnd
Class13=CUndoTree

ResourceCount=5
Resource1=IDR_POPUP_CANVAS_SECTION
Resource2=IDR_POPUP_PREVIEW_LIST_EDIT_INFO
Resource3=CG_IDR_POPUP_PREVIEW_LIST
Class14=CListCtrlEx
Resource4=IDR_POPUP_CANVAS_HISTOGRAM
Resource5=IDR_POPUP_CRACK_PROC

[CLS:CCanvasHistogram]
Type=0
BaseClass=CStatic
HeaderFile=CanvasHistogram.h
ImplementationFile=CanvasHistogram.cpp
LastObject=ID_SECTION_NORMAL

[CLS:CCanvasSection]
Type=0
BaseClass=CStatic
HeaderFile=CanvasSection.h
ImplementationFile=CanvasSection.cpp
LastObject=ID_ROCK_RECOG

[CLS:CComboListCtrl]
Type=0
BaseClass=CListCtrl
HeaderFile=ComboListCtrl.h
ImplementationFile=ComboListCtrl.cpp

[CLS:CInPlaceCombo]
Type=0
BaseClass=CComboBox
HeaderFile=InPlaceCombo.h
ImplementationFile=InPlaceCombo.cpp

[CLS:CInPlaceEdit]
Type=0
BaseClass=CEdit
HeaderFile=InPlaceEdit.h
ImplementationFile=InPlaceEdit.cpp

[CLS:CLogEdit]
Type=0
BaseClass=CEdit
HeaderFile=LogEdit.h
ImplementationFile=LogEdit.cpp

[CLS:CLogInfo]
Type=0
BaseClass=CListBox
HeaderFile=LogEdit.h
ImplementationFile=LogEdit.cpp

[CLS:CPreviewList]
Type=0
BaseClass=CListCtrl
HeaderFile=previewlist.h
ImplementationFile=PreviewList.cpp
Filter=W
VirtualFilter=FWC
LastObject=ID_IMAGE_INFO

[CLS:CRulerSplitterWnd]
Type=0
BaseClass=CSplitterWnd
HeaderFile=ruler.h
ImplementationFile=ruler.cpp

[CLS:CRulerView]
Type=0
BaseClass=CView
HeaderFile=ruler.h
ImplementationFile=ruler.cpp

[CLS:CRulerCornerView]
Type=0
BaseClass=CView
HeaderFile=ruler.h
ImplementationFile=ruler.cpp

[CLS:CSplashWnd]
Type=0
BaseClass=CWnd
HeaderFile=Splash.h
ImplementationFile=Splash.cpp

[CLS:CUndoTree]
Type=0
BaseClass=CListBox
HeaderFile=UndoTree.h
ImplementationFile=UndoTree.cpp

[MNU:CG_IDR_POPUP_PREVIEW_LIST]
Type=1
Class=?
Command1=ID_OPEN_DIR
CommandCount=1

[MNU:IDR_POPUP_PREVIEW_LIST_EDIT_INFO]
Type=1
Class=?
Command1=ID_PREVIEWLIST_ADD_FILE
Command2=ID_IMAGE_INFO
Command3=ID_RECOG_TEXTURE
Command4=ID_ADD_TO_FACE
CommandCount=4

[MNU:IDR_POPUP_CANVAS_HISTOGRAM]
Type=1
Class=?
Command1=ID_HISTOGRAM_RED
Command2=ID_HISTOGRAM_GREEN
Command3=ID_HISTOGRAM_BLUE
Command4=ID_HISTOGRAM_GRAY
Command5=ID_HISTOGRAM_PROPERTY
CommandCount=5

[MNU:IDR_POPUP_CANVAS_SECTION]
Type=1
Class=?
Command1=ID_SECTION_NORMAL
Command2=ID_SECTION_PROFILE
CommandCount=2

[CLS:CListCtrlEx]
Type=0
HeaderFile=ListCtrlEx.h
ImplementationFile=ListCtrlEx.cpp
BaseClass=CListCtrl
Filter=W
LastObject=ID_CRACK_DEL
VirtualFilter=FWC

[MNU:IDR_POPUP_CRACK_PROC]
Type=1
Class=?
Command1=ID_CRACK_DEL
CommandCount=1

