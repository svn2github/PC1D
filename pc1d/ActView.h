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
// actview.h : interface of the CInteractiveGraphView class
//
/////////////////////////////////////////////////////////////////////////////

const int MAXGRAPHSTOSAVE=100;
    
#ifndef INTERACTIVEGRAPH_HEADER
#include "actgraph.h"
#endif

#ifndef PLOTLOAD_HEADER
#include "plotload.h"
#endif


class CInteractiveGraphView : public CInteractiveGraph
{

// Construction / Destruction        
public:	
	CInteractiveGraphView();
	DECLARE_DYNCREATE(CInteractiveGraphView)
	virtual ~CInteractiveGraphView(); 

// Attributes
public:
	CPc1dDoc* GetDocument();
private:
	CPc1dDoc*   pDoc;		// set by OnUpdate
	double x[MAX_ELEMENTS+1];
	double y[4][MAX_ELEMENTS+1]; 
	CGraph CurrentGraph;
	BOOL m_bRetainZoom;	// false=autoscale history graphs, true=retain zoom
	double m_xmin, m_xmax, m_ymin, m_ymax;

	// previous graphs
	int m_NumberOfHistoryGraphs;	// 0=no simulations have been completed
	int m_NextFreeHistoryGraph;	// where should the next data go?
	int m_CurrentlyVisibleHistoryGraph; // -1=latest data, 0=last completed simulation, 1=2nd last,...
	double *m_HistoryX[MAXGRAPHSTOSAVE];
	double *m_HistoryY[4][MAXGRAPHSTOSAVE];
	CGraph m_HistoryGraph[MAXGRAPHSTOSAVE];
	CString m_HistoryName[MAXGRAPHSTOSAVE];
	void SaveCurrentDataAsHistoryGraph(CString name);

// Implementation
private:
	void ReloadGraphData(); 
	void SwitchToDifferentGraph(GraphType newgraphtype);
	void SwitchToGraph(CGraph *newgraph);
	void ShowHistoryGraph(int historytoshow);
public:
   	virtual void OnUpdate(CView* pView, LPARAM lHint, CObject* pHint);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif   
protected:
	virtual void OnZoomOutFromFullSize(void);	
	virtual void OnDoubleClickOnAxis(UINT nFlags, CPoint point);

	// Printing support
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Generated message map functions
protected:
	void RescaleGraphData();
	//{{AFX_MSG(CInteractiveGraphView)
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnGraphRedefine();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnOptionsAutoscaleHistorygraphs();
	afx_msg void OnUpdateOptionsAutoscaleHistorygraphs(CCmdUI* pCmdUI);
	afx_msg void OnGraphHistoryNext();
	afx_msg void OnUpdateGraphHistoryNext(CCmdUI* pCmdUI);
	afx_msg void OnGraphHistoryPrevious();
	afx_msg void OnUpdateGraphHistoryPrevious(CCmdUI* pCmdUI);
	afx_msg void OnGraphHistoryReset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#ifndef _DEBUG  // debug version in actview.cpp
inline CPc1dDoc* CInteractiveGraphView::GetDocument()
   { return (CPc1dDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

