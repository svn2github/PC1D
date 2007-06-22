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
// MatDlgs.h - header file for PC1D:Device:Material dialog boxes.
// Combines the old files:
// banddlg.h - CBandDlg - band structure dialog
// bgndlg.h - CBGNDlg - Band gap narrowing model dialog
// permdlg.h - CPermDlg - Permittivity dialog
// and adds a new dialog for Material Recombination parameters (PAB 22/5/96)
// and for free carrier absorption CParasiticsDlg (23/8/96)

/////////////////////////////////////////////////////////////////////////////
// CBandDlg dialog

class CBandDlg : public CDialog
{
// Construction
public:
	CBandDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CBandDlg)
	enum { IDD = IDD_BANDSTRUCTURE };
	double	m_Affinity;
	double	m_BandGap;
	double	m_NcNv;
	double	m_Ni400;
	double	m_Ni300;
	double	m_Ni200;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CBandDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};




/////////////////////////////////////////////////////////////////////////////
// CBGNDlg dialog

class CBGNDlg : public CDialog
{
// Construction
public:
	CBGNDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CBGNDlg)
	enum { IDD = IDD_BGNMODEL };
	double	m_nSlope;
	double	m_pSlope;
	double	m_nNref;
	double	m_pNref;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CBGNDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////
// CPermDlg dialog

class CPermDlg : public CDialog
{
// Construction
public:
	CPermDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPermDlg)
	enum { IDD = IDD_PERMITTIVITY };
	double	m_Permittivity;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CPermDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CMatRecomb dialog

class CMatRecomb : public CDialog
{
// Construction
public:
	CMatRecomb(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMatRecomb)
	enum { IDD = IDD_MATRECOMB };
	double	m_BB;
	double	m_Cn;
	double	m_Cnp;
	double	m_Cp;
	double	m_BulkNalphaN;
	double	m_BulkNalphaP;
	double	m_BulkNrefN;
	double	m_BulkNrefP;
	double	m_BulkTalpha;
	double	m_SurfNalphaN;
	double	m_SurfNalphaP;
	double	m_SurfNrefN;
	double	m_SurfNrefP;
	double	m_SurfTalpha;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMatRecomb)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CParasiticsDlg dialog

class CParasiticsDlg : public CPropertyPage
{
// Construction
public:
	CParasiticsDlg();	// standard constructor
	~CParasiticsDlg(void);

// Dialog Data
	//{{AFX_DATA(CParasiticsDlg)
	enum { IDD = IDD_PARASITICS };
	double	m_nCoeff;
	double	m_nLambdaPower;
	double	m_pCoeff;
	double	m_pLambdaPower;
	BOOL	m_Enable;
	//}}AFX_DATA

// Implementation
protected:
	CFont SymbolFont; // uses a Greek font
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();

	// Generated message map functions
	//{{AFX_MSG(CParasiticsDlg)
	afx_msg void DoDisabling();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
