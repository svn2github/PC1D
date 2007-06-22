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
//////////////////////////////////////////////////////////////////////
// CNode

class CNode : public CObject
{
	DECLARE_SERIAL(CNode)
// Attributes
private:
	int		last_node;  // The following pointers are allocated memory in constructor
public:
	int GetNodeCount() const { return last_node+1; };
	int GetLastNode() const { return last_node; };
	int NumElements() const { return GetLastNode(); };
	void SetNumberOfNodes(int numnodes) { last_node=numnodes-1; };

	// Attributes -- solution variables
private:
	double	*x,     // cm
			*Area,	// normalized by Aref
			*Vn, *Vp,	// normalized by Vt
			*Nd, *Na,	// normalized by nir
			*Un, *Up,	// normalized by Vt
			*Phi_n, // normalized by Vt
			*Phi_p,	// normalized by Vt
			*Psi,	// normalized by Vt
			*Psi_eq;// normalized by Vt;
	double	*gamma_n, 
			*gamma_p,	// velocity saturation factors
			*Esat_n,	// cm-1, normalized by Vt
			*Esat_p,	// cm-1, normalized by Vt
			*taun,		// sec
			*taup,		// sec
			*G,			// cm/s, cumm, normalized by nir*Aref
			*eps,		// cm2, normalized by q*nir/Vt (Used by Renode)
			*I_n,	// cm/s, normalized by q*nir*Aref
			*I_p;	// cm/s, normalized by q*nir*Aref

	double	*n_Old, *p_Old;	// Last converged value of n, p -- used in CSolve::Calc_Integrals_and_Currents(int k)
	double  *nieFAST, *neqFAST, *peqFAST; // cached values - constant except when renoding
	double	*m_reciprocal_dx; // reciprocal of x[k+1]-x[k]

	int		*Reg;	// zero-based region number for each node

public: // Access to solution variables
	double GetGamma_n(int k) const { return gamma_n[k]; };
	double GetGamma_p(int k) const { return gamma_p[k]; };
	double GetEsat_n(int k) const { return Esat_n[k]; };
	double GetEsat_p(int k) const { return Esat_p[k]; };
	double GetPsi(int k)    const { return Psi[k]; };
	double GetPhi_n(int k)  const { return Phi_n[k]; };
	double GetPhi_p(int k)  const { return Phi_p[k]; };
	double GetPsi_eq(int k) const { return Psi_eq[k]; };
	double GetX(int k)		const { return x[k]; };
	double GetArea(int k)	const { return Area[k]; };
	double GetTaun(int k)	const { return taun[k]; };
	double GetTaup(int k)	const { return taup[k]; };
	double GetI_n(int k)	const { return I_n[k]; };
	double GetI_p(int k)	const { return I_p[k]; };
	double GetVn(int k)		const { return Vn[k]; };
	double GetVp(int k)		const { return Vp[k]; };
	double GetNa(int k)		const { return Na[k]; };
	double GetNd(int k)		const { return Nd[k]; };
	double GetEps(int k)	const { return eps[k]; };
	double GetGcum(int k)	const { return G[k];   };
	int GetRegion(int k)	const { return Reg[k]; };

	void SetEsat(int k, double esatn, double esatp) { Esat_n[k]=esatn; Esat_p[k]=esatp;};
	void SetPsi_eq(int k, double val) { Psi_eq[k]=val; };
	void SetX(int k, double val) { x[k]=val; };
	void SetArea(int k, double val) { Area[k]=val; };
	void SetTau(int k, double tau_n, double tau_p) { taun[k]=tau_n; taup[k]=tau_p; };
	void SetInIp(int k, double In, double Ip) { I_n[k]=In; I_p[k]=Ip; };
	void SetEps(int k, double val) { eps[k]=val; };
	void SetRegion(int k, int regnum) { Reg[k]=regnum; };
	void SetVnVp(int k, double v_n, double v_p) { Vn[k]=v_n; Vp[k]=v_p; };
	void SetUnUp(int k, double u_n, double u_p) { Un[k]=u_n; Up[k]=u_p; };
	void SetGcum(int k, double Gcum) { G[k]=Gcum; };
	void SetNaNd(int k, double na, double nd) { Na[k]=na; Nd[k]=nd; };

public:
	// Old "PlotLoad.cpp" functions. Returns value at node k.

	// ######## High speed functions #######
	// (in main Solve loop)
	double GetE_Vt(int k) const;
	inline double Get_dxR(int k) const;
	inline double GetReciprocal_dxR(int k) const;
	inline double GetNie_nir(int k) const;
	inline double GetN_nir(int k) const;
	inline double GetP_nir(int k) const;
	inline double GetNeq_nir(int k) const;
	inline double GetPeq_nir(int k) const;
	inline double GetFASTNie_nir(int k) const;
	inline double GetFASTNeq_nir(int k) const;
	inline double GetFASTPeq_nir(int k) const;
	inline double GetMun_times_N(int k) const;
	inline double GetMup_times_P(int k) const;
	inline double GetN_Old(int k) const;
	inline double GetP_Old(int k) const;
	inline bool IsNeqGreaterThanPeq(int k) const;

	double GetDelta_PsiUn(int k) { return (Psi[k+1]+Un[k+1]) - (Psi[k]+Un[k]); };
	double GetDelta_PsiUp(int k) { return (Psi[k+1]-Up[k+1]) - (Psi[k]-Up[k]); };

	void IncrementPsi_eq(int k, double dPsi) { Psi_eq[k]+=dPsi; };
	void IncrementPsiPhinPhip(int k, double dPsi, double dPhin, double dPhip) {
			Psi[k] += dPsi; Phi_n[k] += dPhin; Phi_p[k] += dPhip;
	};
	//	void SetGammas(int k, double gamman, double gammap) { gamma_n[k]=gamman; gamma_p[k]=gammap;};
	void ChangeGamma_n(int k, double newgamman) { 
		Un[k]-=gamma_n[k]; gamma_n[k]=newgamman; Un[k]+=newgamman; };
	void ChangeGamma_p(int k, double newgammap) { 
		Up[k]-=gamma_p[k]; gamma_p[k]=newgammap; Up[k]+=newgammap; };

	// ### Lower priority functions (only in PlotLoad) ###
	// some other functions are in CSolve, where they have
	// access to the region variables

	inline bool IsNGreaterThanP(int k) const;
	inline double GetLogN_nir(int k) const;
	inline double GetLogP_nir(int k) const;
	double GetPNratio(int k) const;
	double GetMup(int k) const;		// must divide by Vt*Area
	double GetMun(int k) const;		// must divide by Vt*Area
	double GetMun_times_Neq(int k) const;
	double GetMup_times_Peq(int k) const;
	double GetMun_times_Vt(int k) const;
	double GetMup_times_Vt(int k) const;
	double GetMun_includinghighfieldeffect(int k) const; // must divide by Vt*Area
	double GetMup_includinghighfieldeffect(int k) const; // must divide by Vt*Area
	double GetGenerationRate(int k) const; // must multiply by nir/Area

// Operations
public:
	CNode();
    ~CNode();
	void Initialize();
	void Serialize(CArchive& ar);	// Stores and recovers public members only
	void Renode(BOOL use_eq, double alpha, const double fixed_nodes[]);
		// use_eq means use Psi_eq rather than Psi for basis
		// alpha is element size factor
		// fixed_nodes are nodes that should not be removed during renode
	// The elements are sized so that no element is too big with respect to
	// the following considerations: (1) Deviation of electric potential from spatial
	// linearity, (2) Fraction of device width, (3) Change in majority doping, and (4) 
	// Logarithmic spacing front/rear for photogeneration.  In addition, steps are taken to 
	// ensure that the location of the contact and interface nodes are not altered.

// Implementation
private:
	double *m_allocatednodearray; // MICROSOFT BUGFIX!!!!!!!!!!!!!
	BOOL m_bUseEQ;		// Use equilibrium values for node placement
	const double *m_pFixed_nodes;	// nodes to remain fixed during renoding
    int m_OldLastNode;
	double m_alpha;		// local copy of alpha
    int m_RenodeList[MAX_ELEMENTS+1];
public:
	void SaveNintoNOld(); // save n, p into nOld, pOld
	void CopyPsi_eqIntoPsi(); // Set Psi[k]=Psi_eq
	void PrecalculateNie_Neq_Peq();
	void PrecalculateReciprocal_dx();
private:
	BOOL InFixedNodeArray(double x, const double a[]);	// Is node at x in fixed-node array a?
	void Setup_Renode_List(); // modifies m_alpha
	void Transfer_node(const double Node_Vector[], double Temp_Vector[],
						int& node_index,  int& temp_index);
	void Transfer_node(const int Node_Vector[], int Temp_Vector[],
						int& node_index,  int& temp_index);
	void Concatenate_Elements(int Concatenation_count,
						const double Node_Vector[], double Temp_Vector[],
						int& node_index, int& temp_index);
	void Concatenate_Elements(int Concatenation_count,
						const int Node_Vector[], int Temp_Vector[],
						int& node_index, int& temp_index);
	void Subdivide_Element(int Subdivision_count,
						const double Node_Vector[], double Temp_Vector[],
						int& node_index, int& temp_index,
						CScaleType Interpolation);
	void Subdivide_Element(int Subdivision_count,
						const int Node_Vector[], int Temp_Vector[],
						int& node_index, int& temp_index);
	void Adjust_Nodes(double Node_Vector[], CScaleType Interpolation);
	void Adjust_Nodes(int Node_Vector[]);
	int  Adjust_count(int& current_node, int whichattempt);
};


// Old "PlotLoad.cpp" functions. Returns value at node k.

// Functions used in the main loop
// These must be VERY fast

inline double CNode::Get_dxR(int k) const
{	return (x[k+1] - x[k]);
}

inline double CNode::GetNie_nir(int k) const
{	return exp( (Vn[k]+Vp[k]) * 0.5  );
}

inline double CNode::GetN_nir(int k) const
{	return exp( Psi[k]+Vn[k]-Phi_n[k] );
}	

inline double CNode::GetP_nir(int k) const
{	return exp(-Psi[k]+Vp[k]+Phi_p[k]);
}

inline double CNode::GetNeq_nir(int k)	const
{	return exp(Psi_eq[k] + Vn[k]);	
}

inline double CNode::GetPeq_nir(int k) const
{	return exp(-Psi_eq[k] + Vp[k]);	
}

inline double CNode::GetMun_times_N(int k) const
{	return exp(Psi[k]+Un[k]-Phi_n[k]);
}

inline double CNode::GetMup_times_P(int k) const
{	return exp(-Psi[k]+Up[k]+Phi_p[k]);
}

inline double CNode::GetN_Old(int k) const
{ return n_Old[k]; }

inline double CNode::GetP_Old(int k) const
{ return p_Old[k]; }

// Speedup functions...
inline double CNode::GetFASTNie_nir(int k) const
{	return nieFAST[k];
}

inline double CNode::GetFASTNeq_nir(int k) const
{	return neqFAST[k];
}

inline double CNode::GetFASTPeq_nir(int k) const
{	return peqFAST[k];
}

inline double CNode::GetReciprocal_dxR(int k) const
{	
	return m_reciprocal_dx[k];
//	return 1/(x[k+1] - x[k]);
}

// FAST! avoid calculating the exponentials
inline bool CNode::IsNeqGreaterThanPeq(int k) const
{	return (2*Psi_eq[k]>(Vp[k]-Vn[k]));
}

// not used in main loop, but it's simple so include here anyway
inline bool CNode::IsNGreaterThanP(int k) const
{	return 2*Psi[k] > ( Vp[k]+Phi_p[k] - Vn[k]+Phi_n[k] );
}

inline double CNode::GetLogN_nir(int k) const
{	return Psi[k]+Vn[k]-Phi_n[k];
}	

inline double CNode::GetLogP_nir(int k) const
{	return -Psi[k]+Vp[k]+Phi_p[k];
}
