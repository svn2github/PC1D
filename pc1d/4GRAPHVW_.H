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

#ifndef PLOTLOAD_HEADER
#include "plotload.h"
#endif

#ifndef PLOT2D_HEADER     
#include "plot2d.h"
#endif


class CGraphsList : public CObject
{
private:
	CGraph graphlist[4];	
	int whichgraphwasdoubleclickedlast;
public:	
	void SetMostRecentlyUsed(int index);
	void AddToList(CGraph *gr);
	int FindInList(CGraph *gr); // returns -1 if not found
	CGraph &GetGraph(int index) { return graphlist[index]; };
	void ResetGraphs(GraphType gr1, GraphType gr2, GraphType gr3, GraphType gr4);
	void SetGraphs(CGraph *list);	// gets the graphs from list
	void GetGraphs(CGraph *list);	// copies the graphs into list

};


class CFourGraphsView : public CView
{
public: // create from serialization only
	CFourGraphsView();
	DECLARE_DYNCREATE(CFourGraphsView)

// Attributes
public:
	CPc1dDoc* GetDocument();
private:
	CPc1dDoc*   pDoc;		// set by OnUpdate
	double x[MAX_ELEMENTS+1];
	double y[4][MAX_ELEMENTS+1];
	CGraphsList nostategraphslist;
	CGraphsList eqstategraphslist;
	CGraphsList ssstategraphslist;
	int whichgraphishilited; // which graph is highlighted for cursor keys
	BOOL ShouldTitlesBeRedrawn; 
	CGraphsList *prevCurrentGraphList;
	CAxisData prevXAxis[4];
	CAxisData prevYAxis[4];
	
// Implementation
public:
	virtual ~CFourGraphsView();
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual void OnUpdate(CView* pView, LPARAM lHint, CObject* pHint);
	void showhilite(BOOL shouldshow);
	void movehilite(int newposition);
/***
	void SetAllGraphs(GraphType no1, GraphType no2, GraphType no3, GraphType no4,
					  GraphType eq1, GraphType eq2, GraphType eq3, GraphType eq4,
					  GraphType ss1, GraphType ss2, GraphType ss3, GraphType ss4) {
		nostategraphslist.ResetGraphs(no1, no2, no3, no4);
		eqstategraphslist.ResetGraphs(eq1, eq2, eq3, eq4);
		ssstategraphslist.ResetGraphs(ss1, ss2, ss3, ss4);
	};
	void GetAllGraphs(GraphType &no1, GraphType &no2, GraphType &no3, GraphType &no4,
					  GraphType &eq1, GraphType &eq2, GraphType &eq3, GraphType &eq4,
					  GraphType &ss1, GraphType &ss2, GraphType &ss3, GraphType &ss4) {
		no1=nostategraphslist.GetGraph(0);
		no2=nostategraphslist.GetGraph(1);
		no3=nostategraphslist.GetGraph(2);
		no4=nostategraphslist.GetGraph(3);
		eq1=nostategraphslist.GetGraph(0);
		eq2=nostategraphslist.GetGraph(1);
		eq3=nostategraphslist.GetGraph(2);
		eq4=nostategraphslist.GetGraph(3);
		ss1=nostategraphslist.GetGraph(0);
		ss2=nostategraphslist.GetGraph(1);
		ss3=nostategraphslist.GetGraph(2);
		ss4=nostategraphslist.GetGraph(3);
	};
***/

	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	// Drawing support
	void DrawGraph(CDC* pDC, COLORREF color, 
					double left, double right, double bottom, double top,
					CGraph &gr, int graphnum);
	void Draw4Graphs(CDC* pDC,
					double left, double right, double bottom, double top);

	void AddGraphToCurrentList(CGraph *whichgraph);
	CGraphsList *GetCurrentGraphList(void);

	// Printing support
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);

// Generated message map functions
private:
	//{{AFX_MSG(CFourGraphsView)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnGraphDefaultgraphs();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in 4graphvw.cpp
inline CPc1dDoc* CFourGraphsView::GetDocument()
   { return (CPc1dDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
