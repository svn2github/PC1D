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
// MatDlgs.cpp - implementation file for PC1D:Device:Material dialog boxes.
// Combines the old files:
// banddlg.cpp - CBandDlg - band structure dialog
// bgndlg.cpp - CBGNDlg - Band gap narrowing model dialog
// permdlg.cpp - CPermDlg - Permittivity dialog
// and adds a new file for CMatRecomb - Material recombination parameters
// and CParasiticsDlg - free carrier absorption (23/8/96)
#include "stdafx.h"
#include "resource.h"
#include "matdlgs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBandDlg dialog


CBandDlg::CBandDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBandDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBandDlg)
	m_Affinity = 0;
	m_BandGap = 0;
	m_NcNv = 0;
	m_Ni400 = 0;
	m_Ni300 = 0;
	m_Ni200 = 0;
	//}}AFX_DATA_INIT
}

void CBandDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBandDlg)
	DDX_Text(pDX, IDC_AFFINITY, m_Affinity);
	DDV_MinMaxDouble(pDX, m_Affinity, 0., 20.);
	DDX_Text(pDX, IDC_BANDGAP, m_BandGap);
	DDV_MinMaxDouble(pDX, m_BandGap, 0., 20.);
	DDX_Text(pDX, IDC_NCNV, m_NcNv);
	DDV_MinMaxDouble(pDX, m_NcNv, 1.e-004, 10000.);
	DDX_ScientificDouble(pDX, IDC_NI400, m_Ni400);
	DDX_ScientificDouble(pDX, IDC_NI300, m_Ni300);
	DDX_ScientificDouble(pDX, IDC_NI200, m_Ni200);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBandDlg, CDialog)
	//{{AFX_MSG_MAP(CBandDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CBandDlg message handlers









/////////////////////////////////////////////////////////////////////////////
// CBGNDlg dialog


CBGNDlg::CBGNDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBGNDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBGNDlg)
	m_nSlope = 0;
	m_pSlope = 0;
	m_nNref = 0;
	m_pNref = 0;
	//}}AFX_DATA_INIT
}

void CBGNDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBGNDlg)
	DDX_Text(pDX, IDC_N_SLOPE, m_nSlope);
	DDV_MinMaxDouble(pDX, m_nSlope, 0., 20000.);
	DDX_Text(pDX, IDC_P_SLOPE, m_pSlope);
	DDV_MinMaxDouble(pDX, m_pSlope, 0., 20000.);
	DDX_ScientificDouble(pDX, IDC_N_NREF, m_nNref);
	DDX_ScientificDouble(pDX, IDC_P_NREF, m_pNref);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBGNDlg, CDialog)
	//{{AFX_MSG_MAP(CBGNDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBGNDlg message handlers








/////////////////////////////////////////////////////////////////////////////
// CPermDlg dialog


CPermDlg::CPermDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPermDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPermDlg)
	m_Permittivity = 0;
	//}}AFX_DATA_INIT
}

void CPermDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPermDlg)
	DDX_Text(pDX, IDC_PERMITTIVITY, m_Permittivity);
	DDV_MinMaxDouble(pDX, m_Permittivity, 1., 100.);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPermDlg, CDialog)
	//{{AFX_MSG_MAP(CPermDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPermDlg message handlers

/////////////////////////////////////////////////////////////////////////////
// CMatRecomb dialog


CMatRecomb::CMatRecomb(CWnd* pParent /*=NULL*/)
	: CDialog(CMatRecomb::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMatRecomb)
	m_BB = 0;
	m_Cn = 0;
	m_Cnp = 0;
	m_Cp = 0;
	m_BulkNalphaN = 0;
	m_BulkNalphaP = 0;
	m_BulkNrefN = 0;
	m_BulkNrefP = 0;
	m_BulkTalpha = 0;
	m_SurfNalphaN = 0;
	m_SurfNalphaP = 0;
	m_SurfNrefN = 0;
	m_SurfNrefP = 0;
	m_SurfTalpha = 0;
	//}}AFX_DATA_INIT
}

void CMatRecomb::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMatRecomb)
	DDX_Text(pDX, IDC_BB, m_BB);
	DDV_MinMaxDouble(pDX, m_BB, 0., 1.e+100);
	DDX_Text(pDX, IDC_CN, m_Cn);
	DDV_MinMaxDouble(pDX, m_Cn, 0., 1.e100);
	DDX_Text(pDX, IDC_CNP, m_Cnp);
	DDV_MinMaxDouble(pDX, m_Cnp, 0., 1.e100);
	DDX_Text(pDX, IDC_CP, m_Cp);
	DDV_MinMaxDouble(pDX, m_Cp, 0., 1.e100);
	DDX_Text(pDX, IDC_BULKNALPHAN, m_BulkNalphaN);
	DDV_MinMaxDouble(pDX, m_BulkNalphaN, -10., 10.);
	DDX_Text(pDX, IDC_BULKNALPHAP, m_BulkNalphaP);
	DDV_MinMaxDouble(pDX, m_BulkNalphaP, -10., 10.);
	DDX_Text(pDX, IDC_BULKNREFN, m_BulkNrefN);
	DDV_MinMaxDouble(pDX, m_BulkNrefN, 1., 1.e030);
	DDX_Text(pDX, IDC_BULKNREFP, m_BulkNrefP);
	DDV_MinMaxDouble(pDX, m_BulkNrefP, 1., 1.e030);
	DDX_Text(pDX, IDC_BULKTALPHA, m_BulkTalpha);
	DDV_MinMaxDouble(pDX, m_BulkTalpha, -10., 10.);
	DDX_Text(pDX, IDC_SURFNALPHAN, m_SurfNalphaN);
	DDV_MinMaxDouble(pDX, m_SurfNalphaN, -10., 10.);
	DDX_Text(pDX, IDC_SURFNALPHAP, m_SurfNalphaP);
	DDV_MinMaxDouble(pDX, m_SurfNalphaP, -10., 10.);
	DDX_Text(pDX, IDC_SURFNREFN, m_SurfNrefN);
	DDV_MinMaxDouble(pDX, m_SurfNrefN, 1., 1.e030);
	DDX_Text(pDX, IDC_SURFNREFP, m_SurfNrefP);
	DDV_MinMaxDouble(pDX, m_SurfNrefP, 1., 1.e030);
	DDX_Text(pDX, IDC_SURFTALPHA, m_SurfTalpha);
	DDV_MinMaxDouble(pDX, m_SurfTalpha, -10., 10.);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMatRecomb, CDialog)
	//{{AFX_MSG_MAP(CMatRecomb)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMatRecomb message handlers

/////////////////////////////////////////////////////////////////////////////
// CParasiticsDlg dialog


CParasiticsDlg::CParasiticsDlg()
	: CPropertyPage(CParasiticsDlg::IDD)
{
	//{{AFX_DATA_INIT(CParasiticsDlg)
	m_nCoeff = 0;
	m_nLambdaPower = 0;
	m_pCoeff = 0;
	m_pLambdaPower = 0;
	m_Enable = FALSE;
	//}}AFX_DATA_INIT
}

void CParasiticsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CParasiticsDlg)
	DDX_Text(pDX, IDC_NCOEFF, m_nCoeff);
	DDX_Text(pDX, IDC_NLAMBDAPOWER, m_nLambdaPower);
	DDV_MinMaxDouble(pDX, m_nLambdaPower, 0., 100.);
	DDX_Text(pDX, IDC_PCOEFF, m_pCoeff);
	DDX_Text(pDX, IDC_PLAMBDAPOWER, m_pLambdaPower);
	DDV_MinMaxDouble(pDX, m_pLambdaPower, 0., 100.);
	DDX_Check(pDX, IDC_ENABLE, m_Enable);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CParasiticsDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CParasiticsDlg)
	ON_BN_CLICKED(IDC_ENABLE, DoDisabling)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CParasiticsDlg message handlers

BOOL CParasiticsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	SymbolFont.CreateFont(32,0,0,0,FW_NORMAL,0,0,0,SYMBOL_CHARSET,OUT_TT_PRECIS,
		CLIP_TT_ALWAYS, PROOF_QUALITY, VARIABLE_PITCH|TMPF_TRUETYPE|FF_DONTCARE, 
		"Symbol");
	SendDlgItemMessage(IDC_STATICGREEK1, WM_SETFONT, (WORD)(SymbolFont.GetSafeHandle()), FALSE);
	SendDlgItemMessage(IDC_STATICGREEK2, WM_SETFONT, (WORD)(SymbolFont.GetSafeHandle()), FALSE);

	DoDisabling();
	return TRUE;
}

CParasiticsDlg::~CParasiticsDlg(void)
{
	SymbolFont.DeleteObject();
}

void CParasiticsDlg::DoDisabling()
{
	BOOL bEnabled=((CButton*)GetDlgItem(IDC_ENABLE))->GetCheck();

	int GlobalItems[]={IDC_STATIC1, IDC_STATIC2, IDC_STATIC3, IDC_STATIC4,
		IDC_STATICGREEK1, IDC_STATICGREEK2,
		IDC_NCOEFF, IDC_NLAMBDAPOWER, IDC_PCOEFF,IDC_PLAMBDAPOWER, 0};
		
	int i;
	for (i=0; GlobalItems[i]!=0; i++) GetDlgItem(GlobalItems[i])->EnableWindow(bEnabled);
}

