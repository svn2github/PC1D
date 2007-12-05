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
// CLumpDlg dialog

class CLumpDlg : public CDialog
{
// Construction
public:
	CLumpDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CLumpDlg)
	enum { IDD = IDD_ELEMENTS };
	BOOL	m_Enable1;
	BOOL	m_Enable2;
	BOOL	m_Enable3;
	BOOL	m_Enable4;
	double	m_n2;
	double	m_n1;
	double	m_n3;
	double	m_n4;
	int		m_Type1;
	int		m_Type2;
	int		m_Type3;
	int		m_Type4;
	double	m_Value1;
	double	m_Value2;
	double	m_Value3;
	double	m_Value4;
	double	m_Xa1;
	double	m_Xa2;
	double	m_Xa3;
	double	m_Xa4;
	double	m_Xc1;
	double	m_Xc2;
	double	m_Xc3;
	double	m_Xc4;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CLumpDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
