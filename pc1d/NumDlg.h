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
// CNumDlg dialog
#include "resource.h"

class CNumDlg : public CDialog
{
// Construction
public:
	CNumDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CNumDlg)
	enum { IDD = IDD_NUMERIC };
	double	m_Clamp;
	BOOL	m_bClampPhi;
	BOOL	m_bClampPsi;
	BOOL	m_bRenodeEQ;
	BOOL	m_bRenodeSS;
	BOOL	m_bRenodeTR;
	double	m_ErrorLimit;
	double	m_SizeFactor;
	int		m_nMaxTime;
	BOOL	m_bGraphsEveryIteration;
	BOOL	m_bTotalSat;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CNumDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
