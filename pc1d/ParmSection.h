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
/////////////////////////////////////////////////////////////////////////////
// CParamSection - text for one section of parameters

#define PARMSECTION_H 1

// The text part of parameter view

CString Num4DecPlaces(double val);
CString Max4DecPlaces(double val);

// coordinate conversions
int Twips2Pixels(int twips);
int Lomets2Pixels(int lomets);
int Twips2LogicalDistance(int twips);
int Lomets2LogicalDistance(int lomets);
             
// a group of parameters arranged in a logical unit ( has a title to be displayed in red)             
class CParamSection : public CObject
{
protected:
	CStringArray m_rowlist; // which row on screen
	CWordArray m_commandlist; // WM_COMMAND index to be sent when line is dbl-clicked on
	static int m_fontheight;
	static int m_fontwidth;
	static CString m_ParmFontName;		// font used for parameter display
	static int m_ParmFontHgt;			// height in twips

	CRect m_bbox;	// bounding box of screen position
public:
	CParamSection();
	static void SetFontForAllSections(CString fontname, int TwipsHgt);
	static BOOL OnSelectFont();
	static void CalculateMetrics(CDC *pDC);
	void Draw(CDC *pDC, int xorigin, int yorigin, int firstrow, int numrows);
	void DrawIfVisible(CDC *pDC); // draws at m_bbox location
	int GetHeight(CDC *pDC=NULL); // returns height (in pixels) which entire section requires when displayed
	int GetWidth(CDC *pDC=NULL);  // ditto for width
	CRect GetBoundingBox(void) { return m_bbox; };
	CRect GetClientBBox(void); // return a bounding box giving dimensions of the client (non-title) area of the section
	void SetBBoxTopBottom(int top, int bottom) { m_bbox.top=top; m_bbox.bottom=bottom; };
	void SetBBoxLeftRight(int left, int right) { m_bbox.left=left; m_bbox.right=right; };
	static int GetFontHeight() { return m_fontheight; };
	static int GetFontWidth() { return m_fontwidth; };
	int GetNumberOfRows(void); // returns number of rows to be displayed
protected:	
//	static int CalculateFontWidth(CDC *pDC);
//	static int CalculateFontHeight(CDC *pDC);
	void ResetRows(void);
	// add this row to the end, showing value if necessary
	void RowOut(CString intro, double val, CString outro, WORD msg=0);
	void RowOut(CString intro, int val, CString outro, WORD msg=0);
	void RowOut(CString str, WORD msg=0);
public:	
	virtual void OnDoubleClick(CPc1dDoc *pDoc,int x, int y);		
};

class CFileSection : public CParamSection
{
public:
	void Update(CPc1dDoc *pDoc);
};

class CDeviceSection : public CParamSection
{
public:
	void Update(CPc1dDoc *pDoc);
};

class CExcitationSection : public CParamSection
{
public:
	void Update(CPc1dDoc *pDoc);
};

class CResultsSection : public CParamSection
{
public:
	void Update(CPc1dDoc *pDoc);
};	

class CBatchSection : public CParamSection
{
public:
	void Update(CPc1dDoc *pDoc);
	void UpdateCurrentSim(CPc1dDoc *pDoc);
};	

class CRegionSection : public CParamSection
{
	int m_regionnumber;  
public:	
	void SetRegionNumber(int n) { m_regionnumber=n; };
	void Update(CPc1dDoc *pDoc);
	void OnClick(CPc1dDoc *pDoc, int xpos, int ypos);
};
