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
// MODS 14/12/95 DAC: Added HINT_ITERATION for graph displaying of non-converged
//					node data. {SolveSystem() + m_bGraphsAfterEveryIteration }	
// 4/5/03 DAC: Moved the region variables into a seperate CRegionData structure

#include "stdafx.h"
#include "ProgramLimits.h"
#include "PhysicalConstants.h"
#include "math.h"
#include "mathstat.h"	// CMath
#include "physics.h"	// CPhysics
#include "solve.h"
#include "numdlg.h"  
#include "useful.h"
#include <float.h>		// for _finite()
#include "hints.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// This module contains the most time-critical code in PC1D.
// To maximize the speed, we must ensure that noncritical code 
// is kept as small as possible, 
// otherwise the CPU cache will run out of space.

#pragma optimize("s", on) // the following code must be small


/////////////////////////////////////////////////////////////////////////////
// CSolve

IMPLEMENT_SERIAL(CSolve, CObject, 0)

CSolve::CSolve() : INIheading("Options"), INIstore("StoreNodes")
{
	Initialize();
//	m_bStoreNodes = (AfxGetApp()->GetProfileString(INIheading, INIstore)=="TRUE");
	m_bStoreNodes = FALSE;
}

CSolve::~CSolve()
{
}

void CSolve::Initialize()
{
	int k;
	node.Initialize();
	for (k=0; k<MAX_SOURCES; k++)
	{
		m_rIntSource[k] = 1;
		m_xSource[k] = m_vSource[k] = m_rSource[k] = 0; m_bSource[k] = TRUE;
		m_SourceV[k] = m_SourceI[k] = 0; 
	}
	for (k=0; k<MAX_LUMPED; k++)
	{
		m_tLumped[k] = CONDUCTOR; m_bLumped[k] = FALSE;
		m_vLumped[k] = 0; m_nLumped[k] = 1;
		m_xLumpedA[k] = m_xLumpedC[k] = 0;
		m_LumpedV[k] = m_LumpedI[k] = 0;
	}
//	for (k=0; k<MAX_REGIONS; k++) 
//	{
//		m_xRegionFront[k] = 0;
//		m_xRegionRear[k] = node.GetX(node.GetLastNode());
//		m_Cn[k] = m_Cp[k] = m_Cnp[k] = m_B[k] = m_Et[k] = m_Aff[k] = 0;
//		m_SnFront[k] = m_SpFront[k] = m_SnRear[k] = m_SpRear[k] = 0;
//		m_EtFront[k] = m_EtRear[k] = 0; m_JoFront[k] = m_JoRear[k] = FALSE;
//		m_Nc[k] = m_Nv[k] = 1;
//	}
	m_regiondata.Initialize(node.GetX(node.GetLastNode()));
	// Numeric dialog
	m_bUsePrevious = FALSE; m_SizeFactor = 0.5; m_ErrorLimit = 1e-9; 
	m_Clamp = 5; m_nMaxTime = 60; m_bClampPhi = TRUE; m_bClampPsi = TRUE;
	m_bRenodeEQ = TRUE; m_bRenodeSS = TRUE; m_bRenodeTR = FALSE;   
	m_bGraphsAfterEveryIteration = FALSE; m_bNewSolve = FALSE;
	m_bPhiSat = FALSE;
	// Other parameters
	m_FrontSurface = m_RearSurface = NEUTRAL_SURFACE;
	m_NssFront = m_NssRear = 0;
	m_rdt = 0; m_Error = 1E6; m_bVelSat = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CSolve serialization


void CSolve::Serialize(CArchive& ar)
{
	WORD ver_num;
	int k;
	if (ar.IsStoring())
	{
		ver_num=2;
		ar << (WORD)ver_num;
		ar << m_SizeFactor << m_ErrorLimit << m_Clamp << (WORD)m_nMaxTime;
		ar << (WORD)m_bClampPhi << (WORD)m_bClampPsi;
		ar << (WORD)m_bRenodeEQ << (WORD)m_bRenodeSS << (WORD)m_bRenodeTR;
		ar << (WORD)m_bGraphsAfterEveryIteration << (WORD)m_bPhiSat;		
		ar << (WORD)m_bStoreNodes;
		if (m_bStoreNodes) 
		{
			node.Serialize(ar);
			for (k=0; k<MAX_SOURCES; k++) 
			{
				ar << (WORD)m_bSource[k] << m_xSource[k] << m_vSource[k] << m_rSource[k] << m_rIntSource[k];
				ar << m_SourceV[k] << m_SourceI[k];
			}
			m_regiondata.Serialize(ar);
/***
			for (k=0; k<MAX_REGIONS; k++)
			{
				ar << m_xRegionFront[k] << m_xRegionRear[k] << m_regiondata.m_Cn[k] << m_regiondata.m_Cp[k] << m_regiondata.m_Cnp[k] << m_regiondata.m_B[k];
				ar << m_regiondata.m_Et[k] << m_regiondata.m_Eref[k] << m_regiondata.m_Egamma[k];
				ar << m_regiondata.m_SnFront[k] << m_regiondata.m_SpFront[k] << m_regiondata.m_EtFront[k] << (WORD)m_regiondata.m_JoFront[k];
				ar << m_regiondata.m_SnRear[k] << m_regiondata.m_SpRear[k] << m_regiondata.m_EtRear[k] << (WORD)m_regiondata.m_JoRear[k];
				ar << m_regiondata.m_Nc[k] << m_regiondata.m_Nv[k] << m_regiondata.m_Aff[k];
			}
***/
			for (k=0; k<MAX_LUMPED; k++)
			{
				ar << (WORD)m_bLumped[k] << m_xLumpedA[k] << m_xLumpedC[k] << m_nLumped[k] << m_vLumped[k];
				ar << m_LumpedV[k] << m_LumpedI[k];
			}
			ar << (WORD)m_FrontSurface << (WORD)m_RearSurface;
			ar << m_NssFront << m_NssRear;
		}
	}
	else
	{
		Initialize();
		ar >> (WORD &)ver_num;
		if (ver_num <1 || ver_num>2) 
		{
			AfxThrowArchiveException(CArchiveException::badIndex);
			return;
		}
		ar >> m_SizeFactor >> m_ErrorLimit >> m_Clamp >> (WORD&)m_nMaxTime;
		ar >> (WORD&)m_bClampPhi >> (WORD&)m_bClampPsi;
		ar >> (WORD&)m_bRenodeEQ >> (WORD&)m_bRenodeSS >> (WORD&)m_bRenodeTR;
		ar >> (WORD &)m_bGraphsAfterEveryIteration >> (WORD &)m_bPhiSat;
		ar >> (WORD&)m_bStoreNodes;
		if (m_bStoreNodes) 
		{
			node.Serialize(ar); 
			if (ver_num>1) 
			{
				for (k=0; k<MAX_SOURCES; k++) 
				{
					ar >> (WORD&)m_bSource[k] >> m_xSource[k] >> m_vSource[k] >> m_rSource[k] >> m_rIntSource[k];
					ar >> m_SourceV[k] >> m_SourceI[k];
				}
				m_regiondata.Serialize(ar);
/***
				for (k=0; k<MAX_REGIONS; k++)
				{
					ar >> m_xRegionFront[k] >> m_xRegionRear[k] >> m_regiondata.m_Cn[k] >> m_regiondata.m_Cp[k] >> m_regiondata.m_Cnp[k] >> m_regiondata.m_B[k];
					ar >> m_regiondata.m_Et[k] >> m_regiondata.m_Eref[k] >> m_regiondata.m_Egamma[k];
					ar >> m_regiondata.m_SnFront[k] >> m_regiondata.m_SpFront[k] >> m_regiondata.m_EtFront[k] >> (WORD&)m_regiondata.m_JoFront[k];
					ar >> m_regiondata.m_SnRear[k] >> m_regiondata.m_SpRear[k] >> m_regiondata.m_EtRear[k] >> (WORD&)m_regiondata.m_JoRear[k];
					ar >> m_regiondata.m_Nc[k] >> m_regiondata.m_Nv[k] >> m_regiondata.m_Aff[k];
				}
****/
				for (k=0; k<MAX_LUMPED; k++)
				{
					ar >> (WORD&)m_bLumped[k] >> m_xLumpedA[k] >> m_xLumpedC[k] >> m_nLumped[k] >> m_vLumped[k];
					ar >> m_LumpedV[k] >> m_LumpedI[k];
				}
				ar >> (WORD&)m_FrontSurface >> (WORD&)m_RearSurface;
				ar >> m_NssFront >> m_NssRear;
			}
		}
		else node.Initialize();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSolve functions that should be in CNode

/// returns the bulk recombination / nir
/// at node k, with the given n and p values.
/// (This flexibility is needed because GetTau() sometimes 
///  needs to modify n, p from their true values).
double CSolve::GetRBulk_np_specified(int k, double n_nir, double p_nir)
{
	double nie_nir;
	int reg;
	nie_nir= node.GetNie_nir(k);
	reg = node.GetRegion(k);
	CRegionData *pRegData = GetRegionData();
	if (pRegData->IsHurkxEnabledForRegion(reg)) {
		return CPhysics::Bulk_Recomb_Hurkx(n_nir, p_nir, nie_nir,
			node.GetTaun(k), node.GetTaup(k), pRegData->GetEt(reg),
			node.GetNd(k), node.GetNa(k), 
			pRegData->GetCn(reg), pRegData->GetCp(reg), pRegData->GetCnp(reg), pRegData->GetB(reg),
			node.GetE_Vt(k), pRegData->GetHurkxPrefactor(reg), pRegData->GetHurkxFgamma_Vt(reg)); 
	} else {
		return CPhysics::Bulk_Recomb(n_nir, p_nir, nie_nir,
			node.GetTaun(k), node.GetTaup(k), pRegData->GetEt(reg),
			node.GetNd(k), node.GetNa(k), 
			pRegData->GetCn(reg), pRegData->GetCp(reg), pRegData->GetCnp(reg), pRegData->GetB(reg));
	}
}

// get bulk recombination at this point in the device
double CSolve::GetBulkRecombination_nir(int k)
{
	return GetRBulk_np_specified(k, node.GetN_nir(k), node.GetP_nir(k));
}

/// DAC 28/5/03: Safer than testing equality.
inline bool OnePercentEqual(double a, double b)
{
	return (a>0.9999*b && a<1.0001*b);
}

/// minority carrier lifetime at node k.
double CSolve::GetTau(int k)
{
	double tau;
	// All quantities normalized to nir
	double n_, p_, neq, peq, bulkrec;

	n_ = node.GetN_nir(k);
	p_ = node.GetP_nir(k);
	neq = node.GetNeq_nir(k);
	peq = node.GetPeq_nir(k);

	double n, p;
	n=n_;
	p=p_;
	// DAC 28/5/03: Originally, the test was p_==peq. But in release mode,
	// p and peq could differ in the last decimal place!
	if (n_>p_) {	if (OnePercentEqual(p_, peq)) p_ = 1.01*peq;
	}else {			if (OnePercentEqual(n_, neq)) n_ = 1.01*neq;
	}
	bulkrec = GetRBulk_np_specified(k, n_, p_);

	if (n_>p_) {			tau=(p_-peq)/bulkrec;
	} else if (p_>n_) {		tau=(n_-neq)/bulkrec;
	} else					tau=0;
	return tau;
}

/////////////////////////////////////////////////////////////////////////////
// CSolve functions


void CSolve::StoreOptions()
{
	if (m_bStoreNodes)
		AfxGetApp()->WriteProfileString(INIheading,INIstore,"TRUE");
	else
		AfxGetApp()->WriteProfileString(INIheading,INIstore,"FALSE");
}

BOOL CSolve::SetNumericParameters()
{
	CNumDlg dlg;
	dlg.m_SizeFactor = m_SizeFactor;
	dlg.m_ErrorLimit = m_ErrorLimit;
	dlg.m_Clamp = m_Clamp;
	dlg.m_nMaxTime = m_nMaxTime;
	dlg.m_bClampPhi = m_bClampPhi;
	dlg.m_bClampPsi = m_bClampPsi;
	dlg.m_bRenodeEQ = m_bRenodeEQ;
	dlg.m_bRenodeSS = m_bRenodeSS;
	dlg.m_bRenodeTR = m_bRenodeTR;
	dlg.m_bGraphsEveryIteration = m_bGraphsAfterEveryIteration;
	dlg.m_bTotalSat = m_bPhiSat;
	if (dlg.DoModal()==IDOK)
	{
		m_SizeFactor = dlg.m_SizeFactor;
		m_ErrorLimit = dlg.m_ErrorLimit;
		m_Clamp = dlg.m_Clamp;
		m_nMaxTime = dlg.m_nMaxTime;
		m_bClampPhi = dlg.m_bClampPhi;
		m_bClampPsi = dlg.m_bClampPsi;
		m_bRenodeEQ = dlg.m_bRenodeEQ;
		m_bRenodeSS = dlg.m_bRenodeSS;
		m_bRenodeTR = dlg.m_bRenodeTR;
		m_bGraphsAfterEveryIteration = dlg.m_bGraphsEveryIteration;
		m_bPhiSat = dlg.m_bTotalSat;
		return TRUE;
	}
	else return FALSE;
}

void CSolve::SetFixedNodes()
{
	int i=0, k=0;
	for (i=0; i<FIXED_NODE_ARRAY_SIZE; i++) fixed_nodes[i]=-1;
	for (i=0; i<MAX_SOURCES; i++)
	{  
if (m_bSource[i])
		fixed_nodes[k] = m_xSource[i]; k++;
	}
	for (i=0; i<MAX_REGIONS; i++) 
	{
		fixed_nodes[k] = m_regiondata.GetRegionFront(i); k++;
		fixed_nodes[k] = m_regiondata.GetRegionRear(i); k++;
	}
	for (i=0; i<MAX_LUMPED; i++) 
	{ 
if (m_bLumped[i]) {
			fixed_nodes[k] = m_xLumpedA[i]; k++;
			fixed_nodes[k] = m_xLumpedC[i]; k++;
} 
	}
}


//  -----------------------------------------------
//				FindFixedNodePositions
//  -----------------------------------------------

/// find the index of the node corresponding to each fixed node x position
/// This dramatically speeds up the perturbation functions. 
/// It must be called whenever a renode occurs.
void CSolve::FindFixedNodePositions()
{
	int n;
	for (n=0; n<=node.GetLastNode(); n++) {
		int p;
		for (p=0; p<MAX_SOURCES; p++) if (m_bSource[p]) {
			if (node.GetX(n)==m_xSource[p]) m_SourcePos[p]=n;
		}
		for (p=0; p<MAX_LUMPED; p++) if (m_bLumped[p]) {
			if (node.GetX(n)==m_xLumpedC[p]) m_CathodePos[p]=n;
			if (node.GetX(n)==m_xLumpedA[p]) m_AnodePos[p]=n;
		}
		for (p=0; p<MAX_REGIONS; p++){
			if (node.GetX(n)==m_regiondata.GetRegionFront(p)) {
				m_RegionFrontPos[p]=n;
				m_NumRegions=p+1;			// messy way of determining number of regions
			}
			if (node.GetX(n)==m_regiondata.GetRegionRear(p)) {
				m_RegionRearPos[p]=n;
				m_NumRegions=p+1;
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////////////////
//		Equilibrium calculations
// 

void CSolve::LoadEqMatrix()
{
	double epsL, epsR;

	mat.SetBandMatrixSize(node.GetNodeCount(), 3);

	for (int k=0; k<=node.GetLastNode(); k++)
	{
		for (int j=0; j<=bw_1; j++) mat.m[k][j]=0;
		if (k==0) {
            PsiC = node.GetPsi_eq(k);
            AreaC = node.GetArea(k);
			nC = node.GetNeq_nir(k);
			pC = node.GetPeq_nir(k);
//            Nd_aC = (node.GetNd(k) - node.GetNa(k));
			rhoC = (pC- node.GetNa(k))  - (nC - node.GetNd(k)) ;
		} else {
            dxL = dxR; 
            rdxL = rdxR;
            epsL = epsR;
            AreaL = AreaC; AreaC = AreaR;
            PsiL = PsiC;   PsiC = PsiR;
            nL = nC;       nC = nR;
            pL = pC;       pC = pR;
//            Nd_aL = Nd_aC; Nd_aC = Nd_aR;
			rhoL = rhoC; rhoC=rhoR;
		}
		if (k<node.GetLastNode()) {
            dxR = node.Get_dxR(k); // node.x[k+1] - node.x[k];
            rdxR = 1 / dxR;
            epsR = node.GetEps(k+1);
            AreaR = node.GetArea(k+1);
            PsiR = node.GetPsi_eq(k+1);
			nR = node.GetNeq_nir(k+1);
			pR = node.GetPeq_nir(k+1);
//            Nd_aR = (node.GetNd(k+1) - node.GetNa(k+1));
//			rhoR = pR - nR + node.GetNd(k+1) - node.GetNa(k+1);
			rhoR = (pR - node.GetNa(k+1)) - (nR - node.GetNd(k+1));
		}
		if (k==0) {
			if (m_FrontSurface==CHARGED_SURFACE) {
                mat.v[k] = epsR * rdxR * (PsiC-PsiR) - m_NssFront;
                mat.m[k][diag] = - epsR * rdxR;
                mat.m[k][diag+1] = epsR * rdxR;
			} else {
                mat.v[k] = 0;
                mat.m[k][diag] = 1.0;
			}
		} else if (k==node.GetLastNode()) {
			if (m_RearSurface==CHARGED_SURFACE)	{
                mat.v[k] = epsL * rdxL * (PsiC-PsiL) - m_NssRear;
                mat.m[k][diag] = - epsL * rdxL;
                mat.m[k][diag-1] = epsL * rdxL;
			} else {			
                mat.v[k] = 0;
                mat.m[k][diag] = 1.0;
			}
		} else {
			double epsArea_dxL = epsL * rdxL * (AreaL+AreaC)/2;
			double epsArea_dxR = epsR * rdxR * (AreaC+AreaR)/2;

			double AreaDxL_6 = AreaL * dxL  / 6;
			double AreaDxR_6 = AreaR * dxR  / 6;
			double AreaDxC_6 = AreaC * (dxL + dxR) / 3;

//			mat.v[k]=     (pL - nL + Nd_aL) * AreaDxL_6
//						+ (pC - nC + Nd_aC) * AreaDxC_6
//						+ (pR - nR + Nd_aR) * AreaDxR_6
//						- (PsiC-PsiL) * epsArea_dxL
//						+ (PsiR-PsiC) * epsArea_dxR;
			mat.v[k]=     rhoL * AreaDxL_6
						+ rhoC * AreaDxC_6
						+ rhoR * AreaDxR_6
						- (PsiC-PsiL) * epsArea_dxL
						+ (PsiR-PsiC) * epsArea_dxR;

            mat.m[k][diag-1] = (pL + nL) * AreaDxL_6 - epsArea_dxL;
            mat.m[k][diag] =   (pC + nC) * AreaDxC_6 + epsArea_dxL + epsArea_dxR;
            mat.m[k][diag+1] = (pR + nR) * AreaDxR_6 - epsArea_dxR;
		}
	}
}

/// calculate new Psi_eq and m_Error
void CSolve::Update_PsiEq()
{
	int k;
	m_Error=0;
	double rClamp;
	double dPsi;
	rClamp=1/m_Clamp; // for speed
	for (k=0; k<=node.GetLastNode(); k++)
	{
		dPsi = mat.v[k];
	    if (fabs( dPsi ) > m_Error) m_Error = fabs( dPsi );
		dPsi = dPsi  / (1 + fabs(dPsi*rClamp));
//		node.Psi_eq[k] += dPsi;
		node.IncrementPsi_eq(k, dPsi);
	}
}


// returns FALSE if needs to be called again (i.e. hasn't converged yet)
// returns TRUE if has converged
BOOL CSolve::DoNextEquilibriumIteration(int& iter, double& progress, 
						CString& message, BOOL &need_renode)
{
//	m_Error=1e6;

	LoadEqMatrix();
	mat.LU_decompose();
	mat.LU_solve();
	Update_PsiEq();
/*****                  
	m_Error=0;
	double rClamp;
	double dPsi;
	rClamp=1/m_Clamp; // for speed
	for (k=0; k<=node.GetLastNode(); k++)
	{
	    if (fabs( mat.v[k] ) > m_Error) m_Error = fabs( mat.v[k] );
		mat.v[k] = mat.v[k]  / (1 + fabs(mat.v[k]*rClamp));
		node.Psi_eq[k] +=mat.v[k];
	}
*****/
    iter++;
	if (m_Error==0) progress = 1;
	else progress = 0.5*(1+CMath::log(m_Error)/CMath::log(m_ErrorLimit));
	message.Empty();
			
    if (need_renode && m_Error<m_ErrorLimit) {
	    message = "Renode";
		CDonStatusBar::UpdateStatusBar();
		SetFixedNodes();
    	node.Renode(TRUE, m_SizeFactor, fixed_nodes);
	    need_renode = FALSE;
	    m_Error = 1e6;	// Prevents exiting do loop
	}
	if (m_Error<=m_ErrorLimit) return TRUE; // has converged
		else return FALSE;
}


BOOL CSolve::SolveEquilibrium(int& iter, double& progress, 
								CString& message, BOOL need_renode, CDocument *pDoc)
{
	iter = 0;
	progress = 0;
//	m_Error = 1e6;
	message.Empty();
	CTime start_time = CTime::GetCurrentTime();
	CTimeSpan max_time(m_nMaxTime);
	while (m_Error>m_ErrorLimit && CTime::GetCurrentTime()-start_time<max_time && m_Error<=1e20)
    {
		LoadEqMatrix();
		mat.LU_decompose();
		mat.LU_solve();
		Update_PsiEq();
/******        
        m_Error=0;
		double rClamp;
		double dPsi;
		rClamp=1/m_Clamp; // for speed
		for (k=0; k<=node.GetLastNode(); k++)
		{
			dPsi = mat.v[k];
			if (fabs( dPsi ) > m_Error) m_Error = fabs( dPsi );
			dPsi = dPsi  / (1 + fabs(dPsi*rClamp));
			node.Psi_eq[k] += dPsi;
		}
****/
        iter++;
		if (m_Error==0) progress = 1;
		else progress = 0.5*(1+CMath::log(m_Error)/CMath::log(m_ErrorLimit));
		message.Empty();
		CDonStatusBar::UpdateStatusBar();
		if (m_Error>m_ErrorLimit && m_bGraphsAfterEveryIteration) 
		{
				pDoc->UpdateAllViews(NULL, HINT_ITERATION);
		}

        if (need_renode && m_Error<m_ErrorLimit)
		{
	        message = "Renode";
			CDonStatusBar::UpdateStatusBar();
			SetFixedNodes();
    	    node.Renode(TRUE, m_SizeFactor, fixed_nodes);
	        need_renode = FALSE;
         	start_time = CTime::GetCurrentTime();
	        m_Error = 1e6;	// Prevents exiting do loop
		}
	}
	if (m_Error<=m_ErrorLimit) return TRUE; else return FALSE;
}


//////////////////////////////////////////////////////////////////////////////////
//		Steady state and transient calculations
// 

// LoadLocalValues for the leftmost node.
// This function is split off from LoadLocalValues() to reduce the code size of
// the LoadSystem() loop. Pentium only has an 8K cache for code, we must ensure the
// whole loop will fit inside the cache. At the moment it's about 7.5K.

void CSolve::Load_Local_Centre_Values(int k)
{
	int Reg;
	double localEfield; // local value of electric field, normalised to Vt (used for Hurkx calculations)
	double nie;

		PsiC = node.GetPsi(k);
	    Phi_nC = node.GetPhi_n(k);
	    Phi_pC = node.GetPhi_p(k);
	    AreaC = node.GetArea(k);

		nC = node.GetN_nir(k);		// exp(node.Psi[k] + node.Vn[k] - node.Phi_n[k]);
		pC = node.GetP_nir(k);		// exp(-node.Psi[k] + node.Vp[k] + node.Phi_p[k]);
		neqC = node.GetNeq_nir(k);	// exp( node.Psi_eq[k]+node.Vn[k]);
		peqC = node.GetPeq_nir(k);	// exp(-node.Psi_eq[k]+node.Vp[k]);
		
//	    Nd_aC = node.GetNd(k) - node.GetNa(k);
		rhoC = pC - nC + node.GetNd(k) - node.GetNa(k);
	    GenC = 0;
		Reg = node.GetRegion(k);

		// Since the 3 bulk calculations use very similar parameters, combine into one function.
		nie = node.GetNie_nir(k);	// nieC=exp((node.Vn[k] + node.Vp[k])/2);
		if (m_regiondata.IsHurkxEnabledForRegion(Reg)) {
				localEfield = node.GetE_Vt(k);
				CPhysics::Bulk_Recomb_Hurkx_3(RecC, dRdPhinC, dRdPhipC,
						nC, pC, nie, node.GetTaun(k), node.GetTaup(k),
						m_regiondata.GetEt(Reg), node.GetNd(k), node.GetNa(k), 
						m_regiondata.GetCn(Reg), m_regiondata.GetCp(Reg), m_regiondata.GetCnp(Reg), m_regiondata.GetB(Reg),
						localEfield, m_regiondata.GetHurkxPrefactor(Reg), m_regiondata.GetHurkxFgamma_Vt(Reg));
		} else {
				CPhysics::Bulk_Recomb_3(RecC, dRdPhinC, dRdPhipC,
						nC, pC, nie, node.GetTaun(k), node.GetTaup(k),
						m_regiondata.GetEt(Reg), node.GetNd(k), node.GetNa(k), 
						m_regiondata.GetCn(Reg), m_regiondata.GetCp(Reg), m_regiondata.GetCnp(Reg), m_regiondata.GetB(Reg));
		}

	    dRdPsiC  = -dRdPhipC - dRdPhinC;
    	mu_times_nC = node.GetMun_times_N(k); // = exp(PsiC+node.Un[k]-Phi_nC);
    	mu_times_pC = node.GetMup_times_P(k); // = exp(-PsiC+node.Up[k]+Phi_pC);
}

#pragma optimize("t", on) // this code is time-critical


//  -----------------------------------------------
//				Compute_Bernoulli_Functions
//  -----------------------------------------------
//	A helper function for Load_Local_Values()

// NOTE: This function replaces the old functions
//   Compute_Zn_functions() and Compute_Zp_functions()

// Compute the Bernoulli functions
//	Z(x) =	x / ( exp(x) - 1 )
//  Y(x) =  ( 1 - Z(x) ) / x
//  Z_(x) = x + Z(x)
//  Y_(x) = 1 - Y(x)
// These need to be computed for both n and p, with Delta =(node.Psi[k+1]+node.Un[k+1]) - (node.Psi[k]+node.Un[k]);
// Also dYndPsi from CalculateDerivatives(), since it needs a small Delta approximation

//  We'll also need some extra values:
//  dZdPsi = Z * Y_
//	dYndPsi =  Y - dZdPsi / ( Z - Z_ )
// Note, dYpdPsi must be negated   dYpdPsi =  -(Y-dZpdPsi) / (Zp - Zp_)
//       dYndPsi can be used 'as is'.

void Compute_Bernoulli_Functions(double Delta, double &Z, double &Y, double &Z_, double &Y_, double &dZdPsi, double &dYdPsi)
{
	double rDelta;
    if (fabs(Delta) < 1E-5)	Z =   1.0 - 0.5 * Delta;
    else					Z =   Delta / (exp(Delta) - 1.0);
	// Works for big Delta: if d<-100, z=-d; if d>100 z=0.
	// it's not necessary to do them as special cases, 
	// the FPU's infinity maths will take care of it.

    Z_	= Delta + Z;		// Note that Zn_ = delta + Zn so Zn-Zn_ = -delta

	if (fabs(Delta)<1E-3)	{ 
		Y =   0.5 - Delta / 12.0;
		Y_	= 1.0 - Y;
		dZdPsi  = Z * Y_;
		dYdPsi = 1.0 / 12;
	} else {
		rDelta= 1/Delta;
		Y =   (1.0 - Z) *rDelta;	//	Yn =   (1.0 - Zn) / Delta;
		Y_	= 1.0 - Y;
	    dZdPsi  = Z * Y_;
		dYdPsi = -(Y - dZdPsi) *rDelta;  // dYndPsi = (Yn - dZndPsi) / (Zn-Zn_);
	}
}

/****** 
void CSolve::Compute_Zn_functions(int k)
{
	double Delta;
   	Delta = (node.Psi[k+1]+node.Un[k+1]) - (node.Psi[k]+node.Un[k]);
    if (fabs(Delta) < 1E-5)	Zn =   1.0 - 0.5 * Delta;
    else if (Delta > 100)	Zn =   0;
    else if (Delta < -100)	Zn = - Delta;
    else					Zn =   Delta / (exp(Delta) - 1.0);
	if (fabs(Delta)<1E-3)	Yn =   0.5 - Delta / 12.0;
    else					{
		rZndelta= 1/Delta;
		Yn =   (1.0 - Zn) *rZndelta;	//	Yn =   (1.0 - Zn) / Delta;
	}
    Zn_	= Delta + Zn;
    Yn_	= 1.0 - Yn;

	// These next 4 lines moved here from CalculateDerivatives(), since they need a small Delta approximation
    dZndPsi  = Zn * Yn_;
    dZn_dPsi = dZndPsi - 1;

	// Note that Zn_ = delta + Zn so Zn-Zn_ = -delta
    if (fabs(Delta) < 1.0E-3)	dYndPsi = 1.0 / 12;
    else                        dYndPsi = -(Yn - dZndPsi) *rZndelta;  // dYndPsi = (Yn - dZndPsi) / (Zn-Zn_);
}
*******/

//  -----------------------------------------------
//				Load_Local_Values
//  -----------------------------------------------

// Loads values for node k.
// Note that exponentials and divisions take about 40 times as long as a multiply or add,
// so this function pre-calculates all exponentials which are used more than once.
// Also tries to minimize the number of divisions
// This function takes about 40% of the total time for an iteration.

//const double EXP_01 = exp(-1E-2), EXP01 = exp(+1E-2);

void CSolve::Load_local_values(int k)
{
	int Reg;
	double localEfield; // local value of electric field, normalised to Vt (used for Hurkx calculations)
	double nie;
	double Delta_n, Delta_p;

	if (k==0)
    {
		Load_Local_Centre_Values(0);
/*********
	    PsiC = node.Psi[k];
	    Phi_nC = node.Phi_n[k];
	    Phi_pC = node.Phi_p[k];
	    AreaC = node.Area[k];

		nC = node.GetN_nir(k);		// exp(node.Psi[k] + node.Vn[k] - node.Phi_n[k]);
		pC = node.GetP_nir(k);		// exp(-node.Psi[k] + node.Vp[k] + node.Phi_p[k]);
		neqC = node.GetNeq_nir(k);	// exp( node.Psi_eq[k]+node.Vn[k]);
		peqC = node.GetPeq_nir(k);	// exp(-node.Psi_eq[k]+node.Vp[k]);
		
	    Nd_aC = node.Nd[k] - node.Na[k];
	    GenC = 0;
		Reg = node.Reg[k];

		// Since the 3 bulk calculations use very similar parameters, combine into one function.
		nie = node.GetNie_nir(k);	// nieC=exp((node.Vn[k] + node.Vp[k])/2);
		if (m_HurkxEnable[Reg]) {
				localEfield = node.GetE_Vt(k);
				CPhysics::Bulk_Recomb_Hurkx_3(RecC, dRdPhinC, dRdPhipC,
						nC, pC, nie, node.taun[k], node.taup[k],
						m_Et[Reg], node.Nd[k], node.Na[k], 
						m_Cn[Reg], m_Cp[Reg], m_Cnp[Reg], m_B[Reg],
						localEfield, m_HurkxPrefactor[Reg], m_HurkxFgamma_Vt[Reg]);
		} else {
				CPhysics::Bulk_Recomb_3(RecC, dRdPhinC, dRdPhipC,
						nC, pC, nie, node.taun[k], node.taup[k],
						m_Et[Reg], node.Nd[k], node.Na[k], 
						m_Cn[Reg], m_Cp[Reg], m_Cnp[Reg], m_B[Reg]);
		}

	    dRdPsiC  = -dRdPhipC - dRdPhinC;
    	mu_times_nC = node.GetMu_times_N(k); // = exp(PsiC+node.Un[k]-Phi_nC);
    	mu_times_pC = node.GetMu_times_P(k); // = exp(-PsiC+node.Up[k]+Phi_pC);
*******/
		rdxL = 0;	// This and next line used in velocity saturation expression at k==0
		Phi_nL = Phi_nC; Phi_pL = Phi_pC;
    } else {	// k>0
	    dxL = dxR; 
	    rdxL = rdxR;
	    m_epsL = m_epsR;
	    AreaL = AreaC;	AreaC = AreaR;
	    PsiL = PsiC;	PsiC = PsiR;
	    Phi_nL=Phi_nC;	Phi_nC = Phi_nR;
	    Phi_pL=Phi_pC;	Phi_pC = Phi_pR;
	    nL = nC;		nC = nR;
	    pL = pC;		pC = pR;
	    neqL = neqC;	neqC = neqR;
	    peqL = peqC;	peqC = peqR;
//	    Nd_aL = Nd_aC;	Nd_aC = Nd_aR;
		rhoL = rhoC; rhoC=rhoR;
	    GenC = GenR;
	    RecC = RecR;
	    dRdPhinC = dRdPhinR;
	    dRdPhipC = dRdPhipR;
	    dRdPsiC  = dRdPsiR;
    	mu_times_nC = mu_times_nR;
    	mu_times_pC = mu_times_pR;
    }
	if (k<node.GetLastNode()) {
		dxR =  node.Get_dxR(k);		// node.x[k+1] - node.x[k];
//		if (dxR==dxL) rdxR=rdxL; // speedup: frequently avoid a (slow) division
//	    else rdxR = 1 / dxR;
rdxR = node.GetReciprocal_dxR(k);
	    m_epsR = (node.GetEps(k)+node.GetEps(k+1))/2;
	    AreaR = node.GetArea(k+1);
	    PsiR = node.GetPsi(k+1);
	    Phi_nR = node.GetPhi_n(k+1);
	    Phi_pR = node.GetPhi_p(k+1);
		nR = node.GetN_nir(k+1);	 // exp( PsiR + node.Vn[k+1] - Phi_nR);
		pR = node.GetP_nir(k+1);	 // exp(-PsiR + node.Vp[k+1] + Phi_pR);

		neqR = node.GetFASTNeq_nir(k+1); // exp( node.Psi_eq[k+1]+node.Vn[k+1]);
		peqR = node.GetFASTPeq_nir(k+1); // exp(-node.Psi_eq[k+1]+node.Vp[k+1]);
		nie = node.GetFASTNie_nir(k+1);	 // nieR=exp((node.Vn[k+1]+node.Vp[k+1])/2);

//        Nd_aR = node.GetNd(k+1) - node.GetNa(k+1);
		rhoR = pR - nR + node.GetNd(k+1) - node.GetNa(k+1);
	    GenR = node.GetGcum(k+1);
		Reg = node.GetRegion(k+1);

		// Since the 3 bulk recombination calculations use very similar parameters, combine into one function.
		// speeds up Load_System by 13%.


		if (m_regiondata.IsHurkxEnabledForRegion(Reg)) {
			//	localEfield = node.GetE_Vt(k);
			if (k==0) localEfield = node.GetE_Vt(k);
			else   //  k<node.GetLastNode() is TRUE, so only one case to consider
			 if (dxL==dxR) localEfield= -( PsiR-PsiL ) *rdxR*0.5;
			 else localEfield=  -( (PsiR-PsiC) * dxL * rdxR +	(PsiC-PsiL) * dxR * rdxL ) / (dxR+dxL);

				CPhysics::Bulk_Recomb_Hurkx_3(RecR, dRdPhinR, dRdPhipR,
	 					nR, pR, nie, node.GetTaun(k+1), node.GetTaup(k+1),
	 					m_regiondata.GetEt(Reg), node.GetNd(k+1), node.GetNa(k+1),
						m_regiondata.GetCn(Reg), m_regiondata.GetCp(Reg), m_regiondata.GetCnp(Reg),
						m_regiondata.GetB(Reg),
						localEfield, m_regiondata.GetHurkxPrefactor(Reg), m_regiondata.GetHurkxFgamma_Vt(Reg)); 
		} else {
				CPhysics::Bulk_Recomb_3(RecR, dRdPhinR, dRdPhipR,
	 					nR, pR, nie, node.GetTaun(k+1), node.GetTaup(k+1),
	 					m_regiondata.GetEt(Reg), node.GetNd(k+1), node.GetNa(k+1),
						m_regiondata.GetCn(Reg), m_regiondata.GetCp(Reg), m_regiondata.GetCnp(Reg),
						m_regiondata.GetB(Reg));
		}
	    dRdPsiR  = -dRdPhipR - dRdPhinR;
    	mu_times_nR = node.GetMun_times_N(k+1); // = exp( PsiR+node.Un[k+1]-Phi_nR);
    	mu_times_pR = node.GetMup_times_P(k+1); // = exp(-PsiR+node.Up[k+1]+Phi_pR);


double expDeltaPhin_1, exp_DeltaPhip_1;

	   	if (fabs(Phi_nR-Phi_nC)<1E-5)   // for small x, exp(x)-1 == x
           		expDeltaPhin_1 = Phi_nC - Phi_nR;
       	else expDeltaPhin_1 = exp(-Phi_nR + Phi_nC) - 1;
       	if (fabs(Phi_pR-Phi_pC)<1E-5)  // for small x, 1-exp(-x) == x
           		exp_DeltaPhip_1 = Phi_pC - Phi_pR;
        else exp_DeltaPhip_1 = 1 - exp( Phi_pR - Phi_pC);

munCexpDeltaPhin_dxR = mu_times_nC * expDeltaPhin_1 * rdxR;
mupCexpDeltaPhip_dxR = mu_times_pC * exp_DeltaPhip_1 * rdxR;


		// Compute Zn and Zp functions and derivatives which depend on delta
//   		Delta_n = (node.Psi[k+1]+node.Un[k+1]) - (node.Psi[k]+node.Un[k]);
//		Delta_p = (node.Psi[k+1]-node.Up[k+1]) - (node.Psi[k]-node.Up[k]);

		Delta_n = node.GetDelta_PsiUn(k); // (node.Psi[k+1]+node.Un[k+1]) - (node.Psi[k]+node.Un[k]);
		Delta_p = node.GetDelta_PsiUp(k); // (node.Psi[k+1]-node.Up[k+1]) - (node.Psi[k]-node.Up[k]);

		double Yp, Yn_, dZndPsi, dYpdPsi; // these are never used elsewhere
		Compute_Bernoulli_Functions(Delta_n, Zn, Yn, Zn_, Yn_, dZndPsi, dYndPsi);
		Compute_Bernoulli_Functions(Delta_p, Zp, Yp, Zp_, Yp_, dZpdPsi, dYpdPsi);
//		Yp_	= 1.0 - Yp;		// Yn_ is never used.
		dZn_dPsi = dZndPsi - 1;
		dYp_dPsi = -dYpdPsi;
    }
}

//  -----------------------------------------------
//			Calc_Integrals_and_Currents
//  -----------------------------------------------
// Calculates In and Ip, the electron & hole currents, for node k.

// Also stores these values in the node.I_n[] array.
// This is about 10% of the calculation time, mostly because of the two calls to log()
void CSolve::Calc_Integrals_and_Currents(int k)
{
	double new_avg, old_avg, ln_new_old;
	double n_OldC, n_OldR, p_OldC, p_OldR;
    if (k==0)	// Initialize for left boundary
    {
        InL = 0;
        IpL = 0;
    } else { // (k > 0)	// Compute for left element
        InL   = InR - (IntGR - Int_dndt);
        IpL   = IpR + (IntGR - Int_dpdt);
    }
    if (k < node.GetLastNode())	// Compute for right element
    {
        IntGR = ((GenR - GenC) - 0.5 * (AreaC*RecC+AreaR*RecR) * dxR);		
        if (m_rdt>0)
        {
			// Electrons
			n_OldC=node.GetN_Old(k);
			n_OldR=node.GetN_Old(k+1);
	        new_avg = 0.5*((nC-neqC)*AreaC + (nR-neqR)*AreaR);
	        old_avg = 0.5*((n_OldC-neqC)*AreaC + (n_OldR-neqR)*AreaR);
	        if (fabs(new_avg)<fabs(old_avg) && new_avg*old_avg>0)
            {		// Exponential
				ln_new_old = log(new_avg/old_avg);
            	Int_dndt = dxR * m_rdt * new_avg * ln_new_old;
            	Exp_decay_factor_n = 1 + ln_new_old;
         	}
	        else
            {		// Linear
               	Int_dndt =   dxR * m_rdt * (new_avg - old_avg);
            	Exp_decay_factor_n = 1;
            }

			// Holes
			p_OldC=node.GetP_Old(k);
			p_OldR=node.GetP_Old(k+1);
            new_avg  =   0.5 * ((pC-peqC)*AreaC + (pR-peqR)*AreaR);
            old_avg  =   0.5 * ((p_OldC-peqC)*AreaC + (p_OldR-peqR)*AreaR);
            if (fabs(new_avg)<fabs(old_avg) && new_avg*old_avg>0)
            {	// Exponential
				ln_new_old = log(new_avg/old_avg);
            	Int_dpdt = dxR * m_rdt * new_avg * ln_new_old;
            	Exp_decay_factor_p = 1 + ln_new_old;
            }
            else
            {	// Linear
            	Int_dpdt =   dxR * m_rdt * (new_avg - old_avg);
            	Exp_decay_factor_p = 1;
            }

        } else Int_dndt = Int_dpdt = 0;

       	InR = (munCexpDeltaPhin_dxR * Zn_ + (IntGR - Int_dndt) * Yn );
		IpR = (mupCexpDeltaPhip_dxR * Zp  - (IntGR - Int_dpdt) * Yp_);

//     	InR = (rdxR*Zn_*mu_times_nC*(expDeltaPhin_1) + (IntGR - Int_dndt) * Yn );
//		IpR = (rdxR*Zp *mu_times_pC*(exp_DeltaPhip_1) - (IntGR - Int_dpdt) * Yp_);
		

	} else { //  (k==node.GetLastNode())	// Terminate at right boundary
        InR = 0;
        IpR = 0;
    }
	// Save the currents for future reference...
	if (k==0) node.SetInIp(k, InR, IpR);
	else node.SetInIp(k, InL, IpL);
//	if (k==0) node.I_n[k] = InR; else node.I_n[k] = InL;	// Electron current
//	if (k==0) node.I_p[k] = IpR; else node.I_p[k] = IpL;	// Hole current
}


//  -----------------------------------------------
//				Calculate_Derivatives
//  -----------------------------------------------

// Calculate the left + right derivatives of In and Ip
// with respect to phi_n, phi_p and psi.

void CSolve::Calculate_Derivatives(int k)
{
	if (k==0) {	// Initialize at left boundary
        dInL_dPhinL = 0; dInL_dPhipL = 0; dInL_dPsiL = 0;
        dInL_dPhinC = 0; dInL_dPhipC = 0; dInL_dPsiC = 0;
        dIpL_dPhinL = 0; dIpL_dPhipL = 0; dIpL_dPsiL = 0;
        dIpL_dPhinC = 0; dIpL_dPhipC = 0; dIpL_dPsiC = 0;
        dInL_dPhinR = dIpL_dPhipR = 0;	// VelSat
    } else {	// Compute for left element
        dInL_dPhinL   =   dInR_dPhinC   - dIntGR_dPhinC + dInt_dndt_dPhinC;
        dInL_dPhinC   =   dInR_dPhinR   - dIntGR_dPhinR + dInt_dndt_dPhinR;
        dInL_dPsiL    =   dInR_dPsiC    - dIntGR_dPsiC  + dInt_dndt_dPsiC;
        dInL_dPsiC    =   dInR_dPsiR    - dIntGR_dPsiR  + dInt_dndt_dPsiR;
        dInL_dPhipL   =   dInR_dPhipC   - dIntGR_dPhipC;
        dInL_dPhipC   =   dInR_dPhipR   - dIntGR_dPhipR;
        dIpL_dPhinL   =   dIpR_dPhinC   + dIntGR_dPhinC;
        dIpL_dPhinC   =   dIpR_dPhinR   + dIntGR_dPhinR;
        dIpL_dPsiL    =   dIpR_dPsiC    + dIntGR_dPsiC  - dInt_dpdt_dPsiC;
        dIpL_dPsiC    =   dIpR_dPsiR    + dIntGR_dPsiR  - dInt_dpdt_dPsiR;
        dIpL_dPhipL   =   dIpR_dPhipC   + dIntGR_dPhipC - dInt_dpdt_dPhipC;
        dIpL_dPhipC   =   dIpR_dPhipR   + dIntGR_dPhipR - dInt_dpdt_dPhipR;
    }
    if (k < node.GetLastNode())	{	// Compute for right element
/**
		double dxAreaC_2 = 0.5 * dxR * AreaC;
		double dxAreaR_2 = 0.5 * dxR * AreaR;
        if (m_rdt>0) {
			// do transient terms. Note: Often exp_factor is 1 so could be simplified
          dInt_dndt_dPhinC = - dxAreaC_2 * nC * m_rdt * Exp_decay_factor_n;
          dInt_dndt_dPhinR = - dxAreaR_2 * nR * m_rdt * Exp_decay_factor_n;
          dInt_dndt_dPsiC  = - dInt_dndt_dPhinC;
          dInt_dndt_dPsiR  = - dInt_dndt_dPhinR;
          dInt_dpdt_dPhipC =   dxAreaC_2 * pC * m_rdt * Exp_decay_factor_p;
          dInt_dpdt_dPhipR =   dxAreaR_2 * pR * m_rdt * Exp_decay_factor_p;
          dInt_dpdt_dPsiC  = - dInt_dpdt_dPhipC;
          dInt_dpdt_dPsiR  = - dInt_dpdt_dPhipR;
        } else {
          dInt_dndt_dPhinC = 0;
          dInt_dndt_dPhinR = 0;
          dInt_dndt_dPsiC  = 0;
          dInt_dndt_dPsiR  = 0;
          dInt_dpdt_dPhipC = 0;
          dInt_dpdt_dPhipR = 0;
          dInt_dpdt_dPsiC  = 0;
          dInt_dpdt_dPsiR  = 0;
        }
        dIntGR_dPhinC    = - dxAreaC_2 * dRdPhinC;
        dIntGR_dPhinR    = - dxAreaR_2 * dRdPhinR;
        dIntGR_dPsiC     = - dxAreaC_2 * dRdPsiC;
        dIntGR_dPsiR     = - dxAreaR_2 * dRdPsiR;
        dIntGR_dPhipC    = - dxAreaC_2 * dRdPhipC;
        dIntGR_dPhipR    = - dxAreaR_2 * dRdPhipR;
**/
        if (m_rdt>0) {
			// do transient terms.
          dInt_dndt_dPhinC = - 0.5 * dxR * AreaC * nC * m_rdt * Exp_decay_factor_n;
          dInt_dndt_dPhinR = - 0.5 * dxR * AreaR * nR * m_rdt * Exp_decay_factor_n;
          dInt_dndt_dPsiC  = - dInt_dndt_dPhinC;
          dInt_dndt_dPsiR  = - dInt_dndt_dPhinR;
          dInt_dpdt_dPhipC =   0.5 * dxR * AreaC * pC * m_rdt * Exp_decay_factor_p;
          dInt_dpdt_dPhipR =   0.5 * dxR * AreaR * pR * m_rdt * Exp_decay_factor_p;
          dInt_dpdt_dPsiC  = - dInt_dpdt_dPhipC;
          dInt_dpdt_dPsiR  = - dInt_dpdt_dPhipR;
        } else {
          dInt_dndt_dPhinC = 0;
          dInt_dndt_dPhinR = 0;
          dInt_dndt_dPsiC  = 0;
          dInt_dndt_dPsiR  = 0;
          dInt_dpdt_dPhipC = 0;
          dInt_dpdt_dPhipR = 0;
          dInt_dpdt_dPsiC  = 0;
          dInt_dpdt_dPsiR  = 0;
        }
        dIntGR_dPhinC    = - 0.5 * dxR * AreaC * dRdPhinC;
        dIntGR_dPhinR    = - 0.5 * dxR * AreaR * dRdPhinR;
        dIntGR_dPsiC     = - 0.5 * dxR * AreaC * dRdPsiC;
        dIntGR_dPsiR     = - 0.5 * dxR * AreaR * dRdPsiR;
        dIntGR_dPhipC    = - 0.5 * dxR * AreaC * dRdPhipC;
        dIntGR_dPhipR    = - 0.5 * dxR * AreaR * dRdPhipR;

		dInR_dPhinL  = 0;	// VelSat
		dIpR_dPhipL  = 0;	// VelSat

        dInR_dPhipC  = (+ dIntGR_dPhipC * Yn);
        dInR_dPhipR  = (+ dIntGR_dPhipR * Yn);

        dIpR_dPhinC  = (- dIntGR_dPhinC * Yp_);
        dIpR_dPhinR  = (- dIntGR_dPhinR * Yp_);


        dInR_dPhinC  = mu_times_nC * rdxR				  * Zn_ 
					 + (dIntGR_dPhinC - dInt_dndt_dPhinC) * Yn;
        dInR_dPhinR  = - mu_times_nR * rdxR				  * Zn
					 + (dIntGR_dPhinR - dInt_dndt_dPhinR) * Yn;

        dIpR_dPhipC  =   mu_times_pC * rdxR				  * Zp  
					 - (dIntGR_dPhipC - dInt_dpdt_dPhipC) * Yp_;
        dIpR_dPhipR  = - mu_times_pR * rdxR				  * Zp_
					 - (dIntGR_dPhipR - dInt_dpdt_dPhipR) * Yp_;

        dInR_dPsiC   = munCexpDeltaPhin_dxR				* (Zn_ + dZn_dPsi)
					 + (IntGR - Int_dndt)				* dYndPsi 
					 + (dIntGR_dPsiC - dInt_dndt_dPsiC) * Yn;
        dInR_dPsiR   = munCexpDeltaPhin_dxR				* (-dZn_dPsi)
					 + (IntGR - Int_dndt)				* (-dYndPsi) 
					 + (dIntGR_dPsiR - dInt_dndt_dPsiR) * Yn;


        dIpR_dPsiC   = mupCexpDeltaPhip_dxR				* (-Zp + dZpdPsi)
                     - (IntGR - Int_dpdt)				* dYp_dPsi
                     - (dIntGR_dPsiC - dInt_dpdt_dPsiC) * Yp_;
        dIpR_dPsiR   = mupCexpDeltaPhip_dxR				* (-dZpdPsi)
                     + (IntGR - Int_dpdt)				* dYp_dPsi
                     - (dIntGR_dPsiR - dInt_dpdt_dPsiR)	* Yp_;

/***
		dInR_dPhinL  = 0;	// VelSat

        dInR_dPhinC  = (+ rdxR * mu_times_nC * Zn_
						+ (dIntGR_dPhinC - dInt_dndt_dPhinC) * Yn);
        dInR_dPhinR  = (- rdxR * mu_times_nR * Zn
						+ (dIntGR_dPhinR - dInt_dndt_dPhinR) * Yn);

        dInR_dPsiC   = (rdxR * mu_times_nC * expDeltaPhin_1 * (Zn_ + dZn_dPsi)
							+ (IntGR - Int_dndt) * dYndPsi 
							+ (dIntGR_dPsiC - dInt_dndt_dPsiC) * Yn);
        dInR_dPsiR   = (rdxR * mu_times_nC * expDeltaPhin_1 * (-dZn_dPsi)
							+ (IntGR - Int_dndt) * (-dYndPsi) 
							+ (dIntGR_dPsiR - dInt_dndt_dPsiR) * Yn);

        dInR_dPhipC  = (+ dIntGR_dPhipC * Yn);
        dInR_dPhipR  = (+ dIntGR_dPhipR * Yn);

        dIpR_dPhinC  = (- dIntGR_dPhinC * Yp_);
        dIpR_dPhinR  = (- dIntGR_dPhinR * Yp_);

        dIpR_dPsiC   = (rdxR * mu_times_pC * exp_DeltaPhip_1 * (-Zp + dZpdPsi)
                        - (IntGR - Int_dpdt) * dYp_dPsi
                        - (dIntGR_dPsiC - dInt_dpdt_dPsiC) * Yp_);
        dIpR_dPsiR   = (rdxR * mu_times_pC * exp_DeltaPhip_1 * (-dZpdPsi)
                        + (IntGR - Int_dpdt) * dYp_dPsi
                        - (dIntGR_dPsiR - dInt_dpdt_dPsiR) * Yp_);
		dIpR_dPhipL  = 0;	// VelSat
        dIpR_dPhipC  = (  rdxR * mu_times_pC * Zp
                        - (dIntGR_dPhipC - dInt_dpdt_dPhipC) * Yp_);
        dIpR_dPhipR  = (- rdxR * mu_times_pR * Zp_
                        - (dIntGR_dPhipR - dInt_dpdt_dPhipR) * Yp_);
***/
	} else {//     (k==node.GetLastNode())	// Terminate at right boundary
        dInR_dPhinR = 0; dInR_dPhipR = 0; dInR_dPsiR = 0;
        dInR_dPhinC = 0; dInR_dPhipC = 0; dInR_dPsiC = 0;
        dIpR_dPhinR = 0; dIpR_dPhipR = 0; dIpR_dPsiR = 0;
        dIpR_dPhinC = 0; dIpR_dPhipC = 0; dIpR_dPsiC = 0;
        dInR_dPhinL = dIpR_dPhipL = 0;	// VelSat
    }
}

//  -----------------------------------------------
//				Load_Jacobian_and_Residual
//  -----------------------------------------------

// Load the matrix using the derivatives from CalculateDerivatives()
void CSolve::Load_Jacobian_and_Residual(int k)
{
	double *poissonsEqn;
	double *electronCurrentEqn, *holeCurrentEqn;
	
	int row = 3*k;
	int i;

	electronCurrentEqn=mat.m[row];
	holeCurrentEqn=mat.m[row+2];
	poissonsEqn=mat.m[row+1];

	double epsL, epsR;
	epsL=m_epsL; epsR=m_epsR;

//	if (k>0) epsL=(node.eps[k-1]+node.eps[k])/2;
//  if (k<node.GetLastNode()) epsR = (node.eps[k]+node.eps[k+1])/2;

	
    if (k==0) {
		for (i=0; i<=bw_1; i++) poissonsEqn[i]=0;

        if (m_FrontSurface==CHARGED_SURFACE) {
            poissonsEqn[diag] = - epsR * rdxR;
            poissonsEqn[diag+3] = epsR * rdxR;
				// Residual
            mat.v[row+1] = epsR * rdxR * (PsiC-PsiR) - m_NssFront;
        } else {
            poissonsEqn[diag] = - (nC + pC);
            poissonsEqn[diag-1] = - (-nC);
            poissonsEqn[diag+1] = - (-pC);
				// Residual
            mat.v[row+1] = (nC - neqC) - (pC - peqC);
        }
    } else if (k==node.GetLastNode()) {

		for (i=0; i<=bw_1; i++) poissonsEqn[i]=0;
        if (m_RearSurface==CHARGED_SURFACE)
        {
            poissonsEqn[diag] = - epsL * rdxL;
            poissonsEqn[diag-3] = epsL * rdxL;
				// Residual
            mat.v[row+1] = epsL * rdxL * (PsiC-PsiL) - m_NssRear;
        }
		else
        {
            poissonsEqn[diag] = - (nC + pC);
            poissonsEqn[diag-1] = - (-nC);
            poissonsEqn[diag+1] = - (-pC);
				// Residual
            mat.v[row+1] = (nC - neqC) - (pC - peqC);
        }
    } else {
		double epsArea_dxL = epsL * rdxL * (AreaL+AreaC)/2;
		double epsArea_dxR = epsR * rdxR * (AreaC+AreaR)/2;
		double dPsidPhinC  =  - ((dxL + dxR) * AreaC * nC / 3);
		double dPsidPhipC  =  - ((dxL + dxR) * AreaC * pC / 3);

		poissonsEqn[0]=0;
        poissonsEqn[1]    = - (				 dxL * AreaL * nL / 6);
		poissonsEqn[2]	  = - (epsArea_dxL - dxL * AreaL * (pL + nL) / 6);
        poissonsEqn[3]	  = - (				 dxL * AreaL * pL / 6);
		poissonsEqn[4]	  =			dPsidPhinC;
		poissonsEqn[diag] = - (-epsArea_dxL - epsArea_dxR + dPsidPhinC +dPsidPhipC);
		poissonsEqn[6]	  =			dPsidPhipC;
        poissonsEqn[7]    = - (				 dxR * AreaR * nR / 6);
		poissonsEqn[8]	  = - (epsArea_dxR - dxR * AreaR * (pR + nR) / 6);
        poissonsEqn[9]    = - (				 dxR * AreaR * pR / 6);
		poissonsEqn[10]=0;
		// Residual
//		mat.v[row+1] = epsArea_dxR * (PsiR-PsiC) - epsArea_dxL * (PsiC-PsiL)
//						  + (pL - nL + Nd_aL) * AreaL * dxL / 6
//						  + (pC - nC + Nd_aC) * AreaC *(dxL+dxR) /3
//						  + (pR - nR + Nd_aR) * AreaR * dxR / 6;
		mat.v[row+1] = epsArea_dxR * (PsiR-PsiC) - epsArea_dxL * (PsiC-PsiL)
						  + rhoL * AreaL * dxL / 6
						  + rhoC * AreaC *(dxL+dxR) /3
						  + rhoR * AreaR * dxR / 6;
    }
    // Electron current equation
	electronCurrentEqn[0]=0;
	electronCurrentEqn[1]=0;
    electronCurrentEqn[2]    = - (-dInR_dPhinL + dInL_dPhinL);	// VelSat
    electronCurrentEqn[3]    = - (+dInL_dPsiL);
    electronCurrentEqn[4]    = - (+dInL_dPhipL);
    electronCurrentEqn[diag] = - (- dInR_dPhinC + dInL_dPhinC);
    electronCurrentEqn[6]    = - (- dInR_dPsiC  + dInL_dPsiC);
    electronCurrentEqn[7]    = - (- dInR_dPhipC + dInL_dPhipC);
    electronCurrentEqn[8]    = - (- dInR_dPhinR + dInL_dPhinR);	// VelSat
    electronCurrentEqn[9]    = - (- dInR_dPsiR);
    electronCurrentEqn[10]   = - (- dInR_dPhipR);
	// Hole current equation 
    holeCurrentEqn[0]    = - (+dIpL_dPhinL);
    holeCurrentEqn[1]    = - (+dIpL_dPsiL);
    holeCurrentEqn[2]    = - (- dIpR_dPhipL + dIpL_dPhipL);	// VelSat
    holeCurrentEqn[3]    = - (- dIpR_dPhinC + dIpL_dPhinC);
    holeCurrentEqn[4]    = - (- dIpR_dPsiC  + dIpL_dPsiC);
    holeCurrentEqn[diag] = - (- dIpR_dPhipC + dIpL_dPhipC);
    holeCurrentEqn[6]    = - (- dIpR_dPhinR);
    holeCurrentEqn[7]    = - (- dIpR_dPsiR);
    holeCurrentEqn[8]    = - (- dIpR_dPhipR + dIpL_dPhipR);	// VelSat
	holeCurrentEqn[9] = 0;
	holeCurrentEqn[10]= 0;

	// Residual

	// Electron current
    mat.v[row] = InL - InR;
	// Hole current
    mat.v[row+2] = IpL - IpR;

	// The Jacobian and residual vector will also be affected by 
	// surface/interface recombination, sources, and lumped elements.
	// These perturbations have now been moved to seperate functions where
	// their function is more obvious, and where they can be computed more quickly.
}


//////////////////////////////////////////////////////////////////////////////
//         Load System() --- Load the matrix and residual,
//						ready for a newton iteration
//	About 60% of the time for an iteration is performing these steps.

//#define TIME_LOADSYSTEM
#ifdef TIME_LOADSYSTEM
// this version is just used for diagnostic purposes
void CSolve::Load_System(BOOL Jacobian)
{
//	mat.SetBandMatrixSize(3*(node.GetLastNode()+1), 11);
	mat.SetBandMatrixSize(3*node.GetNodeCount(), 11);

int localtime=0;
int inttime=0;
int dertime=0;
int jactime=0;
int ztime=0;
int tweentime=CUseful::CyclesSinceLastCall();
	for (int k=0; k<=node.GetLastNode(); k++)
    {
        Load_local_values(k);
localtime+=CUseful::CyclesSinceLastCall();
        Calc_Integrals_and_Currents(k);
inttime+=CUseful::CyclesSinceLastCall();
        if (Jacobian)
        { 
            Calculate_Derivatives(k);
dertime+=CUseful::CyclesSinceLastCall();
            Load_Jacobian_and_Residual(k);
jactime+=CUseful::CyclesSinceLastCall();
        }
    }
	if (Jacobian) DoRegionInterfaceTerms();
	DoSourceTerms();
	DoLumpedElementTerms();
int lumptime=CUseful::CyclesSinceLastCall();
CString timingstr;
timingstr.Format("Tween=%d, Local=%d, Z=%d, Curr=%d, Deriv=%d, Jac=%d, Lump=%d", tweentime, localtime, ztime, inttime, dertime, jactime, lumptime);
if (Jacobian) WriteStatusBarMessage(timingstr);
CUseful::CyclesSinceLastCall();
}
#else

//  -----------------------------------------------
//				Load_System
//  -----------------------------------------------


// Sources, lumped elements and interfaces are treated as perturbations,
// added when the Jacobian and residual are already set up.
// This makes each iteration about 20% faster.
// If Jacobian==FALSE, don't need to load matrix & residual -- just need
// to update node.I_n[], I_p[]. This is done as part of Calc_Integrals_and_Currents.
// Also calculate lumped element currents.

void CSolve::Load_System(BOOL Jacobian)
{
	mat.SetBandMatrixSize(3*node.GetNodeCount(), 11);

	for (int k=0; k<=node.GetLastNode(); k++)
    {
        Load_local_values(k);
        Calc_Integrals_and_Currents(k);
        if (Jacobian)
        { 
            Calculate_Derivatives(k);
            Load_Jacobian_and_Residual(k);
        }
    }
	if (Jacobian) DoRegionInterfaceTerms();
	DoSourceTerms();
	DoLumpedElementTerms();
}

#endif


//////////////////////////////////////////////////////////////////////////////
//         Perturbations: Source currents, Lumped elements, and interfaces
// These functions are only called once per iteration, and have negligible
// effect on the speed.

#pragma optimize("s", on) // favour size

// include terms for source currents in the Jacobian and residual
void CSolve::DoSourceTerms()
{
	double voltage, current;
	int i;
	// Source currents
	for (i=0; i<MAX_SOURCES; i++) if (m_bSource[i]) {
		int maj; // which is the majority carrier?
		double Phimaj; // Phi of majority carrier
		int k=m_SourcePos[i];
		if (node.IsNeqGreaterThanPeq(k)) {
			maj = 0;	Phimaj = node.GetPhi_n(k); 
		} else  { 
			maj = 2;	Phimaj = node.GetPhi_p(k);
		}

		voltage = Phimaj;
		current = + (m_vSource[i] - voltage) / (m_rSource[i] + m_rIntSource[i]);

        mat.v[k*3 + maj] += current;
		mat.m[3*k + maj][diag] -= -1/(m_rSource[i] + m_rIntSource[i]);

        // Assign output results
        m_SourceV[i] = voltage + current*m_rIntSource[i]; m_SourceI[i] = current;
	}
}

// include terms for lumped element currents in the Jacobian and residual
void CSolve::DoLumpedElementTerms()
{
	double voltage, current;
	double Exp_decay_factor_c;
	int i;
	// Lumped element currents
	for (i=0; i<MAX_LUMPED; i++) if (m_bLumped[i]) {
		// anode
		int anodek=m_AnodePos[i];
		int cathodek=m_CathodePos[i];
		int anodemaj, cathodemaj; // which is majority carrier? 0=electron, 2=holes
		double anPhimaj, cathPhimaj;

		if (node.IsNeqGreaterThanPeq(anodek)) {
					anodemaj=0; anPhimaj=node.GetPhi_n(anodek);
		} else {	anodemaj=2; anPhimaj=node.GetPhi_p(anodek); }
		if (node.IsNeqGreaterThanPeq(cathodek)) {
				 cathodemaj=0; cathPhimaj=node.GetPhi_n(cathodek);
		} else { cathodemaj=2; cathPhimaj=node.GetPhi_p(cathodek); }

		voltage = anPhimaj - cathPhimaj;

		double matrixchange; // amount to change anode + cathode Phimaj by.

		switch (m_tLumped[i])
		{
		case CONDUCTOR:
			current = voltage*m_vLumped[i]; 
			matrixchange=-m_vLumped[i];
			break;
		case DIODE:
			current = m_vLumped[i]*CMath::exp(voltage/m_nLumped[i]);
			matrixchange = -m_LumpedI[i]/m_nLumped[i]; 
			break;
		case CAPACITOR:
            if (fabs(voltage)<fabs(V_Old[i]) && voltage*V_Old[i]>0)
			{  	// Exponential
           		current = m_vLumped[i]*m_rdt*voltage*CMath::log(voltage/V_Old[i]);
           		Exp_decay_factor_c = 1 + CMath::log(voltage/V_Old[i]);
           	}
			else
            {	// Linear
				current = m_vLumped[i]*m_rdt*(voltage-V_Old[i]); 
				Exp_decay_factor_c = 1;
			}
			matrixchange = -m_vLumped[i]*m_rdt*Exp_decay_factor_c;
			break;
		}
		mat.v[3*anodek+anodemaj] += -current;
		mat.v[3*cathodek+cathodemaj] += current;

		mat.m[3*anodek+anodemaj][diag] -= matrixchange; 
		mat.m[3*cathodek+cathodemaj][diag] -= matrixchange;

		// Assign output results
		m_LumpedV[i] = voltage; m_LumpedI[i] = current;
	}
}

// Surface/interface recombination
void CSolve::DoRegionInterfaceTerms()
{
	int i;
	int k, row;
	int lastk = -100; // nonsense initialization value. Used to ensure only one surface per node
	double nie;
	double Recomb, dRs_dPhin, dRs_dPhip, dRs_dPsi;

	// Surface/interface recombination
	for (i=0; i<m_NumRegions; i++)
	{
		////////// Front of region /////////////
		k=m_RegionFrontPos[i];
		if (k==lastk) continue; // max one surface at each node
		lastk=k;

	    AreaC = node.GetArea(k);
		nC=node.GetN_nir(k);
		pC=node.GetP_nir(k);
		nie=node.GetNie_nir(k);
/***
		CPhysics::Surf_Recomb_3(Recomb, dRs_dPhin, dRs_dPhip, 
				AreaC, nC, pC, nie, 
      			m_SnFront[i], m_SpFront[i], m_EtFront[i], m_JoFront[i]);
***/
		CSurfaceRecomData ss;
		ss=m_regiondata.GetFrontRecom(i);
		CPhysics::Surf_Recomb_3(Recomb, dRs_dPhin, dRs_dPhip, 
				AreaC, nC, pC, nie, 
      			ss.m_Sn, ss.m_Sp, ss.m_Et, ss.m_Jo);
		dRs_dPsi = - (dRs_dPhin + dRs_dPhip);

		row=k*3;

		// Load Residual
	    mat.v[row] += Recomb;
	    mat.v[row+2] += -Recomb;

		// Load Jacobian
		mat.m[row][diag] -= dRs_dPhin;
	    mat.m[row][diag+1] -= dRs_dPsi;
	    mat.m[row][diag+2] -= dRs_dPhip;
	    mat.m[row+2][diag-2] -= -dRs_dPhin;
	    mat.m[row+2][diag-1] -= -dRs_dPsi;
	    mat.m[row+2][diag] -= -dRs_dPhip;

		////////// Rear of region /////////////

		k=m_RegionRearPos[i];
		if (k==lastk) continue; // max one surface at each node
		lastk=k;

	    AreaC = node.GetArea(k);
		nC=node.GetN_nir(k);
		pC=node.GetP_nir(k);
		nie=node.GetNie_nir(k);

		ss=m_regiondata.GetRearRecom(i);
		CPhysics::Surf_Recomb_3(Recomb, dRs_dPhin, dRs_dPhip, 
				AreaC, nC, pC, nie, 
      			ss.m_Sn, ss.m_Sp, ss.m_Et, ss.m_Jo);
/***
		CPhysics::Surf_Recomb_3(Recomb, dRs_dPhin, dRs_dPhip, 
				AreaC, nC, pC, nie, 
      			m_SnRear[i], m_SpRear[i], m_EtRear[i], m_JoRear[i]);
***/
		dRs_dPsi = - (dRs_dPhin + dRs_dPhip);

		row=k*3;

		// Load Residual
	    mat.v[row] += Recomb;
	    mat.v[row+2] += -Recomb;

		// Load Jacobian
	    mat.m[row][diag] -= dRs_dPhin;
	    mat.m[row][diag+1] -= dRs_dPsi;
	    mat.m[row][diag+2] -= dRs_dPhip;
	    mat.m[row+2][diag-2] -= -dRs_dPhin;
	    mat.m[row+2][diag-1] -= -dRs_dPsi;
	    mat.m[row+2][diag] -= -dRs_dPhip;
	}
}

#pragma optimize("t", on) // favour speed

//  -----------------------------------------------
//				Update_npPsi
//  -----------------------------------------------

// Uses the results of the matrix inversion to generate new values for
// Psi, Phin and Phip.
// * Applies clamps
// * Calculates the convergence error.

// does the work of the old Return_Values() function as well.
void CSolve::Update_npPsi()
{
	double rClamp= 1.0/m_Clamp; // reciprocal of clamp - Doubles speed of this function!
	double dPhi2;
	double nfract1, nfract2, pfract1, pfract2;
	double dPhin_, dPhip_;
	double dPsi, dPhin, dPhip;

	double maxdiff=8;
	double flatclamp=8;

	double dPsiL, dPhinL, dPhipL;
	dPsiL=dPhinL=dPhipL=0.0; // arbitrary value

	m_Error=0;
	double Err;
	for (int k=0; k<=node.GetLastNode(); k++)
	{
		dPhin = mat.v[3*k];
		dPsi  = mat.v[3*k+1];
		dPhip = mat.v[3*k+2];

	 	Err=dPsi;
		if ((fabs(Err) < fabs(dPhin))) Err = dPhin;
		if ((fabs(Err) < fabs(dPhip))) Err = dPhip;
//        m_LastError[k]=Err;
		if ((m_Error < fabs(Err))) m_Error=fabs(Err);


		// Now apply the clamps


		// Electrostatics
        dPsi /= 1 + fabs(dPsi*rClamp);

		// Electrons + holes
		dPhin_ = dPhin;
		dPhip_ = dPhip;

		// The next bit of code is slightly obtuse because it avoids a (slow) division.
		// 
		// Phi clamping:  dPhi = dPhi/fract1
		// Psi clamping:  dPhi = dPsi + (dPhin-dPsi)/fract2
		//   where fract1, fract2 are:
		nfract1 = 1+fabs(dPhin*rClamp);
		nfract2 = 1+fabs((dPhin-dPsi)*rClamp);
		pfract1 = 1+fabs(dPhip*rClamp);
		pfract2 = 1+fabs((dPhip-dPsi)*rClamp);

		if (m_bClampPhi && m_bClampPsi) {

			// want to use smallest one. Which one is smaller?
			// we can find this out without a costly divide:
			// a/b < c/d <===>   a*d < c*b  when a,b,c,d positive
			dPhi2 = nfract2*dPsi + (dPhin-dPsi);
			if (fabs(dPhi2)*nfract1 < fabs(dPhin)*nfract2) 
				 dPhin_ = dPsi + (dPhin-dPsi) / nfract2;	// use psi, it is smaller
			else dPhin_ = dPhin / nfract1;    // use phi

			dPhi2 = pfract2*dPsi + (dPhip-dPsi);
			if (fabs(dPhi2)*pfract1 < fabs(dPhip)*pfract2) 
				 dPhip_ = dPsi + (dPhip-dPsi) / pfract2;
			else dPhip_ = dPhip / pfract1;

		} else if (m_bClampPhi) {

			dPhin_ = dPhin / nfract1;
			dPhip_ = dPhip / pfract1;

		} else if (m_bClampPsi) {

			dPhi2 = dPsi + (dPhin-dPsi) / nfract2;
			if (fabs(dPhi2)<fabs(dPhin_)) dPhin_ = dPhi2;
			dPhi2 = dPsi + (dPhip-dPsi) / pfract2;
			if (fabs(dPhi2)<fabs(dPhip_)) dPhip_ = dPhi2;
		}
		// new spike squasher!
/*******
	if (k>0) {
		if (fabs(dPhinL-dPhin_)>maxdiff) {
			if (dPhin_>dPhinL) dPhin_ = dPhinL + maxdiff;
			else dPhin_ = dPhinL - maxdiff;
		}
		if (fabs(dPhipL-dPhip_)>maxdiff) {
			if (dPhip_>dPhipL) dPhip_ = dPhipL + maxdiff;
			else dPhip_ = dPhipL - maxdiff;
		}
	}
		dPsiL=dPsi;
		dPhinL=dPhin_;
		dPhipL=dPhip_;
*******/
		if (dPhin_>flatclamp) dPhin_=flatclamp;
		if (dPhin_<-flatclamp) dPhin_=-flatclamp;
		if (dPhip_>flatclamp) dPhip_=flatclamp;
		if (dPhip_<-flatclamp) dPhip_=-flatclamp;

#if FALSE
/**
		if (m_bUseGummelThisIteration) {
			switch (m_GummelVariable) {
			case 0:
					node.Psi[k] += dPsi;
					break;
			case 1:
					node.Phi_n[k] += dPhin_;
					break;
			case 2:
					node.Phi_p[k] += dPhip_;
					break;
			}
		} else {
			node.Psi[k] += dPsi;
			node.Phi_n[k] += dPhin_;
			node.Phi_p[k] += dPhip_;
		}
**/
#else 
		if (m_bUseGummelThisIteration) {
			switch (m_GummelVariable) {
			case 0: node.IncrementPsiPhinPhip(k, dPsi, 0, 0);
					break;
			case 1: node.IncrementPsiPhinPhip(k, 0, dPhin_, 0);
					break;
			case 2: node.IncrementPsiPhinPhip(k, 0, 0, dPhip_);
					break;
			}
		} else {
			node.IncrementPsiPhinPhip(k, dPsi, dPhin_, dPhip_);
		}
#endif
	}
}

double CSolve::GetLastErrorAtNode(int k)
{
	double dPhin, dPsi, dPhip, Err;
	if (mat.GetBandwidth()==11) {
		dPhin = mat.v[3*k];
		dPsi  = mat.v[3*k+1];
		dPhip = mat.v[3*k+2];
 		Err=dPsi;
		if ((fabs(Err) < fabs(dPhin))) Err = dPhin;
		if ((fabs(Err) < fabs(dPhip))) Err = dPhip;
	} else { // equilibrium matrix
		dPsi  = mat.v[k];
 		Err=dPsi;
	}
	return Err;
}


//  -----------------------------------------------
//				Adjust_Gammas
//  -----------------------------------------------

void CSolve::Reset_Gammas()
{
	for (int k=0; k<=node.GetLastNode(); k++) 
	{
		node.ChangeGamma_n(k, 0.0);
		node.ChangeGamma_p(k, 0.0);
//		node.Un[k] -= node.GetGamma_n(k);
//		node.Up[k] -= node.GetGamma_p(k);
//		node.gamma_n[k] = node.gamma_p[k] = 0;
	}
}

#if TRUE

// Adjust the gamma values to implement velocity saturation.
// There are two ways of doing this:
// Normally, UsePsi is TRUE (saturate velocity based on the electric field only)
// if UsePsi is FALSE, use total velocity saturation (saturate based on phi_n. This is more accurate).
//
// Returns TRUE if any of the gamma values changed. This indicates that the system has not converged.
// 
// BUGFIX 26/11/02: The electric field was being calculated incorrectly, resulting in a factor of 2 error.
//		It now uses the same function used by the plotting calculations.
BOOL CSolve::Adjust_Gammas(BOOL bUsePsi)
{
	BOOL change = FALSE;
	for (int k=0; k<=node.GetLastNode(); k++)
	{
		double tmp1, new_gamma;
		if (node.GetEsat_n(k)>0)
		{
			if (bUsePsi) tmp1=node.GetE_Vt(k);
			else		 tmp1 = node.GetI_n(k)/node.GetMun_times_N(k);
			tmp1 /= node.GetEsat_n(k);
			new_gamma = -0.5*log(1 + tmp1*tmp1);
			if (fabs(new_gamma-node.GetGamma_n(k))>0.01)
			{
				node.ChangeGamma_n(k, new_gamma);
				change = TRUE;
			}
		}
		if (node.GetEsat_p(k)>0)
		{
			if (bUsePsi) tmp1 = node.GetE_Vt(k);
			else		 tmp1= node.GetI_p(k)/node.GetMup_times_P(k);
			tmp1 /= node.GetEsat_p(k);
			new_gamma = -0.5*log(1 + tmp1*tmp1);
			if (fabs(new_gamma-node.GetGamma_p(k))>0.01)
			{
				node.ChangeGamma_p(k, new_gamma);
				change = TRUE;
			}
		}
	}
	return change;
}


#if 0
/***
// normally, UsePsi is TRUE
// if UsePsi is FALSE, use total velocity saturation
BOOL CSolve::Adjust_Gammas(BOOL bUsePsi)
{
	int k;
	double tmp1, new_gamma;
	BOOL change = FALSE;

    double dxL, dxR, rdxL, rdxR;
	double psitmp;
	dxL=dxR=rdxL=rdxR=1;

	for (k=0; k<=node.GetLastNode(); k++)
	{
		BOOL bEsatn, bEsatp;
		bEsatn=(node.GetEsat_n(k)>0);
		bEsatp=(node.GetEsat_p(k)>0);

		if (bUsePsi) {
				if (k>0)
				{
					dxL = dxR; 
					rdxL = rdxR;
				}
				if (k<node.GetLastNode())
				{
//					dxR = node.GetX(k+1) - node.GetX(k);
					dxR = node.Get_dxR(k); // node.GetX(k+1) - node.GetX(k);
					//rdxR = 1 / dxR;
//					if (dxR==dxL) rdxR=rdxL; // frequently avoid a slow division
//					else rdxR = 1 / dxR;
					rdxR = node.GetReciprocal_dxR(k);
				}

			  if (bEsatn || bEsatp) {
				  psitmp = node.GetE_Vt(k);
//  				if (k==0) psitmp = (node.GetPsi(k+1)-node.GetPsi(k))*rdxR;
//				else if (k==node.GetLastNode()) psitmp = (node.GetPsi(k)-node.GetPsi(k-1))*rdxL;
//				else psitmp = rdxR* ( 
//					 dxL*(node.GetPsi(k+1)-node.GetPsi(k))*rdxR
//					 + dxR*(node.GetPsi(k)-node.GetPsi(k-1))*rdxL  );
			   }
		}

		if (bEsatn)
		{
			if (bUsePsi)	tmp1=psitmp;
			else tmp1 = node.GetI_n(k)/node.GetMu_times_N(k); // = exp( Psi[k]+node.Un[k]-Phi_n[k]);
			tmp1 /= node.GetEsat_n(k);
			new_gamma = -0.5*log(1 + tmp1*tmp1);
			if (fabs(new_gamma-node.GetGamma_n(k))>0.01)
			{
				node.ChangeGamma_n(k, new_gamma);
				change = TRUE;
			}
		}
		if (bEsatp)
		{
			if (bUsePsi)	tmp1=psitmp;
			else
				tmp1= node.GetI_p(k)/node.GetMu_times_P(k); // = I_p[k]/exp(-Psi[k]+Up[k]+Phi_p[k]);
			tmp1 /= node.GetEsat_p(k);
			new_gamma = -0.5*log(1 + tmp1*tmp1);
			if (fabs(new_gamma-node.GetGamma_p(k))>0.01)
			{
				node.ChangeGamma_p(k, new_gamma);
				change = TRUE;
			}
		}
	}
	return change;
}
***/
#endif

#else

// normally, UsePsi is TRUE
// if UsePsi is FALSE, use total velocity saturation

BOOL CSolve::Adjust_Gammas(BOOL bUsePsi)
{
	int k;
	double tmp1, new_gamma;
//	double* Psi;
//	double* x;
	BOOL change = FALSE;
//	Psi = node.Psi; x = node.x;

    double dxL, dxR, rdxL, rdxR;
	double psitmp;
	dxL=dxR=rdxL=rdxR=1;

	for (k=0; k<=node.GetLastNode(); k++)
	{
		BOOL bEsatn, bEsatp;
		bEsatn=(node.GetEsat_n(k)>0);
		bEsatp=(node.GetEsat_p(k)>0);
		  if (bUsePsi) {
				if (k>0)
				{
					dxL = dxR; 
					rdxL = rdxR;
				}
				if (k<node.GetLastNode())
				{
					dxR = node.GetX(k+1) - node.GetX(k);
					//rdxR = 1 / dxR;
					if (dxR==dxL) rdxR=rdxL; // frequently avoid a slow division
					else rdxR = 1 / dxR;
				}

			  if (node.GetEsat_n(k)>0 || node.GetEsat_p(k)>0) {
  				if (k==0) psitmp = (node.GetPsi(k+1)-node.GetPsi(k))*rdxR;
				else if (k==node.GetLastNode()) psitmp = (node.GetPsi(k)-node.GetPsi(k-1))*rdxL;
				else psitmp = rdxR* ( 
					 dxL*(node.GetPsi(k+1)-node.GetPsi(k))*rdxR
					 + dxR*(node.GetPsi(k)-node.GetPsi(k-1))*rdxL  );
			   }
		  }

		if (node.GetEsat_n(k)>0)
		{
			if (bUsePsi)	tmp1=psitmp;
			else
				tmp1 = node.GetI_n(k)/node.GetMu_times_N(k); // = exp( Psi[k]+node.Un[k]-Phi_n[k]);
//				tmp1 = node.I_n[k]*CMath::exp(-(node.Psi[k]+node.Un[k]-node.Phi_n[k]));
			tmp1 /= node.GetEsat_n(k);
			new_gamma = -0.5*log(1 + tmp1*tmp1);
			if (fabs(new_gamma-node.GetGamma_n(k))>0.01)
			{
//				node.Un[k] -= node.GetGamma_n(k);
//				node.SetGamma_n(k, new_gamma);
//				node.Un[k] += node.GetGamma_n(k);
				node.ChangeGamma_n(k, new_gamma);
				change = TRUE;
			}
		}
		if (node.GetEsat_p(k)>0)
		{
			if (bUsePsi)	tmp1=psitmp;
			else
				tmp1= node.GetI_p(k)/node.GetMu_times_P(k); // = I_p[k]/exp(-Psi[k]+Up[k]+Phi_p[k]);
//				tmp1 = node.I_p[k]*CMath::exp(-(-node.Psi[k]+node.Up[k]+node.Phi_p[k]));
			tmp1 /= node.GetEsat_p(k);
			new_gamma = -0.5*log(1 + tmp1*tmp1);
#if FALSE 
/*
			if (fabs(new_gamma-node.gamma_p[k])>0.01)
			{
				node.Up[k] -= node.gamma_p[k];
				node.gamma_p[k] = new_gamma;
				node.Up[k] += node.gamma_p[k];
				change = TRUE;
			}
*/
#else
			if (fabs(new_gamma-node.GetGamma_p(k))>0.01)
			{
//				node.Up[k] -= node.GetGamma_p(k);
//				node.SetGamma_p(k, new_gamma);
//				node.Up[k] += node.GetGamma_p(k);
				node.ChangeGamma_p(k, new_gamma);
				change = TRUE;
			}
#endif
		}
	}
	return change;
}
#endif

void CSolve::UpdateVariablesAfterRenode()
{
	node.PrecalculateNie_Neq_Peq();
	node.PrecalculateReciprocal_dx();
	FindFixedNodePositions();
}


//  -----------------------------------------------
//		  PrepareForFirstSystemIteration
//  -----------------------------------------------

// Does the tasks which only need to be done once per iteration.
//  * renoding if necessary
//  * saving the values of n and p from the last timestep. 
//			They are needed by Calc_Integrals_and_Currents
//  * initializing I, V of lumped elements

// return FALSE=failed to converge                                                           
BOOL CSolve::PrepareForFirstSystemIteration(int &iter, double& progress, CString & message, BOOL need_renode, CDocument *pDoc)
{
	iter = 0;      
	ASSERT(&iter);
	m_Error = 1E6;
	m_bVelSat = FALSE;

	m_bUseGummelForSteadyState=FALSE;
	m_bUseGummelThisIteration=m_bUseGummelForSteadyState;
	m_GummelVariable=0;
	if (m_rdt!=0) m_bUseGummelThisIteration=FALSE;

	Reset_Gammas();
	progress = 0;
	if (need_renode)
	{
        message = "Renode";
		CDonStatusBar::UpdateStatusBar();
		SetFixedNodes();
   	    node.Renode(FALSE, m_SizeFactor, fixed_nodes);
		message.Empty();
		int iterEQ=0; double progressEQ=0;
	    if (!SolveEquilibrium(iterEQ, progressEQ, message, FALSE, pDoc)) return FALSE;
		UpdateVariablesAfterRenode();
    }
	node.SaveNintoNOld();

	int k;
    for (k=0; k<MAX_LUMPED; k++) V_Old[k] = m_LumpedV[k];
	for (k=0; k<MAX_LUMPED; k++) m_LumpedV[k] = m_LumpedI[k] = 0;
	for (k=0; k<MAX_SOURCES; k++) m_SourceV[k] = m_SourceI[k] = 0;	
	return TRUE;
}

//  -----------------------------------------------
//		  Do_Dynamic_Renoding
//  -----------------------------------------------

// Add a dynamic renode section. If we detect that either of the phi exceed 32Vt (~1eV)
// across one element, we do a renode. 35 is about the maximum value that doesn't give
// numerical inaccuracies.
void CSolve::Do_Dynamic_Renoding(CString & message)
{
	int k;
	int MightBlowUp=0;

		for (k=1; k<node.GetLastNode(); k++) {
			if (fabs(node.GetPhi_n(k-1)-node.GetPhi_n(k)) >32 ||
				fabs(node.GetPhi_p(k-1)-node.GetPhi_p(k)) >32 ) MightBlowUp++;
		}
		if (MightBlowUp>0) {
			if (node.GetLastNode()+MightBlowUp>MAX_ELEMENTS) {
				// it will almost certainly fail, because it's run out of nodes.
				// Last ditch effort - invoke Spike Squasher!
				// the idea is that if we can smooth it out a bit, maybe less nodes will
				// be required.
//				message = "Spike Squasher";
//				UpdateStatusBar();
//				Do_Spike_Squasher();
			}
			message = "Renode";
			CDonStatusBar::UpdateStatusBar();
			SetFixedNodes();
	   	    node.Renode(FALSE, m_SizeFactor, fixed_nodes);

//		int iterEQ=0; double progressEQ=0;
//	    if (!SolveEquilibrium(iterEQ, progressEQ, message, FALSE, pDoc)) return FALSE;

			message.Empty();
			UpdateVariablesAfterRenode();
		}
}				


//#define TIME_ITERATION
#ifdef TIME_ITERATION
		
// returns FALSE if needs to be called again (i.e. hasn't converged yet)
// returns TRUE if has converged
BOOL CSolve::DoNextSystemIteration(int& iter, double& progress, CString & message, BOOL allow_renode)
{

	CUseful::InitializeFPU();	// so we can detect numeric problems easily

	if (allow_renode) Do_Dynamic_Renoding(message);

	// Newton iterations

int tweentime=CUseful::CyclesSinceLastCall();

     Load_System( TRUE );

	if (CUseful::GetFPUStatus() &0x01) { //  NAN
		m_Error=2e100;
		return FALSE; // BAD value somewhere! -- convergence failure
		// Alternatively, we could try renoding at this point.
	}

int loadtime= CUseful::CyclesSinceLastCall();
	mat.LU_decompose();
//     mat.lu_decom(3*(node.GetLastNode()+1)-1);
int decomtime= CUseful::CyclesSinceLastCall();
//    mat.lu_solve(3*(node.GetLastNode()+1)-1);
	mat.LU_solve();

	if (CUseful::GetFPUStatus() &0x01) { //  NAN
		m_Error=2e100;
		return FALSE; // BAD value somewhere! -- convergence failure
	}
int solvetime= CUseful::CyclesSinceLastCall();
int updatetime;
//    Return_Values();
//	if (m_Error<1e100) {
		Update_npPsi();
updatetime= CUseful::CyclesSinceLastCall();
	if (m_Error<1e100) {
 		iter++;
		if (!m_bVelSat) Adjust_Gammas(TRUE);
	} 
int gammatime=CUseful::CyclesSinceLastCall();
	if (m_Error<m_ErrorLimit) 
	{
		m_bVelSat = TRUE;
		if (Adjust_Gammas(!m_bPhiSat) && m_Error<=m_ErrorLimit) m_Error = 1; // avoid exit
	}
	if (m_Error==0) progress = 1;
	else progress = 0.5*(1+CMath::log(m_Error)/CMath::log(m_ErrorLimit));

CString timingstr;
timingstr.Format("Tween=%d, LoadSys=%d, Decom=%d, Solve=%d, Update=%d, Gamma=%d", tweentime, loadtime, decomtime, solvetime, updatetime, gammatime);
CDonStatusBar::WriteStatusBarMessage(timingstr);
CUseful::CyclesSinceLastCall();
	if (m_Error<=m_ErrorLimit)
	{
		Load_System(FALSE);	// Ensures good values for result outputs
		return TRUE;
	}
	else return FALSE;
}

#else

//  -----------------------------------------------
//		  DoNextSystemIteration
//  -----------------------------------------------

// The central loop of PC1D! Perform one iteration (transient or steady-state)

// Implements Newton iterations
// * Load the Jacobian matrix representing current continuity & Poisson's eqn
// * Decompose the matrix into lower & upper triangular
// * Solve for update vector
// * Use update vector to update values of phi, phip, psi + estimate error.

// returns FALSE if needs to be called again (i.e. hasn't converged yet)
// returns TRUE if has converged
BOOL CSolve::DoNextSystemIteration(int& iter, double& progress, CString & message, BOOL allow_renode)
{
	CUseful::InitializeFPU();	// so we can detect numeric problems easily

	if (allow_renode) Do_Dynamic_Renoding(message);

	// Newton iterations
    Load_System( TRUE );

	if (CUseful::GetFPUStatus() &0x01) { //  NAN
		m_Error=2e100;	return FALSE; // BAD value somewhere! -- convergence failure
	}

	mat.LU_decompose();
	mat.LU_solve();

	if (CUseful::GetFPUStatus()&0x01) { // NAN
		m_Error=2e100;	return FALSE; // BAD value somewhere! -- convergence failure
	}
	Update_npPsi();

	if (m_bUseGummelThisIteration) {
		m_GummelVariable++;
		if (m_GummelVariable>2) m_GummelVariable=0;
		else return FALSE; // can't exit unless have done all variables
	}

	if (m_Error<1e100) {
 		iter++;
		if (!m_bVelSat) Adjust_Gammas(TRUE);
	} 
	if (m_Error<m_ErrorLimit) 
	{
		m_bVelSat = TRUE;
		if (Adjust_Gammas(!m_bPhiSat) && m_Error<=m_ErrorLimit) m_Error = 1; // avoid exit
	}
	if (m_Error==0) progress = 1;
	else progress = 0.5*(1+CMath::log(m_Error)/CMath::log(m_ErrorLimit));
	if (m_Error<=m_ErrorLimit)
	{
		Load_System(FALSE);	// Ensures good values for result outputs
		return TRUE;
	}
	else return FALSE;
}

#endif
