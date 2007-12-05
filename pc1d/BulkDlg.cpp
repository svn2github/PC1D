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
#include "bulkdlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBulkDlg dialog


CBulkDlg::CBulkDlg()
	: CPropertyPage(CBulkDlg::IDD)
{
	//{{AFX_DATA_INIT(CBulkDlg)
	m_DiffLength = 0;
	m_Et = 0;
	m_TypeStr = "";
	m_TauN = 0;
	m_TauP = 0;
	m_BkgndDoping = 0;
	m_LLItau = 0;
	//}}AFX_DATA_INIT
}

void CBulkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBulkDlg)
	DDX_Text(pDX, IDC_DIFFLENGTH, m_DiffLength);
	DDX_Text(pDX, IDC_ET, m_Et);
	DDV_MinMaxDouble(pDX, m_Et, -20., 20.);
	DDX_Text(pDX, IDC_TYPE, m_TypeStr);
	DDX_ScientificDouble(pDX, IDC_TAUN, m_TauN);
	DDX_ScientificDouble(pDX, IDC_TAUP, m_TauP);
	DDX_ScientificDouble(pDX, IDC_BKGNDDOPING, m_BkgndDoping);
	DDX_ScientificDouble(pDX, IDC_LLITAU, m_LLItau);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBulkDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CBulkDlg)
	ON_EN_CHANGE(IDC_DIFFLENGTH, OnChange)
	ON_EN_KILLFOCUS(IDC_LLITAU, OnLLItau)
	ON_EN_KILLFOCUS(IDC_DIFFLENGTH, OnDiffLength)
	ON_EN_CHANGE(IDC_ET, OnChange)
	ON_EN_CHANGE(IDC_LLITAU, OnChange)
	ON_EN_CHANGE(IDC_TAUN, OnChange)
	ON_EN_CHANGE(IDC_TAUP, OnChange)
	ON_EN_KILLFOCUS(IDC_ET, OnKillfocus)
	ON_EN_KILLFOCUS(IDC_TAUN, OnKillfocus)
	ON_EN_KILLFOCUS(IDC_TAUP, OnKillfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBulkDlg message handlers

BOOL CBulkDlg::OnInitDialog()
{
	if (m_Type==N_TYPE) m_TypeStr = "n-type"; else m_TypeStr = "p-type";
	double Tref = 300;
	double Vt = VT300;
	double taun = 
		CPhysics::tau(1e-6*m_TauN,m_BkgndDoping,m_Nref,m_Nalpha,Tref,m_Talpha);
	double taup =
		CPhysics::tau(1e-6*m_TauP,m_BkgndDoping,m_Nref,m_Nalpha,Tref,m_Talpha);
	m_LLItau =  (1e6 * CMath::round(Tau(taun,taup,Tref),4));	// s to us
	m_DiffLength = 	// cm to um
		(1e4 * CMath::round(sqrt(Mobility(m_BkgndDoping,Tref)*Vt*Tau(taun,taup,Tref)),4));
	m_NeedUpdate = FALSE;
	CDialog::OnInitDialog();
	return TRUE;
}

void CBulkDlg::OnChange()
{m_NeedUpdate = TRUE;}

void CBulkDlg::OnKillfocus()
{
	UpdateData(TRUE);
	if (!m_NeedUpdate) return;
	double Tref = 300;
	double Vt = VT300;
	double taun =
		CPhysics::tau(1e-6*m_TauN,m_BkgndDoping,m_Nref,m_Nalpha,Tref,m_Talpha);
	double taup =
		CPhysics::tau(1e-6*m_TauP,m_BkgndDoping,m_Nref,m_Nalpha,Tref,m_Talpha);
	m_LLItau =  (1e6 * CMath::round(Tau(taun,taup,Tref),4));	// s to us
	m_DiffLength =  	// cm to um
		(1e4 * CMath::round(sqrt(Mobility(m_BkgndDoping,Tref)*Vt*Tau(taun,taup,Tref)),4));
	m_NeedUpdate = FALSE;
	UpdateData(FALSE);
}

double CBulkDlg::MatchLLItau(double logtau)
{
	double Tref = 300;
	double taun =
		CPhysics::tau(CMath::exp(logtau),m_BkgndDoping,m_Nref,m_Nalpha,Tref,m_Talpha);
	double taup = taun;
	return log(Tau(taun,taup,Tref)) - log(1e-6*m_LLItau);
}

void CBulkDlg::OnLLItau()
{
	UpdateData(TRUE);
	if (!m_NeedUpdate) return;
	double Tref = 300;
	double taun =	// value for reference taun = 1 second
		CPhysics::tau(1,m_BkgndDoping,m_Nref,m_Nalpha,Tref,m_Talpha);
	double taup = taun;
	double logtau = log(1e-6);			// starting guess for search
	FIND_ZERO(MatchLLItau, logtau)
	m_TauN = m_TauP = CMath::round(1e6*CMath::exp(logtau),4);	// s to us
// DAC 3/7/96 Quick & dirty bugfix.
// Have to ensure that m_TauN and m_TauP are in allowable range! (1e6 to 1e-6)
if (m_TauN>1e6) m_TauN=1e6;
if (m_TauP>1e6) m_TauP=1e6;
if (m_TauN<1e-6) m_TauN=1e-6;
if (m_TauP<1e-6) m_TauP=1e-6;
	
	UpdateData(FALSE);
	OnKillfocus();
}

void CBulkDlg::OnDiffLength()
{
	UpdateData(TRUE);
	if (!m_NeedUpdate) return;
	double Tref = 300;
	double Vt = VT300; 
	double D = Vt*Mobility(m_BkgndDoping,Tref);
	m_LLItau = (1e6*pow(1e-4*m_DiffLength,2)/D);
	UpdateData(FALSE);
	OnLLItau();
}

double CBulkDlg::Mobility(double N, double T)
{
	if (m_Type==P_TYPE)
		return pMat->m_FixedMobility?pElec->Fixed:CPhysics::Mobility(N, pElec->Max, pElec->MinMin,
			pElec->MinNref, pElec->MinAlpha, pElec->B1, pElec->B2,
			pElec->B3, pElec->B4, T);
	else
		return pMat->m_FixedMobility?pHole->Fixed:CPhysics::Mobility(N, pHole->Max, pHole->MinMin,
			pHole->MinNref, pHole->MinAlpha, pHole->B1, pHole->B2,
			pHole->B3, pHole->B4, T);
}

double CBulkDlg::Tau(double taun, double taup, double T)
{
	double ni, Vt, BGNc, BGNv, Psi, n, p, bulk_recomb;
	ni = CPhysics::ni(m_ni200,m_ni300,m_ni400,T); 
	Vt = VT300*T/300;
	if (m_Type==N_TYPE)
	{
		BGNc = CPhysics::BGN(m_BkgndDoping,m_BGNnNref,m_BGNnSlope);
		BGNv = 0;
		Psi = CPhysics::Neutral_Eq_Psi(ni, m_BkgndDoping, BGNc, BGNv, T);
	}
	else
	{
		BGNc = 0;
		BGNv = CPhysics::BGN(m_BkgndDoping,m_BGNpNref,m_BGNpSlope);
		Psi = CPhysics::Neutral_Eq_Psi(ni,-m_BkgndDoping, BGNc, BGNv, T);
	}
	double neq = CPhysics::neq(ni, Psi, BGNc, T);
	double peq = CPhysics::peq(ni, Psi, BGNv, T);
	if (m_Type==N_TYPE)
	{
	    n = neq; p = 1.01*peq;
		bulk_recomb = CPhysics::Bulk_Recomb(n, p, sqrt(neq*peq),
				taun, taup, m_Et/Vt, m_BkgndDoping, 0, m_Cn, m_Cp, m_Cnp, m_B);
		if (bulk_recomb==0) return 0; else return 0.01*peq/bulk_recomb;
	}
	else
    {
    	n = 1.01*neq; p = peq;
		bulk_recomb = CPhysics::Bulk_Recomb(n, p, sqrt(neq*peq),
				taun, taup, m_Et/Vt, 0, m_BkgndDoping, m_Cn, m_Cp, m_Cnp, m_B);
		if (bulk_recomb==0) return 0; else return 0.01*neq/bulk_recomb;
	}   	
}
