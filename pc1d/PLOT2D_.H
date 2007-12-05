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
// plot2d.h : interface of the CPlot2D class
//
// MODS:
//	10/10/95:DAC: Changed AxisData struct to a class, with coordinate conversions
//	16/10/95:PAB: Moved MaxStrLen to implementation file  
//	30/11/95:DAC: Moved GetMax() & GetMin() to CAxisData.
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Constant Declarations

#define PLOT2D_HEADER 1
#include <math.h>

/////////////////////////////////////////////////////////////////////////////
// Type Declarations

#ifndef PHYSICALCONSTANTS_H
enum CScaleType{LINEAR, LOG};
#endif
typedef double PercentScreen;	// number 0 to 100, representing the position
								// of the graph (from left and bottom sides)
/////////////////////////////////////////////////////////////////////////////

CString FormatSIString(double data, const CString sUnits);

class CAxisData
{
private:
	double Min, Max;     // Values used to draw the Axes
	int Int,   // number of intervals
		Div,   // number of divisions per interval
		Dec,   // number of decimal places
		Eng;   // Exponent in engineering notation
	CScaleType Scale;
	double pow10Eng; // pow(10,Eng). This is used so often it's put here as a speedup.
public:
	CAxisData(void);
// used for math<->client conversions
	double math2normalise(double data);
	double normalise2math(double normalise);
	BOOL IsPointVisible(double data);
	int math2pixel(double data, int axislengthinpixels);
	void matharray2pixel(int numpoints, double *matharray, int *pixarray, int axislengthinpixels); 
	double GetMin(void); // returns smallest xvalue on graph (this is not quite the same as Min)
	double GetMax(void); // returns largest x
	CScaleType GetScale(void) { return Scale; };
	void FormatLabel(CString &sLabel);
	// Converts the units so that it displays well on the screen.
	void SetLimits(double MinIn, double MaxIn, CScaleType sc);
	void AutoScale(int nMaxIntervals);
	// Modifies the Max/Min values to give the best fit for the graph 
	int GetNumIntervals(void) { return Int; }; 
	int GetNumDivisions(void) { return Div; };
	double GetIntervalSize(void) { return (Max-Min)/Int; };	// Data size of interval
	double GetDataValueForTic(int n); // returns axis val for the nth major tick.
	CString GetTextForTic(int n); 
	double GetDataValueForShortTic(int nmajor, int nminor);
	int GetPixelPosForTic(int n, unsigned int axislengthinpixels);
	int GetPixelPosForShortTic(int nmajor, int nminor, unsigned int axislengthinpixels);
	
	friend BOOL operator ==(CAxisData &first, CAxisData &second);
};


/////////////////////////////////////////////////////////////////////////////

class CPlot2D : public CObject
{

// Attributes 
private:
	CDC*  pDC;
protected:
	CFont Font;	// created in constructor
	CFont Font90;	// Font rotated 90 degrees
	TEXTMETRIC tm, tm90; // created in constructor, font characteristics
	CSize m_Extent;  // created in constructor, screen size
	int m_FontWidthOfDigit; // Width of a '0' in the font

	CRect	m_GraphBox,		// Rectangle area where graph is drawn
			m_GraphBorder;	// Rectangle area including graph, titles, etc.
 
	COLORREF	m_GraphBorderColor, m_GraphBackGroundColor,
				m_GraphBoxColor, m_TicColor, m_LabelColor;

	CAxisData m_x, m_y;

// Operations
protected:
 void LocateGraphAxes(BOOL bTitle);
 // Calculates the size and position of the box where the graph is
 // plotted. It is only determined as a ratio of the font.

 void XTics(BOOL bNeedNumbers);
 void YTics(BOOL bNeedNumbers);
 // Draws the axes and units.
 
 void PrintXLabel(CString sLabel);
 void PrintYLabel(CString sLabel);
 void PrintTitle (CString sTitle);

public:
 CPlot2D(CDC* pDC, CView* pView);
  // must pass pointers to current device context and view
 ~CPlot2D(); // destructor to remove font resources
 void InitializeFonts(CDC *pDC, CString fontname, int maxgraphheight);

 void IntegerPlotLine(int numpoints, int *XIntArray, int *YIntArray, 
						COLORREF LineColor, int nPenStyle, int nPenWidth);
 void IntegerPlotPoints( int numpoints, int *XIntArray, int *YIntArray,
 					COLORREF PointColor);						
 void MakeXIntArray(int nStartIndex, int nStopIndex, double *DataX, int *XIntArray);
 void MakeYIntArray(int nStartIndex, int nStopIndex, double *DataY, int *YIntArray);

 void CalculateAxes( double dwXMinIn, double dwXMaxIn, CScaleType XScale,
					double dwYMinIn, double dwYMaxIn, CScaleType YScale, BOOL bTitle);
 void DrawTitleAndBackground(CString sTitle);
 void DrawAxesAndGraphArea(CString sXAxisLabel, CString sYAxisLabel, BOOL bXAxisChanged, BOOL bYAxisChanged);
    
 void LocateGraph(	PercentScreen left, PercentScreen right,
					PercentScreen bottom, PercentScreen top,
					COLORREF BorderGraphColor, COLORREF GraphColor, 
					COLORREF TicColor, COLORREF LabelColor);  // Setup the initial Graph area on the window
 CSize GetClientSize() {return m_Extent;}
 
 // Returns the min/max math values actually used to draw the graph
 CRect GetGraphBox() {return m_GraphBox;}
 CAxisData GetXAxisData() {return m_x; }
 CAxisData GetYAxisData() {return m_y; }

protected:
 int  xmath2client(double mathx);
 int  ymath2client(double mathy);

#ifdef _DEBUG
 void Dump(CDumpContext& dc) const;
#endif

};


//void PlotLine( int nStartIndex, int nStopIndex,
//				double DataX[], double DataY[],
//				COLORREF LineColor, int nPenStyle, int nPenWidth);
// void PlotPoints( int nStartIndex, int nStopIndex,
//					double DataX[], double DataY[],
//					COLORREF PointColor);
