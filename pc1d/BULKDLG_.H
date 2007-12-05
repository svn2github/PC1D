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
// CBulkDlg dialog

class CBulkDlg : public CPropertyPage
{
// Construction
public:
	CBulkDlg();	// standard constructor
	CMobility*	pElec;
	CMobility*	pHole;
	CMaterial*	pMat;
	CMatType	m_Type;
	double		m_ni200, m_ni300, m_ni400;	// cm-3
	double		m_BGNnNref, m_BGNpNref;		// cm-3
	double		m_BGNnSlope, m_BGNpSlope;	// cm-3
	double		m_Cn, m_Cp, m_Cnp;			// cm6/s
	double		m_B;						// cm3/s
	double		m_Nalpha;					 
	double		m_Nref;                     // cm-3
	double		m_Talpha;
public:
// Dialog Data
	//{{AFX_DATA(CBulkDlg)
	enum { IDD = IDD_BULKRECOMBINATION };
	double	m_DiffLength;
	double	m_Et;
	CString	m_TypeStr;
	double	m_TauN;
	double	m_TauP;
	double	m_BkgndDoping;
	double	m_LLItau;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	BOOL OnInitDialog();
	double Mobility(double N, double T);
	double Tau(double taun, double taup, double T);
	double MatchLLItau(double tau);
	BOOL m_NeedUpdate;
	
	// Generated message map functions
	//{{AFX_MSG(CBulkDlg)
	afx_msg void OnChange();
	afx_msg void OnKillfocus();
	afx_msg void OnLLItau();
	afx_msg void OnDiffLength();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
