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

#include "stdafx.h"
#include "path.h"
#include "pc1d.h"
#include "mainfrm.h"
#include "Splash.h"
#include <afxcmn.h>		// for toolbars

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CPersistentFrame)

BEGIN_MESSAGE_MAP(CMainFrame, CPersistentFrame)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	// Global help commands
	ON_COMMAND(ID_HELP_INDEX, CPersistentFrame::OnHelpIndex)
	ON_COMMAND(ID_HELP_USING, CPersistentFrame::OnHelpUsing)
	ON_COMMAND(ID_HELP, CPersistentFrame::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CPersistentFrame::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CPersistentFrame::OnHelpIndex)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// arrays of IDs used to initialize control bars

/****
// toolbar buttons - IDs are command buttons
static UINT BASED_CODE buttons[] =
{
	// same order as in the bitmap 'toolbar.bmp'
	ID_FILE_NEW,
	ID_FILE_OPEN,
	ID_FILE_SAVE,
		ID_SEPARATOR,
	ID_EDIT_COPY,
		ID_SEPARATOR,
	ID_FILE_PRINT_PREVIEW,
	ID_FILE_PRINT,
		ID_SEPARATOR,
	ID_REGION_NEXT,
	// BEWARE!!! there's a hardcoded value in pc1ddoc.cpp UpdateRegionButton() Must update it if you change the bitmap.
	ID_DEVICE_DOPING_DIFFUSIONS_FIRSTFRONT,
	ID_DEVICE_DOPING_DIFFUSIONS_FIRSTREAR,
	ID_DEVICE_RECOMBINATION_BULK,
	ID_DEVICE_RECOMBINATION_FRONTSURFACE,
	ID_DEVICE_RECOMBINATION_REARSURFACE,
		ID_SEPARATOR,
	ID_EXCITATION_OPEN,
		ID_SEPARATOR,
	ID_COMPUTE_RUN,
	ID_COMPUTE_SINGLESTEP,
	ID_COMPUTE_QUICKBATCH,
		ID_SEPARATOR,
	ID_VIEW_PARAMETERS,
	ID_VIEW_FOURGRAPHS,
	ID_VIEW_INTERACTIVEGRAPH,
    	ID_SEPARATOR,
	ID_HELP_INDEX,
	ID_CONTEXT_HELP,
	// BEWARE!!! there's a hardcoded value in pc1ddoc.cpp UpdateRegionButton() Must update it if you change the bitmap.
};
***/

static UINT BASED_CODE indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_MODE,
	ID_INDICATOR_PROGRESS,
	ID_INDICATOR_STATE,
	ID_INDICATOR_TIME,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPersistentFrame::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.Create(this, WS_CHILD|WS_VISIBLE|CBRS_TOP|CBRS_TOOLTIPS) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	CToolBarCtrl &Bar=m_wndToolBar.GetToolBarCtrl();
	Bar.HideButton(ID_DUD_BUTTON1, TRUE);
	Bar.HideButton(ID_DUD_BUTTON2, TRUE);
	Bar.HideButton(ID_DUD_BUTTON3, TRUE);
	Bar.HideButton(ID_DUD_BUTTON4, TRUE);
	Bar.HideButton(ID_DUD_BUTTON5, TRUE);
/**
	if (!m_wndToolBar.Create(this, WS_CHILD|WS_VISIBLE|CBRS_TOP|CBRS_TOOLTIPS) ||
		!m_wndToolBar.LoadBitmap(IDR_MAINFRAME) ||
		!m_wndToolBar.SetButtons(buttons,
		  sizeof(buttons)/sizeof(UINT)))
	{
		TRACE("Failed to create toolbar\n");
		return -1;      // fail to create
	}

//	m_wndToolBar.SetButtonInfo(9, ID_REGION_NEXT, TBBS_BUTTON, 20+0);
**/
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE("Failed to create status bar\n");
		return -1;      // fail to create
	}
	// CG: The following line was added by the Splash Screen component.
	CSplashWnd::ShowSplashScreen(this);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CPersistentFrame::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CPersistentFrame::Dump(dc);
}

#endif //_DEBUG

