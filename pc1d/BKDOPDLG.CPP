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

// PAB: Changed resistivity function to reflect whether fixed mobilities were active, or use variable model. 12/2/96

#include "stdafx.h"
#include "resource.h"
#include "math.h"
#include "mathstat.h"	// for CMath
#include "physics.h"	// for CPhysics
#include "device.h"		// for CMobility, CMaterial
#include "bkdopdlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBkDopDlg dialog


CBkDopDlg::CBkDopDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBkDopDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBkDopDlg)
	m_Type = -1;
	m_Resistivity = 0;
	m_ElecDif = 0;
	m_ElecMob = 0;
	m_HoleDif = 0;
	m_HoleMob = 0;
	m_Doping = 0;
	//}}AFX_DATA_INIT
}

void CBkDopDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBkDopDlg)
	DDX_Radio(pDX, IDC_NTYPE, m_Type);
	DDX_Text(pDX, IDC_RESISTIVITY, m_Resistivity);
	DDX_Text(pDX, IDC_EDIF, m_ElecDif);
	DDX_Text(pDX, IDC_EMOB, m_ElecMob);
	DDX_Text(pDX, IDC_HDIF, m_HoleDif);
	DDX_Text(pDX, IDC_HMOB, m_HoleMob);
	DDX_ScientificDouble(pDX, IDC_DOPING, m_Doping);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBkDopDlg, CDialog)
	//{{AFX_MSG_MAP(CBkDopDlg)
	ON_BN_CLICKED(IDC_NTYPE, OnNtype)
	ON_BN_CLICKED(IDC_PTYPE, OnPtype)
	ON_EN_KILLFOCUS(IDC_DOPING, OnKillfocusDoping)
	ON_EN_KILLFOCUS(IDC_RESISTIVITY, OnKillfocusResistivity)
	ON_EN_CHANGE(IDC_DOPING, OnChangeDoping)
	ON_EN_CHANGE(IDC_RESISTIVITY, OnChangeResistivity)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBkDopDlg message handlers

double CBkDopDlg::Resistivity(double N)
{
	double e_mobility, h_mobility, n, p;
	if (m_Type==N_TYPE) 
	{
		e_mobility = pMat->m_FixedMobility?pElec->Fixed:CPhysics::Mobility(N, pElec->Max, pElec->MajMin,
				pElec->MajNref, pElec->MajAlpha, pElec->B1, pElec->B2,
				pElec->B3, pElec->B4, 300);
		h_mobility = pMat->m_FixedMobility?pHole->Fixed:CPhysics::Mobility(N, pHole->Max, pHole->MinMin,
				pHole->MinNref, pHole->MinAlpha, pHole->B1, pHole->B2,
				pHole->B3, pHole->B4, 300);
		n = N/2 + sqrt(N*N+4*ni300*ni300)/2;
		p = ni300*ni300/n;
	}
	else
	{
		e_mobility = pMat->m_FixedMobility?pElec->Fixed:CPhysics::Mobility(N, pElec->Max, pElec->MinMin,
			pElec->MinNref, pElec->MinAlpha, pElec->B1, pElec->B2,
			pElec->B3, pElec->B4, 300);
		h_mobility = pMat->m_FixedMobility?pHole->Fixed:CPhysics::Mobility(N, pHole->Max, pHole->MajMin,
			pHole->MajNref, pHole->MajAlpha, pHole->B1, pHole->B2,
			pHole->B3, pHole->B4, 300);
		p = N/2 + sqrt(N*N+4*ni300*ni300)/2;
		n = ni300*ni300/p;
	}
	m_ElecMob = (float)CMath::round(e_mobility,4);
	m_ElecDif = (float)CMath::round(e_mobility*VT300,4);
	m_HoleMob = (float)CMath::round(h_mobility,4);
	m_HoleDif = (float)CMath::round(h_mobility*VT300,4);
	return CPhysics::Resistivity(n, e_mobility, p, h_mobility);
}

BOOL CBkDopDlg::OnInitDialog()
{
	m_Resistivity = (float)CMath::round(Resistivity(m_Doping),4);
	m_ChangeResistivity = FALSE;
	m_ChangeDoping = FALSE;
	CDialog::OnInitDialog();
	return TRUE;
}

void CBkDopDlg::OnNtype()
{
	UpdateData(TRUE);
	m_Resistivity = (float)CMath::round(Resistivity(m_Doping),4);
	UpdateData(FALSE);	
}

void CBkDopDlg::OnPtype()
{
	UpdateData(TRUE);
	m_Resistivity = (float)CMath::round(Resistivity(m_Doping),4);
	UpdateData(FALSE);
}

void CBkDopDlg::OnKillfocusDoping()
{
	UpdateData(TRUE);
	if (!m_ChangeDoping) return;
	m_Resistivity = (float)CMath::round(Resistivity(m_Doping),4);
	m_ChangeDoping = FALSE;
	UpdateData(FALSE);
}

double CBkDopDlg::MatchResistivity(double logN)
{
	return log(Resistivity(exp(logN)))-log(m_Resistivity);
}

void CBkDopDlg::OnKillfocusResistivity()
{
	UpdateData(TRUE);
	if (!m_ChangeResistivity) return;
	BeginWaitCursor();
	m_Doping=10*ni300;
	if (m_Resistivity>Resistivity(ni300)) 
		m_Resistivity=(float)CMath::round(Resistivity(ni300),4);
	double logdoping = log(m_Doping);
	FIND_ZERO(MatchResistivity, logdoping)
	m_Doping = (float)CMath::round(exp(logdoping),4);
	m_ChangeResistivity = FALSE;
	UpdateData(FALSE);
	EndWaitCursor();
}

void CBkDopDlg::OnChangeDoping()
{m_ChangeDoping = TRUE;}

void CBkDopDlg::OnChangeResistivity()
{m_ChangeResistivity = TRUE;}
