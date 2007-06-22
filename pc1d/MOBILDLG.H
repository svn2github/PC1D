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
// CMobilDlg dialog

class CMobilDlg : public CDialog
{
// Construction
public:
	CMobilDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMobilDlg)
	enum { IDD = IDD_MOBILITY };
	double	m_ElecB1;
	double	m_ElecB2;
	double	m_ElecB3;
	double	m_ElecB4;
	double	m_ElecMajAlpha;
	double	m_ElecMajMin;
	double	m_ElecMax;
	double	m_ElecMinAlpha;
	double	m_ElecMinMin;
	double	m_HoleB1;
	double	m_HoleB2;
	double	m_HoleB3;
	double	m_HoleB4;
	double	m_HoleMajAlpha;
	double	m_HoleMajMin;
	double	m_HoleMax;
	double	m_HoleMinAlpha;
	double	m_HoleMinMin;
	double	m_ElecVmax;
	double	m_HoleVmax;
	double	m_ElecMajNref;
	double	m_ElecMinNref;
	double	m_HoleMajNref;
	double	m_HoleMinNref;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMobilDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
