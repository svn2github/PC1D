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
// 4graphvw.cpp : implementation of the CFourGraphsView class
//

#include "stdafx.h"
#include "hints.h"
#include "mathstat.h"	// CMath
#include "pc1ddoc.h" 
#include "plotload.h"
#include "4graphvw.h"
#include "useful.h"


#ifndef PLOT2D_HEADER     
#include "plot2d.h"
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

const COLORREF BLACK = RGB(0,0,0);
const COLORREF WHITE = RGB(255,255,255);
const COLORREF RED   = RGB(255,0,0);
const COLORREF GREEN = RGB(0,255,0);
const COLORREF BLUE	 = RGB(0,0,127);
const COLORREF YELLOW= RGB(255,255,0);
const COLORREF CYAN  = RGB(0,255,255);
const COLORREF DKGRAY= RGB(31,31,63);
const COLORREF PURPLE= RGB(15,0,127);

/////////////////////////////////////////////////////////////////////////////
// CFourGraphsView

IMPLEMENT_DYNCREATE(CFourGraphsView, CView)

BEGIN_MESSAGE_MAP(CFourGraphsView, CView)
	//{{AFX_MSG_MAP(CFourGraphsView)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_GRAPH_DEFAULTGRAPHS, OnGraphDefaultgraphs)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFourGraphsView construction/destruction

static BOOL bNotInitialized;
CFourGraphsView::CFourGraphsView() 
{
	whichgraphishilited=0;
	ShouldTitlesBeRedrawn=TRUE;
	prevCurrentGraphList=&nostategraphslist;
	nostategraphslist.ResetGraphs(DopingDensities, CarrierMobilities, CarrierDensities, DiffusionLength);
	eqstategraphslist.ResetGraphs(EnergyBands, ElectricField, CarrierDensities, ChargeDensity);
	ssstategraphslist.ResetGraphs(EnergyBands, Photogeneration, CarrierDensities, CurrentDensity);
	bNotInitialized=TRUE;
}

CFourGraphsView::~CFourGraphsView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CFourGraphsView drawing

BOOL bJustGotResized=FALSE;

void CFourGraphsView::OnUpdate(CView* /*pView*/, LPARAM lHint, CObject* pHint)
{            
	CRect invalrect;
	pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (bNotInitialized) {
		nostategraphslist.GetGraphs(pDoc->GetProblem()->GetExcite()->m_nostategraphs);
		eqstategraphslist.GetGraphs(pDoc->GetProblem()->GetExcite()->m_eqstategraphs);
		ssstategraphslist.GetGraphs(pDoc->GetProblem()->GetExcite()->m_ssstategraphs);
		bNotInitialized=FALSE;
	}

	
	switch (lHint){
		case 0:		// generic update - e.g. first call
		case HINT_EVERYTHINGCHANGED:
		case HINT_SWITCHTO4GRAPHS:
			Invalidate(FALSE); 	// Don't erase the background first
			ShouldTitlesBeRedrawn=TRUE;
			break;		
		case HINT_GRAPHDATACHANGED:
		case HINT_ITERATION:
			if (GetUpdateRect(invalrect)) {
				// client area is invalid already (for some other reason).
				// So we can't use shortcuts
				Invalidate(FALSE); 	// Don't erase the background first
				ShouldTitlesBeRedrawn=TRUE;
			} else {
				Invalidate(FALSE); 	// Don't erase the background first
				if (prevCurrentGraphList==GetCurrentGraphList()) {
					ShouldTitlesBeRedrawn=FALSE;
					if (bJustGotResized) { ShouldTitlesBeRedrawn=TRUE; bJustGotResized=FALSE; }
					UpdateWindow();
				} else ShouldTitlesBeRedrawn=TRUE;
			}
			break;
		case HINT_NEWGRAPHS:
			nostategraphslist.SetGraphs(pDoc->GetProblem()->GetExcite()->m_nostategraphs);
			eqstategraphslist.SetGraphs(pDoc->GetProblem()->GetExcite()->m_eqstategraphs);
			ssstategraphslist.SetGraphs(pDoc->GetProblem()->GetExcite()->m_ssstategraphs);
			Invalidate(FALSE);
			break;

		case HINT_DIFFERENTGRAPH:
			// a different graph is being requested for actview.
			// *pHint is the desired graph.
			// We take this as a signal to update our list of recent graphs.
			AddGraphToCurrentList((CGraph *)pHint);
			ShouldTitlesBeRedrawn=TRUE;
			break;
		default:				// otherwise, don't need to redraw the screen
			break;
	}
}


void CFourGraphsView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	bJustGotResized=TRUE;
}


void CFourGraphsView::OnDraw(CDC* pDC)
{
	Draw4Graphs(pDC, 0, 100, 0, 100);
}

void CFourGraphsView::DrawGraph(CDC* pDC, COLORREF color, 
					double left, double right, double bottom, double top,
					CGraph &graph, int graphnum)					
{
	int n, k;
	double xmin, xmax, ymin, ymax, ymin0, ymax0;
	CString title, ylabel, xlabel;           
	BOOL ShouldXAxisBeDrawn, ShouldYAxisBeDrawn;
	CAxisData tempaxis;
	CData xQuantity, yQuantity;

//int tweentime=CUseful::CyclesSinceLastCall();
	
	graph.LoadQuantities(pDoc->GetProblem(), 0, NULL, NULL, NULL, NULL, NULL);	

	graph.GetXQuantity(xQuantity);
	graph.GetYQuantity(0, yQuantity);

 	if (xQuantity.type==SPACE_ARRAY)
		n = pDoc->GetProblem()->GetNode()->GetNodeCount();
	else if (xQuantity.type==TIME_ARRAY)
		n = pDoc->GetProblem()->GetStatus()->time_step+2;	// index 0 is SS, index 1 is t=0
		
	graph.LoadQuantities(pDoc->GetProblem(), n, x, y[0], y[1], y[2], y[3]);

//int loadtime=CUseful::CyclesSinceLastCall();



	// find data bounds

	CMath::ArrayMinMax(n, x, xmin, xmax);
	CMath::ArrayMinMax(n, y[0], ymin, ymax);
	
	for (k=1; k<graph.NumberOfValidCurves(); k++)
	{
		CMath::ArrayMinMax(n, y[k], ymin0, ymax0);
		if (ymin0<ymin) ymin = ymin0; if (ymax0>ymax) ymax = ymax0;
	}

	title=graph.GetTitle();
	xlabel=graph.GetXLabel();
	ylabel=graph.GetYLabel();
	if (title.IsEmpty())  title  = yQuantity.label;
	if (xlabel.IsEmpty()) xlabel = xQuantity.label+" ("+xQuantity.units+")";
	if (ylabel.IsEmpty()) ylabel = yQuantity.units; 
/***
	if (graph.title.IsEmpty()) title = graph.pY[0].label; else title = graph.title;
	if (graph.xLabel.IsEmpty()) xlabel = graph.pX.label+" ("+graph.pX.units+")";
		else xlabel = graph.xLabel;
	if (graph.yLabel.IsEmpty()) 	ylabel = graph.pY[0].units; 
		else ylabel = graph.yLabel;
***/
	CPlot2D plot(pDC, this);
//	CPlot2D plot(pDC, this, GetDocument()->m_GraphFont, GetDocument()->m_GraphFont90);
//int maxtime=CUseful::CyclesSinceLastCall();
	if (pDC->IsPrinting())
		plot.LocateGraph(left, right, bottom, top, WHITE, WHITE, BLACK, BLACK);
	else
		plot.LocateGraph(left, right, bottom, top, BLUE, BLACK, WHITE, color);


	plot.CalculateAxes(xmin, xmax, graph.xScale, 
					   ymin, ymax, graph.yScale, !title.IsEmpty());
					   
//int calctime=CUseful::CyclesSinceLastCall();

					   
	ShouldXAxisBeDrawn=ShouldYAxisBeDrawn=TRUE;
	if (!ShouldTitlesBeRedrawn) {
		// if redrawing titles, ALWAYS redraw axes. This also avoids
		// probs with uninitialized axes.
		// check to see whether the new axes are the same as the old ones
		tempaxis=plot.GetXAxisData();
		if (tempaxis==prevXAxis[graphnum]) ShouldXAxisBeDrawn=FALSE;
		tempaxis=plot.GetYAxisData();
		if (tempaxis==prevYAxis[graphnum]) ShouldYAxisBeDrawn=FALSE;
	}
	prevXAxis[graphnum]=plot.GetXAxisData();
	prevYAxis[graphnum]=plot.GetYAxisData();

	if (ShouldTitlesBeRedrawn) plot.DrawTitleAndBackground(title);
	plot.DrawAxesAndGraphArea(xlabel, ylabel, ShouldXAxisBeDrawn, ShouldYAxisBeDrawn);
//int axestime=CUseful::CyclesSinceLastCall();
				
	const COLORREF linecolr[4]={RED, GREEN, YELLOW, CYAN};
	const COLORREF printerlinecolr[4]={RED, GREEN, BLACK, CYAN};
	const int linestyle[4]={PS_SOLID, PS_DASH, PS_DOT, PS_DASHDOT};
	
	int *XIntArray=new int[n+1];
	int *YIntArray=new int[n+1];
 	plot.MakeXIntArray(0, n-1, x, XIntArray);

//int xtime=CUseful::CyclesSinceLastCall();
//int plottime=0;
	
	for (k=0; k<graph.NumberOfValidCurves(); k++)
	{
	 	plot.MakeYIntArray(0, n-1, y[k], YIntArray);     
//		xtime+=CUseful::CyclesSinceLastCall();
		if (pDC->IsPrinting()) plot.IntegerPlotLine(n-1, XIntArray, YIntArray, printerlinecolr[k], PS_SOLID, 3);
		else plot.IntegerPlotLine(n-1, XIntArray, YIntArray, linecolr[k], PS_SOLID, 1);
//		if (pDC->IsPrinting())	plot.IntegerPlotLine(n-1, XIntArray, YIntArray, BLACK, PS_SOLID, 3);
//		else  					plot.IntegerPlotLine(n-1, XIntArray, YIntArray, linecolr[k], linestyle[k], 1);
//		plottime+=CUseful::CyclesSinceLastCall();
	}
/***
CString timingstr;
timingstr.Format("Xtime=%d, Load=%d, Calc=%d, Axes=%d, Plot=%d, Max=%d", xtime, loadtime, calctime, axestime, plottime, maxtime);
WriteStatusBarMessage(timingstr);
CUseful::CyclesSinceLastCall();
***/
	delete XIntArray;	
	delete YIntArray;
}

void CGraphsList::ResetGraphs(GraphType gr1, GraphType gr2, GraphType gr3, GraphType gr4)
{ 
	graphlist[0].SetPredefinedGraph(gr1);
	graphlist[1].SetPredefinedGraph(gr2);
	graphlist[2].SetPredefinedGraph(gr3);
	graphlist[3].SetPredefinedGraph(gr4);
	whichgraphwasdoubleclickedlast=2;
}

void CGraphsList::SetGraphs(CGraph *list)
{
	int i;
	for (i=0; i<4; i++) {
		graphlist[i]=list[i];
	}
}

void CGraphsList::GetGraphs(CGraph *list)
{
	int i;
	for (i=0; i<4; i++) {
		list[i]=graphlist[i];
	}
}

	

void CGraphsList::SetMostRecentlyUsed(int index)
{
	
	whichgraphwasdoubleclickedlast=index;
}

void CGraphsList::AddToList(CGraph *gr)
{
	int oldpos;
	oldpos=FindInList(gr);
	if (oldpos!=-1) { 
		graphlist[oldpos]=graphlist[whichgraphwasdoubleclickedlast];
	}
	graphlist[whichgraphwasdoubleclickedlast]=*gr;
}

int CGraphsList::FindInList(CGraph *gr)
{
	int n;
	for (n=0; n<4; n++) {
		if (graphlist[n]==*gr) return n;
	}
	return -1;
}


CGraphsList *CFourGraphsView::GetCurrentGraphList(void)
{
	switch (pDoc->GetProblem()->GetStatus()->state) {
	default:
	case NO_STATE:
		return &nostategraphslist;
	case EQ_STATE:
		return &eqstategraphslist;
	case SS_STATE:
	case TR_STATE:
		return &ssstategraphslist;
	}
}	
	
void CFourGraphsView::AddGraphToCurrentList(CGraph *gr)
{
	CGraphsList *grlist;
	grlist=GetCurrentGraphList();
	grlist->AddToList(gr);

	nostategraphslist.GetGraphs(pDoc->GetProblem()->GetExcite()->m_nostategraphs);
	eqstategraphslist.GetGraphs(pDoc->GetProblem()->GetExcite()->m_eqstategraphs);
	ssstategraphslist.GetGraphs(pDoc->GetProblem()->GetExcite()->m_ssstategraphs);
}	
	

void CFourGraphsView::Draw4Graphs(CDC* pDC,
					double left, double right, double bottom, double top)
{
	CGraphsList *grlist=GetCurrentGraphList();
	
DrawGraph(pDC, YELLOW, left,  (left+right)/2, (bottom+top)/2,    top, grlist->GetGraph(0), 0);
DrawGraph(pDC, RED,    (left+right)/2, right, (bottom+top)/2,    top, grlist->GetGraph(1), 1);
DrawGraph(pDC, GREEN,  left,  (left+right)/2, bottom, (bottom+top)/2, grlist->GetGraph(2), 2);
DrawGraph(pDC, CYAN,   (left+right)/2, right, bottom, (bottom+top)/2, grlist->GetGraph(3), 3);
	
		if (!pDC->IsPrinting()) showhilite(TRUE);
		ShouldTitlesBeRedrawn=TRUE;	
		prevCurrentGraphList=grlist;
		return;
}

// if shouldshow=FALSE, undraws the hilite(i.e. draws in white)
// if TRUE, draws in red
void CFourGraphsView::showhilite(BOOL shouldshow)
{
	int position=whichgraphishilited;
	COLORREF colr;
	if (shouldshow) colr=RED;
	else colr=WHITE;
	CRect rClient;
	GetClientRect(rClient);
	
	
	CClientDC pDC(this); 
	pDC.SelectStockObject(HOLLOW_BRUSH);
	CPen pen(PS_SOLID, 1, colr);
	pDC.SelectObject(&pen);
	
	CRect hiliterect;
	if (position&1) {
		hiliterect.left=rClient.right/2;
		hiliterect.right=rClient.right;
	} else {
		hiliterect.left=0;
		hiliterect.right=rClient.right/2;
	}
	if (position&2) {
		hiliterect.top=rClient.bottom/2;
		hiliterect.bottom=rClient.bottom;
	} else {
		hiliterect.top=0;
		hiliterect.bottom=rClient.bottom/2;
	}
    pDC.Rectangle(hiliterect);
    pDC.SelectStockObject(BLACK_PEN);
}

void CFourGraphsView::movehilite(int newposition)
{
	showhilite(FALSE);
	whichgraphishilited=newposition;
	showhilite(TRUE);
}


/////////////////////////////////////////////////////////////////////////////
// CFourGraphsView printing

BOOL CFourGraphsView::OnPreparePrinting(CPrintInfo* pInfo)
{
	pInfo->SetMaxPage(1);
	return DoPreparePrinting(pInfo);
}

void CFourGraphsView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CFourGraphsView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CFourGraphsView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	CRect rect;
	rect = pInfo->m_rectDraw;
	int pagewidth = rect.right-rect.left;
	int pageheight = rect.bottom-rect.top;
	pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	Draw4Graphs(pDC, 5, 95, 5, 95);
}

/////////////////////////////////////////////////////////////////////////////
// CFourGraphsView diagnostics

#ifdef _DEBUG
void CFourGraphsView::AssertValid() const
{
	CView::AssertValid();
}

void CFourGraphsView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPc1dDoc* CFourGraphsView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPc1dDoc)));
	return (CPc1dDoc*)m_pDocument;
}
#endif //_DEBUG    


/////////////////////////////////////////////////////////////////////////////
// CFourGraphsView message handlers


void CFourGraphsView::OnLButtonDown(UINT nFlags, CPoint point)
{
	int position;
	CRect rClient;
	GetClientRect(rClient);
	position=0;
	if (point.x*2 > rClient.right) position++;
	if (point.y*2 > rClient.bottom) position+=2;
	movehilite(position);
	CView::OnLButtonDown(nFlags, point);
}

                                               
void CFourGraphsView::OnLButtonDblClk(UINT /*nFlags*/, CPoint point)
{
	int position;
	CRect rClient;
	
	GetClientRect(rClient);
	position=0;
	if (point.x*2 > rClient.right) position++;
	if (point.y*2 > rClient.bottom) position+=2;
	
	CGraphsList *grlist=GetCurrentGraphList();
	grlist->SetMostRecentlyUsed(position);
	pDoc->SwitchToInteractiveGraph(this, &grlist->GetGraph(position));
}

void CFourGraphsView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	 {
	 case VK_LEFT: 
	 		if (whichgraphishilited&1) 	movehilite(whichgraphishilited^1);
			break;
	 case VK_RIGHT:
	 		if (!(whichgraphishilited&1)) movehilite(whichgraphishilited^1);
			break;
	 case VK_UP:
	 		if (whichgraphishilited&2) 	movehilite(whichgraphishilited^2);
			break;
     case VK_DOWN:
	 		if (!(whichgraphishilited&2)) movehilite(whichgraphishilited^2);
	 		break;
	 default:
	 	CView::OnKeyDown(nChar, nRepCnt, nFlags);
	 	break;
	}
}

void CFourGraphsView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CGraphsList *grlist=GetCurrentGraphList();

	switch (nChar)
	{
	   case VK_RETURN:
	    grlist->SetMostRecentlyUsed(whichgraphishilited);
		pDoc->SwitchToInteractiveGraph(this, &grlist->GetGraph(whichgraphishilited));
		break;
	  default:
		CView::OnChar(nChar, nRepCnt, nFlags);
	}
}

void CFourGraphsView::OnGraphDefaultgraphs()
{
	nostategraphslist.ResetGraphs(DopingDensities, CarrierMobilities, CarrierDensities, DiffusionLength);
	eqstategraphslist.ResetGraphs(EnergyBands, ElectricField, CarrierDensities, ChargeDensity);
	ssstategraphslist.ResetGraphs(EnergyBands, Photogeneration, CarrierDensities, CurrentDensity);
	pDoc = GetDocument();
	nostategraphslist.GetGraphs(pDoc->GetProblem()->GetExcite()->m_nostategraphs);
	eqstategraphslist.GetGraphs(pDoc->GetProblem()->GetExcite()->m_eqstategraphs);
	ssstategraphslist.GetGraphs(pDoc->GetProblem()->GetExcite()->m_ssstategraphs);
	pDoc->UpdateAllViews(NULL, HINT_SWITCHTO4GRAPHS);
}
