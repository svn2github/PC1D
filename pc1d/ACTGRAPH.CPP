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
// actgraph.cpp :  Implementation of CInteractiveGraph
// ---------------------------------------------------------------
// Revision history
// 21-9-95  Created by Don Clugston (DAC) for PC1D-Windows 1.0 UNSW
// 30-11-95 DAC:Fixed interface & minor bugs to enable it to be used with PC1D.
//			Large changes, esp to marquee.
//  9-12-95 DAC: Changes to drawing code so it's more efficient
//			- faster, less flickery
// ---------------------------------------------------------------
// CInteractiveGraph:a graph which a user can interact with using a mouse.
// It's implemented as a CView, but is not associated with a particular document.
// Therefore, usage is similar to CScrollView etc:
// A view for a PC1D document will be derived from CInteractiveGraph
// rather than from CView. All the view needs to do is specify the data
// to be graphed. CInteractiveGraph does the rest.
//
//   CPlot2D - draws the graph
// 	    :
//	 CInteractiveGraph - does all doc-independent view functions
//		|
//	 CInteractiveGraphView	- gets data from document
//
// Uses CPlot2D to draw the graphs. Unlike CPlot2D, it is intended to be a
// true object, rather than just a set of functions.
// Functionality provided:
//  -- select rectangular region on screen using mouse; can zoom into this region
//      copy the data to clipboard, or save to disk.
// Press ENTER to zoom in. 
// -- Use cursor keys to move caret. Caret location displayed in status bar.
// Definitions:
// Marquee = the rectangular area which user has selected with the mouse (or keys)
//
// Implementation notes: 
// * This doesn't have a GetDocument() function, since it's not actually
// associated with a document.
// * Many problems are caused by CPlot2D requiring a device context. Most
// importantly, the graph axes etc only exist during OnDraw().
// See esp. OnSetFocus(), OnDraw(), OnMouseMove().
// * If you only redraw part of the client area, and half of the caret is in the
// area you update, but half is outside, Windows won't hide the caret properly.
// (It thinks you drew everywhere, (including the part you didn't draw), so it
// doesn't need to undraw the caret). Simple way to stop this is to hide the caret
// before you invalidate(). Once the area's been invalidated, there's not much 
// you can do.
//=============================================================================

#include "stdafx.h"    
#include <math.h> 
#include <stdio.h> // for sprintf

#include "plot2d.h"
#include "actgraph.h"   
#include "resource.h"
#include "mathstat.h"

#define CARETHOTSPOTY 8
#define CARETHOTSPOTX 8
       

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CInteractiveGraph

IMPLEMENT_DYNCREATE(CInteractiveGraph, CView)

BEGIN_MESSAGE_MAP(CInteractiveGraph, CView)
	//{{AFX_MSG_MAP(CInteractiveGraph)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_CHAR()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInteractiveGraph construction/destruction

CInteractiveGraph::CInteractiveGraph()
{
	m_hMagnifyCursor=AfxGetApp()->LoadCursor(IDC_MAGNIFYCURSOR);
	m_CaretBitmap.LoadBitmap(IDB_CROSSHAIRCARET);
	NewGraph("Untitled");
	if (!ShouldTitlesBeRedrawn) ShowCaret(); // was hidden by SetShouldTitlesBeRedrawn()
	ShouldTitlesBeRedrawn=TRUE;
    SetShouldDrawDots(TRUE);
}

CInteractiveGraph::~CInteractiveGraph()
{         
}

/////////////////////////////////////////////////////////////////////////////
// CInteractiveGraph drawing

// used for optimizing the redraws. Set to FALSE to allow speedup
void CInteractiveGraph::SetTitleRedrawNeeded(BOOL bNeeded)
{ 
	ShouldTitlesBeRedrawn=bNeeded; 
	if(!ShouldTitlesBeRedrawn) HideCaret();
}


void CInteractiveGraph::OnDraw(CDC* pDC)
{
   	DrawPlot(pDC);
   	m_IsMarqueeValid=FALSE; // it's not visible anymore bcos has been drawn over    
    CancelMarquee();
    if (!ShouldTitlesBeRedrawn) ShowCaret(); // was hidden in SetTitleRedrawNeeded().
    ShouldTitlesBeRedrawn=TRUE; // this flag only ever valid for one redraw
}

const COLORREF BLACK=  RGB(0,0,0);
const COLORREF WHITE=  RGB(255,255,255);
const COLORREF RED=    RGB(255,0,0);
const COLORREF GREEN=  RGB(0,255,0);
const COLORREF BLUE	 = RGB(0,0,127);
const COLORREF YELLOW= RGB(255,255,0);
const COLORREF CYAN=   RGB(0,255,255);
const COLORREF GRAY=   RGB(127,127,127);
const COLORREF DKGRAY= RGB(31,31,63);
const COLORREF PURPLE= RGB(15,0,127);
const COLORREF MAGENTA=RGB(255,0,255);


void CInteractiveGraph::DrawPlot(CDC *pDC)
{   
	int firstindex;
	int lastindex;	
//	ASSERT(IsWindowVisible());     // If this fails, it means the paint logic isn't very clever & is repainting invisible windows
	
	BOOL ShouldXAxisBeDrawn, ShouldYAxisBeDrawn;
	CAxisData tempaxis;

	plot= new CPlot2D(pDC,this);
                                      
    m_l=0;    
    m_r=100;
    m_b=0;
    m_t=100;                                  
	if (pDC->IsPrinting()) plot->LocateGraph(m_l,m_r,m_b,m_t, WHITE, WHITE, BLACK, BLACK);
	else plot->LocateGraph(m_l,m_r,m_b,m_t, BLUE, BLACK, WHITE, WHITE);

	plot->CalculateAxes( m_dXMin, m_dXMax, m_xscale,	m_dYMin, m_dYMax, m_yscale, !sTitle.IsEmpty());

	
	ShouldXAxisBeDrawn=ShouldYAxisBeDrawn=TRUE;
	if (!ShouldTitlesBeRedrawn) {
		// if redrawing titles, ALWAYS redraw axes. This also avoids
		// probs with uninitialized axes.
		// check to see whether the new axes are the same as the old ones
		tempaxis=plot->GetXAxisData();
		if (tempaxis==xaxis) ShouldXAxisBeDrawn=FALSE;
		tempaxis=plot->GetYAxisData();
		if (tempaxis==yaxis) ShouldYAxisBeDrawn=FALSE;
	}
	if (ShouldTitlesBeRedrawn) plot->DrawTitleAndBackground(sTitle);
	plot->DrawAxesAndGraphArea(sXAxisLabel, sYAxisLabel[0], ShouldXAxisBeDrawn,ShouldYAxisBeDrawn);

	
    xaxis=plot->GetXAxisData();
    yaxis=plot->GetYAxisData();

	m_leftmostvisiblepoint=0;
	m_rightmostvisiblepoint=numberofpoints-1;
		
	while (m_leftmostvisiblepoint<numberofpoints && !xaxis.IsPointVisible(xarray[m_leftmostvisiblepoint]) )
	{
		 m_leftmostvisiblepoint++;
	}
	while (m_rightmostvisiblepoint>0 && !xaxis.IsPointVisible(xarray[m_rightmostvisiblepoint]) )
	{
		 m_rightmostvisiblepoint--;
	}
	
	// need to move out by 1 index so that the edge sections of graph get drawn	                                            
	firstindex=m_leftmostvisiblepoint;
	lastindex=m_rightmostvisiblepoint;
	if (firstindex>0) firstindex--;
	if (lastindex<numberofpoints-1) lastindex++;
	
	m_leftmostvisiblepoint=firstindex;
	m_rightmostvisiblepoint=lastindex;
	
	const COLORREF linecolr[]={RED, GREEN, YELLOW, CYAN};
	const COLORREF printerlinecolr[4]={RED, GREEN, BLACK, CYAN};
	const int linestyle[]={PS_SOLID, PS_DASH, PS_DOT, PS_DASHDOT};
	
	int *XIntArray=new int[lastindex-firstindex+2];
	int *YIntArray=new int[lastindex-firstindex+2];
 	plot->MakeXIntArray(firstindex, lastindex, xarray, XIntArray);
	
	for (int k=0; k<numberofarrays; k++)
	{
	 	plot->MakeYIntArray(firstindex, lastindex, yarray[k], YIntArray);

		if (pDC->IsPrinting())	
			plot->IntegerPlotLine(   lastindex-firstindex, XIntArray, YIntArray, printerlinecolr[k], PS_SOLID, 3);
//			plot->IntegerPlotLine(lastindex-firstindex, XIntArray, YIntArray, BLACK, linestyle[k], 1);
		else  {
			plot->IntegerPlotLine(   lastindex-firstindex, XIntArray, YIntArray, linecolr[k], PS_SOLID, 1);
			if (m_bDrawDotsOnGraph){
			 plot->IntegerPlotPoints(lastindex-firstindex, XIntArray, YIntArray, linecolr[k]);
			}
		}	
	}
	delete XIntArray;	
	delete YIntArray;
    // Retrieve info needed to determine xy values for mouse clicks in window
    // This info is saved between messages (even though rest of plot isn't).
    xaxis=plot->GetXAxisData();
    yaxis=plot->GetYAxisData();
    m_graphbox=plot->GetGraphBox();
	delete plot;
	
 	
	if (!pDC->IsPrinting())  {
	  // If this is the first time the graph's ever been drawn, we can do some initialization
	  if (!m_bPlotDataValid) { 
		// This is really OnSetFocus() code, placed here as a workaround.
		// MOD 15/1/96: Just make sure the caret coords are valid; don't change them
		// unless necessary (this lets us move caret around while graph is changing, eg
		// during a run).
		if (m_CaretIndexIntoArray>m_rightmostvisiblepoint) 
			m_CaretIndexIntoArray=m_rightmostvisiblepoint;
		if (m_CaretIndexIntoArray<m_leftmostvisiblepoint) 
			m_CaretIndexIntoArray=m_leftmostvisiblepoint;
		if (m_CaretCurveNumber>=numberofarrays) m_CaretCurveNumber=numberofarrays-1;
		if (m_CaretCurveNumber<0) m_CaretCurveNumber=0;
		m_bPlotDataValid=TRUE;
		RepositionCaret();
		ShowCaret();
	  } else {
		// curves have probably changed, so caret will be in the wrong
		// position. Need to update it.
		if (m_CaretIndexIntoArray>m_rightmostvisiblepoint) 
			m_CaretIndexIntoArray=m_rightmostvisiblepoint;
		if (m_CaretIndexIntoArray<m_leftmostvisiblepoint) 
			m_CaretIndexIntoArray=m_leftmostvisiblepoint;		
		RepositionCaret();
	  }
	}
}


/////////////////////////////////////////////////////////////////////////////
// CInteractiveGraph diagnostics

#ifdef _DEBUG
void CInteractiveGraph::AssertValid() const
{
	CView::AssertValid();
}

void CInteractiveGraph::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
 
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CInteractiveGraph message handlers


// an alternative is to use: if (m_graphbox.PtInRect(point)) { ...}
// instead of the clipping call.
// The advantage of the clipping is that it makes it easier to select edges of graph.
void CInteractiveGraph::OnLButtonDown(UINT /*nFlags*/, CPoint point)
{
	point=ClipToGraphBox(point);
    UndrawMarquee();
	m_StartDragPoint=point;
	m_EndDragPoint=point;
	m_IsMarqueeValid=TRUE;
	SetCapture();          
}


void CInteractiveGraph::OnMouseMove(UINT nFlags, CPoint point)
{
	if (GetCapture()) { // must be us - if another window had captured it, WE wouldn't get the message
		// make the marquee extend to the edges of the plot area if required
        point=ClipToGraphBox(point);  
		UndrawMarquee();
 		
   		m_IsMarqueeValid=TRUE;
		m_EndDragPoint=point; 
		DrawMarquee();
		if (IsTooCloseForDrag(m_EndDragPoint, m_StartDragPoint)) {
			WriteMessage("Release mouse button to cancel zoom");
		} else {
			WriteMessage("Release mouse button to zoom into selected region");		
		}
	} else	CView::OnMouseMove(nFlags, point);
}

// returns TRUE if difference between downpoint and uppoint isn't large enough to be a drag
BOOL CInteractiveGraph::IsTooCloseForDrag(CPoint ButtonDownPt, CPoint ButtonUpPt)
{
 	// use system dblclick distance to define what constitutes a click as opposed to drag
	// For this application we might want to use a larger number. So we multiply the value by 2.
 	int cxsmallest=2*GetSystemMetrics(SM_CXDOUBLECLK); 
 	int cysmallest=2*GetSystemMetrics(SM_CYDOUBLECLK);
  	return  (   abs(ButtonUpPt.x-ButtonDownPt.x) <= cxsmallest ||
                abs(ButtonUpPt.y-ButtonDownPt.y) <= cysmallest        );
}
  
void CInteractiveGraph::OnLButtonUp(UINT nFlags, CPoint point)
{
 	double EndDragmathx, EndDragmathy;            
 	int newpos;
 	double xnormalise, leftdiff, rightdiff;
 	
	if (GetCapture()) {
		ReleaseCapture();
		// make the marquee extend to the edges of the plot area if required
        point=ClipToGraphBox(point);
 		if (IsTooCloseForDrag(m_StartDragPoint, point)) {
    		// selected region is too small - therefore it must be a click
    		UndrawMarquee();
	 		m_EndDragPoint=point;	 		
            m_IsMarqueeValid=FALSE;
		    clientpt2math(point, EndDragmathx, EndDragmathy);
				newpos=CMath::FindBoundingInterval(EndDragmathx, m_rightmostvisiblepoint-m_leftmostvisiblepoint, &xarray[m_leftmostvisiblepoint]);
				if (newpos<0) newpos=0;
				newpos+=m_leftmostvisiblepoint;
				// now we know what interval it's in -- need to find which end of
				// interval is closer.
				if (newpos < m_leftmostvisiblepoint) newpos = m_leftmostvisiblepoint;
				else if (newpos<m_rightmostvisiblepoint) {
					xnormalise=xaxis.math2normalise(EndDragmathx);
    				leftdiff=fabs(xaxis.math2normalise(xarray[newpos])-xnormalise);
    				rightdiff=fabs(xaxis.math2normalise(xarray[newpos+1])-xnormalise);
    				if (leftdiff>rightdiff) newpos++; // right side is closer
				}
            	m_CaretIndexIntoArray=newpos;
				m_CaretCurveNumber=NearestCurve(EndDragmathy, newpos);
				RepositionCaret();
         } else {         
 			UndrawMarquee(); 		
	 		m_EndDragPoint=point;
   			m_IsMarqueeValid=TRUE; 		
			ZoomIn();
 		}
 	} else CView::OnLButtonUp(nFlags, point);
}

// cancels mouse and keyboard marquees
void CInteractiveGraph::CancelMarquee(void)
{
	if (GetCapture()){  
		// cancel mouse marquee if its being drawn
		ReleaseCapture(); 
		UndrawMarquee(); 
		m_IsMarqueeValid=FALSE; 
		WriteCaretCoordinates(); // make sure status bar is sensible
	}else if (m_IsMarqueeValid) {
		// cancel keyboard marquee
		UndrawMarquee(); 
		m_IsMarqueeValid=FALSE; 
		WriteCaretCoordinates(); // make sure status bar is sensible
	}
}
void CInteractiveGraph::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (m_IsMarqueeValid){
		CancelMarquee();		
	} else {
		if (CanZoomOut()) ZoomOut();
		// else switch to 4graph view
		else OnZoomOutFromFullSize();
		
	}
	CView::OnRButtonDown(nFlags, point);
}


void CInteractiveGraph::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{     
	 switch (nChar)
	 {
	 case VK_RETURN:
	 			if (CanZoomIn()) ZoomIn();
	 			else if (!m_IsMarqueeValid){ 
	 				double mathx;
	 				mathx = xarray[m_CaretIndexIntoArray];
	 				m_StartDragPoint=math2clientpt(mathx, m_dYMax);
	 				m_EndDragPoint=math2clientpt(mathx, m_dYMin);  
	 				m_StartDragPoint.y=m_graphbox.top;
	 				m_EndDragPoint.y=m_graphbox.bottom;
	 				m_IsMarqueeValid=TRUE; 
			 		WriteMessage("Use cursor keys to select a region to zoom in");
	 			}
	 			break;
/**	 
	 		if (GetKeyState(VK_SHIFT)>=0) {
	 			// Enter
	 			if (CanZoomIn()) ZoomIn();
	 		} else {
	 			// SHIFT+Enter
	 			if (CanZoomOut()) ZoomOut();
	 		}
	 		break;   
**/
	 case VK_ESCAPE:
	 /**
	 			if (GetCapture()) {
	 				// if we're currently drawing a marquee with mouse, stop it
	 				ReleaseCapture();
	 				UndrawMarquee();
		            m_IsMarqueeValid=FALSE;
	 				WriteCaretCoordinates(); // make sure status bar is sensible
	 			} else if (m_IsMarqueeValid) {
	 				// if we're currently drawing a marquee with keyboard, stop it.
	 				UndrawMarquee();
		            m_IsMarqueeValid=FALSE;
	 				WriteCaretCoordinates(); // make sure status bar is sensible
	**/
				if (m_IsMarqueeValid){ 
					CancelMarquee();	 				
	 			} else if (CanZoomOut()) ZoomOut();
	 				else OnZoomOutFromFullSize();
	 			break;
	 default:	 
		CView::OnChar(nChar, nRepCnt, nFlags);
	}
}

void CInteractiveGraph::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	 switch (nChar)
	 {
	 case VK_LEFT:
			if (GetKeyState(VK_CONTROL)<0) m_CaretIndexIntoArray-=10; else m_CaretIndexIntoArray--;
			if (m_CaretIndexIntoArray<m_leftmostvisiblepoint) m_CaretIndexIntoArray=m_leftmostvisiblepoint;
			RepositionCaret();
			break;
	 case VK_RIGHT:
			if (GetKeyState(VK_CONTROL)<0) m_CaretIndexIntoArray+=10; else m_CaretIndexIntoArray++;
			if (m_CaretIndexIntoArray>m_rightmostvisiblepoint) m_CaretIndexIntoArray=m_rightmostvisiblepoint;
			RepositionCaret();
			break;
	 case VK_UP:
            m_CaretCurveNumber=NextHigherCurve(m_CaretIndexIntoArray, m_CaretCurveNumber);
			RepositionCaret();
			break;
     case VK_DOWN:
            m_CaretCurveNumber=NextLowerCurve(m_CaretIndexIntoArray, m_CaretCurveNumber);
			RepositionCaret();
			break;
     case VK_HOME:
     		m_CaretIndexIntoArray=m_leftmostvisiblepoint;
			RepositionCaret();
     		break;
	 case VK_END:
     		m_CaretIndexIntoArray=m_rightmostvisiblepoint;
			RepositionCaret();
     		break;
/**     		
	// there's no purpose in including these since there's at most 4 curves.
	// later on, someone might want to use them to switch graphs or something.
	 case VK_PRIOR:	// page up
	 case VK_NEXT:	// page down
	 		break;
**/	 		
	 default:
		CView::OnKeyDown(nChar, nRepCnt, nFlags);
		break;
	 }
}     

/////////////////////////////////////////////////////////////////////////////
// CInteractiveGraph functions to be overridden in derived class
 
// override this function in the derived class.
// e.g. call SwitchToFourGraphView() 
void CInteractiveGraph::OnZoomOutFromFullSize(void)
{
}

void CInteractiveGraph::OnDoubleClickOnAxis(UINT /*nFlags*/, CPoint /*point*/)
{
}



/////////////////////////////////////////////////////////////////////////////
// CInteractiveGraph internal functions

//------------------------------ Coordinate functions---------------------------------------

CPoint CInteractiveGraph::ClipToGraphBox(CPoint clientpoint)
{
    CPoint result=clientpoint;
    // is it outside the graph area? If so, move it to the nearest edge.
    if (clientpoint.x<m_graphbox.left) result.x=m_graphbox.left;
    if (clientpoint.x>m_graphbox.right) result.x=m_graphbox.right;
    if (clientpoint.y<m_graphbox.top) result.y=m_graphbox.top;
    if (clientpoint.y>m_graphbox.bottom) result.y=m_graphbox.bottom;
    return result;
}

// converts from client coordinates to mathematical coords
// clips the point to the graph box
void CInteractiveGraph::clientpt2math(CPoint clientpoint, double &mathx, double &mathy)
{
	double xnormalise, ynormalise;
	clientpoint=ClipToGraphBox(clientpoint);        
    xnormalise = (clientpoint.x-m_graphbox.left)/(double)m_graphbox.Width();
    ynormalise = (m_graphbox.bottom-clientpoint.y)/(double)m_graphbox.Height();

    mathx=xaxis.normalise2math(xnormalise);
    mathy=yaxis.normalise2math(ynormalise);
}

// converts from client coordinates to mathematical coords
// Does not clip the point to the graph box
void CInteractiveGraph::clientpt2mathnoclip(CPoint clientpoint, double &mathx, double &mathy)
{
	double xnormalise, ynormalise;
    xnormalise = (clientpoint.x-m_graphbox.left)/(double)m_graphbox.Width();
    ynormalise = (m_graphbox.bottom-clientpoint.y)/(double)m_graphbox.Height();

    mathx=xaxis.normalise2math(xnormalise);
    mathy=yaxis.normalise2math(ynormalise);
}
                                                          
// converts mathematical x,y to client area coordinates.
// does not clip to graph box
CPoint CInteractiveGraph::math2clientpt(double mathx, double mathy)
{
    double xnormalise, ynormalise;
    double xout, yout;
    CPoint result;

	xnormalise=xaxis.math2normalise(mathx);
	ynormalise=yaxis.math2normalise(mathy);	
	                                        
	xout=m_graphbox.left+xnormalise*m_graphbox.Width();
	yout=m_graphbox.bottom-ynormalise*m_graphbox.Height();
	if (xout-(int)xout>0.5) result.x=(int)xout+1; else result.x=(int)xout;
	if (yout-(int)yout>0.5) result.y=(int)yout+1; else result.y=(int)yout;
	
	return result;
}


//------------------------------ Marquee functions---------------------------------------
        
// if marquee is active, draws it. If inactive, does nothing.
// we never draw tiny marquees. If it's below the minimum size, we hide it.
void CInteractiveGraph::DrawMarquee(void)
{
	if (m_IsMarqueeValid) 
	 if (!IsTooCloseForDrag(m_EndDragPoint, m_StartDragPoint)) {
	
		// draw new marquee
		CClientDC pDC(this); 
		pDC.SelectStockObject(HOLLOW_BRUSH);  
		pDC.SetROP2(R2_NOT);      // XOR drawing mode
		CRect marqueerect;
		marqueerect.left  =min( m_StartDragPoint.x, m_EndDragPoint.x);
		marqueerect.right =max( m_StartDragPoint.x, m_EndDragPoint.x)+1;
		marqueerect.top   =min( m_StartDragPoint.y, m_EndDragPoint.y);
		marqueerect.bottom=max( m_StartDragPoint.y, m_EndDragPoint.y)+1;
       	pDC.Rectangle(marqueerect);
    }
}
 
// if marquee is active, undraws it. If inactive, does nothing.
void CInteractiveGraph::UndrawMarquee(void)
{    
	DrawMarquee(); // because it's drawn in XOR mode, draw & undraw are identical
}  

void CInteractiveGraph::WriteMarqueeCoordinates(void)
{
	double StartDragmathx, StartDragmathy;
	double EndDragmathx, EndDragmathy;
	char outstr[257];
		
    clientpt2math(m_StartDragPoint, StartDragmathx, StartDragmathy);
    clientpt2math(m_EndDragPoint, EndDragmathx, EndDragmathy);
	    
	sprintf(outstr, "Selection:(%g,%g)-(%g,%g)", StartDragmathx, StartDragmathy,
 			EndDragmathx, EndDragmathy);
	WriteMessage(outstr);	
}

//------------------------------ Initialization functions ---------------------------------------

// clears all data, gives a new name for the graph
void CInteractiveGraph::NewGraph(CString title)
{
	int n;
	
	sTitle=title;
	
	
	m_StartDragPoint=0;
	m_EndDragPoint=0;
	m_IsMarqueeValid=FALSE;
	m_bPlotDataValid=FALSE;
	
	m_dXMin=-2; m_dXMax=2;
	m_dYMin=-2; m_dYMax=2;
	m_ZoomLevel=0;


	m_xscale=m_yscale=LINEAR;	
    sXAxisLabel= "Xlabel";
    m_sXUnits="Units"; m_sXDescription="X"; m_sXAbbrev="x";
	numberofarrays=0;
	numberofpoints=0;  
	xarray=0;
	for (n=0; n<MAXIMUMNUMBEROFCURVES; n++) {
		yarray[n]=0;
	    sYAxisLabel[n]= "Ylabel";
	    m_sYUnits[n]="Units"; m_sYDescription[n]="Y"; m_sYAbbrev[n]="y";
	}
}
	

void CInteractiveGraph::SetXArray(int numpoints, double arrayX[], CString xlabel, CString xdescription, CString xunits, CString xabbreviation)
{
	numberofpoints=numpoints;
	sXAxisLabel=xlabel;
	xarray=arrayX;
	m_sXDescription=xdescription;
	m_sXUnits=xunits;
	m_sXAbbrev=xabbreviation;
}

void CInteractiveGraph::SetNextYArray(int numpoints, double arrayY[], CString ylabel, CString ydescription, CString yunits, CString yabbreviation)
{
	numpoints;
	ASSERT (numberofpoints==numpoints);
	ASSERT(numberofarrays<MAXIMUMNUMBEROFCURVES);
	if (numberofarrays>=MAXIMUMNUMBEROFCURVES) return; //comment this line to generate a compiler error! I don't know why!
	yarray[numberofarrays]=arrayY;
	sYAxisLabel[numberofarrays]=ylabel;
	m_sYDescription[numberofarrays]=ydescription;
	m_sYUnits[numberofarrays]=yunits; 
	m_sYAbbrev[numberofarrays]=yabbreviation;
	numberofarrays++;
}

void CInteractiveGraph::SetDataLimits(double xmin, double xmax, CScaleType xScale, double ymin, double ymax, CScaleType yScale)
{
	m_dXMin=xmin; m_dXMax=xmax;
	m_dYMin=ymin; m_dYMax=ymax;
	m_xscale=xScale; m_yscale=yScale;
	m_ZoomLevel=0;	
} 
                                                                  
// default = write to status bar of main window                                                                  
// This function can be overridden by the derived class if desired
void CInteractiveGraph::WriteMessage(CString msg)
{
	ASSERT(IsWindowVisible());     // If this fails, it means the paint logic isn't very clever & is repainting invisible windows
	CWnd *parent=AfxGetMainWnd();
	CWnd *statusbar;    
	statusbar=parent->GetDescendantWindow(AFX_IDW_STATUS_BAR, TRUE);
	statusbar->SetWindowText(msg);

}

void CInteractiveGraph::SetZoom(double xmin, double xmax, double ymin, double ymax)
{
	// save current coords onto zoom stack
	m_ZoomStack[m_ZoomLevel].XMin=m_dXMin;
	m_ZoomStack[m_ZoomLevel].XMax=m_dXMax;
	m_ZoomStack[m_ZoomLevel].YMin=m_dYMin;
	m_ZoomStack[m_ZoomLevel].YMax=m_dYMax;
	m_ZoomLevel++;
	m_dXMin=xmin; m_dXMax=xmax; m_dYMin=ymin; m_dYMax=ymax;
}

void CInteractiveGraph::GetCurrentGraphLimits(double &xmin, double &xmax, double &ymin, double &ymax)
{
	xmin=m_dXMin; xmax=m_dXMax; ymin=m_dYMin; ymax=m_dYMax;
}

// zooms into current marquee of graph 
// coords of marquee become coords of graph 
void CInteractiveGraph::ZoomIn(void)
{
 	double StartDragmathx, StartDragmathy;
 	double EndDragmathx, EndDragmathy;
	if (m_IsMarqueeValid) {
		if (m_ZoomLevel<MAXIMUMNUMBEROFZOOMS){
			// save current coords onto zoom stack
			m_ZoomStack[m_ZoomLevel].XMin=m_dXMin;
			m_ZoomStack[m_ZoomLevel].XMax=m_dXMax;
			m_ZoomStack[m_ZoomLevel].YMin=m_dYMin;
			m_ZoomStack[m_ZoomLevel].YMax=m_dYMax;
			m_ZoomLevel++;			
		}

// If it's near the edge, expand out by 1% so we see bits off the edge.
	int deltax=(m_graphbox.right-m_graphbox.left)/100;
	int deltay=(m_graphbox.bottom-m_graphbox.top)/100;		
	if (m_EndDragPoint.x<=m_graphbox.left) m_EndDragPoint.x-=deltax;
	if (m_EndDragPoint.x>=m_graphbox.right) m_EndDragPoint.x+=deltax;
	if (m_StartDragPoint.x<=m_graphbox.left) m_StartDragPoint.x-=deltax;
	if (m_StartDragPoint.x>=m_graphbox.right) m_StartDragPoint.x+=deltax;
	if (m_EndDragPoint.y<=m_graphbox.top) m_EndDragPoint.y-=deltay;
	if (m_EndDragPoint.y>=m_graphbox.bottom) m_EndDragPoint.y+=deltay;
	if (m_StartDragPoint.y<=m_graphbox.top) m_StartDragPoint.y-=deltay;
	if (m_StartDragPoint.y>=m_graphbox.bottom) m_StartDragPoint.y+=deltay;
	
	    clientpt2mathnoclip(m_EndDragPoint, EndDragmathx, EndDragmathy);
   	    clientpt2mathnoclip(m_StartDragPoint, StartDragmathx, StartDragmathy);
   	    
 		if (EndDragmathx<StartDragmathx) {
				m_dXMin=EndDragmathx;
				m_dXMax=StartDragmathx;
		} else {
				m_dXMin=StartDragmathx;
				m_dXMax=EndDragmathx;
		}
 		if (EndDragmathy<StartDragmathy) {
				m_dYMin=EndDragmathy;
				m_dYMax=StartDragmathy;
		} else {
				m_dYMin=StartDragmathy;
				m_dYMax=EndDragmathy;
		}
		m_IsMarqueeValid=FALSE; 
		Invalidate(FALSE);     
	}
} 

// zooms out of current marquee of graph 
// ??? might be better to do this as undo ???
void CInteractiveGraph::ZoomOut(void)
{
		// restore original data limits  
		if (m_ZoomLevel>0) {
			m_ZoomLevel--;
			m_dXMin=m_ZoomStack[m_ZoomLevel].XMin;
			m_dXMax=m_ZoomStack[m_ZoomLevel].XMax;
			m_dYMin=m_ZoomStack[m_ZoomLevel].YMin;
			m_dYMax=m_ZoomStack[m_ZoomLevel].YMax;
			m_IsMarqueeValid=FALSE;
		}
		Invalidate(FALSE);     
}

// zooms out to top level of graph
void CInteractiveGraph::ZoomOutFully(void)
{
	if (m_ZoomLevel>0) {
		m_ZoomLevel=1;
		ZoomOut();
	}
}

// Puts the data for all curves onto the clipboard using the CF_TEXT format
// i.e. num TAB num TAB num CRLF 
//		num TAB num TAB num CRLF
//		0
void CInteractiveGraph::CopyGraphDataToClipboard(int firstxindex, int lastxindex)
{
    HANDLE hGlobalMemory;
    LPSTR lpGlobalMemory;
    int n, m;
    char outstr[200]; 
    CString myString;
	CString sxunit,syunit;

	myString=sTitle;
	myString+="\r\n";	
	myString+=m_sXDescription;
	for (m=0; m<numberofarrays; m++) {
		myString+='\t';
		myString+=m_sYDescription[m];
	}
	myString+="\r\n";
	    
    for (n=firstxindex; n<=lastxindex; n++) {
    	sprintf(outstr, "%g", xarray[n]);
    	myString+=outstr;
    	for (m=0; m<numberofarrays; m++){
    		sprintf(outstr, "\t%g", yarray[m][n]);
    		myString+=outstr;
    	}
    	myString+="\r\n";
    }
    
    hGlobalMemory=GlobalAlloc(GHND, (DWORD)myString.GetLength()+1);
    if (!hGlobalMemory) return; // Error: could not allocate
    lpGlobalMemory=(LPSTR)GlobalLock(hGlobalMemory);
    GlobalUnlock(hGlobalMemory);
    lstrcpy(lpGlobalMemory, myString);

    OpenClipboard();
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hGlobalMemory);
    CloseClipboard();
}
                     
// copies from m_leftmostvisiblepoint to m_rightmostvisiblepoint, all graphs                     
void CInteractiveGraph::CopyGraphSelectionToClipboard(void)
{
	CopyGraphDataToClipboard(m_leftmostvisiblepoint, m_rightmostvisiblepoint);
}

void CInteractiveGraph::CopyEntireGraphToClipboard(void)
{
	CopyGraphDataToClipboard(0, numberofpoints);
}

//------------------------------ Caret functions---------------------------------------

// Need to create the caret and position it correctly                          
// There's a problem here: To position it requires knowledge of the axes.
// But, the axes are ONLY set during the WM_PAINT processing. This is OK,
// except that OnSetFocus() is called *before* the first call to OnDraw().
// So we need a m_bPlotDataValid flag.
void CInteractiveGraph::OnSetFocus(CWnd* pOldWnd)
{
	// BIZARRE DISCOVERY: When you call ShowWindow(HIDE), the window being hidden will
	// briefly get the focus (even though it's invisible)! Don't know why Windows does this.
	if (!IsWindowVisible()) return; // we don't want to do anything if the window's invisible
	
	CView::OnSetFocus(pOldWnd);
//	CreateSolidCaret(2, CARETHEIGHT);
	CreateCaret(&m_CaretBitmap);
	
	if (m_bPlotDataValid) {
		RepositionCaret();
		ShowCaret();
	}
}

void CInteractiveGraph::OnKillFocus(CWnd* pNewWnd)
{
	CView::OnKillFocus(pNewWnd);
	
	HideCaret();
	DestroyCaret();
	UndrawMarquee();
	m_IsMarqueeValid=FALSE;
}
                                
// moves the caret to the the correct position, based on the member variables.
// Also writes the new coordinates in the status line.
// Also, if a marquee is being drawn, changes the coords of the marquee and redraws it.
// This way, can use cursor keys to change size of the marquee.
void CInteractiveGraph::RepositionCaret(void)
{
    double mathx, mathy;
    CPoint where;

    mathx=xarray[m_CaretIndexIntoArray];
    mathy=yarray[m_CaretCurveNumber][m_CaretIndexIntoArray]; 
    where=ClipToGraphBox(math2clientpt(mathx, mathy));
    where.y-=CARETHOTSPOTY;
    where.x-=CARETHOTSPOTX;
	SetCaretPos(where);
	
	if (m_IsMarqueeValid && !GetCapture()){ 
			// marquee is being drawn AND it's a keyboard (not mouse) marquee
			UndrawMarquee();
	 		m_EndDragPoint=math2clientpt(mathx, m_dYMin);
			m_EndDragPoint.y=m_graphbox.bottom; // make it extend to edges of graph
	 		DrawMarquee();
	 		WriteMessage("Press ENTER to zoom in, ESC to cancel");
    } else {
    	WriteCaretCoordinates();   
    }
}

// gets width in pixels that string will have when displayed in status bar.
// clever but a bit dirty
int CInteractiveGraph::GetTextWidthForStatusBarFont(CString str)
{
	CStatusBar *pBar;
	CFont *barfont;

	pBar=(CStatusBar *)( AfxGetMainWnd()->GetDescendantWindow(AFX_IDW_STATUS_BAR, TRUE));
	barfont=pBar->GetFont();
	
	CClientDC dc(this);
	dc.SelectObject(barfont);
	CSize strsize=dc.GetTextExtent(str, str.GetLength());
	return strsize.cx; 
}

// gets width in pixels of pane 0 of status bar
// clever but a bit dirty
// GetItemRect is wrong if window is really small - never goes below a certain minimum size.
// This routine seems to work regardless.
int CInteractiveGraph::GetWidthOfStatusBar(void)
{
	CRect rect;
	CStatusBar *pBar;
	int itemwid;

	pBar=(CStatusBar *)( AfxGetMainWnd()->GetDescendantWindow(AFX_IDW_STATUS_BAR, TRUE));
	pBar->GetItemRect(0, rect);
	itemwid=rect.Width(); 
	pBar->GetClientRect(rect);
	if (rect.Width()<itemwid) return rect.Width()-16; 
	else return itemwid-8;// -8 because of borders either side of the text
	
}
	
void CInteractiveGraph::WriteCaretCoordinates(void)
{
	char outstr[257];
	double mathx, mathy;
	CString sxunit,syunit;

//	ASSERT(!m_sYUnits[m_CaretCurveNumber].IsEmpty());
    mathx=xarray[m_CaretIndexIntoArray];
    mathy=yarray[m_CaretCurveNumber][m_CaretIndexIntoArray];
	sxunit=FormatSIString(mathx, m_sXUnits);
    syunit=FormatSIString(mathy, m_sYUnits[m_CaretCurveNumber]);
                                                   
	int statbarwid;
	statbarwid=GetWidthOfStatusBar();
    sprintf(outstr, "%s=%s, %s=%s", (const char *)m_sXDescription, (const char *)sxunit, (const char *)m_sYDescription[m_CaretCurveNumber], (const char *)syunit);
    if (GetTextWidthForStatusBarFont(outstr)>statbarwid) {
    	// it won't fit! Try making it shorter
	    sprintf(outstr, "%s=%s, %s=%s", (const char *)m_sXAbbrev, (const char *)sxunit, (const char *)m_sYDescription[m_CaretCurveNumber], (const char *)syunit);
	    if (GetTextWidthForStatusBarFont(outstr)>statbarwid) {
	    	sprintf(outstr, "%s=%s, %s=%s", (const char *)m_sXAbbrev, (const char *)sxunit, (const char *)m_sYAbbrev[m_CaretCurveNumber], (const char *)syunit);
	    	if (GetTextWidthForStatusBarFont(outstr)>statbarwid) 
				sprintf(outstr, "%s, %s", (const char *)sxunit, (const char *)syunit);
		}
    }
	WriteMessage(outstr);
}

// returns the index of the first curve which is lower than y.
// returns -1 if there are none below
int CInteractiveGraph::NearestCurveBelow(double y, int x)
{
    int n;
    int bestindex=-1;
    double startnorm=yaxis.math2normalise(y);
    double ynorm;
    for (n=0; n<numberofarrays; n++) {
    	ynorm=yaxis.math2normalise(yarray[n][x]);
        if (ynorm<=startnorm) {
            if (bestindex==-1 || ynorm>yaxis.math2normalise(yarray[bestindex][x])) bestindex=n;
        }
    }
    return bestindex;
}

int CInteractiveGraph::NearestCurveAbove(double y, int x)
{
    int n;
    int bestindex=-1;
    double startnorm=yaxis.math2normalise(y);
    double ynorm;
    for (n=0; n<numberofarrays; n++) {
    	ynorm=yaxis.math2normalise(yarray[n][x]);
        if (ynorm>=startnorm) {
            if (bestindex==-1 || ynorm<yaxis.math2normalise(yarray[bestindex][x])) bestindex=n;
        }
    }
    return bestindex;
}

int CInteractiveGraph::NearestCurve(double y, int xindex)
{
    double ynormalise;
    double topdiff, bottomdiff;
    int top, bottom;
    top=NearestCurveAbove(y,xindex);
    bottom=NearestCurveBelow(y,xindex);
    if (top==-1) return bottom;
    if (bottom==-1) return top;
    // we can't use a simple comparison, since it could be a log scale
    // it's OK to use normalised coords though.
    ynormalise=yaxis.math2normalise(y);
    topdiff=yaxis.math2normalise(yarray[top][xindex])-ynormalise;
    bottomdiff=ynormalise-yaxis.math2normalise(yarray[bottom][xindex]);
    if (topdiff<bottomdiff) return top; else return bottom;
}

// MICROSOFT BUG FIX - these next two functions fail with optimizer on
#pragma optimize("g", off)

// returns curveindex of curve above startcurve at pt.
// We must ensure that, as this function is called repeatedly, every index
// will be returned.
// So, if two curves have the same y value at x=pt, we want to return the
// lowest curveindex, but it must be greater than startcurve.
// It's complicated by log scales: we have to know whether to take the
// absolute value, so we can't use a simple comparison
// BUT it's OK to use normalised coords. This allows us to add bizarre scales to
// CPlot2D later on. (The original log scale is also a bit bizarre since it take abs value
// first)
int CInteractiveGraph::NextHigherCurve(int x, int startcurve)
{
    double startvalue;
	double bestvalue;
    int bestcurve;  // the best curve & closest value so far
    int i;
    double ynorm;

	startvalue=yaxis.math2normalise(yarray[startcurve][x]);
    bestcurve=startcurve;
    for (i=0; i<numberofarrays; i++) {
         ynorm=yaxis.math2normalise(yarray[i][x]);
		 // _never_ go backwards
         if (ynorm<startvalue) continue;
	     // is there a curve which is equal to it, with a higher curveindex?
         if (ynorm==startvalue)  {
			 if (i<=startcurve) continue; else return i;
         }
         // now we know ynorm > startvalue
         if (bestcurve==startcurve || ynorm < bestvalue) {
			 bestcurve=i; bestvalue=ynorm; 
		 }
    }
	return bestcurve;
}

// Same as NextHigherCurve(), except
// returns first curve below startcurve at pt.
int CInteractiveGraph::NextLowerCurve(int pt, int startcurve)
{
    double startvalue, bestvalue;
    int bestcurve;  // the best curve & closest value so far
    int i;
    double ynorm;

    startvalue=yaxis.math2normalise(yarray[startcurve][pt]);
    bestcurve=startcurve;
    for (i=numberofarrays-1; i>=0; i--) {
         ynorm=yaxis.math2normalise(yarray[i][pt]);
         if (ynorm>startvalue) continue;
	     // is there a curve which is equal to it, with a lower curveindex?
         if (ynorm==startvalue)  {
         	if (i>=startcurve) continue; else return i;
         }
         // now we know ynorm < startvalue
         if (bestcurve==startcurve || ynorm > bestvalue) {
			 bestcurve=i; bestvalue=ynorm; 
		 }
    }
	return bestcurve;
}

#pragma optimize("", on)

void CInteractiveGraph::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if (!m_graphbox.PtInRect(point))	
		OnDoubleClickOnAxis(nFlags, point);	// double-click outside graph box
	
	CView::OnLButtonDblClk(nFlags, point);
}
