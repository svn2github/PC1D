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

#include "stdafx.h"
#include "resource.h"
#include "matrec.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMatRecDoping dialog


CMatRecDoping::CMatRecDoping()
	: CPropertyPage(CMatRecDoping::IDD)
{
	//{{AFX_DATA_INIT(CMatRecDoping)
	m_BulkNalphaN = 0;
	m_BulkNalphaP = 0;
	m_SurfNalphaN = 0;
	m_SurfNalphaP = 0;
	m_BulkNrefN = 0;
	m_BulkNrefP = 0;
	m_SurfNrefN = 0;
	m_SurfNrefP = 0;
	//}}AFX_DATA_INIT
}

void CMatRecDoping::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMatRecDoping)
	DDX_Text(pDX, IDC_BULKNALPHAN, m_BulkNalphaN);
	DDV_MinMaxDouble(pDX, m_BulkNalphaN, -10., 10.);
	DDX_Text(pDX, IDC_BULKNALPHAP, m_BulkNalphaP);
	DDV_MinMaxDouble(pDX, m_BulkNalphaP, -10., 10.);
	DDX_Text(pDX, IDC_SURFNALPHAN, m_SurfNalphaN);
	DDV_MinMaxDouble(pDX, m_SurfNalphaN, -10., 10.);
	DDX_Text(pDX, IDC_SURFNALPHAP, m_SurfNalphaP);
	DDV_MinMaxDouble(pDX, m_SurfNalphaP, -10., 10.);
	DDX_ScientificDouble(pDX, IDC_BULKNREFN, m_BulkNrefN);
	DDV_MinMaxSciDouble(pDX, m_BulkNrefN, 1., 1.e+030);
	DDX_ScientificDouble(pDX, IDC_BULKNREFP, m_BulkNrefP);
	DDV_MinMaxSciDouble(pDX, m_BulkNrefP, 1., 1.e+030);
	DDX_ScientificDouble(pDX, IDC_SURFNREFN, m_SurfNrefN);
	DDV_MinMaxSciDouble(pDX, m_SurfNrefN, 1., 1.e+030);
	DDX_ScientificDouble(pDX, IDC_SURFNREFP, m_SurfNrefP);
	DDV_MinMaxSciDouble(pDX, m_SurfNrefP, 1., 1.e+030);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMatRecDoping, CPropertyPage)
	//{{AFX_MSG_MAP(CMatRecDoping)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMatRecDoping message handlers  



/////////////////////////////////////////////////////////////////////////////
// CMatRecAuger dialog


CMatRecAuger::CMatRecAuger()
	: CPropertyPage(CMatRecAuger::IDD)
{
	//{{AFX_DATA_INIT(CMatRecAuger)
	m_BB = 0;
	m_Cn = 0;
	m_Cnp = 0;
	m_Cp = 0;
	//}}AFX_DATA_INIT
}

void CMatRecAuger::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMatRecAuger)
	DDX_ScientificDouble(pDX, IDC_BB, m_BB);
	DDV_MinMaxSciDouble(pDX, m_BB, 0., 1.e+100);
	DDX_ScientificDouble(pDX, IDC_CN, m_Cn);
	DDV_MinMaxSciDouble(pDX, m_Cn, 0., 1.e+100);
	DDX_ScientificDouble(pDX, IDC_CNP, m_Cnp);
	DDV_MinMaxSciDouble(pDX, m_Cnp, 0., 1.e+100);
	DDX_ScientificDouble(pDX, IDC_CP, m_Cp);
	DDV_MinMaxSciDouble(pDX, m_Cp, 0., 1.e+100);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMatRecAuger, CPropertyPage)
	//{{AFX_MSG_MAP(CMatRecAuger)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMatRecAuger message handlers
/////////////////////////////////////////////////////////////////////////////
// CMatRecTemperature dialog


CMatRecTemperature::CMatRecTemperature()
	: CPropertyPage(CMatRecTemperature::IDD)
{
	//{{AFX_DATA_INIT(CMatRecTemperature)
	m_BulkTalpha = 0;
	m_SurfTalpha = 0;
	//}}AFX_DATA_INIT
}

void CMatRecTemperature::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMatRecTemperature)
	DDX_Text(pDX, IDC_BULKTALPHA, m_BulkTalpha);
	DDV_MinMaxDouble(pDX, m_BulkTalpha, -10., 10.);
	DDX_Text(pDX, IDC_SURFTALPHA, m_SurfTalpha);
	DDV_MinMaxDouble(pDX, m_SurfTalpha, -10., 10.);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMatRecTemperature, CPropertyPage)
	//{{AFX_MSG_MAP(CMatRecTemperature)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMatRecTemperature message handlers





/////////////////////////////////////////////////////////////////////////////
// CMatRecHurkx dialog


CMatRecHurkx::CMatRecHurkx()
	: CPropertyPage(CMatRecHurkx::IDD)
{
	//{{AFX_DATA_INIT(CMatRecHurkx)
	m_Fgamma = 0;
	m_prefactor = 0;
	m_Enable = FALSE;
	//}}AFX_DATA_INIT
}

BOOL CMatRecHurkx::OnInitDialog()
{
	CDialog::OnInitDialog();
	DoDisabling();
	return TRUE;
}


void CMatRecHurkx::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMatRecHurkx)
	DDX_Text(pDX, IDC_FGAMMA, m_Fgamma);
	DDX_Text(pDX, IDC_PREFACTOR, m_prefactor);
	DDX_Check(pDX, IDC_ENABLE, m_Enable);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMatRecHurkx, CPropertyPage)
	//{{AFX_MSG_MAP(CMatRecHurkx)
	ON_BN_CLICKED(IDC_ENABLE, DoDisabling)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMatRecHurkx message handlers

void CMatRecHurkx::DoDisabling() 
{
  	BOOL b1=((CButton*)GetDlgItem(IDC_ENABLE))->GetCheck();

	int Radio1items[]={IDC_FGAMMA,IDC_PREFACTOR,0};
	int i;
	for (i=0; Radio1items[i]!=0;  i++) GetDlgItem(Radio1items[i])->EnableWindow(b1);	
}
