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

#ifndef DEVICEDIAGRAM_H
#include "DeviceDiagram.h"
#endif

#ifndef PARMSECTION_H
#include "ParmSection.h"
#endif

#ifndef __AFXCMN_H__
#include <afxcmn.h>
#endif


class CParameterView : public CScrollView
{
public: 
	CParameterView();
	DECLARE_DYNCREATE(CParameterView)
	
// Attributes
public:
	CPc1dDoc* GetDocument();
protected:
	CPc1dDoc*   pDoc;		// set by OnUpdate
	int margin;			// used for parm printing  
	CSize m_sizeParmDoc;
	CFileSection  	   m_filesection;
	CDeviceSection 	   m_devicesection;
	CExcitationSection m_excitationsection;
	CResultsSection    m_resultssection;
	CRegionSection 	   m_regionsection[MAX_REGIONS];
	CBatchSection	   m_batchsection;
	CDeviceDiagram	   m_devicediagram;

//	CString m_ParmFontName;		// font used for parameter display
//	int m_ParmFontHgt;			// height in twips


// Operations
public:

protected:

// Implementation
public:
	virtual ~CParameterView();
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual void OnUpdate(CView* pView, LPARAM lHint, CObject* pHint);
	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	// Drawing support
	void RedoTextForAllSections(void);
	void CalculateMetrics(CDC *pDC);
	void CalculateSectionCoordinates(CDC *pDC);
	int TotalDocLength();	// calculates total length of document
	int TotalDocWidth();	
	void CalculateScrollSizes(void);
	// Printing support
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);

// Generated message map functions
protected:
	//{{AFX_MSG(CParameterView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnSelectParmFont();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in parmview.cpp
inline CPc1dDoc* CParameterView::GetDocument()
   { return (CPc1dDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
