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

#include "node.h"
#include "matrix.h"

/////////////////////////////////////////////////////////////////////////////
// CSolve

#include "RegionData.h"

class CSolve: public CObject
{   
	DECLARE_SERIAL(CSolve)
public:
	CSolve();
	virtual ~CSolve();
	virtual void Serialize(CArchive& ar);
	// Attributes -- Problem definition
private:
	CNode	node;		///< All node data, including last_node, and renode function
	CRegionData m_regiondata; ///< All region data
public:
	CNode *GetNodeData() { return &node; };
	CRegionData *GetRegionData() { return &m_regiondata; };
	/// External voltage sources
public:
	BOOL	m_bSource[MAX_SOURCES];	// source is connected
	double	m_vSource[MAX_SOURCES]; // voltages, normalized by Vt
	double	m_rSource[MAX_SOURCES];	// resistances, s/cm, normalized by Vt/(q*nir*Aref)
private:
	double	m_xSource[MAX_SOURCES];	// source contact locations
	double	m_rIntSource[MAX_SOURCES];	// internal resistance, s/cm, normalized as above
public:
	void SetSource(int num, BOOL bEnabled, double voltage, double resistance) {
		m_bSource[num] = bEnabled; m_vSource[num]=voltage; m_rSource[num]=resistance;
	}
	void SetSourceLocation(int num, double position) {
		m_xSource[num] = position;
	}
	void SetSourceResistance(int num, double internalresistance) {
		m_rIntSource[num]=internalresistance;
	}

	/// Lumped elements
private:
	CElemType m_tLumped[MAX_LUMPED]; // lumped element type
	BOOL	m_bLumped[MAX_LUMPED];	// element is connected
	double	m_xLumpedA[MAX_LUMPED];	// lumped element anode contact location
	double	m_xLumpedC[MAX_LUMPED];	// lumped element cathode contact location
	double	m_nLumped[MAX_LUMPED];	// lumped element ideality factor
	double	m_vLumped[MAX_LUMPED];	// lumped element value, normalized using:
									//  Diode Io (cm/s) - q*nir*Aref
									//	Conductance G (cm/s) - q*nir*Aref/Vt
									//	Capacitance C (cm) - q*nir*Aref/Vt
public:
	void SetLumpedElement(int num, CElemType ty, BOOL enabled, double value, double nfactor) {
		m_tLumped[num]=ty; m_bLumped[num]=enabled; m_vLumped[num]=value; m_nLumped[num]=nfactor; 
	};
	void SetLumpedElementPosition(int elem, double anode, double cathode) {
		if (m_bLumped[elem]){
			int k=0; while (k<node.GetLastNode() && node.GetX(k)<anode) k++;
			m_xLumpedA[elem] = node.GetX(k);
			k=0; while (k<node.GetLastNode() && node.GetX(k)<cathode) k++;
			m_xLumpedC[elem] = node.GetX(k);
		}
	};
public:
	CSurface m_FrontSurface, m_RearSurface;
	double	m_NssFront, m_NssRear;		// cm, normalized by nir
	double 	m_rdt;			// sec-1 (reciprocal time step size)

	// Attributes -- Numerical options
public:
// Set from INI file
	BOOL	m_bStoreNodes;
// Set from numeric dialog
	BOOL	m_bUsePrevious;
	BOOL	m_bRenodeEQ, m_bRenodeSS, m_bRenodeTR;
	BOOL	m_bClampPhi, m_bClampPsi;
	BOOL	m_bUseGummelForSteadyState;
	BOOL	m_bGraphsAfterEveryIteration;
	int		m_nMaxTime;     // sec
	double	m_SizeFactor, m_ErrorLimit, m_Clamp;
	BOOL	m_bNewSolve;	// indicates use of log conductivity instead of linear psi
	// Attributes -- Calculated results
public:
	double	m_SourceV[MAX_SOURCES];	// normalized by Vt
	double	m_SourceI[MAX_SOURCES];	// cm/s, normalized by q*nir*Aref
	double	m_LumpedV[MAX_LUMPED];	// normalized by Vt
	double	m_LumpedI[MAX_LUMPED]; // cm/s, normalized by q*nir*Aref
private:
	double	m_Error; // Solution error (the worst error)
public:
	bool HasHugeError() const { return m_Error>1e100; }; 

	// Attributes -- Calculations (this should be in CNode)
private:
	double GetRBulk_np_specified(int k, double n_nir, double p_nir);
public:
	double GetBulkRecombination_nir(int k);
	double GetTau(int k);

	// Attributes -- Local variables
private:
	CString INIheading, INIstore;
	double	fixed_nodes[FIXED_NODE_ARRAY_SIZE];
	CMatrix mat;
	double	V_Old[MAX_LUMPED];	// previous voltage across each lumped element
	BOOL	m_bVelSat;			// Enables velocity saturation after error drops below threshhold
	BOOL	m_bPhiSat;			// Enables use of grad-Phi instead of grad-Psi for velocity saturation
	double	Exp_decay_factor_n, Exp_decay_factor_p;

	double	dxL, dxR, rdxL, rdxR, 
			m_epsL, m_epsR,
			nL, nC, nR, pL, pC, pR,
			neqL, neqC, neqR, peqL, peqC, peqR,
//			Nd_aL, Nd_aC, Nd_aR,
			rhoL, rhoC, rhoR,
			GenC, GenR, RecC, RecR, 
			dRdPhinC, dRdPhipC, dRdPsiC, dRdPhinR, dRdPhipR, dRdPsiR,
			PsiL, PsiC, PsiR,
			Phi_nL, Phi_nC, Phi_nR,
			Phi_pL, Phi_pC, Phi_pR,

			mu_times_nC, mu_times_nR,
			mu_times_pC, mu_times_pR,
	//			expDeltaPhin_1, exp_DeltaPhip_1,
			munCexpDeltaPhin_dxR, mupCexpDeltaPhip_dxR,

			AreaL, AreaC, AreaR,
			InL, IpL, InR, IpR,
			dInL_dPhinL, dInL_dPhinC, dInL_dPsiL, dInL_dPsiC, dInL_dPhipL, dInL_dPhipC,
			dIpL_dPhipL, dIpL_dPhipC, dIpL_dPsiL, dIpL_dPsiC, dIpL_dPhinL, dIpL_dPhinC,
			dInR_dPhinR, dInR_dPhinC, dInR_dPsiR, dInR_dPsiC, dInR_dPhipR, dInR_dPhipC,
			dIpR_dPhipR, dIpR_dPhipC, dIpR_dPsiR, dIpR_dPsiC, dIpR_dPhinR, dIpR_dPhinC,
			dInR_dPhinL, dInL_dPhinR, dIpR_dPhipL, dIpL_dPhipR,	// VelSat
			Zn, Zn_, Zp, Zp_, 
			Yn, Yp_,			// Yn_, Yp are not needed
			dZn_dPsi, dZpdPsi,
			dYndPsi, dYp_dPsi,

			IntGR,
			dIntGR_dPsiR,  dIntGR_dPsiC,
			dIntGR_dPhinC, dIntGR_dPhinR,
			dIntGR_dPhipC, dIntGR_dPhipR,
			Int_dndt, Int_dpdt,
			dInt_dndt_dPsiC, dInt_dndt_dPsiR, dInt_dndt_dPhinC, dInt_dndt_dPhinR,
			dInt_dpdt_dPsiC, dInt_dpdt_dPsiR, dInt_dpdt_dPhipC, dInt_dpdt_dPhipR;
// Implementation
public:
	void Initialize();
	BOOL SetNumericParameters();	// dialog box
	void StoreOptions();			// call when solve-oriented options are affected
	BOOL SolveEquilibrium(int& iter, double& progress, CString& message, BOOL need_renode, CDocument *pDoc);
	BOOL DoNextEquilibriumIteration(int& iter, double& progress, CString& message, BOOL &need_renode);
	void UpdateVariablesAfterRenode();
	BOOL PrepareForFirstSystemIteration(int &iter, double& progress, CString & message, BOOL need_renode, CDocument *pDoc);
	BOOL DoNextSystemIteration(int& iter, double& progress, CString & message, BOOL allow_renode);
	double GetLastErrorAtNode(int k); // the last error at each node, used by LoadError in plotload.cpp
private:
	void SetFixedNodes();	// loads the fixed_nodes[] array
	void LoadEqMatrix();
	void Update_PsiEq();
	void Compute_Zn_functions(double Delta);
	void Compute_Zp_functions(double Delta);
	void Load_Local_Centre_Values(int k);
	void Load_local_values(int k);
	void Calc_Integrals_and_Currents(int k);
	void Calculate_Derivatives(int k);
	void Load_Residual(int k);
	void Load_Jacobian_and_Residual(int k);
	void DoSourceTerms();
	void DoLumpedElementTerms();
	void DoRegionInterfaceTerms();
	void Load_System(BOOL Jacobian);
	void Update_npPsi();
	void Reset_Gammas();
	BOOL Adjust_Gammas(BOOL UsePsi);
	void Do_Dynamic_Renoding(CString & message);
	void Do_Spike_Squasher();

private:
	BOOL m_bUseGummelThisIteration; // TRUE = use Gummel iterations, FALSE=use Newton
	int m_GummelVariable; // 0=psi, 1=phin, 2=phip

// shortcuts used for sources, lumped elements and regions
private:
	int m_AnodePos[MAX_LUMPED];
	int m_CathodePos[MAX_LUMPED];
	int m_SourcePos[MAX_SOURCES];
	int m_RegionFrontPos[MAX_REGIONS];
	int m_RegionRearPos[MAX_REGIONS];
	int m_NumRegions;
private:
	void FindFixedNodePositions();

};
/////////////////////////////////////////////////////////////////////////////
