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
// CBkDopDlg dialog

class CBkDopDlg : public CDialog
{
// Construction
public:
	CBkDopDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CBkDopDlg)
	enum { IDD = IDD_BKGNDDOPING };
	int		m_Type;
	double	m_Resistivity;  // This & remaining values are filled in by this Dialog
	double	m_ElecDif;		
	double	m_ElecMob;
	double	m_HoleDif;
	double	m_HoleMob;
	double	m_Doping;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	BOOL OnInitDialog();
	double Resistivity(double N);
	double MatchResistivity(double N);
public:
	CMaterial*	pMat;
	CMobility*	pElec;
	CMobility*	pHole;
	double		ni300;
	BOOL		m_ChangeResistivity, m_ChangeDoping;

	// Generated message map functions
	//{{AFX_MSG(CBkDopDlg)
	afx_msg void OnNtype();
	afx_msg void OnPtype();
	afx_msg void OnKillfocusDoping();
	afx_msg void OnKillfocusResistivity();
	afx_msg void OnChangeDoping();
	afx_msg void OnChangeResistivity();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
