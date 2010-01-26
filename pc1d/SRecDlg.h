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
// CSrecDlg dialog

class CSrecDlg : public CPropertyPage
{
// Construction
public:
	CSrecDlg(UINT tabnameID);	// standard constructor
	CMatType	m_Type;
	double		m_ni200, m_ni300, m_ni400;	// cm-3
	double		m_BGNnNref, m_BGNpNref;		// cm-3
	double		m_BGNnSlope, m_BGNpSlope;	// cm-3
	CString		m_title;
	double		m_Nalpha;
	double		m_Nref;  					// cm-3
	double		m_Talpha;
public:
// Dialog Data
	//{{AFX_DATA(CSrecDlg)
	enum { IDD = IDD_SURFRECOMBINATION };
	double	m_Et;
	double	m_Jo;
	CString	m_TypeStr;
	BOOL	m_UseJo;
	double	m_Sn;
	double	m_Sp;
	double	m_Doping;
	double	m_LLIs;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	BOOL	OnInitDialog();
	double	Recomb(double Sn, double Sp, double T,
				double& neq, double& peq, double& delta);
	double	S(double Sn, double Sp, double T);
	double	Jo(double Sn, double Sp, double T);
	double	Match_S(double logS);
	double	Match_Jo(double logS);
	BOOL	m_NeedUpdate;

	// Generated message map functions
	//{{AFX_MSG(CSrecDlg)
	afx_msg void OnChange();
	afx_msg void OnKillfocus();
	afx_msg void OnKillfocusJo();
	afx_msg void OnKillfocusLLIs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
