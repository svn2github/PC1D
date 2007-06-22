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

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#define PC1D_H 1

/////////////////////////////////////////////////////////////////////////////
// CPc1dApp:
// See pc1d.cpp for the implementation of this class


class CPc1dApp : public CWinApp
{
private:
	CView *m_pFourGraphsView;   
	CView *m_pInteractiveGraphView;
	CView *m_pParameterView;
	static int idleRecurseCount;      // used by OnIdle()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CPath m_Path;	

public:
	CPc1dApp();
	CView* CPc1dApp::SwitchView(CView* pNewView);

// Overrides
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);

// Implementation

	//{{AFX_MSG(CPc1dApp)
	afx_msg void OnAppAbout();
	afx_msg void OnViewInteractivegraph();
	afx_msg void OnViewFourgraphs();
	afx_msg void OnViewParameters();
	afx_msg void OnUpdateViewParameters(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewInteractivegraph(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewFourgraphs(CCmdUI* pCmdUI);
	afx_msg void OnOptionsFilelocations();
	afx_msg void OnOptionsDisplay();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


