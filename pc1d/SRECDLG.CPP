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
#include "mathstat.h"	// CMath
#include "physics.h"	// CPhysics
#include "srecdlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSrecDlg dialog


CSrecDlg::CSrecDlg(UINT tabnameID)
	: CPropertyPage(CSrecDlg::IDD, tabnameID)
{
	//{{AFX_DATA_INIT(CSrecDlg)
	m_Et = 0;
	m_Jo = 0;
	m_TypeStr = "";
	m_UseJo = FALSE;
	m_Sn = 0;
	m_Sp = 0;
	m_Doping = 0;
	m_LLIs = 0;
	//}}AFX_DATA_INIT
}

void CSrecDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSrecDlg)
	DDX_Text(pDX, IDC_ET, m_Et);
	DDV_MinMaxDouble(pDX, m_Et, -20., 20.);
	DDX_Text(pDX, IDC_JO, m_Jo);
	DDX_Text(pDX, IDC_TYPE, m_TypeStr);
	DDX_Check(pDX, IDC_USEJO, m_UseJo);
	DDX_ScientificDouble(pDX, IDC_SN, m_Sn);
	DDX_ScientificDouble(pDX, IDC_SP, m_Sp);
	DDX_ScientificDouble(pDX, IDC_DOPING, m_Doping);
	DDX_ScientificDouble(pDX, IDC_LLIS, m_LLIs);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSrecDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CSrecDlg)
	ON_EN_CHANGE(IDC_ET, OnChange)
	ON_EN_KILLFOCUS(IDC_ET, OnKillfocus)
	ON_EN_KILLFOCUS(IDC_JO, OnKillfocusJo)
	ON_EN_KILLFOCUS(IDC_LLIS, OnKillfocusLLIs)
	ON_EN_CHANGE(IDC_JO, OnChange)
	ON_EN_CHANGE(IDC_LLIS, OnChange)
	ON_EN_CHANGE(IDC_SN, OnChange)
	ON_EN_CHANGE(IDC_SP, OnChange)
	ON_EN_KILLFOCUS(IDC_SN, OnKillfocus)
	ON_EN_KILLFOCUS(IDC_SP, OnKillfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSrecDlg message handlers

BOOL CSrecDlg::OnInitDialog()
{
	if (!m_title.IsEmpty()) SetWindowText(m_title);
	if (m_Type==N_TYPE) m_TypeStr = "n-type"; else m_TypeStr = "p-type";
	double Tref = 300;
	double Sn = CPhysics::S(m_Sn,m_Doping,m_Nref,m_Nalpha,Tref,m_Talpha);
	double Sp = CPhysics::S(m_Sp,m_Doping,m_Nref,m_Nalpha,Tref,m_Talpha);
	m_LLIs =  CMath::round(S(Sn,Sp,Tref),4);
	m_Jo =  CMath::round(Jo(Sn,Sp,Tref),4);
	m_NeedUpdate = FALSE;
	CDialog::OnInitDialog();
	return TRUE;	
}

void CSrecDlg::OnChange()
{m_NeedUpdate = TRUE;}

void CSrecDlg::OnKillfocus()
{
	UpdateData(TRUE);
	if (!m_NeedUpdate) return;
	double Tref = 300;
	double Sn = CPhysics::S(m_Sn,m_Doping,m_Nref,m_Nalpha,Tref,m_Talpha);
	double Sp = CPhysics::S(m_Sp,m_Doping,m_Nref,m_Nalpha,Tref,m_Talpha);
	m_LLIs =  CMath::round(S(Sn,Sp,Tref),4);
	m_Jo =  CMath::round(Jo(Sn,Sp,Tref),4);
	m_NeedUpdate = FALSE;
	UpdateData(FALSE);
}

void CSrecDlg::OnKillfocusJo()
{
	UpdateData(TRUE);
	if (!m_NeedUpdate) return;
	if (m_Jo<=0)
	{
		m_Sn = 0; m_Sp = 0;
	}
	else
	{
		double logS = log((double)1000);	// starting point for search
		FIND_ZERO(Match_Jo, logS)
		m_Sn = m_Sp = CMath::round(CMath::exp(logS),4);
	}
	UpdateData(FALSE);
	OnKillfocus();
}

void CSrecDlg::OnKillfocusLLIs()
{
	UpdateData(TRUE);
	if (!m_NeedUpdate) return;
	if (m_LLIs<=0)
	{
		m_Sn = 0; m_Sp = 0;
	}
	else
	{
		double logS = log((double)1000);	// starting point for search
		FIND_ZERO(Match_S, logS)
		m_Sn = m_Sp = CMath::round(CMath::exp(logS),4);
	}
	UpdateData(FALSE);
	OnKillfocus();
}

double CSrecDlg::Recomb(double Sn, double Sp, double T,
				double& neq, double& peq, double& delta)
{
	double ni, Vt, BGNc, BGNv, Psi, n, p;
	ni = CPhysics::ni(m_ni200, m_ni300, m_ni400, T);
	Vt = VT300*T/300;
	if (m_Type==N_TYPE)
	{
		BGNc = CPhysics::BGN(m_Doping,m_BGNnNref,m_BGNnSlope);
		BGNv = 0;
		Psi = CPhysics::Neutral_Eq_Psi(ni, m_Doping, BGNc, BGNv, T);
	}
	else
	{
		BGNc = 0;
		BGNv = CPhysics::BGN(m_Doping,m_BGNpNref,m_BGNpSlope);
		Psi = CPhysics::Neutral_Eq_Psi(ni,-m_Doping, BGNc, BGNv, T);
	}
	neq = CPhysics::neq(ni, Psi, BGNc, T);
	peq = CPhysics::peq(ni, Psi, BGNv, T);
	if (m_Type==N_TYPE) delta = 0.001*peq; else delta = 0.001*neq;
	n = neq + delta; p = peq + delta;
	return CPhysics::Surf_Recomb(n, p, sqrt(neq*peq), Sn, Sp, m_Et/Vt, m_UseJo);
}

double CSrecDlg::Jo(double Sn, double Sp, double T)
{
	double neq, peq, delta;
	return Recomb(Sn, Sp, T, neq, peq, delta)*(peq*neq)*Q
			/(delta*(peq+neq+delta));
}

double CSrecDlg::S(double Sn, double Sp, double T)
{
	double neq, peq, delta;
	return Recomb(Sn, Sp, T, neq, peq, delta)/delta;
}

double CSrecDlg::Match_S(double logS)
{
	double Tref = 300;
	double Sn = CPhysics::S(CMath::exp(logS),m_Doping,m_Nref,m_Nalpha,Tref,m_Talpha);
	double Sp = Sn;
	return log(S(Sn,Sp,Tref)) - log(m_LLIs);
}

double CSrecDlg::Match_Jo(double logS)
{
	double Tref = 300;
	double Sn = CPhysics::S(CMath::exp(logS),m_Doping,m_Nref,m_Nalpha,Tref,m_Talpha);
	double Sp = Sn;
	return log(Jo(Sn,Sp,Tref)) - log(m_Jo);
}

