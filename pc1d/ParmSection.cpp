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

// Format of strings in calls to RowOut:
// $i in a string = switch to italic font.
// $b = switch to bold, red font.
// $g = switch to "greek" font (symbol) 
// $n = switch back to normal
// $^ = superscript  (until $n or $g)
// $_ = subscript    (until $n or $g)
// $$ = $

#include "stdafx.h"
#include "hints.h"
#include "math.h"
#include "mathstat.h" 	// CMath
#include "pc1ddoc.h"
#include "parmview.h"
#include "useful.h"


int m_LOGPIXELSY;
int	m_LOGPIXELSX;
int Twips2Pixels(int twips)
{
	return MulDiv(twips, m_LOGPIXELSY, 72*20);
}

int Lomets2Pixels(int lomets)
{
	return MulDiv(lomets, m_LOGPIXELSY, 254);  // lomets 2 pixels
}

int Twips2LogicalDistance(int twips) 
{ return Twips2Pixels(twips); }

int Lomets2LogicalDistance(int lomets)
{ return Lomets2Pixels(lomets); }

/////////////////////////////////////////////////////////////////
// Globals

int CParamSection::m_fontheight=0;
int CParamSection::m_fontwidth=0;

CString CParamSection::m_ParmFontName;		// font used for parameter display
int CParamSection::m_ParmFontHgt;			// height in twips
LOGFONT m_logfont;
LOGFONT m_GreekLogfont;
CPoint m_superscriptoffset, m_subscriptoffset;
CPoint m_superscriptsize, m_subscriptsize;
CPoint m_greekoffset;



void CParamSection::SetFontForAllSections(CString fontname, int TwipsHgt)
{
	m_ParmFontHgt=TwipsHgt;
	m_ParmFontName=fontname;
	CalculateMetrics(NULL);
}

BOOL CParamSection::OnSelectFont()
{
	m_logfont.lfHeight=-abs(Twips2Pixels(m_ParmFontHgt));
	CFontDialog dlg(&m_logfont, CF_SCREENFONTS);
	if (dlg.DoModal()==IDOK) {
		m_ParmFontHgt=dlg.GetSize()*2;
		m_ParmFontName=dlg.GetFaceName();
	    AfxGetApp()->WriteProfileString("Parameter View", "Font Name", m_ParmFontName);
		AfxGetApp()->WriteProfileInt("Parameter View", "Font Size", m_ParmFontHgt); // font size in twips
		CalculateMetrics(NULL);
		return TRUE;
	}
	return FALSE;
}

void CParamSection::CalculateMetrics(CDC *pDC)
{
	BOOL bIsScreen= FALSE;
	if (pDC==NULL) {
		pDC=new CWindowDC(NULL);
		bIsScreen=TRUE;
	}

	m_LOGPIXELSY=pDC->GetDeviceCaps(LOGPIXELSY);
	m_LOGPIXELSX=pDC->GetDeviceCaps(LOGPIXELSX);

	m_logfont.lfHeight=-Twips2LogicalDistance(m_ParmFontHgt);
	m_logfont.lfWidth=0;
	m_logfont.lfEscapement=0;
	m_logfont.lfOrientation=0;
	m_logfont.lfWeight=FW_REGULAR;
	m_logfont.lfItalic=FALSE;
	m_logfont.lfUnderline=FALSE;
	m_logfont.lfStrikeOut=FALSE;
	m_logfont.lfCharSet=ANSI_CHARSET;
	m_logfont.lfOutPrecision=OUT_DEFAULT_PRECIS;
	m_logfont.lfClipPrecision=CLIP_DEFAULT_PRECIS;
	m_logfont.lfQuality=DEFAULT_QUALITY;
	m_logfont.lfPitchAndFamily=DEFAULT_PITCH | FF_ROMAN;
	lstrcpy(m_logfont.lfFaceName, m_ParmFontName);

	m_GreekLogfont.lfHeight=-Twips2LogicalDistance(m_ParmFontHgt);
	m_GreekLogfont.lfWidth=0;
	m_GreekLogfont.lfEscapement=0;
	m_GreekLogfont.lfOrientation=0;
	m_GreekLogfont.lfWeight=FW_REGULAR;
	m_GreekLogfont.lfItalic=FALSE;
	m_GreekLogfont.lfUnderline=FALSE;
	m_GreekLogfont.lfStrikeOut=FALSE;
	m_GreekLogfont.lfCharSet=SYMBOL_CHARSET;
	m_GreekLogfont.lfOutPrecision=OUT_DEFAULT_PRECIS;
	m_GreekLogfont.lfClipPrecision=CLIP_DEFAULT_PRECIS;
	m_GreekLogfont.lfQuality=DEFAULT_QUALITY;
	m_GreekLogfont.lfPitchAndFamily=DEFAULT_PITCH | FF_DONTCARE;
	lstrcpy(m_GreekLogfont.lfFaceName, "Symbol");

	CFont boldfon;
	m_logfont.lfWeight=FW_BOLD;
	boldfon.CreateFontIndirect(&m_logfont);
	m_logfont.lfWeight=FW_REGULAR;
	CFont *pOldFont=(CFont *)pDC->SelectObject(&boldfon);

	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);

	m_fontheight= tm.tmHeight;
	m_fontwidth=tm.tmAveCharWidth; 

	m_superscriptsize.x=m_subscriptsize.x=m_superscriptsize.y=m_subscriptsize.y=tm.tmHeight*2/3;
//	m_superscriptoffset.y  = tm.tmHeight/3;
	m_superscriptoffset.y  = 0;
	m_subscriptoffset.y = tm.tmAscent+tm.tmDescent-m_subscriptsize.y; // just a guess
	m_superscriptoffset.x=m_subscriptoffset.x=0;

	/******
	if (tm.tmPitchAndFamily & TMPF_TRUETYPE) {
		// if it is a TrueType font, we can find the correct size for sub&superscripts
		int sz;
		sz=pDC->GetOutlineTextMetrics( 0, NULL);	 // find size required
		OUTLINETEXTMETRIC *otm = (OUTLINETEXTMETRIC *)new BYTE[sz];

		if (pDC->GetOutlineTextMetrics(sz, otm)) {
			m_subscriptoffset=otm->otmptSubscriptOffset;
			m_subscriptsize=otm->otmptSubscriptSize;
			m_superscriptsize=otm->otmptSuperscriptSize;
			m_superscriptoffset.x=otm->otmptSuperscriptOffset.x;
			m_superscriptoffset.y=tm.tmAscent-otm->otmptSuperscriptOffset.y-otm->otmptSuperscriptSize.y;
		}
		delete [] otm;
	}
*****/

	// Symbol font is a pain - it isn't aligned correctly. We need to
	// work out how much to move it up by
	int normascent=tm.tmAscent;
	CFont greekfon;
	greekfon.CreateFontIndirect(&m_GreekLogfont);
	pDC->SelectObject(&greekfon);
	pDC->GetTextMetrics(&tm);
	m_greekoffset.y=normascent-tm.tmAscent;
	m_greekoffset.x=0;

	pDC->SelectObject(pOldFont);
	if (bIsScreen) delete pDC;
}

CParamSection::CParamSection(void)
{
	m_rowlist.SetSize(0,1);
	m_commandlist.SetSize(0,1);
}

int CParamSection::GetNumberOfRows(void)
{
	int numlines=m_rowlist.GetSize();
	return numlines;
}

int CParamSection::GetHeight(CDC * /*pDC*/)
{
	return GetNumberOfRows()*GetFontHeight();
}  

int CParamSection::GetWidth(CDC * /*pDC*/)
{
	int largestlinesize=0;
	int largestline=0;
	int numlines=m_rowlist.GetSize();
	int k;
	for (k=0; k<numlines; k++) {
		int tmp= m_rowlist[k].GetLength(); // This won't work for tabs!!!!!!
		if (tmp>largestlinesize) { largestlinesize=tmp; largestline=k; }
	} 
	return 32+largestlinesize*GetFontWidth(); // 32 is fudge factor!!!!
}

CRect CParamSection::GetClientBBox(void)
{
	return m_bbox;
}

void CParamSection::ResetRows(void)
{
	m_rowlist.RemoveAll();
	m_commandlist.RemoveAll();
}
 
void CParamSection::RowOut(CString str, WORD msg)
{
	m_rowlist.Add(str);
	m_commandlist.Add(msg);
}


CString SciDouble2Str(double val)
{
	CString cstr;
	char *str=cstr.GetBuffer(40);
	FormatScientificDouble(str, val);
	cstr.ReleaseBuffer();

	// convert from 'e'-form exponential to snazzy string 3.6x10^4
	// note, "$g´$n" is symbol font for cross.
	int i;
	for (i=0; i<cstr.GetLength(); i++) {
		if (str[i]=='e') {
			int k;
			if (str[i-1]=='.') k=i-1; else k=i;	// remove fullstop  2x10^5 not 2.x10^5
			cstr=cstr.Left(k)+"$g´$n10$^"+cstr.Right(cstr.GetLength()-i-1)+"$n"; // 3.6x10^-17
			return cstr;
		}
	}
	return cstr;
}

// want to remove spurious zeros from exponent. eg. want 35e16 not 35e+016
void RemoveSpuriousZerosFromExponent(char *s)
{
		char *p;		// ptr into read buffer
		char *q;		// ptr into write buffer

	p=s;
	q=p;
	while (*p!=0 && *p!='e') {
		if (*p=='#') {	return; } // don't mess with infinity!
		if (*p!='0') q=p;
		p++;
	}

	if (*p!=0) {
		q++; 
		*q++=*p++; // copy 'e'
		if (*p=='-') *q++='-';  // copy '-', or ignore '+'
		p++;
		if (*p!='0')  { 
				*q++=*p++; // copy '1'
				*q++=*p++; // copy '2' 
		} else {
			p++;
			if (*p!='0') *q++=*p; // copy '2'
			p++;
		}

		*q++=*p++; // copy '3'
		*q=0; // end of string
	}
}


// makes a string of val to 4 decimal places
CString Num4DecPlaces(double val)
{
	CString cstr;
	if (val==0) cstr="0.000";
	else if (fabs(val)>1e5 || fabs(val)<1e-2)	
							cstr.Format("%0.3e", val);
	else if (fabs(val)<1)	cstr.Format("%0.4f", val);
	else if (fabs(val)<10)	cstr.Format("%0.3f", val);
	else if (fabs(val)<100)	cstr.Format("%0.2f", val);
	else if (fabs(val)<1000)cstr.Format("%0.1f", val);
	else					cstr.Format("%0.0f", val);

	char *str=cstr.GetBuffer(20);
	RemoveSpuriousZerosFromExponent(str);
	cstr.ReleaseBuffer();	
	return cstr;
}

// makes a string of val with at most 4 decimal places
CString Max4DecPlaces(double val)
{
	CString cstr;
	char *s=cstr.GetBuffer(20);
	if (fabs(val)>1e-2 && fabs(val)<1e5) sprintf(s, "%.4g", val);
	else if (val==0) sprintf(s,"0");
	else sprintf(s, "%.3g", val);	
	RemoveSpuriousZerosFromExponent(s);
	cstr.ReleaseBuffer();
	return cstr;
}


void CParamSection::RowOut(CString intro, int val, CString outro, WORD msg)
{

	char str[16];
	CString cstr;
	
	safesprintf(str, 15, "%0d", val);		
	cstr = str;
	m_rowlist.Add(intro + cstr + outro);
	m_commandlist.Add(msg);
}

void CParamSection::RowOut(CString intro, double val, CString outro, WORD msg)
{
//	m_rowlist.Add(intro+Num4DecPlaces(val)+outro);	
	m_rowlist.Add(intro+SciDouble2Str(val)+outro);	
	m_commandlist.Add(msg);
}

void CParamSection::DrawIfVisible(CDC *pDC)
{
	if ( pDC->RectVisible(m_bbox) ) {
//		Draw(pDC, m_bbox.left, m_bbox.top, 0, GetNumberOfRows());

		// SPEEDUP: Draw only the lines which are actually visible
		int fontheight=GetFontHeight();
		CRect rect, cliprect;
		pDC->GetClipBox(cliprect);
		int firstrow, numrows;
		if (cliprect.top<m_bbox.top) firstrow=0;
		else firstrow=(cliprect.top-m_bbox.top)/fontheight;
		numrows=(cliprect.bottom-m_bbox.top)/fontheight+1;
		if (numrows > GetNumberOfRows()-firstrow) numrows=GetNumberOfRows()-firstrow;
		Draw(pDC, m_bbox.left, m_bbox.top, firstrow, numrows);
	}
}

const int RGB_BLACK=RGB(0,0,0);
const int RGB_RED=RGB(255,0,0);

void CParamSection::Draw(CDC *pDC, int xorigin, int yorigin, int firstrow, int numrows)
{
	int lh;
	CString str;

	if (firstrow+numrows>GetNumberOfRows()) numrows=GetNumberOfRows()-firstrow;
	if (numrows<=0) return;

	// Create the fonts...
	CFont Font;
	CFont ItalicFont, BoldFont, GreekFont, SubFont;

	Font.CreateFontIndirect(&m_logfont);
	CFont *pOldFont=(CFont *)pDC->SelectObject(&Font);
	m_logfont.lfItalic=TRUE;
	ItalicFont.CreateFontIndirect(&m_logfont);
	m_logfont.lfItalic=FALSE;
	m_logfont.lfWeight=FW_BOLD;
	BoldFont.CreateFontIndirect(&m_logfont);
	m_logfont.lfWeight=FW_REGULAR;
	GreekFont.CreateFontIndirect(&m_GreekLogfont);
	int oh=m_logfont.lfHeight;
	m_logfont.lfHeight=m_subscriptsize.y;
	SubFont.CreateFontIndirect(&m_logfont);
	m_logfont.lfHeight=oh;

	lh=m_fontheight;

	int rowcount=0;
  	int ypos=yorigin+firstrow*lh;

    int n;

	int tabwid=16*m_fontwidth;
	int tabsofar;
	
	n=firstrow;	

	pDC->SetTextAlign(TA_TOP | TA_LEFT | TA_UPDATECP);


	while (rowcount<numrows)
	{
		CPoint curpt;

		pDC->SetTextColor(RGB_BLACK);
		pDC->SelectObject(&Font);		
		pDC->MoveTo(xorigin, ypos);	

		str=m_rowlist[n];
		int k;
		CString outstr="";
		tabsofar=0;
		for (k=0; k<str.GetLength(); k++) {
			if (str[k]!='$' || k==str.GetLength()) {
				if (str[k]=='\t') {
					pDC->TextOut(0, 0, outstr);
					outstr="";
					tabsofar++;
					pDC->MoveTo(xorigin+tabsofar*tabwid, ypos);
				} else outstr+=str[k];
			} else { // found a special character!
				// display the text we have so far
				if (!outstr.IsEmpty()) {
					pDC->TextOut(0, 0, outstr);
				}
				outstr="";

				k++;
				switch (str[k]) {
				case 'n':	// Normal
					curpt=pDC->GetCurrentPosition();
					pDC->MoveTo(curpt.x, ypos);
					pDC->SetTextColor(RGB_BLACK);
					pDC->SelectObject(&Font);		
					break;
				case 'i':	// Italic
					pDC->SetTextColor(RGB_BLACK);
					pDC->SelectObject(&ItalicFont);		
					break;
				case 'b':	// Bold
					pDC->SelectObject(&BoldFont);		
					if (pDC->IsPrinting()) pDC->SetTextColor(RGB_BLACK);
					else pDC->SetTextColor(RGB_RED);
					break;
				case 'g':	// Greek
					// don't care what colour it is.
					curpt=pDC->GetCurrentPosition();
					pDC->MoveTo(curpt.x, ypos+m_greekoffset.y);
					pDC->SelectObject(&GreekFont);		
					break;
				case '^':	// Superscript
					curpt=pDC->GetCurrentPosition();
					pDC->MoveTo(curpt.x, ypos+m_superscriptoffset.y);
					pDC->SelectObject(&SubFont);
					break;
				case '_':
					curpt=pDC->GetCurrentPosition();
					pDC->MoveTo(curpt.x, ypos+m_subscriptoffset.y);
					pDC->SelectObject(&SubFont);
					break;
				case '$':
					outstr='$';
					break; // ignore the second $.
				default: 
					outstr="$"+(char)str[k];
					break;
				}
			}
		} // for
		// now display the leftover text
		pDC->TextOut(0, 0, outstr);
		ypos+=lh;
		n++;
		rowcount++;
	}

	pDC->SetTextAlign(TA_TOP | TA_LEFT | TA_NOUPDATECP);
	pDC->SetTextColor(RGB_BLACK);
	pDC->SelectObject(pOldFont);
	Font.DeleteObject();
	ItalicFont.DeleteObject();
	BoldFont.DeleteObject();
	GreekFont.DeleteObject();
	SubFont.DeleteObject();
}

// assumes x,y, in ParamSection "client coordinates"
void CParamSection::OnDoubleClick(CPc1dDoc * /*pDoc*/, int /* x */, int y)
{
	// which line was it on?
	int index=y/m_fontheight;
	if (index<0) return;
	if (m_commandlist[index]!=0) {
		AfxGetApp()->m_pMainWnd->SendMessage(WM_COMMAND, m_commandlist[index],0);
	}
}



/////////////////////////////////////////////////////////////////////////////
// Functions for updating the text in different sections


CString GetContactStr(double seriesR, CString contactname)
{
	CString tmpstr;
	if (seriesR>1e-6) tmpstr = contactname +" contact: " + SciDouble2Str(seriesR)+" $gW$n";
	else tmpstr= contactname+ " contact enabled";
	return tmpstr;
}

CString GetReflectanceStr(CReflectance* pRefl, CString frontrear)
{
	CString str;
	if (pRefl->m_bExternal)	str=frontrear + " reflectance from " + pRefl->m_Filename;
	else if (pRefl->m_bCoated) str=frontrear+ " surface optically coated";
	else // (pRefl->m_bFixed) 
		if (pRefl->m_Fixed==0) str="$iNo Exterior "+frontrear+" Reflectance";
		else str="Exterior "+frontrear+" Reflectance: "+SciDouble2Str(pRefl->m_Fixed*100)+"%";
	return str;
}


void CDeviceSection::Update(CPc1dDoc *pDoc)
{
	CDevice* pD = pDoc->GetProblem()->GetDevice();
	CExcite* pE = pDoc->GetProblem()->GetExcite();

	ResetRows();
	RowOut("$b   DEVICE");
	
	if (!pD->m_Filename.IsEmpty())
	{
		if (pD->m_bModified) RowOut("Device modified from " + pD->m_Filename, ID_DEVICE_OPEN);
		else                 RowOut("Device from " + pD->m_Filename, ID_DEVICE_OPEN);
	}

	// --- Area ---

	double area=pD->m_Area; 
	char *unitstr;

	switch (pD->m_Aunit) {
		case CM2: unitstr=" cm$^2";			   break;
		case MM2: unitstr=" mm$^2"; area*=100; break;
		case UM2: unitstr=" µm$^2"; area*=1e8; break;
	}
	RowOut("Device area: ", area, unitstr, ID_DEVICE_AREA);

	// --- Texture ---
		
	if (pD->m_FrontTexture)
		RowOut("Front surface texture depth: ", pD->m_FrontDepth*1e4, " µm",ID_DEVICE_TEXTURE);
	if (pD->m_RearTexture)
		RowOut("Rear surface texture depth: ", pD->m_RearDepth*1e4, " µm",ID_DEVICE_TEXTURE);
	if (!pD->m_FrontTexture && !pD->m_RearTexture) RowOut("$iNo surface texturing",ID_DEVICE_TEXTURE);

	// --- Surface charge ---

	if (pD->m_FrontSurface==NEUTRAL_SURFACE && pD->m_RearSurface==NEUTRAL_SURFACE)
		RowOut("$iNo surface charge", ID_DEVICE_SURFACES_FRONT);
	else {
		switch (pD->m_FrontSurface) {
		case BARRIER_SURFACE: RowOut("Front surface barrier: ", pD->m_FrontBarrier, " eV",ID_DEVICE_SURFACES_FRONT); break;
		case CHARGED_SURFACE: RowOut("Front surface charge: ", pD->m_FrontCharge, " cm$^-2",ID_DEVICE_SURFACES_FRONT); break;
		default:			  RowOut("$iFront surface neutral", ID_DEVICE_SURFACES_FRONT); break;
		}
		switch (pD->m_RearSurface) {
		case BARRIER_SURFACE: RowOut("Rear surface barrier: ", pD->m_RearBarrier, " eV",ID_DEVICE_SURFACES_REAR); break;
		case CHARGED_SURFACE: RowOut("Rear surface charge: ", pD->m_RearCharge, " cm$^-2",ID_DEVICE_SURFACES_REAR); break;
		default:			  RowOut("$iRear surface neutral", ID_DEVICE_SURFACES_REAR); break;
		}
	}
	
	// --- Reflectance ---

	RowOut(GetReflectanceStr(&(pD->m_FrontRfl), "Front"), ID_REFLECTANCE_FRONT);
	RowOut(GetReflectanceStr(&(pD->m_RearRfl), "Rear"), ID_REFLECTANCE_REAR);

/***
	if (pD->m_FrontRfl.m_bExternal)
		RowOut("Front reflectance from " + pD->m_FrontRfl.m_Filename, ID_REFLECTANCE_FRONT);
	else if (pD->m_FrontRfl.m_bCoated)
		RowOut("Front surface optically coated",ID_REFLECTANCE_FRONT);
	else if (pD->m_FrontRfl.m_bFixed) {
		if (pD->m_FrontRfl.m_Fixed==0) RowOut("$iNo exterior front reflectance", ID_REFLECTANCE_FRONT);
		else RowOut("Exterior front reflectance: ", pD->m_FrontRfl.m_Fixed*100, "%",ID_REFLECTANCE_FRONT);
	}
	
	if (pD->m_RearRfl.m_bExternal)
		RowOut("Rear reflectance from " + pD->m_RearRfl.m_Filename, ID_REFLECTANCE_REAR);
	else if (pD->m_RearRfl.m_bCoated)
		RowOut("Rear surface optically coated", ID_REFLECTANCE_REAR);
	else if (pD->m_RearRfl.m_bFixed)
		if (pD->m_RearRfl.m_Fixed==0) RowOut("$iNo exterior rear reflectance", ID_REFLECTANCE_REAR);
		else RowOut("Exterior rear reflectance: ", pD->m_RearRfl.m_Fixed*100, "%", ID_REFLECTANCE_REAR);
***/

  if (pD->m_FrontRfl.m_Internal1>0 || pD->m_RearRfl.m_Internal1>0)
	{
		RowOut("Internal optical reflectance enabled", ID_DEVICE_REFLECTANCE_INTERNAL);
		if (pD->m_FrontRfl.m_Rough>PLANAR_SURFACE) 
			RowOut("  Front surface optically rough", ID_DEVICE_REFLECTANCE_INTERNAL);
		if (pD->m_RearRfl.m_Rough>PLANAR_SURFACE)  
			RowOut("  Rear surface optically rough", ID_DEVICE_REFLECTANCE_INTERNAL);
	} else RowOut("$iNo internal optical reflectance", ID_DEVICE_REFLECTANCE_INTERNAL);


	// --- Contacts & internal elements ---
	
	if (pD->m_EnableEmitter)   RowOut(GetContactStr(pD->m_EmitterR, "Emitter"), ID_DEVICE_CIRCUIT_CONTACTS);
	if (pD->m_EnableBase) 	   RowOut(GetContactStr(pD->m_BaseR, "Base"), ID_DEVICE_CIRCUIT_CONTACTS);
	if (pD->m_EnableCollector) RowOut(GetContactStr(pD->m_CollectorR, "Collector"), ID_DEVICE_CIRCUIT_CONTACTS);

	if (!pD->m_EnableEmitter&&!pD->m_EnableBase&&!pD->m_EnableCollector) 
		RowOut("All contacts disabled", ID_DEVICE_CIRCUIT_CONTACTS);
	
	BOOL bAnyInternalElements=FALSE;	
	for (int k=0; k<MAX_LUMPED; k++) {
	  CLumped *pL = pD->GetLumpedElement(k);
	  if (pL->m_Enable) { 
		bAnyInternalElements=TRUE;
		switch (pL->m_Type) {
		case CONDUCTOR:
			{RowOut("Internal conductor: ", pL->m_Value, " S", ID_DEVICE_CIRCUIT_INTERNALELEMENTS); break;}
		case DIODE:
			{RowOut("Internal diode: ", pL->m_Value, " A", ID_DEVICE_CIRCUIT_INTERNALELEMENTS); break;}
		case CAPACITOR:
			{RowOut("Internal capacitor: ", pL->m_Value, " F", ID_DEVICE_CIRCUIT_INTERNALELEMENTS); break;}
		}
	  }
	}
	if (!bAnyInternalElements) RowOut("$iNo internal shunt elements", ID_DEVICE_CIRCUIT_INTERNALELEMENTS);
}


CString GetDiffusionStr(CDiffusion* pDiff, CString name)
{
	CString tstr;
	if (pDiff->IsEnabled()) {
		if (pDiff->m_Type==P_TYPE) tstr="P-type"; else tstr="N-type";
		return "  "+name+" diff.:\t"+tstr+", "+SciDouble2Str(pDiff->m_Npeak)+" cm$^-3$n peak";
	} else return "$i  No "+name+" diffusion";
}

// surface recombination
CString GetSurfRecombStr(CString frontrear, double Sn, double Sp, BOOL bUseJoModel)
{
	CString str;
	if (Sn!=0 || Sp!=0)
	{
		if (bUseJoModel)	str = "  "+frontrear+"-surface recom.:  J$_o$n model,";
		else				str = "  "+frontrear+"-surface recom.:  S model,";
		if (Sn!=Sp)
			str+=" S$_n$n ="+SciDouble2Str(Sn)+", S$_p$n = "+SciDouble2Str(Sp)+" cm/s";
		else str+=" S$_n$n = S$_p$n = "+SciDouble2Str(Sn)+" cm/s";
	} else					str = "$i  No "+frontrear+"-surface recombination";
	return str;
}


void CRegionSection::Update(CPc1dDoc *pDoc)
{
	CDevice* pD = pDoc->GetProblem()->GetDevice();
	CExcite* pE = pDoc->GetProblem()->GetExcite();
	
//	int oldregion=pD->m_CurrentRegion;
//	pD->SelectRegion(m_regionnumber);
	CRegion* pR = pD->GetRegion(m_regionnumber);
	CMaterial *pMat = pR->GetMaterial();

	ResetRows();

	CString str;
	str.Format("$b   REGION %d", m_regionnumber+1);
	RowOut(str);
	
	RowOut("Thickness: ", pR->m_Thickness*1e4, " µm", ID_DEVICE_THICKNESS);

	if (pMat->IsModified()) str = "Material modified from ";
	else					   str = "Material from ";
	if (!pMat->m_Filename.IsEmpty()) str += pMat->m_Filename;
	else								 str += "program defaults";
	RowOut(str, ID_MATERIAL_OPEN);

	if (pMat->m_FixedMobility) {
		RowOut("  Fixed electron mobility: ", pMat->m_Elec.Fixed, " cm$^2$n/Vs",ID_DEVICE_MATERIAL_MOBILITIES_FIXED);
		RowOut("  Fixed hole mobility: ", pMat->m_Hole.Fixed, " cm$^2$n/Vs",ID_DEVICE_MATERIAL_MOBILITIES_FIXED);
	} else
		RowOut("  Carrier mobilities from internal model",			ID_DEVICE_MATERIAL_MOBILITIES_VARIABLE);
	
	RowOut("  Dielectric constant: " + SciDouble2Str(pMat->m_Permittivity),	ID_DEVICE_MATERIAL_PERMITTIVITY);
	RowOut("  Band gap: ", pMat->m_BandGap, " eV",ID_DEVICE_MATERIAL_BANDSTRUCTURE);
	RowOut("  Intrinsic conc. at 300 K: ", pMat->m_ni300, " cm$^-3",ID_DEVICE_MATERIAL_BANDSTRUCTURE);
	
	if (pMat->m_IndexExternal)
		 RowOut("  Refractive index from " + pMat->m_IndexFilename, ID_DEVICE_MATERIAL_REFRACTIVEINDEX);
	else RowOut("  Refractive index: ", pMat->m_FixedIndex, "",	ID_DEVICE_MATERIAL_REFRACTIVEINDEX);
	
	if (pMat->m_AbsExternal)
		 RowOut("  Absorption coeff. from " + pMat->m_AbsFilename, ID_MATERIAL_INTRINSIC);
	else RowOut("  Absorption coeff. from internal model",	ID_MATERIAL_INTRINSIC);

	if (!pMat->m_FreeCarrEnable || (pMat->m_FreeCarrCoeffP==0 && pMat->m_FreeCarrCoeffP==0))
		 RowOut("$i  No free carrier absorption", ID_MATERIAL_PARASITICS);
	else RowOut("  Free carrier absorption enabled", ID_MATERIAL_PARASITICS);
	
	// --- Doping ---

	if (pR->m_BkgndType==P_TYPE) str="P-type"; else str="N-type";
	RowOut(str+" background doping: "+SciDouble2Str(pR->m_BkgndDop)+" cm$^-3", ID_DEVICE_DOPING_BACKGROUND);

	if (pR->m_FrontExternal)
		RowOut("  Front doping from " + pR->m_FrontFilename, 			ID_DEVICE_DOPING_FRONT_EXTERNAL);
	else if (!pR->m_FrontDiff1.IsEnabled() && !pR->m_FrontDiff2.IsEnabled()) 
				RowOut("$i  No front diffusion", ID_DEVICE_DOPING_DIFFUSIONS_FIRSTFRONT);
	else { 	// at least one diffusion is enabled
		RowOut(GetDiffusionStr(&pR->m_FrontDiff1, "1st front"), ID_DEVICE_DOPING_DIFFUSIONS_FIRSTFRONT);
		RowOut(GetDiffusionStr(&pR->m_FrontDiff2, "2nd front"), ID_DEVICE_DOPING_DIFFUSIONS_SECONDFRONT);
	}
	if (pR->m_RearExternal)
		RowOut("  Rear doping from " + pR->m_RearFilename, 	ID_DEVICE_DOPING_REAR_EXTERNAL);
	else if (!pR->m_RearDiff1.IsEnabled() && !pR->m_RearDiff2.IsEnabled()) 
				RowOut("$i  No rear diffusion", ID_DEVICE_DOPING_DIFFUSIONS_FIRSTREAR);		
	else {
		RowOut(GetDiffusionStr(&pR->m_RearDiff1, "1st rear"), ID_DEVICE_DOPING_DIFFUSIONS_FIRSTREAR);
		RowOut(GetDiffusionStr(&pR->m_RearDiff2, "2nd rear"), ID_DEVICE_DOPING_DIFFUSIONS_SECONDREAR);
	}

	// --- Recombination ---
	
	if (pR->m_TauN==pR->m_TauP) {
		RowOut("Bulk recombination: $gt$_n$n = $gt$_p$n = ",pR->m_TauN*1e6, " µs", ID_DEVICE_RECOMBINATION_BULK);
	} else {
		RowOut("Bulk recombination: $gt$_n$n = " + SciDouble2Str(pR->m_TauN*1e6)
			+" µs, $gt$_p$n = "+ SciDouble2Str(pR->m_TauP*1e6)+" µs", ID_DEVICE_RECOMBINATION_BULK);
	}

	RowOut(GetSurfRecombStr("Front", pR->m_FrontSn, pR->m_FrontSp, pR->m_FrontJo), ID_DEVICE_RECOMBINATION_FRONTSURFACE);
	RowOut(GetSurfRecombStr("Rear", pR->m_RearSn, pR->m_RearSp, pR->m_RearJo), ID_DEVICE_RECOMBINATION_REARSURFACE);

//	pD->SelectRegion(oldregion);
}

void CRegionSection::OnClick(CPc1dDoc *pDoc, int /*xpos*/, int /*ypos*/)
{
	pDoc->GetProblem()->GetDevice()->SetCurrentRegionNumber(m_regionnumber);
	pDoc->UpdateRegionButton();
//	CParamSection::OnClick(pDoc, xpos, ypos);
}

CString GetIntensityStr(CLight* pLgt, BOOL bIsTransient)
{
	CString tmpstr;
	if (pLgt->m_IntensityExternal)
				tmpstr="Intensity from "+pLgt->m_IntensityFile;
	else if (bIsTransient) {
		if (pLgt->m_IntensityTR1==pLgt->m_IntensityTR2)
				tmpstr="Constant intensity: "+SciDouble2Str(pLgt->m_IntensityTR1)+" W cm$^-2$n";
		else	tmpstr="Intensity from "+SciDouble2Str(pLgt->m_IntensityTR1)+" to "+SciDouble2Str(pLgt->m_IntensityTR2)+ " W cm$^-2$n";
		if (pLgt->m_IntensityTR1!=pLgt->m_IntensitySS) tmpstr+=", Step at t=0";
	} else		tmpstr="Steady state intensity: "+SciDouble2Str(pLgt->m_IntensitySS)+" W cm$^-2";
	return tmpstr;
}

CString GetSpectrumStr(CLight* pLgt, BOOL bIsTransient)
{
	CString tmpstr;
	if (pLgt->m_SpectrumExternal)
			 tmpstr="Spectrum from "+pLgt->m_SpectrumFile;
	if (pLgt->m_SpectrumMono) {
		if (bIsTransient) 
			 tmpstr="Monochrome, wavelength from "+SciDouble2Str(pLgt->m_LambdaTR1)+" to "+SciDouble2Str(pLgt->m_LambdaTR2)+" nm";
		else tmpstr="Monochrome, steady-state wavelength = "+SciDouble2Str(pLgt->m_LambdaSS)+" nm";
	} if (pLgt->m_SpectrumBlack)
			 tmpstr="Black Body spectrum, temperature = "+SciDouble2Str(pLgt->m_BlackTemperature)+"K"; 
	return tmpstr;
}

CString GetCircuitStr(CCircuit* pCirc, CString name, BOOL bIsTransient)
{
	CString str;
	if (pCirc->m_External) str=name+" circuit from "+pCirc->m_File;	
	else if (pCirc->m_RSS==0 && pCirc->m_VSS==0 && 
			(!bIsTransient || (pCirc->m_VTR1==0 && pCirc->m_VTR2==0 && pCirc->m_RTR==0))) 
		str="$i"+name+" circuit: Zero";
	else {
		if (bIsTransient) {
			if (pCirc->m_VTR1!=pCirc->m_VTR2) str=name+" circuit: Sweep from "+SciDouble2Str(pCirc->m_VTR1)+" to "+SciDouble2Str(pCirc->m_VTR2)+" V";
			else str=name+" circuit: "+SciDouble2Str(pCirc->m_VTR1)+" V";
			if (pCirc->m_VSS!=pCirc->m_VTR1 || pCirc->m_RSS!=pCirc->m_RTR) str+=", Step at t=0";
		} else str=name+" circuit: "+SciDouble2Str(pCirc->m_VSS)+" V";
	}
	return str;
}

void CExcitationSection::Update(CPc1dDoc *pDoc)
{
	CExcite* pE = pDoc->GetProblem()->GetExcite();
	BOOL bIsTransient;
	CString str;

	ResetRows();
	RowOut("$b   EXCITATION");
	
	if (!pE->m_Filename.IsEmpty())
	{
		if (pE->m_bModified) RowOut("Excitation modified from " + pE->m_Filename, ID_EXCITATION_OPEN);
		else				 RowOut("Excitation from " + pE->m_Filename, ID_EXCITATION_OPEN);
	}

	bIsTransient=FALSE;	
	switch (pE->m_Mode) {
	case EQ_MODE: str="Excitation mode: Equilibrium"; break;
	case SS_MODE: str="Excitation mode: Steady State"; break;
	case TR_MODE: str.Format("Excitation mode: Transient, %d timesteps",pE->m_TranNum);
				  bIsTransient=TRUE; break;
	}
	RowOut(str, ID_EXC_MODE);
	
	if (pE->m_Tunit==KELVIN) RowOut("Temperature: ", pE->m_Temp, " K", ID_EXC_TEMPERATURE);
	else					 RowOut("Temperature: ", pE->m_Temp-TKC, "ºC", ID_EXC_TEMPERATURE);

	// --- Circuit ---

	RowOut(GetCircuitStr(&pE->m_Base, "Base", bIsTransient), ID_EXC_BASE_SOURCE);
	RowOut(GetCircuitStr(&pE->m_Coll, "Collector", bIsTransient), ID_EXC_COLL_SOURCE);

	// --- Photogeneration ---

	if (pE->m_LightExternal)
		RowOut("Photogeneration from " + pE->m_LightExternalFile, ID_EXC_PHOTO_EXTERNAL);
	else if (!(pE->m_LightPri.m_On || pE->m_LightSec.m_On)) {
		RowOut("$iLight sources disabled", ID_EXC_PRI_INTENSITY);
	} else {
		if (pE->m_LightPri.m_On) {
			RowOut("Primary light source enabled", ID_EXC_PRI_INTENSITY);

			RowOut("  "+GetIntensityStr(&pE->m_LightPri, bIsTransient), ID_EXC_PRI_INTENSITY);
			RowOut("  "+GetSpectrumStr(&pE->m_LightPri, bIsTransient), ID_EXC_PRI_SPECTRUM);
		} else {
			RowOut("$iPrimary light source disabled", ID_EXC_PRI_INTENSITY);
		}
		if (pE->m_LightSec.m_On) {
			RowOut("Secondary light source enabled", ID_EXC_SEC_INTENSITY);

			RowOut("  "+GetIntensityStr(&pE->m_LightSec, bIsTransient), ID_EXC_SEC_INTENSITY);
			RowOut("  "+GetSpectrumStr(&pE->m_LightSec, bIsTransient), ID_EXC_SEC_SPECTRUM);
		} else {
			RowOut("$iSecondary light source disabled", ID_EXC_SEC_INTENSITY);
		}
	}
}

void CResultsSection::Update(CPc1dDoc *pDoc)
{
	CDevice* pD = pDoc->GetProblem()->GetDevice();
	CExcite* pE = pDoc->GetProblem()->GetExcite();
	CStatus* pStatus = pDoc->GetProblem()->GetStatus();
	double result;	// intermediate result holder
	double* z = new double[pStatus->time_step+2];	// intermediate result holder
	int k;

	ResetRows();
	RowOut("$b   RESULTS");
	if (pD->m_EnableBase) {
		switch (pE->m_Mode) {
		case SS_MODE:
			result = pE->m_Base.m_Volts[pStatus->time_step+1];
			if (result!=0) RowOut("Base voltage: "+Num4DecPlaces(result)+" volts"); else RowOut("");
			result = pE->m_Base.m_Amps[pStatus->time_step+1];			
			if (result!=0) RowOut("Base current: "+Num4DecPlaces(result)+" amps"); else RowOut("");
			break;
		case TR_MODE:
			result = CMath::Intcpt(pStatus->time_step+2, 
						pE->m_Base.m_Volts, pE->m_Base.m_Amps);
			if (result!=0) RowOut("Short-circuit Ib: "+Num4DecPlaces(result)+" amps"); else RowOut("");
			for (k=0; k<pStatus->time_step+2; k++) 
				z[k]=pE->m_Base.m_Volts[k]*pE->m_Base.m_Amps[k];
			result = CMath::InterpMin(pStatus->time_step+2, z);
			if (result<0) RowOut("Max base power out: "+Num4DecPlaces(-result)+" watts"); else RowOut("");
			result = CMath::Intcpt(pStatus->time_step+2, 
						pE->m_Base.m_Amps, pE->m_Base.m_Volts);
			if (result!=0) RowOut("Open-circuit Vb: "+Num4DecPlaces(result)+" volts"); else RowOut("");
			break;
		default:
//			if (pDoc->pBatch->m_Enable && pE->m_Mode==SS_MODE) { RowOut(" "); RowOut(" "); }
//			if (pDoc->pBatch->m_Enable && pE->m_Mode==TR_MODE) { RowOut(" "); RowOut(" "); RowOut(" "); }
			break;
	 	}
	}
	if (pD->m_EnableCollector) {
		switch (pE->m_Mode) {
		case SS_MODE:
			result = pE->m_Coll.m_Volts[pStatus->time_step+1];
			if (result!=0) RowOut("Collector voltage: "+Num4DecPlaces(result)+" volts"); else RowOut("");
			result = pE->m_Coll.m_Amps[pStatus->time_step+1];			
			if (result!=0) RowOut("Collector current: "+Num4DecPlaces(result)+" amps"); else RowOut("");
			break;
		case TR_MODE:
			result = CMath::Intcpt(pStatus->time_step+2, 
						pE->m_Coll.m_Volts, pE->m_Coll.m_Amps);
			if (result!=0) RowOut("Short-circuit Ic: "+Num4DecPlaces(result)+" amps"); else RowOut("");
			for (k=0; k<pStatus->time_step+2; k++) 
					z[k]=pE->m_Coll.m_Volts[k]*pE->m_Coll.m_Amps[k];
			result = CMath::InterpMin(pStatus->time_step+2, z);
			if (result<0) RowOut("Max collector power out: "+Num4DecPlaces(-result)+" watts"); else RowOut("");
			result = CMath::Intcpt(pStatus->time_step+2, 
						pE->m_Coll.m_Amps, pE->m_Coll.m_Volts);
				if (result!=0) RowOut("Open-circuit Vc: "+Num4DecPlaces(result)+" volts"); else RowOut("");
			break;
		default:
//			if (pDoc->pBatch->m_Enable && pE->m_Mode==SS_MODE) { RowOut(" "); RowOut(" "); }
//			if (pDoc->pBatch->m_Enable && pE->m_Mode==TR_MODE) { RowOut(" "); RowOut(" "); RowOut(" "); }
			break;
		}
	}
	delete z;
}
 
void CFileSection::Update(CPc1dDoc *pDoc)
{
	int i;
	ResetRows();
	if (!pDoc->GetPathName().IsEmpty()) RowOut("File: "+ pDoc->GetPathName(), ID_FILE_OPEN);
	else RowOut("$iFile: (New Parameters)", ID_FILE_OPEN);
	
	if (!pDoc->GetProblem()->m_Description.IsEmpty()) {
			int lastspace=0;
			int startofline=0;
			for (i=0; i<pDoc->GetProblem()->m_Description.GetLength(); i++){
				if (pDoc->GetProblem()->m_Description[i]==' ') lastspace=i;
				if (i-startofline>70) { 
					if (lastspace==startofline) lastspace=i;
					RowOut(pDoc->GetProblem()->m_Description.Mid(startofline, lastspace-startofline), ID_FILE_DESCRIPTION);
					startofline=lastspace+1;
				}
			}
			if (startofline!=pDoc->GetProblem()->m_Description.GetLength())
				RowOut(pDoc->GetProblem()->m_Description.Mid(startofline,pDoc->GetProblem()->m_Description.GetLength()-startofline), ID_FILE_DESCRIPTION);
	} 
	else RowOut("$i(Double-click to add a description)", ID_FILE_DESCRIPTION);
	RowOut("");
}

void CBatchSection::Update(CPc1dDoc *pDoc)
{
	CBatch *pB = pDoc->GetProblem()->GetBatch();
	ResetRows();
	if (!pB->m_Enable) return;
	RowOut("$b   BATCH");
	if (pB->m_bExternal) RowOut("Batch run from file: "+pB->m_Filename, ID_COMPUTE_QUICKBATCH);
	else RowOut("QuickBatch, ", pB->GetNumberOfLines(), " simulations", ID_COMPUTE_QUICKBATCH);
	CString str;
	int i;
	str="$iSim #$n";
	for (i=0; i<pB->GetNumberOfFields(); i++) {
		str+="\t"+pB->GetFieldName(i);
	}
	RowOut(str, ID_COMPUTE_QUICKBATCH);
	int n;
	for(n=0; n<pB->GetNumberOfLines(); n++) {
		str.Format("$i%d$n", n+1);
		for (i=0; i<pB->GetNumberOfFields(); i++) {
			if (pB->IsValueValid(n,i)) str+="\t"+Max4DecPlaces(pB->GetValue(n,i));
			else str+="\t?";
		}
		RowOut(str, ID_COMPUTE_QUICKBATCH);
	}
}

void CBatchSection::UpdateCurrentSim(CPc1dDoc *pDoc)
{
	CBatch *pB = pDoc->GetProblem()->GetBatch();
	if (!pB->m_Enable) return;
	int simnum;
	simnum=pB->GetPreviousSim();
	CString str;
	if (simnum>0) {
		int i;
		str.Format("$i%d$n", simnum+1);
		for (i=0; i<pB->GetNumberOfFields(); i++) {
			if (pB->IsValueValid(simnum,i)) str+="\t"+Max4DecPlaces(pB->GetValue(simnum,i));					  
			else str+="\t?";
		}
		m_rowlist.SetAt(simnum+3, str);
	} else Update(pDoc);
}
