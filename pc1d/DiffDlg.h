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
// CDiffDlg dialog

class CDiffDlg : public CDialog
{
// Construction
public:
	CDiffDlg(CWnd* pParent = NULL);	// standard constructor
	CMaterial*	pMat;
	CMobility* pElec;
	CMobility* pHole;
	CString m_title;
public:
// Dialog Data
	//{{AFX_DATA(CDiffDlg)
	enum { IDD = IDD_DIFFUSION };
	double	m_Depth;
	double	m_Xpeak;
	int		m_Profile;
	int		m_Type;
	BOOL	m_Enable;
	double	m_Junction;
	double	m_SheetRho;
	double	m_BkgndDoping;
	double	m_Npeak;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	BOOL OnInitDialog();
	double Mobility(double N);
	double SheetRho();
	double MatchDoping(double x);
	double Junction();
	double MatchJunction(double xD);
	double MatchSheetRho(double logNs);
	BOOL m_NeedUpdate;
	
	// Generated message map functions
	//{{AFX_MSG(CDiffDlg)
	afx_msg void OnUpdate();
	afx_msg void SetUpdate();
	afx_msg void OnKillfocus();
	afx_msg void OnKillfocusSheetrho();
	afx_msg void OnKillfocusJunction();
	afx_msg void DoDisabling();
	afx_msg void OnChangeJunction();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
