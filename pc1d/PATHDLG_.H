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
// CPathDlg dialog

#include "resource.h"

class CPathDlg : public CDialog
{
// Construction
public:
	CPathDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPathDlg)
	enum { IDD = IDD_PATHS };
	CString	m_abs;
	CString	m_dev;
	CString	m_dop;
	CString	m_exc;
	CString	m_gen;
	CString	m_inr;
	CString	m_lgt;
	CString	m_mat;
	CString	m_prm;
	CString	m_ref;
	CString	m_spc;
	CString	m_vlt;
	CString	m_bat;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CPathDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
