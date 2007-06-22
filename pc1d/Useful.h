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
// Some useful functions (mostly involving strings)
// Essentially, they are extensions to the API.

class CDonStatusBar {
protected:
	static bool m_bSilent;
public:
	static void SetSilent(bool bSilent) { m_bSilent =  bSilent; };
	static void WriteStatusBarMessage(CString msg);
	static void UpdateStatusBar();
};

class CUseful {
public:
	static void InitializeFPU();
	static int GetFPUStatus();
	static long CyclesSinceLastCall();
	static double GetProfileDouble(CString iniheading, CString entryname, double defaultval);
	static void WriteProfileDouble(CString iniheading, CString entryname, double val);
};

void JustifyTabbedTextOut(CDC *pDC, int x, int y, const CString& str, int nTabPositions, LPINT lpnTabStopPositions, int nTabOrigin, LPINT lpbRightJustify);
CString GetNthTabbedField(CString tabbedstring, int n);
void ConvertToTabbedString(CString &str);
int GetNumFieldsInString(CString tabbedstring);


void CopyStringToClipboard(CString str);
BOOL IsClipboardStringAvailable();
void GetStringFromClipboard(CString &str);

BOOL RangeStr2AsterixStr(const char * rangestr, char *asterixstr, int asterixlen);
CString AsterixStr2RangeStr(const CString asterixstr);

/////////////////////////////////////////////////////////////////////////////
// useful utilities
// safesprintf() - like _snprintf, except always null-terminates the string

inline void safesprintf(char *buffer, size_t maxlength, const char *szFormat, ...) 
{
   _vsnprintf( buffer, maxlength, szFormat, (char *)&szFormat + sizeof szFormat);
   buffer[maxlength-1]=0;
}
