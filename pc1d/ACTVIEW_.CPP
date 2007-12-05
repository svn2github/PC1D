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
// actview.cpp : implementation of the CInteractiveGraphView class 
//
// This is the view which shows a PC-1D interactive graph.
// Since graph switching has been moved to Pc1dDoc and graph definitions have
// been moved to plotload, this is a trivial class.

#include "stdafx.h"    
#include "hints.h"
#include "mathstat.h"	// CMath
#include "pc1ddoc.h"

#include "plotload.h"
#include "actgraph.h"
#include "actview.h"         

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CInteractiveGraphView


IMPLEMENT_DYNCREATE(CInteractiveGraphView, CInteractiveGraph)

BEGIN_MESSAGE_MAP(CInteractiveGraphView, CInteractiveGraph)
	//{{AFX_MSG_MAP(CInteractiveGraphView)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_GRAPH_REDEFINE, OnGraphRedefine)
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_OPTIONS_AUTOSCALEHISTORYGRAPHS, OnOptionsAutoscaleHistorygraphs)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_AUTOSCALEHISTORYGRAPHS, OnUpdateOptionsAutoscaleHistorygraphs)
	ON_COMMAND(ID_GRAPH_HISTORYNEXT, OnGraphHistoryNext)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_HISTORYNEXT, OnUpdateGraphHistoryNext)
	ON_COMMAND(ID_GRAPH_HISTORYPREVIOUS, OnGraphHistoryPrevious)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_HISTORYPREVIOUS, OnUpdateGraphHistoryPrevious)
	ON_COMMAND(ID_GRAPH_HISTORYRESET, OnGraphHistoryReset)
	//}}AFX_MSG_MAP
		// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInteractiveGraphView construction/destruction

// This isn't such a great place to do initialization, because the view isn't visible yet.
// Better to initialize in OnUpdate where lHint==0.
static bool bNotInitialized;
CInteractiveGraphView::CInteractiveGraphView()
{    
    CInteractiveGraph::CInteractiveGraph(); // call default to initialize 
    CurrentGraph.SetPredefinedGraph(CarrierDensities);
    SetShouldDrawDots(TRUE);
	bNotInitialized=true;
	m_bRetainZoom= AfxGetApp()->GetProfileInt("Interactive Graph", "Retain Zoom", 0);
	int i;
	for (i=0; i<MAXGRAPHSTOSAVE; i++) {
		m_HistoryX[i]=new double[MAX_ELEMENTS+1];
		m_HistoryY[0][i]=new double[MAX_ELEMENTS+1];
		m_HistoryY[1][i]=new double[MAX_ELEMENTS+1];
		m_HistoryY[2][i]=new double[MAX_ELEMENTS+1];
		m_HistoryY[3][i]=new double[MAX_ELEMENTS+1];
	}
	m_NumberOfHistoryGraphs=0;
	m_NextFreeHistoryGraph=0;
	m_CurrentlyVisibleHistoryGraph=-1;
}

CInteractiveGraphView::~CInteractiveGraphView()
{
	AfxGetApp()->WriteProfileInt("Interactive Graph", "Retain Zoom", m_bRetainZoom);
	int i;
	for (i=0; i<MAXGRAPHSTOSAVE; i++) {
		delete [] m_HistoryX[i];
		delete [] m_HistoryY[0][i];
		delete [] m_HistoryY[1][i];
		delete [] m_HistoryY[2][i];
		delete [] m_HistoryY[3][i];
	}
}
 
/////////////////////////////////////////////////////////////////////////////
// CInteractiveGraphView drawing
                                                             
void CInteractiveGraphView::OnUpdate(CView* , LPARAM lHint, CObject* pHint)
{
#pragma
	pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (bNotInitialized) {
		pDoc->GetProblem()->GetExcite()->m_InteractiveGraph=CurrentGraph;
		bNotInitialized=false;
	}
	CString name;

	switch (lHint){
		case 0:		// generic update - e.g. first call
		case HINT_EVERYTHINGCHANGED:
			ReloadGraphData();
			RescaleGraphData();
			ShowHistoryGraph(m_CurrentlyVisibleHistoryGraph);
			if (IsWindowVisible()) {
				SetTitleRedrawNeeded(TRUE);
				Invalidate(FALSE); 	// Don't erase the background first
			}
			break;
			
		case HINT_GRAPHDATACHANGED:
		case HINT_ITERATION:
			ReloadGraphData();
			RescaleGraphData();
			if (m_CurrentlyVisibleHistoryGraph>=0) ShowHistoryGraph(m_CurrentlyVisibleHistoryGraph);
			if (m_CurrentlyVisibleHistoryGraph<0 && IsWindowVisible()) {
				SetTitleRedrawNeeded(FALSE); // can optimise the drawing
				Invalidate(FALSE); 	// Don't erase the background first  
				UpdateWindow();		// Make sure other WM_PAINTs dont happen caused by other things,
									// which WOULD require a full repaint
			}
			break;
		case HINT_SIMFINISHED:
			if (pDoc->GetProblem()->GetBatch()->m_Enable)	name.Format("Batch #%d", pDoc->GetProblem()->GetBatch()->GetPreviousSim()+1);
			else name.Empty();
			SaveCurrentDataAsHistoryGraph(name);
			break;
		case HINT_NEWGRAPHS:
			SwitchToGraph(&(pDoc->GetProblem()->GetExcite()->m_InteractiveGraph));
			break;

		case HINT_DIFFERENTGRAPH:
			// a different graph is being requested. *pHint is the desired graph.
			SwitchToGraph((CGraph*)pHint);
			break;				
		default:				// otherwise, don't need to redraw the screen
			break;
	}
}
                                                             
/////////////////////////////////////////////////////////////////////////////
// CInteractiveGraphView printing

BOOL CInteractiveGraphView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CInteractiveGraphView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CInteractiveGraphView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CInteractiveGraphView diagnostics

#ifdef _DEBUG
void CInteractiveGraphView::AssertValid() const
{
	CView::AssertValid();
}

void CInteractiveGraphView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPc1dDoc* CInteractiveGraphView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPc1dDoc)));
	return (CPc1dDoc*)m_pDocument;
}
#endif //_DEBUG

//------------------------------------------------------------
// Trivial functions required to implement interactive graphs

void CInteractiveGraphView::OnEditCopy()
{	CopyGraphSelectionToClipboard();   }

void CInteractiveGraphView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{	pCmdUI->SetText("&Copy Graph Data\tCtrl+C");	}

//------------------------------------------------------------
// Overridden functions

// Used to switch to 4graphs view.    
void CInteractiveGraphView::OnZoomOutFromFullSize(void)
{                      
	// we use a trick so that all views are aware that we've used this graph.
	// that way, 4graphs view adds it to the current list.
	pDoc->UpdateAllViews(this, HINT_DIFFERENTGRAPH, &CurrentGraph);
	pDoc->SwitchToFourGraphsView();	
}

void CInteractiveGraphView::OnDoubleClickOnAxis(UINT , CPoint )
{
		AfxGetApp()->m_pMainWnd->SendMessage(WM_COMMAND, ID_GRAPH_REDEFINE,0);
}


void CInteractiveGraphView::SwitchToDifferentGraph(GraphType newgraphtype)
{
	CurrentGraph.SetPredefinedGraph(newgraphtype);
	ReloadGraphData();
	RescaleGraphData();

	m_CurrentlyVisibleHistoryGraph=-1;
	ShowHistoryGraph(m_CurrentlyVisibleHistoryGraph);

    Invalidate(FALSE);
	pDoc->GetProblem()->GetExcite()->m_InteractiveGraph=CurrentGraph;
}

void CInteractiveGraphView::SwitchToGraph(CGraph *newgraph)
{
	pDoc->GetProblem()->GetExcite()->m_InteractiveGraph=*newgraph;

	CurrentGraph=*newgraph;
	CurrentGraph.LoadQuantities(pDoc->GetProblem(), 0, x, y[0], y[1], y[2], y[3]);
	ReloadGraphData();
	RescaleGraphData();

	m_CurrentlyVisibleHistoryGraph=-1;
	ShowHistoryGraph(m_CurrentlyVisibleHistoryGraph);

	SetTitleRedrawNeeded(TRUE);
    Invalidate(FALSE);
}

// loads new values 
void CInteractiveGraphView::ReloadGraphData()
{
	int n;

	pDoc=GetDocument();
	ASSERT_VALID(pDoc);
    

	// load the array type...
	CurrentGraph.LoadQuantities(pDoc->GetProblem(), 0, NULL, NULL, NULL, NULL, NULL);	

	// find the new number of points
	CData xQuantity;
	CurrentGraph.GetXQuantity(xQuantity);
	
 	if (xQuantity.type==SPACE_ARRAY)
		n = pDoc->GetProblem()->GetNode()->GetNodeCount();
	else if (xQuantity.type==TIME_ARRAY)
		n = pDoc->GetProblem()->GetStatus()->time_step+2;	// index 0 is SS, index 1 is t=0
	CurrentGraph.SetNumPoints(n);

    // load the new data      
	CurrentGraph.LoadQuantities(pDoc->GetProblem(), n, x, y[0], y[1], y[2], y[3]);


	// get labels
	CData yQuantity;
	CurrentGraph.GetYQuantity(0, yQuantity);

	CString title, ylabel, xlabel;

	title=CurrentGraph.GetTitle();
	xlabel=CurrentGraph.GetXLabel();
	ylabel=CurrentGraph.GetYLabel();
	if (title.IsEmpty())  title  = yQuantity.label;
	if (xlabel.IsEmpty()) xlabel = xQuantity.label+" ("+xQuantity.units+")";
	if (ylabel.IsEmpty()) ylabel = yQuantity.units; 

	// set the graph data
	int i;	                         
	NewGraph(title);
	SetXArray(n, x, xlabel, xQuantity.label, xQuantity.units, xQuantity.abbreviation);
	for (i=0; i<CurrentGraph.NumberOfValidCurves(); i++)
	{
		CurrentGraph.GetYQuantity(i, yQuantity);
		SetNextYArray(n, y[i], ylabel, yQuantity.label, yQuantity.units, yQuantity.abbreviation);
	}
//	if (!m_bAutoscale) 
//		SetDataLimits(m_xmin, m_xmax, CurrentGraph.xScale, m_ymin, m_ymax, CurrentGraph.yScale);
}

// autoscales the graph data
void CInteractiveGraphView::RescaleGraphData()
{
	double ymin0, ymax0;

	int n=CurrentGraph.GetNumPoints();

	// find data bounds
	CMath::ArrayMinMax(n, x, m_xmin, m_xmax);
	CMath::ArrayMinMax(n, y[0], m_ymin, m_ymax);

	int k;
	for (k=1; k<CurrentGraph.NumberOfValidCurves(); k++)
	{
		CMath::ArrayMinMax(n, y[k], ymin0, ymax0);
		if (ymin0<m_ymin) m_ymin = ymin0; if (ymax0>m_ymax) m_ymax = ymax0;
	}
    SetDataLimits(m_xmin, m_xmax, CurrentGraph.xScale, m_ymin, m_ymax, CurrentGraph.yScale);
}

void CInteractiveGraphView::OnGraphRedefine() 
{
	if ( CurrentGraph.AskUserToRedefineGraph()) {
		pDoc->GetProblem()->GetExcite()->m_InteractiveGraph=CurrentGraph;
		ReloadGraphData();
		RescaleGraphData();
		SetTitleRedrawNeeded(TRUE);
		Invalidate(FALSE);
	}
}

// Increasing the graph pointers - use cyclic buffer
// eg. Suppose MAX is 3.
	//				num=0 free=0
	//    A			num=1 free=1
	//    A B		num=2 free=2
	//    A B C		num=3 free=0
	//    D B C		num=3 free=1
void CInteractiveGraphView::SaveCurrentDataAsHistoryGraph(CString name)
{
	int i;
	int k;
	m_HistoryGraph[m_NextFreeHistoryGraph]=CurrentGraph;
	m_HistoryName[m_NextFreeHistoryGraph]=name;
	int n=CurrentGraph.GetNumPoints();
	for (i=0; i<n; i++){
		m_HistoryX[m_NextFreeHistoryGraph][i]=x[i];
		for (k=0; k<4;k++) {
			m_HistoryY[k][m_NextFreeHistoryGraph][i]=y[k][i];
		}
	}
	// get ready for next graph
	m_NextFreeHistoryGraph++;
	if (m_NextFreeHistoryGraph>=MAXGRAPHSTOSAVE) m_NextFreeHistoryGraph=0;
	if (m_NumberOfHistoryGraphs<MAXGRAPHSTOSAVE) m_NumberOfHistoryGraphs++;
}

void CInteractiveGraphView::ShowHistoryGraph(int historytoshow)
{
	double zoomxmin, zoomxmax, zoomymin, zoomymax;
	GetCurrentGraphLimits(zoomxmin, zoomxmax, zoomymin, zoomymax);
	CScaleType xscale, yscale;
	xscale=GetXScaleType(); yscale=GetYScaleType();

	if (historytoshow<0) {
		ReloadGraphData();
		RescaleGraphData();
		if (m_bRetainZoom && CurrentGraph.xScale==xscale && CurrentGraph.yScale==yscale)
			SetZoom(zoomxmin, zoomxmax, zoomymin, zoomymax);
		return;
	}

	int n, k;
	double xmin, xmax, ymin, ymax, ymin0, ymax0;
	CString title, ylabel, xlabel;           

	pDoc=GetDocument();
	ASSERT_VALID(pDoc);

	int graphindex= m_NextFreeHistoryGraph-1-historytoshow;
	if (graphindex<0) graphindex+=MAXGRAPHSTOSAVE;

	// load the array type...
	m_HistoryGraph[graphindex].LoadQuantities(pDoc->GetProblem(), 0, NULL, NULL, NULL, NULL, NULL);	
	CData xQuantity, yQuantity;
	m_HistoryGraph[graphindex].GetXQuantity(xQuantity);
	
	n=m_HistoryGraph[graphindex].GetNumPoints();

	// find data bounds
	CMath::ArrayMinMax(n, m_HistoryX[graphindex], xmin, xmax);
	CMath::ArrayMinMax(n, m_HistoryY[0][graphindex], ymin, ymax);
	
	for (k=1; k<m_HistoryGraph[graphindex].NumberOfValidCurves(); k++)
	{
		CMath::ArrayMinMax(n, m_HistoryY[k][graphindex], ymin0, ymax0);
		if (ymin0<ymin) ymin = ymin0; if (ymax0>ymax) ymax = ymax0;
	}

	// get labels
	m_HistoryGraph[graphindex].GetYQuantity(0, yQuantity);

	title=m_HistoryGraph[graphindex].GetTitle();
	xlabel=m_HistoryGraph[graphindex].GetXLabel();
	ylabel=m_HistoryGraph[graphindex].GetYLabel();
	if (title.IsEmpty())  title  = yQuantity.label;
	if (xlabel.IsEmpty()) xlabel = xQuantity.label+" ("+xQuantity.units+")";
	if (ylabel.IsEmpty()) ylabel = yQuantity.units; 

	CString fulltitle;
	fulltitle.Format("Previous simulation #%d - %s", historytoshow+1, title);
	CString str;
	str=m_HistoryName[graphindex];
	if (!str.IsEmpty()) fulltitle += " - " + str;
	NewGraph(fulltitle);
	SetXArray(n, m_HistoryX[graphindex], xlabel, xQuantity.label, xQuantity.units, xQuantity.abbreviation);
	int i;	                         
	for (i=0; i<m_HistoryGraph[graphindex].NumberOfValidCurves(); i++)
	{
		CurrentGraph.GetYQuantity(i, yQuantity);
		SetNextYArray(n, m_HistoryY[i][graphindex], ylabel, yQuantity.label, yQuantity.units, yQuantity.abbreviation);
	}

	SetDataLimits(xmin, xmax, m_HistoryGraph[graphindex].xScale, ymin, ymax, m_HistoryGraph[graphindex].yScale);

	if (m_bRetainZoom && m_HistoryGraph[graphindex].xScale==xscale && m_HistoryGraph[graphindex].yScale==yscale)
		SetZoom(zoomxmin, zoomxmax, zoomymin, zoomymax);


//	if (m_bAutoscale) SetDataLimits(xmin, xmax, CurrentGraph.xScale, ymin, ymax, CurrentGraph.yScale);
//	else SetDataLimits(m_xmin, m_xmax, CurrentGraph.xScale, m_ymin, m_ymax, CurrentGraph.yScale);
}

void CInteractiveGraphView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	 switch (nChar)
	 {
	 case VK_PRIOR:	// page up
		 OnGraphHistoryPrevious();
/***
		 if (m_CurrentlyVisibleHistoryGraph<m_NumberOfHistoryGraphs-1) m_CurrentlyVisibleHistoryGraph++; else return;
		 ShowHistoryGraph(m_CurrentlyVisibleHistoryGraph);
			if (IsWindowVisible()) {
				SetTitleRedrawNeeded(TRUE);
				Invalidate(FALSE); 	// Don't erase the background first
			}
***/
		 break;
	 case VK_NEXT:	// page down
		 OnGraphHistoryNext();
		 break;
/***
		 if (m_CurrentlyVisibleHistoryGraph>=0) m_CurrentlyVisibleHistoryGraph--; else return;
		 ShowHistoryGraph(m_CurrentlyVisibleHistoryGraph);
		 if (IsWindowVisible()) {
			SetTitleRedrawNeeded(TRUE);
			Invalidate(FALSE); 	// Don't erase the background first
		 }
	 	 break;
***/
	 default:
		CInteractiveGraph::OnKeyDown(nChar, nRepCnt, nFlags);
		break;
	 }
}

void CInteractiveGraphView::OnOptionsAutoscaleHistorygraphs() 
{	m_bRetainZoom=!m_bRetainZoom; }

void CInteractiveGraphView::OnUpdateOptionsAutoscaleHistorygraphs(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck(m_bRetainZoom); }

void CInteractiveGraphView::OnGraphHistoryNext() 
{
		 if (m_CurrentlyVisibleHistoryGraph>=0) m_CurrentlyVisibleHistoryGraph--; else return;
		 ShowHistoryGraph(m_CurrentlyVisibleHistoryGraph);
		 if (IsWindowVisible()) {
			SetTitleRedrawNeeded(TRUE);
			Invalidate(FALSE); 	// Don't erase the background first
		 }
}

void CInteractiveGraphView::OnUpdateGraphHistoryNext(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_CurrentlyVisibleHistoryGraph>=0);
}

void CInteractiveGraphView::OnGraphHistoryPrevious() 
{
	if (m_CurrentlyVisibleHistoryGraph<m_NumberOfHistoryGraphs-1) m_CurrentlyVisibleHistoryGraph++; else return;
	ShowHistoryGraph(m_CurrentlyVisibleHistoryGraph);
	if (IsWindowVisible()) {
		SetTitleRedrawNeeded(TRUE);
		Invalidate(FALSE); 	// Don't erase the background first
	}
}

void CInteractiveGraphView::OnUpdateGraphHistoryPrevious(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_CurrentlyVisibleHistoryGraph<m_NumberOfHistoryGraphs-1);
}

void CInteractiveGraphView::OnGraphHistoryReset() 
{
	m_NumberOfHistoryGraphs=0;
	m_NextFreeHistoryGraph=0;
	m_CurrentlyVisibleHistoryGraph=-1;
	ShowHistoryGraph(m_CurrentlyVisibleHistoryGraph);
	if (IsWindowVisible()) {
		SetTitleRedrawNeeded(TRUE);
		Invalidate(FALSE); 	// Don't erase the background first
	}
}
