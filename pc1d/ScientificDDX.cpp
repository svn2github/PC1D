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
// Modified from dlgfloat.cpp in the MFC library.
// Created to overcome the limitations of the default DDX floating point routines.
// DDX_ScientificDouble -- a double that is always displayed in scientific notation.

#include "stdafx.h"
#include <float.h>              // floating point precision

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Extra data validation procs for scientific float/double support
/////////////////////////////////////////////////////////////////////////////

static BOOL AFXAPI SimpleFloatParse(LPCTSTR lpszText, double& d);

static BOOL AFXAPI SimpleFloatParse(LPCTSTR lpszText, double& d)
{
	ASSERT(lpszText != NULL);
	while (*lpszText == ' ' || *lpszText == '\t')
		lpszText++;

	TCHAR chFirst = lpszText[0];
	d = _tcstod(lpszText, (LPTSTR*)&lpszText);
	if (d == 0.0 && chFirst != '0')
		return FALSE;   // could not convert
	while (*lpszText == ' ' || *lpszText == '\t')
		lpszText++;

	if (*lpszText != '\0')
		return FALSE;   // not terminated properly

	return TRUE;
}

// anything in this range will be displayed in fixed point
static double SciDoubleLower=0.01;
static double SciDoubleUpper=1e5;

void SetScientificDoubleDisplayRange(double lo, double hi)
{
	SciDoubleLower=lo;
	SciDoubleUpper=hi;
}

void GetScientificDoubleDisplayRange(double &lo, double &hi)
{
	lo=SciDoubleLower;
	hi=SciDoubleUpper;
}


// prints double in exponential notation in buffer + removes unnecessary zeros
void FormatScientificDouble(char *buffer, double value)
{
	if (value==0) { lstrcpy(buffer, "0"); return; }
	if ( (value>=SciDoubleLower && value<SciDoubleUpper) || (-value>=SciDoubleLower && -value<SciDoubleUpper) ){ 
		sprintf(buffer, "%g", value); 
		return; 
	}

	// Now will be of form -7.890000e+123
	sprintf(buffer, "%e", value);
	char *p=buffer; // ptr into read buffer
	char *q;		// ptr into write buffer

	q=p;
	while (*p!=0 && *p!='e') {
		if (*p=='#') return; // don't mess with infinity!
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


void AFXAPI TextScientificFloatFormat(CDataExchange* pDX, int nIDC,
	void* pData, double value, int nSizeGcvt)
{
	ASSERT(pData != NULL);

	HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
	TCHAR szBuffer[32];
	if (pDX->m_bSaveAndValidate)
	{
		::GetWindowText(hWndCtrl, szBuffer, 32);
		double d;
		if (!SimpleFloatParse(szBuffer, d))
		{
			AfxMessageBox(AFX_IDP_PARSE_REAL);
			pDX->Fail();            // throws exception
		}
		if (nSizeGcvt == FLT_DIG)
			*((float*)pData) = (float)d;
		else
			*((double*)pData) = d;
	}
	else
	{
		FormatScientificDouble(szBuffer, value);
//		_stprintf(szBuffer, _T("%e"), value);

        int nNewLen = lstrlen(szBuffer);
        char szOld[64];
        // fast check to see if text really changes (reduces
        // flash in controls)
        if (nNewLen > sizeof(szOld) ||
            ::GetWindowText(hWndCtrl, szOld, sizeof(szOld)) !=
                            nNewLen ||
            lstrcmp(szOld, szBuffer) != 0)
        {
            // change it
            ::SetWindowText(hWndCtrl, szBuffer);
        }

	}
}

void AFXAPI DDX_ScientificDouble(CDataExchange* pDX, int nIDC, double& value)
{
	TextScientificFloatFormat(pDX, nIDC, &value, value, DBL_DIG);
}

/////////////////////////////////////////////////////////////////////////////
// Validation procs

static void AFXAPI FailMinMaxSci(CDataExchange* pDX,
	 double minVal, double maxVal, int precision, UINT nIDPrompt)
	// error string must have '%1' and '%2' in it
{
	if (!pDX->m_bSaveAndValidate)
	{
		TRACE0("Warning: initial dialog data is out of range.\n");
		return;         // don't stop now
	}
	TCHAR szMin[32], szMax[32];
	CString prompt;

	_stprintf(szMin, _T("%.*g"), precision, minVal);
	_stprintf(szMax, _T("%.*g"), precision, maxVal);
	AfxFormatString2(prompt, nIDPrompt, szMin, szMax);

	AfxMessageBox(prompt, MB_ICONEXCLAMATION, nIDPrompt);
	prompt.Empty(); // exception prep
	pDX->Fail();
}

void AFXAPI DDV_MinMaxSciDouble(CDataExchange* pDX, double const& value, double minVal, double maxVal)
{
	ASSERT(minVal <= maxVal);
	if (value < minVal || value > maxVal)
		FailMinMaxSci(pDX, (double)minVal, (double)maxVal, DBL_DIG,
			AFX_IDP_PARSE_REAL_RANGE);
}

/////////////////////////////////////////////////////////////////////////////
