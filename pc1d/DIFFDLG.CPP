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
#include "math.h"
#include "mathstat.h"
#include "physics.h"	// for CPhysics
#include "device.h"		// for CMobility
#include "diffdlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDiffDlg dialog


CDiffDlg::CDiffDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDiffDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDiffDlg)
	m_Depth = 0;
	m_Xpeak = 0;
	m_Profile = -1;
	m_Type = -1;
	m_Enable = FALSE;
	m_Junction = 0;
	m_SheetRho = 0;
	m_BkgndDoping = 0;
	m_Npeak = 0;
	//}}AFX_DATA_INIT
}

void CDiffDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDiffDlg)
	DDX_Text(pDX, IDC_DEPTH, m_Depth);
	DDV_MinMaxDouble(pDX, m_Depth, 1.e-003, 10000.);
	DDX_Text(pDX, IDC_XPEAK, m_Xpeak);
	DDV_MinMaxDouble(pDX, m_Xpeak, -10000., 10000.);
	DDX_Radio(pDX, IDC_UNIFORM, m_Profile);
	DDX_Radio(pDX, IDC_NTYPE, m_Type);
	DDX_Check(pDX, IDC_ENABLE, m_Enable);
	DDX_Text(pDX, IDC_JUNCTION, m_Junction);
	DDX_Text(pDX, IDC_SHEETRHO, m_SheetRho);
	DDX_ScientificDouble(pDX, IDC_BKDOPING, m_BkgndDoping);
	DDX_ScientificDouble(pDX, IDC_NPEAK, m_Npeak);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDiffDlg, CDialog)
	//{{AFX_MSG_MAP(CDiffDlg)
	ON_BN_CLICKED(IDC_NTYPE, OnUpdate)
	ON_EN_CHANGE(IDC_DEPTH, SetUpdate)
	ON_EN_KILLFOCUS(IDC_NPEAK, OnKillfocus)
	ON_EN_KILLFOCUS(IDC_SHEETRHO, OnKillfocusSheetrho)
	ON_EN_KILLFOCUS(IDC_JUNCTION, OnKillfocusJunction)
	ON_BN_CLICKED(IDC_ENABLE, DoDisabling)
	ON_BN_CLICKED(IDC_PTYPE, OnUpdate)
	ON_EN_CHANGE(IDC_NPEAK, SetUpdate)
	ON_EN_CHANGE(IDC_XPEAK, SetUpdate)
	ON_BN_CLICKED(IDC_ERFC, OnUpdate)
	ON_BN_CLICKED(IDC_EXPONENTIAL, OnUpdate)
	ON_BN_CLICKED(IDC_GAUSSIAN, OnUpdate)
	ON_BN_CLICKED(IDC_UNIFORM, OnUpdate)
	ON_EN_KILLFOCUS(IDC_XPEAK, OnKillfocus)
	ON_EN_KILLFOCUS(IDC_DEPTH, OnKillfocus)
	ON_EN_CHANGE(IDC_SHEETRHO, SetUpdate)
	ON_EN_CHANGE(IDC_JUNCTION, SetUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDiffDlg message handlers

double CDiffDlg::Mobility(double N)
{
	BOOL isFixed=pMat->m_FixedMobility;
	if (m_Type==N_TYPE) 
		return isFixed  ?  pElec->Fixed  :
			 CPhysics::Mobility(N, pElec->Max, pElec->MajMin,
			pElec->MajNref, pElec->MajAlpha, pElec->B1, pElec->B2,
			pElec->B3, pElec->B4, 300);
	else
		return isFixed   ?  pHole->Fixed  :
			 CPhysics::Mobility(N, pHole->Max, pHole->MajMin,
			pHole->MajNref, pHole->MajAlpha, pHole->B1, pHole->B2,
			pHole->B3, pHole->B4, 300);
}

double CDiffDlg::MatchDoping(double x)
{
	return log(CPhysics::Doping(x,(CProfile)m_Profile,m_Npeak,m_Depth,m_Xpeak)) 
			- log(m_BkgndDoping);
}

double CDiffDlg::Junction()
{
	if (m_Npeak<=m_BkgndDoping) return 0;
	if (m_Profile==UNIFORM_PROFILE) return (m_Xpeak+m_Depth);
	double xj = m_Xpeak + 2*m_Depth;	// starting point for search
	FIND_ZERO(MatchDoping, xj)
	return xj;
}

double CDiffDlg::SheetRho()
{
	const int num_elements = 50;
	double xj = m_Junction;
	if (xj<=0) return 0;
	double x, sum=0, delta=xj/num_elements, N;
	BeginWaitCursor();
	x = delta/2;
	for (int k=0; k<num_elements; k++)
	{
		N = CPhysics::Doping(x, (CProfile)m_Profile, m_Npeak, m_Depth, m_Xpeak);
		sum += Mobility(N)*N;
		x += delta;
	}
	EndWaitCursor();
	return 1/(1e-4*delta*Q*sum);
}

double CDiffDlg::MatchJunction(double xD)
{
	return log(CPhysics::Doping(m_Junction,(CProfile)m_Profile,m_Npeak,xD,m_Xpeak)) 
			- log(m_BkgndDoping);
}	

double CDiffDlg::MatchSheetRho(double logNs)
{
	const int num_elements = 50;
	double xj = m_Junction;
	if (xj<=0) return 0;
	double x, sum=0, delta=xj/num_elements, N;
	x = delta/2;
	for (int k=0; k<num_elements; k++)
	{
		N = CPhysics::Doping(x,(CProfile)m_Profile,CMath::exp(logNs),m_Depth,m_Xpeak);
		sum += 	Mobility(N)*N;
		x += delta;
	}
	return log(1/(1e-4*delta*Q*sum)) - log(m_SheetRho);
}
    
BOOL CDiffDlg::OnInitDialog()
{
	if (!m_title.IsEmpty()) SetWindowText(m_title);
	if (m_Enable)
	{
		m_Junction = CMath::round(Junction(),4);
		m_SheetRho = CMath::round(SheetRho(),4);
	}
	m_NeedUpdate = FALSE;
	CDialog::OnInitDialog();	// moved here to set Junction, SheetRho when enabled
	DoDisabling();
	return TRUE;
}

void CDiffDlg::OnUpdate()
{
	UpdateData(TRUE);
	m_Junction = CMath::round(Junction(),4);
	m_SheetRho = CMath::round(SheetRho(),4);
	if (m_SheetRho==0) AfxMessageBox("Peak doping must exceed the background doping.");
	UpdateData(FALSE);
}

void CDiffDlg::SetUpdate()
{m_NeedUpdate = TRUE;}

void CDiffDlg::OnKillfocus()
{
	if (!m_NeedUpdate) return; else m_NeedUpdate = FALSE;
	OnUpdate();
}

void CDiffDlg::OnKillfocusSheetrho()
{
	if (!m_NeedUpdate) return; else m_NeedUpdate = FALSE;
	UpdateData(TRUE);
	BeginWaitCursor();
	double logNs = log(1e20);	// starting point for search
	FIND_ZERO(MatchSheetRho, logNs)
	m_Npeak = CMath::round(CMath::exp(logNs),4);
	m_Junction = CMath::round(Junction(),4);
	m_SheetRho = CMath::round(SheetRho(),4);
	UpdateData(FALSE);
	EndWaitCursor();
}	

void CDiffDlg::OnKillfocusJunction()
{
	if (!m_NeedUpdate) return; else m_NeedUpdate = FALSE;
	UpdateData(TRUE);
	if (m_Junction<=m_Xpeak) 
	{
		AfxMessageBox("Junction depth must exceed peak position",
						MB_OK || MB_ICONINFORMATION); 
		OnUpdate();
		return;
	}
	if (m_Profile==UNIFORM_PROFILE)
	{
		m_Depth = m_Junction-m_Xpeak;
	}
	else
	{
		double xD = (m_Junction-m_Xpeak)/2;	// starting point for search
		FIND_ZERO(MatchJunction, xD)
		m_Depth = CMath::round(xD,4);
	}
	m_Junction = CMath::round(Junction(),4);
	m_SheetRho = CMath::round(SheetRho(),4);
	UpdateData(FALSE);
}

void CDiffDlg::DoDisabling()
{
	BOOL b=((CButton*)GetDlgItem(IDC_ENABLE))->GetCheck();
	
	int Items[]= {IDC_NTYPE, IDC_PTYPE,IDC_NPEAK, IDC_DEPTH,IDC_XPEAK,
//	IDC_UNIFORM, IDC_EXPONENTIAL, IDC_GAUSSIAN, IDC_ERFC,
//  disabling the above causes a crash when initializing if diffusion is not disabled
	IDC_JUNCTION,IDC_SHEETRHO,IDC_BKDOPING,
	IDC_STATIC1,IDC_STATIC2,IDC_STATIC3,IDC_STATIC4,IDC_STATIC5,IDC_STATIC6,IDC_STATIC7,
	IDC_STATIC_B1,IDC_STATIC_B2,IDC_STATIC_B3,IDC_STATIC_B4,IDC_STATIC_B5,IDC_STATIC_B6,IDC_STATIC_B7,
	0};
	int i;
	for (i=0; Items[i]!=0;  i++) GetDlgItem(Items[i])->EnableWindow(b);	
	if (b) OnUpdate();
}
