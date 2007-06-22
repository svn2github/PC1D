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
// plot2d.cpp
// implementation of the CPlot2D class
//	Created: by YPS.
//	MODS:
//	30/11/95:DAC: Moved GetMax() & GetMin() to CAxisData. 
// 				  Bugfix:Made math<->normalise funcs use Eng.
//				  Added CAxisData::FormatSIString()                
//	 4/12/95:DAC: Bugfix in CPlot2D:FormatNum()- avoid string overflows
//	 8/02/96:DAC: Moved all of the axis code into CAxisData. CPlot2D
//					is now primarily a graphics class (maths & graphics
//					have been separated).
//	 9/02/96:DAC: Separated Axes() function into calculation & drawing.
//				  This lets us do conditional redraws based on whether
//				  axis has changed. *massive* speed improvement (up to 2x).
//	19/02/96:DAC: Changed many functions to use integers instead of floating
//				  point. In some cases, this was a stupid idea.
//	21/02/96:DAC: Changed IntegerDrawLines to use Polyline instead of LineTo().
//				  Doubles the speed of the graphics on Pentium!
//  05/05/98:DAC: PolyPolyLine() for tic-drawing instead of LineTo(). 18% faster graphs.


/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "plot2d.h"
#include <math.h>

const int MaxStrLen = 6;
const char SI_Prefix[] = {'f','p','n','µ','m',' ','k','M','G','T'};


// Does the SI units stuff - converts # in sUnits with correct SI unit.
// COMMENT ABOUT DISPLAYING NUMBERS IN THE STATUS BAR:
// It'd be nice if we could ensure the string was always the same length. This way we
// wouldn't get flicker when the numbers change.
// From looking in barcore.cpp in the MFC source, we see that the status bar uses
// 10 point Sans Serif font & doesn't support tabs. But in Sans Serif, the digits 0 to 9
// are the same width.
CString FormatSIString(double data, const CString sUnits)
{
	int pos;
	CString sLabel=sUnits;
	BOOL bWasReplaced; // if it doesn't have an SI suffix, we need to retain the exponent
	int eng;
	if(data==0)	eng = 0;
			else
				eng = (int) 3*((int)floor(log10(fabs(data))/3));
                       
	bWasReplaced=FALSE;                       
	if ((pos=sLabel.Find('#'))>=0 && eng<=12 && eng>=-15) {
		if (eng/3 ==0) {
			// No prefix - so just remove the #
			sLabel = sLabel.Left(pos) + sLabel.Right(sLabel.GetLength()-pos-1);
        } else {
        	// replace the #
		 	sLabel.SetAt(pos,SI_Prefix[eng/3 + 5]);	// replace # with correct SI prefix
		}
		bWasReplaced=TRUE;
	}
	while ((pos=sLabel.Find('#'))>=0 && eng<=12 && eng>=-15) {
		if (eng/3 ==0) 	sLabel = sLabel.Left(pos) + sLabel.Right(sLabel.GetLength()-pos-1);
        else sLabel.SetAt(pos,SI_Prefix[eng/3 + 5]);	// replace other # with same prefix
		bWasReplaced=TRUE;
	}
	while ((pos=sLabel.Find('#'))>=0)	// remove any leftover #
		sLabel = sLabel.Left(pos) + sLabel.Right(sLabel.GetLength()-pos-1);
		
		// Now, convert the number 
	CString buffer;
	if (bWasReplaced)	buffer.Format("%#-4.4g", data/pow(10,eng));
	else  buffer.Format("%#-4.4g", data);
	return buffer+sLabel;
}

/////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------
// CAxisData
//-----------------------------------------------------------------------

CAxisData::CAxisData(void)
{
	Min=0; Max=1;
	Int=Div=Dec=0;
	Eng=0; pow10Eng=1;
	Scale=LINEAR;
}


// The following two functions are used for conversions of mathematical points to client coordinates and back.
// returns the fractional position of 'data' across the axis
double CAxisData::math2normalise(double data)
{
	double fr;
	if(Scale == LOG) {
	  if (data!=0)  fr=((double)(log10(fabs(data))) - Min)/(Max - Min);
	  else fr=0;
 	} 
    else { 
 		// this also takes into account engineering notation
		fr=(data/pow10Eng - Min)/(Max - Min);
	}
	if (fr<-1) fr=-1;	// avoid major overflows
	if (fr>2) fr=2;
	return fr;
}


double CAxisData::normalise2math(double normalise)
{         
 	double math;
    if(Scale == LOG) math = (double)pow(10.0,normalise*(Max - Min) + Min);
 	else math = (normalise*(Max - Min) + Min)*pow10Eng;
 	return math;
}

int CAxisData::math2pixel(double data, int axislengthinpixels)
{
	double out = math2normalise(data) * axislengthinpixels;
	return (int)(out+0.5);
}

// Converts an entire array of doubles to a set of integer pixel distances, based on
// the axis. Equations are:
//	Log scale:
//	  	 out= axislengthinpixels*((double)(log10(fabs(matharray[i]))) - Min)/(Max - Min);
//	  	 pixarray[i]=(int)(out+0.5);
//	Linear:
//		out= axislengthinpixels*(matharray[i]/pow10Eng - Min)/(Max - Min);
//    	pixarray[i]=(int)(out+0.5);
// Calculate the constants const_a, const_b to reduce to one add + one multiply.
void CAxisData::matharray2pixel(int numpoints, double *matharray, int *pixarray, int axislengthinpixels) 
{
	double const_a, const_b;
	if(Scale == LOG) {
	 const_a=axislengthinpixels/(Max-Min);
	 const_b=0.5-axislengthinpixels*Min/(Max-Min);
	 for (int i=0; i<=numpoints; i++) {
	   if (matharray[i]!=0){
	   		pixarray[i]=(int)(  const_a*log10(fabs(matharray[i]))+const_b  );
	   } else pixarray[i]=0;
 	 } 
    } else { 
     const_a=axislengthinpixels/(pow10Eng*(Max-Min));
     const_b=0.5-axislengthinpixels*Min/(Max-Min);
	 for (int i=0; i<=numpoints; i++) {
	 	pixarray[i]=(int)(  const_a*matharray[i]+const_b  );
	 }	
	}
}

double CAxisData::GetMin(void)
{
	if (Scale==LOG) return pow(10.0, Min);
	else return Min*pow10Eng;
}

double CAxisData::GetMax(void)
{
	if (Scale==LOG)
		return pow(10.0, Max);
	else return Max*pow10Eng;
}
      
// returns TRUE if data is within the limits of the axes (i.e. if not off the graph)      
BOOL CAxisData::IsPointVisible(double data)
{
	double v;
	
	if (Scale==LOG) { 
		if (data==0) return FALSE; // 0 can never appear on a log scale
		else v=log10(fabs(data));
	} else v=data/pow10Eng;
		
	return (v>=Min) && (v<=Max);
}

void CAxisData::FormatLabel(CString &sLabel)
{ 
	CString str;
	int pos;
 	if ((pos=sLabel.Find('#'))>=0 && Eng<=12 && Eng>=-15) {
		if (Eng/3 ==0)	sLabel = sLabel.Left(pos) + sLabel.Right(sLabel.GetLength()-pos-1);
        else sLabel.SetAt(pos,SI_Prefix[Eng/3 + 5]);	// replace # with correct SI prefix
		}
	else if (Eng!=0)
	{
		str.Format("%0d", Eng);
		sLabel = "10^" + str + "  " + sLabel;
	}
	while ((pos=sLabel.Find('#'))>=0 && Eng<=12 && Eng>=-15) {
		if (Eng/3 ==0) 	// No prefix - so just remove the #
			sLabel = sLabel.Left(pos) + sLabel.Right(sLabel.GetLength()-pos-1);
        else   	// replace the #
			sLabel.SetAt(pos,SI_Prefix[Eng/3 + 5]);	// replace other # with same prefix
	}
	while ((pos=sLabel.Find('#'))>=0)	// remove any leftover #
		sLabel = sLabel.Left(pos) + sLabel.Right(sLabel.GetLength()-pos-1);
}

void CAxisData::SetLimits(double MinIn, double MaxIn, CScaleType sc)
{
	double temp;				// used when swapping terms
	
	Scale = sc;
	
	if(MinIn > MaxIn)	
	{   // swap terms if reversed
		temp = MaxIn;
		MaxIn = MinIn;
		MinIn = temp;
	}

	// Setup the min and max values //////////////////////////////////////
	int Eng1;
	int Eng2;

	if(Scale == LOG)
	{
		if(MaxIn==0 && MinIn==0) MaxIn = 1;
		if(MaxIn>=0 && MinIn<=0)
		{
			MaxIn = max(fabs(MaxIn), fabs(MinIn));
			MinIn = MaxIn/1e6;
		}
        MaxIn = fabs(MaxIn); MinIn = fabs(MinIn);
		if(MinIn > MaxIn)	
		{   // fabs may change the relative magnitudes of min and max
			temp = MaxIn;
			MaxIn = MinIn;
			MinIn = temp;
		}
		Max = log10(MaxIn);
		Min = log10(MinIn);
		Eng = 0; pow10Eng=1;
	}
	else	// case for linear scale
	{		// use Engineering notation for large or small numbers
		if (MaxIn==0 && MinIn==0) {MaxIn = 1; MinIn = -1;}
		if( fabs(MinIn) > 1e4 || fabs(MaxIn) > 1e4 ||
			(fabs(MinIn) < 0.1 && fabs(MaxIn) < 0.1) ) 
		{
			if(MaxIn==0)
				Eng1 = -100;
			else
				Eng1 = (int) 3*((int)floor(log10(fabs(MaxIn))/3));
			if(MinIn==0)
				Eng2 = -100;
			else
				Eng2 = (int) 3*((int)floor(log10(fabs(MinIn))/3));
				
			Eng = Eng1>Eng2 ? Eng1:Eng2;
//			if (Eng>12 || Eng<-15) Eng = 0;	// out of range for SI notation

			pow10Eng=pow(10.0, Eng);
			
//			Max=MaxIn/pow(10, Eng);
//			Min=MinIn/pow(10, Eng);
			
			Max = MaxIn/pow10Eng;   
			Min = MinIn/pow10Eng;
			
		}
		else // use normal notation
		{
			Max = MaxIn;
			Min = MinIn;
        	Eng = 0; pow10Eng=1;
		}
	}
}

void CAxisData::AutoScale(int nMaxIntervals)
{
	const int LastAllowedInterval = 23;	// zero-based index of last entry
	const double AllowedIntervals[] = {	0.001, 0.002, 0.005, 0.01 , 0.02, 0.05,
									0.1	 , 0.2  , 0.5  , 1.0  , 2.0,
									5.0  , 10.0 , 20.0 , 25.0 , 50.0,
									100  , 200  , 250  , 500  , 1000,
									2000 , 2500 , 5000};

	const int DecimalIntervals[] = {	3, 3, 3, 2, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0,
								0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	const int DivisionsPerInterval[] = {	5, 4, 5, 5, 4, 5, 5, 4, 5, 5, 4, 5,
									5, 4, 5, 5, 5, 4, 5, 5, 5, 4, 5, 5};

    // this takes a linearised min/max and calculates the best number of
    // intervals and divisions.
	// Interval widths can only be those specifed in AllowedIntervals[]

	double dwInter;		// size of the interval in data units
	double temp, ratio;
	int n;
	double kmin, kmax;

	// Check min/max /////////////////////////////////////////////////////
	if(Min == 0 && Max == 0)	
	{
		Max = 1.0;
		Min = -1.0;
		Scale = LINEAR;
	}
	if (fabs(Max-Min)<0.002*fabs(Max))
	{
		Min=0.999*Min;
		Max=1.001*Max;
	}
	if(Min > Max)	
	{	// above adjustment may necessitate a swap
		temp = Min;
		Min = Max;
		Max = temp;
	}

	// Get the width for an interval given a desired number of Intervals
	dwInter = (Max - Min)/nMaxIntervals;

	// Find the closest allowed interval width ///////////////////////////
	n = 0;    
	if (Scale == LOG)	
	{
		while(AllowedIntervals[n] < 1.0) n++;
	}	// makes sure that lowest interval for a log scale is 1
	while(dwInter > AllowedIntervals[n] && n<LastAllowedInterval) n++;

	// Find the min/max to use
	ratio = Max/AllowedIntervals[n];
	kmax=floor(ratio); if (kmax<0) kmax++;	
	if (Max>0 && ratio-kmax>0.01) kmax++;
	if (Max<0 && kmax-ratio>0.99) kmax--;
	ratio = Min/AllowedIntervals[n];
	kmin=floor(ratio); if (kmin<0) kmin++;
	if (Min<0 && kmin-ratio>0.01) kmin--;
	if (Min>0 && ratio-kmin>0.99) kmin++;

    // Set the number of intervals involved
	Int = (int) (kmax - kmin);
	if (Int==0) {kmax++; kmin--; Int=2;}
	
    Max = kmax*AllowedIntervals[n];
	Min = kmin*AllowedIntervals[n];

	if(Scale == LINEAR)	
	{
		Div = DivisionsPerInterval[n];
		Dec = DecimalIntervals[n];
	}
	else 
	{
		if (AllowedIntervals[n] == 1.0) Div = 10; else Div = 0;
		Dec = 0;	// irrelevant initialization
	}
}	// AutoScale


// returns axis val for the nth major tic.
double CAxisData::GetDataValueForTic(int n) 
{
	double v = Min + n*GetIntervalSize();
	if (Scale == LOG) v=pow(10.0,v);
	else  v*=pow10Eng;
	return v;
}

// returns position along axis (in pixels) for the nth major tic
int CAxisData::GetPixelPosForTic(int n, unsigned int lengthinpixels)
{
	// trunc(n*lengthinpixels/Int + 0.5)
	return (int)( (2L*n*lengthinpixels+Int)/(2L*Int) );
}

// GetPixelPosForShortTic
// for linear, nminor=1..NumIntervals-1; for log, nminor=2..9
// Equation for log: x=(lengthinpixels/Int)*( nmajor + log10(nminor) );
//	            linear: x=nmajor*Div+nminor/Int*Div;
// *** FAST, CLEVER, DIRTY CODE ***
// The original code for this function used floating point to calculate the xvalue at the tic, 
// then converted this to client coords. This code is 100% integers - F A S T !
// DIRTY BIT OF CODE FOR LOG SCALE: It's not obvious, but the floating point code can be
// eliminated from the log tics as well. Floating point logs are very slow.
// Use rational approximations to the logs of
// integers. log10(n) ~ log10num[n]/log10denom[n] for n=2,3,..9.
// eg log 2=0.301029995, 12655/42039=0.301029996. Values were obtained by continued fractions.
// It's necessary to round all values to the nearest integer.
// for x>0, xint=(int) (x+0.5); for x<0, xint=(int) (x-0.5); For tics, x>0 always.
// Using integer math, the integer closest to a/b is (int)(a/b+0.5) = (int)( (2a+b)/2b )
int CAxisData::GetPixelPosForShortTic(int nmajor, int nminor, unsigned int lengthinpixels)
{
	static const unsigned log10numerator[10]=  {0,0, 12655,  8519, 33961, 29384, 463, 431, 12655, 17038};
	static const unsigned log10denominator[10]={1,1, 42039, 17855, 56408, 42039, 595, 510, 14013, 17855};
	unsigned long a, b;
	if (Scale == LOG) {
//		return (int) (lengthinpixels*log10(nminor)/Int+(lengthinpixels*(double)nmajor/Int)+0.5);
		a=(unsigned long)log10denominator[nminor]*nmajor+(unsigned long)log10numerator[nminor];
		b=(unsigned long)Int*log10denominator[nminor];
		return (int)( (2L*lengthinpixels*a+b)/(2L*b) );
	}
	else return (int)( (2L*lengthinpixels*(nmajor*Div+nminor)+(long)Int*Div)/(2L*Int*Div) );
};			


CString CAxisData::GetTextForTic(int ticnum)
{
	// Calculate the format of the numbers to put on the axes
	// x is the number to be formatted	
	double x = Min + ticnum*GetIntervalSize();
	
	// formats the number on the basis of the Scale and Dec                                     
	CString buffer;

	if(Scale == LOG)	
	{
		x=pow(10.0, x);
		buffer.Format("%.0e",x);
		int n = buffer.GetLength()-3;	// characters to left of one to be removed
		buffer = buffer.Left(n) + buffer.Right(2);	// drop one 0 from exponent
	}
	else buffer.Format("%.*f", Dec, x);
	return buffer;
}
                                                      
// for linear, nminor=1..NumIntervals-1; for log, nminor=2..9                                                      
double CAxisData::GetDataValueForShortTic(int nmajor, int nminor)
{
	double v = Min + nmajor*GetIntervalSize();
	if (Scale == LOG) return nminor*pow(10.0,v);
	else return pow10Eng*( 
			 v + nminor*GetIntervalSize()/GetNumDivisions()  );
};			

// only true if they are identical - check everything
BOOL operator ==(CAxisData &first, CAxisData &second)
{
	if (first.Min!=second.Min) return FALSE;
	if (first.Max!=second.Max) return FALSE;
	if (first.Int!=second.Int) return FALSE;
	if (first.Div!=second.Div) return FALSE;
	if (first.Eng!=second.Eng) return FALSE;
	if (first.Scale!=second.Scale) return FALSE;
	return TRUE;
}





/////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------
// CPlot2D
//-----------------------------------------------------------------------

#ifdef _DEBUG
void CPlot2D::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
	// variables to be displayed in DEBUG window
	//dc << m_GraphBox << m_GraphBorder;
}
#endif

/////////////////////////////////////////////////////////////////////////////
// Static variables

//CFont CPlot2D::Font;
//CFont CPlot2D::Font90;
//TEXTMETRIC CPlot2D::tm;
//TEXTMETRIC CPlot2D::tm90;

/////////////////////////////////////////////////////////////////////////////
// Constructor

CPlot2D::CPlot2D(CDC* pDC, CView* pView)
{
	CPlot2D::pDC = pDC;
	pDC->SetMapMode(MM_TEXT);
	CRect ClientRect(0,0,0,0);
	if (pDC->IsPrinting())
	{
		ClientRect.right = pDC->GetDeviceCaps(HORZRES);
		ClientRect.bottom = pDC->GetDeviceCaps(VERTRES);
	}
	else 
		pView->GetClientRect(&ClientRect);
	m_Extent = CSize(ClientRect.Width(), ClientRect.Height());
	pDC->SetBkMode(TRANSPARENT);

	InitializeFonts(pDC, "Arial", min(ClientRect.Width(), ClientRect.Height()));

/***
	static BOOL bScreenFontInitialized=FALSE;
	if (!pDC->IsPrinting() && !bScreenFontInitialized) {
		InitializeFonts(pDC, "Arial", ClientRect.Height());
		bScreenFontInitialized=TRUE;
	}
	if (pDC->IsPrinting())
	{
		InitializeFonts(pDC, "Arial", ClientRect.Height());
		bScreenFontInitialized=FALSE;
	}
***/

	pDC->SelectObject(Font);

/****
	
	// Set font
	int fontheight;
	if (pDC->IsPrinting())
		fontheight = max(pDC->GetDeviceCaps(VERTRES)/60,
						 pDC->GetDeviceCaps(HORZRES)/60);
	else
		fontheight = pDC->GetDeviceCaps(VERTRES)/35;
	Font.CreateFont(fontheight,0,0,0,FW_NORMAL,0,0,0,0,OUT_TT_PRECIS,
		CLIP_TT_ALWAYS, PROOF_QUALITY, VARIABLE_PITCH|TMPF_TRUETYPE|FF_SWISS, 
		"Arial");	
	Font90.CreateFont(fontheight,0,900,900,FW_NORMAL,0,0,0,0,OUT_TT_PRECIS,
		CLIP_TT_ALWAYS, PROOF_QUALITY, VARIABLE_PITCH|TMPF_TRUETYPE|FF_SWISS, 
		"Arial");
	m_Font=&Font;
	m_Font90=&Font90;
	pDC->SelectObject(m_Font90);
    pDC->GetTextMetrics(&tm90);		// get info about rotated font
	pDC->SelectObject(m_Font);
	pDC->GetTextMetrics(&tm);		// get info about normal font
***/
}

void CPlot2D::InitializeFonts(CDC *pDC, CString fontname, int maxgraphheight)
{
	int fontheight;
	fontheight = 12 + maxgraphheight/70;
/****
	int LogUnitsY=pDC->GetDeviceCaps(LOGPIXELSY);
	int FontHgt_Twips=20*16; // 16 point numbers

	fontheight = MulDiv(FontHgt_Twips,	 LogUnitsY,  72*20);
***/
/***
	if (pDC->IsPrinting())
		fontheight = max(pDC->GetDeviceCaps(VERTRES)/60,
						 pDC->GetDeviceCaps(HORZRES)/60);
	else
		fontheight = pDC->GetDeviceCaps(VERTRES)/35;
***/

	pDC->SelectStockObject(SYSTEM_FONT); // just in case Font90 is in use..

	Font.CreateFont(fontheight,0,0,0,FW_NORMAL,0,0,0,0,OUT_TT_PRECIS,
		CLIP_TT_ALWAYS, PROOF_QUALITY, VARIABLE_PITCH|TMPF_TRUETYPE|FF_SWISS, 
		"Arial");	
	Font90.CreateFont(fontheight,0,900,900,FW_NORMAL,0,0,0,0,OUT_TT_PRECIS,
		CLIP_TT_ALWAYS, PROOF_QUALITY, VARIABLE_PITCH|TMPF_TRUETYPE|FF_SWISS, 
		"Arial");
	pDC->SelectObject(&Font90);
    pDC->GetTextMetrics(&tm90);		// get info about rotated font
	pDC->SelectObject(&Font);
	pDC->GetTextMetrics(&tm);		// get info about normal font
	CSize sz=pDC->GetTextExtent("0",1);
	m_FontWidthOfDigit=sz.cx;
}

/////////////////////////////////////////////////////////////////////////////
// Destructor

CPlot2D::~CPlot2D()
{
	pDC->SelectStockObject(SYSTEM_FONT);
	Font.DeleteObject();
	Font90.DeleteObject();
}	


///////////////////////////////////////////////////////////////////////////
// Public Functions
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////

// plots using integer arrays which have been preconverted.
// Uses pDC->PolyLine() which is a MASSIVE speed improvement over LineTo()
void CPlot2D::IntegerPlotLine(int numpoints, int *XIntArray, int *YIntArray, 
						COLORREF LineColor, int nPenStyle, int nPenWidth)
{
	int i; 
	CPen Pen;
	Pen.CreatePen(nPenStyle, nPenWidth, LineColor);
	CPen* pOldPen = pDC->SelectObject(&Pen);
	CBrush Brush(LineColor);
	CBrush* pOldBrush = pDC->SelectObject(&Brush);
	CRgn Rgn;
	Rgn.CreateRectRgn(	m_GraphBox.left,
						m_GraphBox.top,
						m_GraphBox.right+1,
						m_GraphBox.bottom+1);
	pDC->SelectClipRgn(&Rgn);	// limit access to plotting area

	CPoint pt;
	const int delta = 4;	// half-width of point locator

	
	pt.x=XIntArray[0]+m_GraphBox.left;
	pt.y=m_GraphBox.bottom-YIntArray[0];
	pDC->MoveTo(pt);
	
	if (numpoints==0)
	{	// put a point locator
		pDC->Ellipse(pt.x-delta, pt.y-delta, pt.x+delta+1, pt.y+delta+1);
	}
	else { // draw a line to connect the remaining points
		int *pointarray=new int[2*numpoints+2];
		for (i=0; i<=numpoints; i++){
			pointarray[2*i]=XIntArray[i]+m_GraphBox.left;
			pointarray[2*i+1]=m_GraphBox.bottom-YIntArray[i];
		}
		pDC->Polyline((CPoint *)pointarray, numpoints+1);
		delete pointarray;
	}
	pDC->SelectClipRgn(NULL);	// restore access to full client window
	Rgn.DeleteObject();			// release memory used for region definition
	pDC->SelectObject(pOldBrush);
	Brush.DeleteObject();
	pDC->SelectObject(pOldPen);
	Pen.DeleteObject();
}

void CPlot2D::MakeXIntArray(int nStartIndex, int nStopIndex, double *DataX, int *XIntArray)
{
	m_x.matharray2pixel(nStopIndex-nStartIndex, DataX+nStartIndex, XIntArray, m_GraphBox.Width());
}

void CPlot2D::MakeYIntArray(int nStartIndex, int nStopIndex, double *DataY, int *YIntArray)
{	
	m_y.matharray2pixel(nStopIndex-nStartIndex, DataY+nStartIndex, YIntArray, m_GraphBox.Height());
}	


void CPlot2D::IntegerPlotPoints(int numpoints, int *XIntArray, int *YIntArray, 
						COLORREF PointColor)
{
	CPen Pen(PS_SOLID,1,PointColor);
	CPen* pOldPen = pDC->SelectObject(&Pen);
	CRgn Rgn;
	Rgn.CreateRectRgn(	m_GraphBox.left,
						m_GraphBox.top,
						m_GraphBox.right+1,
						m_GraphBox.bottom+1);
	pDC->SelectClipRgn(&Rgn);	// limit access to plotting area

	CPoint pt;
		
	if (numpoints==0)
	{	    // not necessary to show the point
	}else for(int i=0; i<=numpoints; i++)
	{	// draw a line to connect the remaining points
		pt.x=XIntArray[i]+m_GraphBox.left;
		pt.y=m_GraphBox.bottom-YIntArray[i];
		pDC->SetPixel(pt, RGB(255,255,255));  
		pDC->SetPixel(pt.x, pt.y+1, RGB(255,255,255));
		pDC->SetPixel(pt.x, pt.y-1, RGB(255,255,255));
	}

	pDC->SelectClipRgn(NULL);	// restore access to full client window
	Rgn.DeleteObject();			// release memory used for region definition
	pDC->SelectObject(pOldPen);
	Pen.DeleteObject();
}



/////////////////////////////////////////////////////////////////////////////

int CPlot2D::xmath2client(double data)
{
	return m_GraphBox.left+m_x.math2pixel(data, m_GraphBox.Width());
}

int CPlot2D::ymath2client(double data)
{
	return m_GraphBox.bottom - m_y.math2pixel(data, m_GraphBox.Height());
}


/////////////////////////////////////////////////////////////////////////////
void CPlot2D::LocateGraph(	PercentScreen left, PercentScreen right,
							PercentScreen bottom, PercentScreen top,
							COLORREF BorderGraphColor, COLORREF GraphColor, 
							COLORREF TicColor, COLORREF LabelColor)
{
	// Set colors
	m_GraphBorderColor = BorderGraphColor;
	m_GraphBackGroundColor = GraphColor;
	m_TicColor = TicColor;
	m_LabelColor = LabelColor;

	// Set main rectangle size
	m_GraphBorder.SetRect(	(int)(m_Extent.cx*left/100),
							(int)(m_Extent.cy*(100-top)/100),
							(int)(m_Extent.cx*right/100),
							(int)(m_Extent.cy*(100-bottom)/100));
}


/////////////////////////////////////////////////////////////////////////////
                        
// Calculates where graph & axes should go, and what axes limits should be,
// but doesn't actually draw anything.
void CPlot2D::CalculateAxes(	double dwXMinIn, double dwXMaxIn, CScaleType XScale,
					double dwYMinIn, double dwYMaxIn, CScaleType YScale, BOOL bTitle)
{
    // Get position for the graph box
	LocateGraphAxes(bTitle);	
	
	// Y AXIS ///////////////
	m_y.SetLimits(dwYMinIn, dwYMaxIn, YScale);

	// Calc max possible y intervals
	int nyMaxIntervals;			// used to get best number of intervals
	nyMaxIntervals = (int) (m_GraphBox.Height()/(1.5*tm.tmHeight));
    if(nyMaxIntervals < 2) nyMaxIntervals = 2;
	m_y.AutoScale(nyMaxIntervals);  // calc best number of intervals
                                    
    // X AXIS /////////////
	m_x.SetLimits(dwXMinIn, dwXMaxIn, XScale);

	// Calc max possible x intervals	
	int nxMaxIntervals;			// used to get best number of intervals
	
// Mod: DAC 8/5/96. It's better to use the width of a '0' (the widest number) rather than the average
// width of the font. (in Arial font, many chars are narrower than the digits).
	
//	nxMaxIntervals = (int) (m_GraphBox.Width()/(1.5*MaxStrLen*1.5*tm.tmAveCharWidth));
	nxMaxIntervals = (int) (m_GraphBox.Width()/(1.5*MaxStrLen*m_FontWidthOfDigit));
	
    if(nxMaxIntervals < 1) nxMaxIntervals = 1;
	m_x.AutoScale(nxMaxIntervals);

}

void CPlot2D::DrawTitleAndBackground(CString sTitle)
{
	// Draw screen colors and box ////////////////////////////////////////
	CPen Pen(PS_SOLID,1,m_TicColor);
	CPen* pOldPen = pDC->SelectObject(&Pen);
	
	CBrush BrushBorderBk(m_GraphBorderColor);

	CBrush* pOldBrush = pDC->SelectObject(&BrushBorderBk);
	
//  DAC 30/1/96: "Flickerless" draw of background...
// ...fill the background, except for the Graphbox (we don't want it drawn twice) 
// ...this also saves some time
//		BBBBBBB
//      AA****D   A,B,C,D,E are the rectangles to be filled with background colr *=the graph
//		AA****D
//      EECCCCD
// old version was the following line:
//		pDC->Rectangle(	m_GraphBorder );			// draw border rectangle and fill
// B,D,E only drawn for title. (hardly ever changes).
// A for Xaxis, C for Yaxis
	pDC->SelectStockObject(NULL_PEN);
	pDC->Rectangle(m_GraphBorder.left+1, m_GraphBorder.top+1, m_GraphBorder.right, m_GraphBox.top+1);	
	pDC->Rectangle(m_GraphBox.right, m_GraphBox.top, m_GraphBorder.right, m_GraphBorder.bottom);	
	pDC->Rectangle(m_GraphBorder.left+1, m_GraphBox.bottom, m_GraphBox.left+1, m_GraphBorder.bottom);
	
	pDC->SelectObject(&Pen);
	pDC->SelectStockObject(NULL_BRUSH);
	pDC->Rectangle(m_GraphBorder); // draw border
		
	pDC->SelectObject(pOldBrush);
	BrushBorderBk.DeleteObject();
	pDC->SelectObject(pOldPen);
	Pen.DeleteObject();

	// Print title ///////////////////////////////////////////////////////
	if(!sTitle.IsEmpty()) PrintTitle(sTitle);

}
                                        
// draws everything except the title area - i.e. the parts which may change
// if bXAxisChanged is FALSE, doesn't redraw xaxis label & tic numbers. ditto for y
void CPlot2D::DrawAxesAndGraphArea(CString sXAxisLabel, CString sYAxisLabel, BOOL bXAxisChanged, BOOL bYAxisChanged)
{
	CPen Pen(PS_SOLID,1,m_TicColor);
	CPen* pOldPen = pDC->SelectObject(&Pen);
	
	CBrush BrushBorderBk(m_GraphBorderColor);
	CBrush BrushGraphBk(m_GraphBackGroundColor);

	CBrush* pOldBrush = pDC->SelectObject(&BrushBorderBk);
	
	pDC->SelectStockObject(NULL_PEN);

	if (bYAxisChanged) 
	 	pDC->Rectangle(m_GraphBorder.left+1, m_GraphBox.top-1, m_GraphBox.left+1, m_GraphBox.bottom+1);
	if (bXAxisChanged)
	 	pDC->Rectangle(m_GraphBox.left, m_GraphBox.bottom, m_GraphBox.right+1, m_GraphBorder.bottom);	
	pDC->SelectObject(&Pen);
	
	if ( (bYAxisChanged || bXAxisChanged) && pDC->IsPrinting()) {
		// avoid printing problems - where border of graph is incomplete
		pDC->SelectStockObject(NULL_BRUSH);
		pDC->Rectangle(m_GraphBorder); // draw border
	}
	
	pDC->SelectObject(&BrushGraphBk);
	pDC->Rectangle(	m_GraphBox.left,
					m_GraphBox.top,
					m_GraphBox.right+1,
					m_GraphBox.bottom+1);	// draw Graph box and set box color
	pDC->SelectObject(pOldBrush);
	BrushBorderBk.DeleteObject();
	BrushGraphBk.DeleteObject();
	pDC->SelectObject(pOldPen);
	Pen.DeleteObject();
	
	YTics(bYAxisChanged); 						// plot the axes units and tics
	if (bYAxisChanged)	PrintYLabel(sYAxisLabel);		// draw labels
	XTics(bXAxisChanged);
	if (bXAxisChanged) PrintXLabel(sXAxisLabel);

}

/////////////////////////////////////////////////////////////////////////////
// Protected Functions
/////////////////////////////////////////////////////////////////////////////

void CPlot2D::LocateGraphAxes(BOOL bTitle)
{
	int left, top, right, bottom;
	left = (MaxStrLen+1)*tm.tmAveCharWidth + 2*tm90.tmHeight;
	right = tm.tmMaxCharWidth;
	if (bTitle) top = tm.tmHeight + tm.tmHeight/2;
	else top = tm.tmHeight/4;
	bottom = 2*tm.tmHeight + tm.tmHeight/2;
	m_GraphBox.SetRect(	m_GraphBorder.left + left,
						m_GraphBorder.top + top,
						m_GraphBorder.right - right,
						m_GraphBorder.bottom - bottom);
}

/////////////////////////////////////////////////////////////////////////////

void CPlot2D::PrintTitle(CString sTitle)
{
	int CentreLine;

	if(sTitle.GetLength()*tm.tmAveCharWidth > m_GraphBox.Width())	// long title
		CentreLine = m_GraphBorder.left + m_GraphBorder.Width()/2;
	else
		CentreLine = m_GraphBox.left + m_GraphBox.Width()/2;
		
	pDC->SetTextColor(m_LabelColor);
	pDC->SetTextAlign(TA_CENTER | TA_TOP);
	pDC->TextOut(CentreLine, m_GraphBorder.top + tm.tmHeight/4,
				sTitle, sTitle.GetLength());
}	// PrintTitle


/////////////////////////////////////////////////////////////////////////////
void CPlot2D::PrintXLabel(CString sLabel)
{
	int CentreLine;
	
	m_x.FormatLabel(sLabel);
	
	if( sLabel.GetLength()*tm.tmAveCharWidth > m_GraphBox.Width() )	// long label
		CentreLine = m_GraphBorder.left + m_GraphBorder.Width()/2;
	else
		CentreLine = m_GraphBox.left + m_GraphBox.Width()/2;
    
	pDC->SetTextColor(m_LabelColor);
	pDC->SetTextAlign(TA_CENTER | TA_TOP);
	pDC->TextOut( CentreLine, m_GraphBox.bottom + tm.tmHeight + tm.tmHeight/4,
				sLabel, sLabel.GetLength());
}

/////////////////////////////////////////////////////////////////////////////
void CPlot2D::PrintYLabel(CString sLabel)
{
	int CentreColumn;
	
	m_y.FormatLabel(sLabel);
	
	if( sLabel.GetLength()*tm90.tmAveCharWidth > m_GraphBox.Height() )
		CentreColumn = m_GraphBorder.top + m_GraphBorder.Height()/2;
	else
		CentreColumn = m_GraphBox.top + m_GraphBox.Height()/2;
	
	pDC->SetTextColor(m_LabelColor);
	pDC->SelectObject(&Font90);	
	pDC->SetTextAlign(TA_CENTER | TA_TOP);
	pDC->TextOut(m_GraphBorder.left + tm90.tmHeight/4, CentreColumn,
				sLabel, sLabel.GetLength());
	pDC->SelectObject(&Font);
}

/////////////////////////////////////////////////////////////////////////////

void CPlot2D::XTics(BOOL bNeedNumbers)
{
	// independent variables
	int	TicLength, ShortTicLength;
	int	yp, TxtXp, xp;
	int y;
	int i, j, k, k1;
	CString sNumber;

	DWORD TwoArray[100];
	CPoint points[100];

	// Preliminary routines
	ShortTicLength = (int)(m_FontWidthOfDigit*0.6);
	TicLength = 2 * ShortTicLength;
        
	CPen Pen(PS_SOLID,1,m_TicColor);
	CPen* pOldPen = pDC->SelectObject(&Pen);
	pDC->SetTextColor(m_TicColor);

	yp = m_GraphBox.bottom + tm.tmHeight/4;	// y position for numbers

	for(i=0; i<=m_x.GetNumIntervals(); i++)	
	{
		xp=m_GraphBox.left+m_x.GetPixelPosForTic(i, m_GraphBox.Width());
		if (bNeedNumbers) {
		
			sNumber=m_x.GetTextForTic(i);
		              
			// Plot numbers on axes
			TxtXp = xp;
			if (i == 0)
				pDC->SetTextAlign(TA_LEFT | TA_TOP);
			else if (i == m_x.GetNumIntervals())
				pDC->SetTextAlign(TA_RIGHT | TA_TOP);
			else
  				pDC->SetTextAlign(TA_CENTER | TA_TOP);

			pDC->TextOut(TxtXp,yp,sNumber,sNumber.GetLength());
		}

		if (xp==xmath2client(0) && i!=0 && i!=m_x.GetNumIntervals())
		{   // plot x=0 line
			pDC->MoveTo(xp, m_GraphBox.bottom);
			pDC->LineTo(xp, m_GraphBox.top);
		}

		// Plot tic for major interval ///////////////////////////////////
		if(i>0 && i<m_x.GetNumIntervals())	
		{
			pDC->MoveTo(xp, m_GraphBox.bottom);
			pDC->LineTo(xp, m_GraphBox.bottom - TicLength);
			pDC->MoveTo(xp, m_GraphBox.top);
			pDC->LineTo(xp, m_GraphBox.top + TicLength);
		}

		// Plotting tics for the interval divisions //////////////////////
				// Plotting tics for the minor intervals /////////////////////////
		k=0;
		k1=0;
		int first;
		// plot a  logarithmic scale
		if( m_x.GetScale() == LOG && i < m_x.GetNumIntervals() && m_x.GetNumDivisions()==10)
		{ 
			first=2; 
		} else 	if( (m_x.GetScale() == LINEAR) && (i < m_x.GetNumIntervals()) )
		{
			first=1;
		} else first=-1;
		if (first!=-1) {
			for(j=first; j<m_x.GetNumDivisions(); j++)	{
				y=m_GraphBox.left+m_x.GetPixelPosForShortTic(i, j, m_GraphBox.Width());

				points[k].y=m_GraphBox.bottom;
				points[k+1].y=m_GraphBox.bottom - ShortTicLength;
				points[k+2].y=m_GraphBox.top;
				points[k+3].y=m_GraphBox.top + ShortTicLength;
				points[k].x=y;
				points[k+1].x=y;
				points[k+2].x=y;
				points[k+3].x=y;
				TwoArray[k1]=2;
				TwoArray[k1+1]=2;
				k1+=2;
				k+=4;
			}
			pDC->PolyPolyline(points, TwoArray, 2*(m_x.GetNumDivisions()-first));
		}
	}
	pDC->SelectObject(pOldPen);
	Pen.DeleteObject();
}	// XTics

/////////////////////////////////////////////////////////////////////////////
void CPlot2D::YTics(BOOL bNeedNumbers)
{
	// independent variables
	int	TicLength, ShortTicLength;
	int xp, yp, TxtYp;
	int y;
	int i,j, k, k1;
	CString sNumber;
	CSize StrSize;

	DWORD TwoArray[100];
	CPoint points[100];

	// Preliminary routines
	ShortTicLength = (int)(m_FontWidthOfDigit*0.6);
	TicLength = 2 * ShortTicLength;

	CPen Pen(PS_SOLID,1,m_TicColor);
	CPen* pOldPen = pDC->SelectObject(&Pen);

	pDC->SetTextColor(m_TicColor);

	yp = m_GraphBox.bottom + tm.tmHeight/4;	// y position for numbers

	for(i=0; i<=m_y.GetNumIntervals(); i++)	
	{
		yp=m_GraphBox.bottom-m_y.GetPixelPosForTic(i, m_GraphBox.Height());
 		xp = m_GraphBox.left - (int)(0.5*tm.tmAveCharWidth);	// x position for numbers

		if (bNeedNumbers) {
			sNumber=m_y.GetTextForTic(i);

			// Plot numbers on axes
			if(i == 0)
			{
				pDC->SetTextAlign(TA_RIGHT | TA_BASELINE);
				TxtYp = yp;
			}
			else if (i == m_y.GetNumIntervals())
			{
				pDC->SetTextAlign(TA_RIGHT | TA_TOP);
				TxtYp = yp - tm.tmHeight/4;
			}
			else
			{
				pDC->SetTextAlign(TA_RIGHT | TA_TOP);
				TxtYp = yp - tm.tmHeight/2;
			}

			pDC->TextOut(xp,TxtYp,sNumber,sNumber.GetLength());
		}

		if (yp==ymath2client(0) && i!=0 && i!=m_y.GetNumIntervals())
		{   // plot line for y=0
			pDC->MoveTo(m_GraphBox.left, yp);
			pDC->LineTo(m_GraphBox.right, yp);
		}

		// Plot tic for major interval ///////////////////////////////////
		if(i > 0 && i < m_y.GetNumIntervals())	
		{
			pDC->MoveTo(m_GraphBox.left, yp); 
			pDC->LineTo(m_GraphBox.left + TicLength, yp);
			pDC->MoveTo(m_GraphBox.right, yp);
			pDC->LineTo(m_GraphBox.right - TicLength, yp);
		}


		// Plotting tics for the minor intervals /////////////////////////
		k=0; k1=0;
		int first;
		// plot a  logarithmic scale
		if( m_y.GetScale() == LOG && i < m_y.GetNumIntervals() && m_y.GetNumDivisions()==10)
		{ 
			first=2; 
		} else 	if( (m_y.GetScale() == LINEAR) && (i < m_y.GetNumIntervals()) )
		{
			first=1;
		} else first=-1;
		if (first!=-1) {
			for(j=first; j<m_y.GetNumDivisions(); j++)	{
				y=m_GraphBox.bottom-m_y.GetPixelPosForShortTic(i, j, m_GraphBox.Height());

				points[k].x=m_GraphBox.left;
				points[k+1].x=m_GraphBox.left + ShortTicLength;
				points[k+2].x=m_GraphBox.right;
				points[k+3].x=m_GraphBox.right - ShortTicLength;
				points[k].y=y;
				points[k+1].y=y;
				points[k+2].y=y;
				points[k+3].y=y;
				TwoArray[k1]=2;
				TwoArray[k1+1]=2;
				k+=4;
				k1+=2;
			}
			pDC->PolyPolyline(points, TwoArray, 2*(m_y.GetNumDivisions()-first));
		}
	}
	pDC->SelectObject(pOldPen);
	Pen.DeleteObject();
}	// YTics
