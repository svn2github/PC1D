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
#include "stdio.h"
#include "persist.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
///////////////////////////////////////////////////////////////////////
//	CPersistentFrame

IMPLEMENT_DYNAMIC(CPersistentFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CPersistentFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CPersistentFrame)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////
CPersistentFrame::CPersistentFrame():
	m_profileHeading("Window size"), m_profileRect("Rect"),
	m_profileMax("max"), m_profileTool("tool"), m_profileStatus("status")
{
	m_bFirstTime = TRUE;
}

//////////////////////////////////////////////////////////////////////
CPersistentFrame::~CPersistentFrame()
{
}

//////////////////////////////////////////////////////////////////////
void CPersistentFrame::OnDestroy()
{
	CString	text;
	CWnd*	pBar;
	BOOL	bMaximized;
	
	WINDOWPLACEMENT wndpl;
	wndpl.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(&wndpl);
	if (wndpl.showCmd == SW_SHOWNORMAL) bMaximized = FALSE;
	else if (wndpl.showCmd == SW_SHOWMAXIMIZED) bMaximized = TRUE;
	else if (wndpl.showCmd == SW_SHOWMINIMIZED) bMaximized = wndpl.flags;
	sprintf(text.GetBuffer(20), "%04d %04d %04d %04d",
		wndpl.rcNormalPosition.left, wndpl.rcNormalPosition.top,
		wndpl.rcNormalPosition.right, wndpl.rcNormalPosition.bottom);
	AfxGetApp()->WriteProfileString(m_profileHeading, m_profileRect, text);
	AfxGetApp()->WriteProfileInt(m_profileHeading, m_profileMax, bMaximized);
	text.ReleaseBuffer();
	if (0!=(pBar = GetDescendantWindow(AFX_IDW_TOOLBAR)))
		AfxGetApp()->WriteProfileInt(m_profileHeading, m_profileTool,
			(pBar->GetStyle() & WS_VISIBLE) != 0L);
	if (0!=(pBar = GetDescendantWindow(AFX_IDW_STATUS_BAR)))
		AfxGetApp()->WriteProfileInt(m_profileHeading, m_profileStatus,
			(pBar->GetStyle() & WS_VISIBLE) != 0L);
	CFrameWnd::OnDestroy();
}

//////////////////////////////////////////////////////////////////////
void CPersistentFrame::ActivateFrame(int nCmdShow)
{
	static const width = 750;
	static const margin = 50;

	CWnd*	pBar;
	CString	text;
	BOOL	bMaximized, bTool, bStatus;
	WINDOWPLACEMENT wndpl;
	CRect	rect;
	
	if (!m_bFirstTime) {CFrameWnd::ActivateFrame(nCmdShow); return;}
	m_bFirstTime = FALSE;
	text = AfxGetApp()->GetProfileString(m_profileHeading, m_profileRect);
	if (!text.IsEmpty())
	{
		rect.left = atoi((const char*) text);
		rect.top = atoi((const char*) text + 5);
		rect.right = atoi((const char*) text + 10);
		rect.bottom = atoi((const char*) text + 15);
		bMaximized = AfxGetApp()->GetProfileInt(m_profileHeading, m_profileMax, 0);
	}
	else 
	{
		rect.SetRect(margin, margin, margin+width, margin+3*width/4);
		bMaximized = ::GetSystemMetrics(SM_CXSCREEN)<margin+width;
	}
	if (bMaximized) nCmdShow = SW_SHOWMAXIMIZED;
	else nCmdShow = SW_NORMAL;

	wndpl.length = sizeof(WINDOWPLACEMENT);
	wndpl.showCmd = nCmdShow;
	wndpl.flags = 0;
	wndpl.ptMinPosition = CPoint(0,0);
	wndpl.ptMaxPosition = CPoint(-::GetSystemMetrics(SM_CXBORDER),
								 -::GetSystemMetrics(SM_CYBORDER));
	wndpl.rcNormalPosition = rect;
	
	bTool = AfxGetApp()->GetProfileInt(m_profileHeading, m_profileTool, 1);
	if (0!=(pBar = GetDescendantWindow(AFX_IDW_TOOLBAR))) pBar->ShowWindow(bTool);
	bStatus = AfxGetApp()->GetProfileInt(m_profileHeading, m_profileStatus, 1);
	if (0!=(pBar = GetDescendantWindow(AFX_IDW_STATUS_BAR))) pBar->ShowWindow(bStatus);
	SetWindowPlacement(&wndpl);
	CFrameWnd::ActivateFrame(nCmdShow);
}
	