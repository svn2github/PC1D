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

#include "stdio.h"
#include "stdafx.h"
#include "ascfile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAscFile

BOOL CAscFile::Read(CString FileName, int& nmax, double x[], double y[])
{
	char str[81];
	CStdioFile f;
	if (!f.Open(FileName, CFile::modeRead | CFile::typeText)) return FALSE;
	int i = 0;
	while (i<nmax && f.ReadString(str, 80))
	{
		x[i] = y[i] = 0;
		if (sscanf(str, "%lf %lf", &x[i], &y[i])) i++;
	}
	nmax = i;
	return (i>0);
}

BOOL CAscFile::Read(CString FileName, int& nmax, double x[], double y1[], double y2[])
{
	char str[81];
	CStdioFile f;
	if (!f.Open(FileName, CFile::modeRead | CFile::typeText)) return FALSE;
	int i = 0;
	while (i<nmax && f.ReadString(str, 80))
	{
		x[i] = y1[i] = y2[i] = 0;
		if (sscanf(str, "%lf %lf %lf", &x[i], &y1[i], &y2[i])) i++;
	}
	nmax = i;
	return (i>0);
}
