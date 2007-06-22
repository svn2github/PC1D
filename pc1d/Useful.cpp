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
///////////////////////////////////////////////////////////////////////////
// Useful.cpp -- useful functions for Win 95, independent of any program
// i.e. they're not part of a class + have no static variables.
// Essentially, they are extensions to the API.

#include "stdafx.h"
#include "useful.h"

bool CDonStatusBar::m_bSilent=false;
// Write a string to the status bar
void CDonStatusBar::WriteStatusBarMessage(CString msg)
{
	if (m_bSilent) return;
	CWnd *parent=AfxGetMainWnd();
	CWnd *statusbar;
	if (!parent) return; // ignore if main frame not made yet
	statusbar=parent->GetDescendantWindow(AFX_IDW_STATUS_BAR, TRUE);
	if (!statusbar) return;	// ignore if status bar not made yet
	statusbar->SetWindowText(msg);
	statusbar->UpdateWindow();
}

// This is TERRIBLE. The call to OnIdleUpdateCmdUI takes 400000 cycles.
// this is about 15% of total PC1D simulation time for one iteration!
void CDonStatusBar::UpdateStatusBar()
{
	if (m_bSilent) return;
	CWnd *parent=AfxGetMainWnd();
	CWnd *statusbar;
	if (!parent) return; // ignore if main frame not made yet
	statusbar=parent->GetDescendantWindow(AFX_IDW_STATUS_BAR, TRUE);
	if (!statusbar) return;	// ignore if status bar not made yet
	((CStatusBar *)statusbar)->OnIdleUpdateCmdUI(TRUE, 0);
	statusbar->UpdateWindow();
}

/////////////////////////////////////////////////////////////////////////////////
//  FPU Helper functions for detecting errors
// sets up the math coprocessor (FPU) -- clears all errors
void CUseful::InitializeFPU()
{
	static short fpcontrol;
	fpcontrol=0x360;			// enable debugger for overflow, underflow, div by 0, denormals, NANs
	_asm {
		finit
#ifdef _DEBUG
		fldcw WORD PTR fpcontrol
#endif
	}
}

// which exceptions have occurred?
// result & 0x01 ===> invalid operation (NaN is present!)
// result & 0x02 ===> denormal operand
int CUseful::GetFPUStatus()
{
	static int fpstatus; 
	_asm {
		xor eax, eax
		fnstsw ax
		mov DWORD PTR fpstatus, eax
	}
   return fpstatus;
}

///////////////////////////////////////////////////

// returns number of cycles since this function was last called
long CUseful::CyclesSinceLastCall(void)
{
	static long lastval;
	long oldval;
	
	oldval=lastval;
	_asm {
		_emit 0x0F
		_emit 0x31			// rdtsc  - Read Time Stamp Counter (new Pentium-only instruction)
		mov lastval, eax
	}
	return lastval-oldval-55; // takes 55 cycles to execute the call to this function
}

double CUseful::GetProfileDouble(CString iniheading, CString entryname, double defaultval)
{
	CString str=AfxGetApp()->GetProfileString(iniheading, entryname);
	if (str.IsEmpty()) return defaultval;
	return atof(str);
	
}

void CUseful::WriteProfileDouble(CString iniheading, CString entryname, double val)
{
	CString str;
	str.Format("%g", val);
	AfxGetApp()->WriteProfileString(iniheading, entryname, str);
}

///////////////////////////////////////////////////


// Display a string in which each tab item is either left- or right- justified.
// Exactly the same as tabbed text out, except for the lpbRightJustify field
// which if TRUE = right justify this field, FALSE= left justify.
void JustifyTabbedTextOut(CDC *pDC, int x, int y, const CString& str, int /*nTabPositions*/, LPINT lpnTabStopPositions, int /*nTabOrigin*/, LPINT lpbRightJustify)
{
	int start, p;
	int oldalign;
	int fieldnum;

	oldalign=pDC->SetTextAlign(TA_UPDATECP | TA_LEFT);
	p=0;
	fieldnum=0;
	int indx;
	indx=0;
	while (p<str.GetLength()) {
		if (lpbRightJustify[fieldnum]) { 
			pDC->SetTextAlign(TA_UPDATECP | TA_RIGHT);
			pDC->MoveTo(x+lpnTabStopPositions[indx],y);
			indx++;
		} else {
			pDC->SetTextAlign(TA_UPDATECP | TA_LEFT);
			if (fieldnum>0) {
				pDC->MoveTo(x+lpnTabStopPositions[indx],y);
				indx++;
			} else pDC->MoveTo(x, y);
		}
		start=p;
		while (p<str.GetLength() && str[p]!='\t') p++;
		CString field=str.Mid(start, p-start);
		if (p<str.GetLength() && str[p]=='\t') p++;
		pDC->TextOut(0,0, field);
		fieldnum++;
	 }
	pDC->SetTextAlign(oldalign);
}

// obtains the Nth field in a tab-delimited string. first field is 0.
CString GetNthTabbedField(CString tabbedstring, int n)
{
	int tabsofar=0;
	int i=0;
	int numchars;
	int len=tabbedstring.GetLength();
	while (tabsofar<n && i<len) {
		if (tabbedstring[i]=='\t') tabsofar++;
		i++;
	}
	if (i>=len) return "";
	// i is first non-tab in the field we want.
	numchars=0;
	while(len > (i+numchars) && tabbedstring[i+numchars]!='\t') numchars++;
	return tabbedstring.Mid(i, numchars);
}

// replace multiple spaces by tabs
void ConvertToTabbedString(CString &str)
{
	CString newstr;
	int i;
	for (i=0; i<str.GetLength(); i++) {
		if (str[i]=='\n') continue; // remove \n and spaces.
		if (str[i]==' ') continue;
		newstr+=str[i];
	}
	str=newstr;
}

// returns the number of fields in the tabbed string
// This is equal to the number of tabs, plus 1.
int GetNumFieldsInString(CString tabbedstring)
{
	int tabsofar=0;
	int len=tabbedstring.GetLength();
	for (int i=0; i<len; i++) if (tabbedstring[i]=='\t') tabsofar++;
	return tabsofar+1;
}


///////////////////////////////////////////////////////////////////
// Clipboard string functions
//

void CopyStringToClipboard(CString str)
{
    HANDLE hGlobalMemory;
    LPSTR lpGlobalMemory;

	hGlobalMemory=GlobalAlloc(GHND, (DWORD)str.GetLength()+1);
    if (!hGlobalMemory) return; // Error: could not allocate
    lpGlobalMemory=(LPSTR)GlobalLock(hGlobalMemory);
    GlobalUnlock(hGlobalMemory);
    lstrcpy(lpGlobalMemory, str);

    AfxGetMainWnd()->OpenClipboard();
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hGlobalMemory);
    CloseClipboard();
}

BOOL IsClipboardStringAvailable()
{
	return IsClipboardFormatAvailable(CF_TEXT);
}

void GetStringFromClipboard(CString &str)
{
	AfxGetMainWnd()->OpenClipboard();	
    HANDLE hGlobalMemory;
    LPSTR lpGlobalMemory;

	hGlobalMemory=GetClipboardData(CF_TEXT);
	if (!hGlobalMemory) {
		CloseClipboard();
		str.Empty();
		return;
	}

	lpGlobalMemory=(LPSTR)GlobalLock(hGlobalMemory);

	int len=lstrlen(lpGlobalMemory);

	LPSTR strbuffer;
	strbuffer = str.GetBufferSetLength(len);
	lstrcpy(strbuffer, lpGlobalMemory);
	str.ReleaseBuffer(len);

	GlobalUnlock(hGlobalMemory);
	CloseClipboard();
}


///////////////////////////////////////////////////////////////////
//  RangeString functions
//

// A rangestring is a (human-readable) text list of a series of cardinal numbers.
// Can be seperated by commas, can include hyphens to indicate a range.
// Example of use: 'Print' dialog for selecting page numbers to print.



// Converts an asterix string e.g "*** * ***   "
// to sample string eg "1-3, 5, 7-9"
CString AsterixStr2RangeStr(const CString asterixstr)
{
	CString result;
	result.Empty();
	int j;
	BOOL bLastWasOn;
	BOOL bAtStart;
	int startofrun;
	int endofrun;
	bLastWasOn=FALSE;
	bAtStart=TRUE;
	char buf[80];
	for (j=0; j<asterixstr.GetLength(); j++) {
		if (asterixstr[j]=='*') {
			if (!bLastWasOn) { startofrun=j; endofrun=j; bLastWasOn=TRUE; }
			else endofrun=j;
		} else {
			if (bLastWasOn) {						
				if (bAtStart) bAtStart=FALSE; else result+=", ";
				if (startofrun==endofrun) {
					wsprintf(buf, "%d", startofrun+1);
					result+=buf;
				} else if (startofrun+1==endofrun){
					wsprintf(buf, "%d, %d", startofrun+1, endofrun+1);
					result+=buf;
				} else {
					wsprintf(buf, "%d-%d", startofrun+1, endofrun+1);
					result+=buf;
				}
				bLastWasOn=FALSE;
			}
		}
	}
	return result;
}

// Converts a rangestring eg. " 1-2, 4, 7-9"  
// to an asterixstring eg. "** *  ***   "
// where asterixstr[x-1]=='*' if x is included in rangelist, ==' ' if not in list
// All numbers must be in the range 1..asterixlen  ( "0" can't be in the range)
// asterixlen is highest allowable num = len of asterix str.
// Returns FALSE if error of anykind

// Implemented as a state machine.
BOOL RangeStr2AsterixStr(const char * rangestr, char *asterixstr, int asterixlen)
{
	int startofrun;
	int endofrun;
	int i;
	char ch;

	enum { WAITFIRST, FIRSTNUM, ENDFIRST, WAITSECOND, SECONDNUM, ENDSECOND } state;

	for (i=0; i<asterixlen; i++) asterixstr[i]=' ';
	asterixstr[asterixlen]=0;

	state=WAITFIRST;
	const char *p;
	for (p=rangestr; *p!=0; p++) {
		ch=*p;
		switch (state) {
		case WAITFIRST: // at beginning, or after a comma
			if (isspace(ch)) continue;
			if (isdigit(ch) && ch!='0') { startofrun=ch-'0'; state=FIRSTNUM; continue; }
			return FALSE; // error!
		case FIRSTNUM:
			if (isdigit(ch)) { startofrun=startofrun*10+ch-'0'; continue; }
			if (isspace(ch)) { state=ENDFIRST; continue; }
			if (ch=='-') { state=WAITSECOND; continue; }
			if (ch==',') {
				if (startofrun>asterixlen) return FALSE; // number too big
				asterixstr[startofrun-1]='*'; state=WAITFIRST; continue; }
			return FALSE;
		case ENDFIRST:
			if (isspace(ch)) continue;
			if (ch=='-') { state=WAITSECOND; continue; }
			if (ch==',') { 
				if (startofrun>asterixlen) return FALSE; // number too big
				asterixstr[startofrun-1]='*'; state=WAITFIRST; continue; }
			return FALSE;
		case WAITSECOND: // we've got a hyphen, now want 2nd number
			if (isspace(ch)) continue;
			if (isdigit(ch) && ch!='0') { endofrun=ch-'0'; state=SECONDNUM; continue; }
			return FALSE; // error!
		case SECONDNUM:
			if (isdigit(ch)) { endofrun=endofrun*10+ch-'0'; continue; }
			if (isspace(ch)) { state=ENDSECOND; continue; }
			if (ch==',') { 
				if (startofrun>endofrun) return FALSE; // range is backwards
				if (endofrun>asterixlen) return FALSE; // number too big
				for (i=startofrun; i<=endofrun; i++) asterixstr[i-1]='*';
				state=WAITFIRST; continue; }
			return FALSE;
		case ENDSECOND:
			if (isspace(ch)) continue;
			if (ch==',') { 
				if (startofrun>endofrun) return FALSE; // range is backwards
				if (endofrun>asterixlen) return FALSE; // number too big
				for (i=startofrun; i<=endofrun; i++) asterixstr[i-1]='*';
				state=WAITFIRST; continue; }
			return FALSE;
		}		
	}

	switch (state) {
	case FIRSTNUM:
	case ENDFIRST:
			if (startofrun>asterixlen) return FALSE; // number too big
			asterixstr[startofrun-1]='*'; state=WAITFIRST; 
			break;
	case SECONDNUM:
	case ENDSECOND:
			if (startofrun>endofrun) return FALSE; // range is backwards
			if (endofrun>asterixlen) return FALSE; // number too big
			for (i=startofrun; i<=endofrun; i++) asterixstr[i-1]='*';
			break;
	case WAITSECOND:
		return FALSE; // ended with a hyphen
	}
	return TRUE;
}

