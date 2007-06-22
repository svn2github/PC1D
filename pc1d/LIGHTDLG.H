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
// CLightDlg dialog

class CLightDlg : public CPropertyPage
{
// Construction
public:
	CLightDlg();	// standard constructor
	
// Dialog Data
	//{{AFX_DATA(CLightDlg)
	enum { IDD = IDD_ILLUMINATION_INTENSITY };
	double	m_IntensitySS;
	double	m_IntensityTR1;
	double	m_IntensityTR2;
	int		m_Back;
	int		m_Radio;
	BOOL	m_Enable;
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
	//{{AFX_MSG(CLightDlg)
	afx_msg void OnOpen();
	afx_msg void DoDisabling();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
