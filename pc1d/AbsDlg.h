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
// CAbsDlg dialog

class CAbsDlg : public CPropertyPage
{
// Construction
public:
	CAbsDlg();	// standard constructor

// Dialog Data
	//{{AFX_DATA(CAbsDlg)
	enum { IDD = IDD_ABSORPTION };
	double	m_Ed1;
	double	m_Ed2;
	double	m_Ei1;
	double	m_Ei2;
	double	m_Ep1;
	double	m_Ep2;
	double	m_Tcoeff;
	double	m_Toffset;
	int		m_Radio;
	double	m_A11;
	double	m_A12;
	double	m_A21;
	double	m_A22;
	double	m_Ad1;
	double	m_Ad2;
	//}}AFX_DATA

	CString m_title; 
	CString m_OpenDlgTitle;
	CString m_Filename;
	CString m_Path, m_Ext;			// path & extension used to start open dialog

// Implementation
protected:
	BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CAbsDlg)
	afx_msg void DoDisabling();
	afx_msg void OnOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
