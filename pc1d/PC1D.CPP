/* PC1D Semiconductor Device Simulator
Copyright (C) 2003 University of New South Wales
Authors: Paul A. Basore, Donald A. Clugston

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
// view switching added by DAC 27/11/95

#include "stdafx.h"
#include "path.h"
#include "hints.h"
#include "pc1d.h"
#include "mainfrm.h"
#include "pc1ddoc.h"
#include "parmview.h"
#include "4graphvw.h"
#include "actview.h"	
#include "optionsdlg.h"

#include "Splash.h"
#include "useful.h"

//#include <afxpriv.h>	// needed for view switching
// From <afxpriv.h>. This message is needed for view switching.
//  #including it causes problems for debug compiles sometimes.
#define WM_INITIALUPDATE    0x0364  // (params unused) - sent to children


// #include "splshwnd.h"
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CPc1dApp

BEGIN_MESSAGE_MAP(CPc1dApp, CWinApp)
	//{{AFX_MSG_MAP(CPc1dApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_VIEW_INTERACTIVEGRAPH, OnViewInteractivegraph)
	ON_COMMAND(ID_VIEW_FOURGRAPHS, OnViewFourgraphs)
	ON_COMMAND(ID_VIEW_PARAMETERS, OnViewParameters)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PARAMETERS, OnUpdateViewParameters)
	ON_UPDATE_COMMAND_UI(ID_VIEW_INTERACTIVEGRAPH, OnUpdateViewInteractivegraph)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FOURGRAPHS, OnUpdateViewFourgraphs)
	ON_COMMAND(ID_OPTIONS_FILELOCATIONS, OnOptionsFilelocations)
	ON_COMMAND(ID_OPTIONS_DISPLAY, OnOptionsDisplay)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPc1dApp construction

CPc1dApp::CPc1dApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPc1dApp object

CPc1dApp NEAR theApp;

/////////////////////////////////////////////////////////////////////////////
// CPc1dApp initialization

BOOL CPc1dApp::InitInstance()
{
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
//	CSplashWnd::EnableSplashScreen(cmdInfo.m_bShowSplash);
	if (m_lpCmdLine[0] == '\0')
	{
		CSplashWnd::EnableSplashScreen(true);
	} else { 
		CSplashWnd::EnableSplashScreen(false);
	}

	// Standard initialization

//	SetDialogBkColor();        // Set dialog background color to gray
	LoadStdProfileSettings(10);  // Load standard INI file options (including MRU)

	// Register the document templates.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CPc1dDoc),
		RUNTIME_CLASS(CMainFrame),     // main SDI frame window
		RUNTIME_CLASS(CParameterView));
	AddDocTemplate(pDocTemplate); 

	// load ini file values
    m_Path.LoadFromINIfile();
	double lo, hi;
	lo=CUseful::GetProfileDouble("Options", "SciDoubleMin", 0.01);
	hi=CUseful::GetProfileDouble("Options", "SciDoubleMax", 1e5);
	SetScientificDoubleDisplayRange(lo, hi);

	// enable file manager drag/drop and DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes();

	OnFileNew();		

	// This next section of code obtained from Microsoft's WWW knowledge base for C++: (keyword MfcDocView)
	// Article ID: Q99562 "Switching Views in a Single Document Interface Program"
	//   http://198.105.232.5:80/KB/DEVELOPR/visual_c/Q99562.htm
	// BEGIN Microsoft WWW code [[[
       
    CView* pActiveView = ((CFrameWnd*) m_pMainWnd)->GetActiveView();
    m_pParameterView = pActiveView;
    
    // Initialize a CCreateContext to point to the active document.
    // With this context, the new view is added to the document
    // when the view is created in CView::OnCreate().
    CCreateContext newContext;
    newContext.m_pNewViewClass = NULL;
    newContext.m_pNewDocTemplate = NULL;
    newContext.m_pLastView = NULL;
    newContext.m_pCurrentFrame = NULL;
    newContext.m_pCurrentDoc = ((CFrameWnd*) m_pMainWnd)->GetActiveDocument();
    
    CRect rect(0, 0, 0, 0); // gets resized later

    m_pInteractiveGraphView = (CView*) new CInteractiveGraphView;

    // The ID of the initial active view is AFX_IDW_PANE_FIRST.
    // Incrementing this value by one for additional views works
    // in the standard document/view case but the technique cannot
    // be extended for the CSplitterWnd case.
    UINT viewID = AFX_IDW_PANE_FIRST + 1;

    // Create the new view. In this example, the view persists for
    // the life of the application. The application automatically
    // deletes the view when the application is closed.
    m_pInteractiveGraphView->Create(NULL, "AnyWindowName", WS_CHILD, rect,
                            m_pMainWnd, viewID, &newContext);

    // When a document template creates a view, the WM_INITIALUPDATE
    // message is sent automatically. However, this code must
    // explicitly send the message, as follows.
    m_pInteractiveGraphView->SendMessage(WM_INITIALUPDATE, 0, 0);
 
  // ]]] END Microsoft WWW code
    
    m_pFourGraphsView = (CView*) new CFourGraphsView;
    viewID = AFX_IDW_PANE_FIRST + 2;
    m_pFourGraphsView->Create(NULL, "AnyWindowName2", WS_CHILD, rect,
                            m_pMainWnd, viewID, &newContext);
    m_pFourGraphsView->SendMessage(WM_INITIALUPDATE, 0, 0);

	CDonStatusBar::SetSilent(false);

	// simple command line parsing
	if (m_lpCmdLine[0] == '\0')
	{
		// create a new (empty) document
//		OnFileNew();		
	}else if (((m_lpCmdLine[0] == '/') || (m_lpCmdLine[0] == '-')) && m_lpCmdLine[1]=='g') {
//		AfxMessageBox("Opening using -g option");
//		m_pMainWnd->SendMessage(WM_COMMAND, ID_COMPUTE_RUN);
//	   	CPc1dDoc* pCurrentDoc = (CPc1dDoc *) ( ((CFrameWnd*) m_pMainWnd)->GetActiveDocument() );
//		pCurrentDoc->OnComputeRun();
		CDonStatusBar::SetSilent(true);
		char *p = m_lpCmdLine+2;
		while (*p!=0 && (*p==' ')) p++;

		OpenDocumentFile(p);
//		OpenDocumentFile("g:\\pc1d5\\pvcell.prm");
	   	CPc1dDoc* pCurrentDoc = (CPc1dDoc *) ( ((CFrameWnd*) m_pMainWnd)->GetActiveDocument() );

//		for (int qq=0; qq<5; qq++) {
			pCurrentDoc->GetProblem()->DoRun(true);
   			while (pCurrentDoc->IsCalculationInProgress()) {
				pCurrentDoc->DoNextPartOfCalculation();
			}
//		}
		CInteractiveGraphView *actview = (CInteractiveGraphView *)m_pInteractiveGraphView;
		actview->CopyEntireGraphToClipboard();
		return FALSE;

//		AfxMessageBox("Opening using -g option - results on clipboard");


//		OnFileNew();
	} else {
		// open an existing document
		OpenDocumentFile(m_lpCmdLine);
	}               

	m_pMainWnd->DragAcceptFiles();

	return TRUE;
}

int CPc1dApp::ExitInstance()
{
//	delete SplashWnd;
	CWinApp::ExitInstance();
	return 0;
}	

 
	// This next function obtained from Microsoft's WWW knowledge base for C++: (keyword MfcDocView)
	// Article ID: Q99562 "Switching Views in a Single Document Interface Program"
	//   http://198.105.232.5:80/KB/DEVELOPR/visual_c/Q99562.htm

CView* CPc1dApp::SwitchView(CView* pNewView)
{
         CView* pActiveView =
            ((CFrameWnd*) m_pMainWnd)->GetActiveView();
            
         if (pNewView==pActiveView) 
         		return pActiveView; //DAC 20/12/95: Don't allow switch to same view

         // exchange view window ID's so RecalcLayout() works
/****
         UINT temp = ::GetWindowWord(pActiveView->m_hWnd, GWW_ID);
         ::SetWindowWord(pActiveView->m_hWnd, GWW_ID,
                         ::GetWindowWord(pNewView->m_hWnd, GWW_ID));
         ::SetWindowWord(pNewView->m_hWnd, GWW_ID, temp);
****/
         UINT temp = ::GetWindowLong(pActiveView->m_hWnd, GWL_ID);
         ::SetWindowLong(pActiveView->m_hWnd, GWL_ID,
                         ::GetWindowWord(pNewView->m_hWnd, GWL_ID));
         ::SetWindowLong(pNewView->m_hWnd, GWL_ID, temp);

         pActiveView->ShowWindow(SW_HIDE);
         pNewView->ShowWindow(SW_SHOW);
         ((CFrameWnd*) m_pMainWnd)->SetActiveView(pNewView);
         ((CFrameWnd*) m_pMainWnd)->RecalcLayout();
         pNewView->Invalidate();
         return pActiveView;
}
	
/////////////////////////////////////////////////////////////////////////////
// CPc1dApp commands

void CPc1dApp::OnOptionsFilelocations()
{
	m_Path.SetPaths();	
}

void CPc1dApp::OnViewParameters()
{
	SwitchView(m_pParameterView);
	
}

void CPc1dApp::OnViewFourgraphs()
{                                
	CDocument *pDoc=((CFrameWnd*)m_pMainWnd)->GetActiveDocument();
	pDoc->UpdateAllViews(NULL, HINT_SWITCHTO4GRAPHS);
	SwitchView(m_pFourGraphsView);	
}

void CPc1dApp::OnViewInteractivegraph()
{
	SwitchView(m_pInteractiveGraphView);	
}

void CPc1dApp::OnUpdateViewParameters(CCmdUI* pCmdUI)
{
    CView* pActiveView =((CFrameWnd*) m_pMainWnd)->GetActiveView();
    pCmdUI->SetCheck(pActiveView==m_pParameterView);	
}

void CPc1dApp::OnUpdateViewInteractivegraph(CCmdUI* pCmdUI)
{
    CView* pActiveView =((CFrameWnd*) m_pMainWnd)->GetActiveView();
    pCmdUI->SetCheck(pActiveView==m_pInteractiveGraphView);	
}

void CPc1dApp::OnUpdateViewFourgraphs(CCmdUI* pCmdUI)
{
    CView* pActiveView =((CFrameWnd*) m_pMainWnd)->GetActiveView();
    pCmdUI->SetCheck(pActiveView==m_pFourGraphsView);	
}
                              
BOOL CPc1dApp::OnIdle(LONG lCount)
{
	BOOL bMore = CWinApp::OnIdle(lCount);
   	CPc1dDoc* pCurrentDoc = (CPc1dDoc *) ( ((CFrameWnd*) m_pMainWnd)->GetActiveDocument() );
   	if (pCurrentDoc->IsCalculationInProgress()) {
		bMore |= pCurrentDoc->DoNextPartOfCalculation();
	}
    return bMore;
}   
	

 
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CPc1dApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

BOOL CPc1dApp::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following line was added by the Splash Screen component.
	CSplashWnd::PreTranslateAppMessage(pMsg);
	return CWinApp::PreTranslateMessage(pMsg);
}

void CPc1dApp::OnOptionsDisplay() 
{
	COptionsDlg dlg;
	GetScientificDoubleDisplayRange(dlg.m_SciLower, dlg.m_SciUpper);
	if (dlg.DoModal()) {
		SetScientificDoubleDisplayRange(dlg.m_SciLower, dlg.m_SciUpper);
		CUseful::WriteProfileDouble("Options", "SciDoubleMin", dlg.m_SciLower);
		CUseful::WriteProfileDouble("Options", "SciDoubleMax", dlg.m_SciUpper);
	}
}
