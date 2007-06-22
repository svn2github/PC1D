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
// CCircDlg dialog

#include "resource.h"

class CCircDlg : public CPropertyPage
{
// Construction
public:
	CCircDlg(UINT nIDCaption);
	CString m_title; 
	CString m_OpenDlgTitle;
	CString m_Filename;
	CString m_Path, m_Ext;			// path & extension used to start open dialog

// Dialog Data
	//{{AFX_DATA(CCircDlg)
	enum { IDD = IDD_CIRCUIT };
	double	m_RSS;
	double	m_RTR;
	double	m_VSS;
	double	m_VTR1;
	double	m_VTR2;
	int		m_OhmsSS;
	int		m_OhmsTR;
	int		m_Radio;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	BOOL OnInitDialog();

	// Generated message map functions
	//{{AFX_MSG(CCircDlg)
	afx_msg void DoDisabling();
	afx_msg void OnOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
