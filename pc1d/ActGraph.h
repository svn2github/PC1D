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
// actgraph.h : interface of the CInteractiveGraph class
//
// NOTE: This doesn't have a GetDocument() function, since it's not
// actually associated with a document.
/////////////////////////////////////////////////////////////////////////////

#define INTERACTIVEGRAPH_HEADER
                                              
#ifndef PLOT2D_HEADER     
#include "plot2d.h"
#endif
      
#define MAXIMUMNUMBEROFCURVES 4
#define MAXIMUMNUMBEROFZOOMS 10
    
struct CDoubleRect
{
		double XMin;
		double XMax;
		double YMin;
		double YMax;
};
		      
class CInteractiveGraph : public CView
{

// Construction/Destruction
protected: // create from serialization only
	CInteractiveGraph();
	DECLARE_DYNCREATE(CInteractiveGraph)
	virtual ~CInteractiveGraph();

// Attributes
private:
    // Marquee variables
	CPoint m_StartDragPoint; // point at which user started dragging
	CPoint m_EndDragPoint; // point at which user ended dragging  
	BOOL m_IsMarqueeValid; // are both start & end coords valid?
    // Caret variables
    int m_CaretIndexIntoArray; // index into array
    int m_CaretCurveNumber;  // which yarray(curve) is caret on
    CBitmap m_CaretBitmap;
	HCURSOR m_hMagnifyCursor;

    // Plot variables
	BOOL m_bPlotDataValid; // TRUE if the following variables are valid
	double m_b;
	double m_l;
	double m_r;
	double m_t; 
	CScaleType m_xscale;
	CScaleType m_yscale;
    CPlot2D *plot;
    BOOL ShouldTitlesBeRedrawn; // used for optimizing the redraws. Set to FALSE to allow speedup
    BOOL m_bDrawDotsOnGraph;  // if TRUE, draws dots on graph at graph points. if FALSE, just lines
    
    double  m_dXMin, m_dYMin, m_dXMax, m_dYMax;
    CString sXAxisLabel, sTitle;
    CString sYAxisLabel[MAXIMUMNUMBEROFCURVES];
    CString m_sXDescription, m_sXUnits; // used for caret output string
    CString m_sYDescription[MAXIMUMNUMBEROFCURVES], m_sYUnits[MAXIMUMNUMBEROFCURVES];
    CString m_sXAbbrev, m_sYAbbrev[MAXIMUMNUMBEROFCURVES];
    CAxisData xaxis, yaxis;
    CRect m_graphbox;       
    
    CDoubleRect m_ZoomStack[MAXIMUMNUMBEROFZOOMS]; // original data limits-used by ZoomOut()      
    int m_ZoomLevel;
	
    double *xarray; // the x data
    double *yarray[MAXIMUMNUMBEROFCURVES]; // the y data
    int numberofpoints;
    int numberofarrays;
    int m_leftmostvisiblepoint;
    int m_rightmostvisiblepoint;

// Operations
private:
	void DrawPlot(CDC *pDC); 
	
	static BOOL IsTooCloseForDrag(CPoint ButtonDownPt, CPoint ButtonUpPt);
	void DrawMarquee(void);
	void UndrawMarquee(void);
	void CancelMarquee(void);
	void WriteMarqueeCoordinates(void);
	BOOL IsMarqueeSelected(void) { return m_IsMarqueeValid; };
		
	void RepositionCaret(void);    
	void WriteCaretCoordinates(void);
    void WriteMessage(CString msg);
	int GetTextWidthForStatusBarFont(CString str);
	int GetWidthOfStatusBar(void);

	void CopyGraphDataToClipboard(int firstxindex, int lastxindex);

public:
	virtual void OnDraw(CDC* pDC); 
	void NewGraph(CString title);
	void SetXArray(int numpoints, double arrayX[], CString xlabel, CString xdescription, CString xunits, CString xabbreviation);
	void SetNextYArray(int numpoints, double arrayY[], CString ylabel, CString ydescription, CString yunits, CString yabbreviation);
    void SetDataLimits(double xmin, double xmax, CScaleType xScale, double ymin, double ymax, CScaleType yScale);    
    void SetTitleRedrawNeeded(BOOL bNeeded); // used for optimizing the redraws. Set to FALSE to allow speedup
    void SetShouldDrawDots(BOOL bshoulddraw) { m_bDrawDotsOnGraph=bshoulddraw; };
    
	void ZoomIn(void);
	void ZoomOut(void);
	void ZoomOutFully(void);
	BOOL CanZoomIn(void){ return m_IsMarqueeValid; };
	BOOL CanZoomOut(void){ return m_ZoomLevel>0; };
	void CopyEntireGraphToClipboard(void);
	void CopyGraphSelectionToClipboard(void);

protected:
	void SetZoom(double xmin, double xmax, double ymin, double ymax);
	void GetCurrentGraphLimits(double &xmin, double &xmax, double &ymin, double &ymax);
	CScaleType GetXScaleType(void) { return m_xscale; };
	CScaleType GetYScaleType(void) { return m_yscale; };


// overrideable functions
protected:
	virtual void OnZoomOutFromFullSize(void);
	virtual void OnDoubleClickOnAxis(UINT nFlags, CPoint point);

// Implementation
private:
	void clientpt2math(CPoint clientpoint, double &mathx, double &mathy);
	void clientpt2mathnoclip(CPoint clientpoint, double &mathx, double &mathy);
	CPoint math2clientpt(double mathx, double mathy);
    CPoint ClipToGraphBox(CPoint clientpoint);
    int NearestCurveAbove(double y, int xindex);
    int NearestCurveBelow(double y, int xindex);
    int NearestCurve(double y, int xindex);
	int NextHigherCurve(int pt, int startcurve);
	int NextLowerCurve(int pt, int startcurve);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CInteractiveGraph)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
    
/////////////////////////////////////////////////////////////////////////////
