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
#include "PhysicalConstants.h"
#include "ProgramLimits.h"
#include "math.h"
#include "mathstat.h"	// CMath
#include "node.h"

//////////////////////////////////////////////////////////////
// CNode

IMPLEMENT_SERIAL(CNode, CObject, 0)


const int NUMBEROFNODEVARIABLES=28; // 'double'-sized node variables
double *m_variables[NUMBEROFNODEVARIABLES];
CScaleType m_interpolation[NUMBEROFNODEVARIABLES];

  CNode::CNode()
{
	// MICROSOFT BUG: The 'new' operator doesn't align doubles properly!
	// Only aligns them on 4-byte boundary instead of 8-byte!
	int m_arraysize=(MAX_ELEMENTS+1)*sizeof(double); // size of each array
	m_allocatednodearray=new double[(MAX_ELEMENTS+1)*NUMBEROFNODEVARIABLES+4];
	char * tmpv;
	tmpv=(char *)((int)(m_allocatednodearray+4)&(-7)); 
	int i;
	for (i=0; i<NUMBEROFNODEVARIABLES; i++) {
		m_variables[i]=(double *)tmpv;
		tmpv+=m_arraysize;
	}
	x		= m_variables[0];
	Area	= m_variables[1];
	Vn		= m_variables[2];
	Vp		= m_variables[3];
	Nd		= m_variables[4];
	Na		= m_variables[5];
	Un		= m_variables[6];
	Up		= m_variables[7];
	gamma_n	= m_variables[8];
	gamma_p = m_variables[9];
	taun	= m_variables[10];
	taup	= m_variables[11];
	G		= m_variables[12];
	eps		= m_variables[13];
	Esat_n	= m_variables[14];
	Esat_p	= m_variables[15];
	Psi_eq	= m_variables[16];
	I_n		= m_variables[17];
	I_p		= m_variables[18];
	Phi_n	= m_variables[19];
	Phi_p	= m_variables[20];
	Psi		= m_variables[21];
	n_Old	= m_variables[22];
	p_Old	= m_variables[23];
	nieFAST = m_variables[24];
	neqFAST = m_variables[25];
	peqFAST = m_variables[26];
	m_reciprocal_dx = m_variables[27];
	Reg		= new int	[MAX_ELEMENTS+1];
 	Initialize();
}

/***
CNode::CNode()
{
	x		= new double[MAX_ELEMENTS+1];
	Area	= new double[MAX_ELEMENTS+1];
	Vn		= new double[MAX_ELEMENTS+1];
	Vp		= new double[MAX_ELEMENTS+1];
	Nd		= new double[MAX_ELEMENTS+1];
	Na		= new double[MAX_ELEMENTS+1];
	Un		= new double[MAX_ELEMENTS+1];
	Up		= new double[MAX_ELEMENTS+1];
	gamma_n	= new double[MAX_ELEMENTS+1];
	gamma_p = new double[MAX_ELEMENTS+1];
	taun	= new double[MAX_ELEMENTS+1];
	taup	= new double[MAX_ELEMENTS+1];
	G		= new double[MAX_ELEMENTS+1];
	eps		= new double[MAX_ELEMENTS+1];
	Esat_n	= new double[MAX_ELEMENTS+1];
	Esat_p	= new double[MAX_ELEMENTS+1];
	Psi_eq	= new double[MAX_ELEMENTS+1];
	I_n		= new double[MAX_ELEMENTS+1];
	I_p		= new double[MAX_ELEMENTS+1];
	Phi_n	= new double[MAX_ELEMENTS+1];
	Phi_p	= new double[MAX_ELEMENTS+1];
	Psi		= new double[MAX_ELEMENTS+1];
	n_Old	= new double[MAX_ELEMENTS+1];
	p_Old	= new double[MAX_ELEMENTS+1];
	nieFAST = new double[MAX_ELEMENTS+1];
	neqFAST = new double[MAX_ELEMENTS+1];
	peqFAST = new double[MAX_ELEMENTS+1];
	Reg		= new int	[MAX_ELEMENTS+1];

	Initialize();
}
*****/

CNode::~CNode()
{
	delete [] m_allocatednodearray;
/*****
    delete [] x;
    delete [] Area;
    delete [] Vn;
    delete [] Vp;
    delete [] Nd;
    delete [] Na;
    delete [] Un;
    delete [] Up;
    delete [] gamma_n;
    delete [] gamma_p;
    delete [] taun;
    delete [] taup;
    delete [] G;
    delete [] eps;
    delete [] Esat_n;
    delete [] Esat_p;
    delete [] Psi_eq;
    delete [] I_n;
    delete [] I_p;
    delete [] Phi_n;
    delete [] Phi_p;
    delete [] Psi;
	delete [] n_Old;
	delete [] p_Old;
	delete [] nieFAST;
	delete [] neqFAST;
	delete [] peqFAST;
***/
    delete [] Reg;
}         

void CNode::Initialize()
{
    last_node = m_OldLastNode = MAX_ELEMENTS;
    m_bUseEQ = TRUE;
    int k;
    for (k=0; k<=last_node; k++) m_RenodeList[k] = 0;
    for (k=0; k<=last_node; k++)
    {
        x[k] = 1e-5*k;  // equal-sized elements, .1 micron each
        Nd[k] = Na[k] = 1;
        Area[k] = 1;
        Vn[k] = Vp[k] = 0;
        Un[k] = Up[k] = 0;
        gamma_n[k] = gamma_p[k] = 0;
        taun[k] = taup[k] = 1e-6;
        G[k] = Psi_eq[k] = 0;
        eps[k] = 1e-5;
        Esat_n[k] = Esat_p[k] = 1e6;
        I_n[k] = I_p[k] = Phi_n[k] = Phi_p[k] = Psi[k] = 0;
        Reg[k] = 0;
    }
}

void CNode::Serialize(CArchive& ar)
{
// TODO: Would be nice to store gamma_n, gamma_p but with backward compatibility
	int k;
	CObject::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << (WORD)last_node;
		for (k=0; k<=last_node; k++)
		{
			ar << x[k] << Area[k] << Vn[k] << Vp[k];
			ar << Nd[k] << Na[k] << Un[k] << Up[k];
			ar << taun[k] << taup[k] << G[k] << Psi_eq[k];
			ar << eps[k] << Esat_n[k] << Esat_p[k];
			ar << I_n[k] << I_p[k] << Phi_n[k] << Phi_p[k] << Psi[k];
			ar << (WORD)Reg[k];
		}
	}
	else
	{
		ar >> (WORD&)last_node;
		for (k=0; k<=last_node; k++)
		{
			ar >> x[k] >> Area[k] >> Vn[k] >> Vp[k];
			ar >> Nd[k] >> Na[k] >> Un[k] >> Up[k];
			ar >> taun[k] >> taup[k] >> G[k] >> Psi_eq[k];
			ar >> eps[k] >> Esat_n[k] >> Esat_p[k];
			ar >> I_n[k] >> I_p[k] >> Phi_n[k] >> Phi_p[k] >> Psi[k];
			ar >> (WORD&)Reg[k];
		}
		m_OldLastNode=last_node;
	}
}

#pragma optimize("",off)	// to avoid problems with the following function call

void CNode::Setup_Renode_List() // modifies m_alpha
{
    double a_factr = 1;
    int passnumber=0;
	int fr_nodes = 0, st_node = 0, cr_node = 0;
	do
	{
		cr_node = 0;
		fr_nodes = MAX_ELEMENTS - last_node;
		m_alpha *= a_factr;
		do
		{
			st_node = cr_node;
			m_RenodeList[st_node] = Adjust_count(cr_node, passnumber);
			fr_nodes = fr_nodes - m_RenodeList[st_node];
		}
		while (cr_node<last_node);
		a_factr = (double)(MAX_ELEMENTS-fr_nodes-20)/(MAX_ELEMENTS-20);
		if (a_factr>2.0) a_factr = 2.0;
		passnumber++;
	}
	while (fr_nodes<0);
	m_OldLastNode = last_node;
	last_node = MAX_ELEMENTS - fr_nodes;
}

#pragma optimize("", on)

void CNode::Transfer_node(const double Node_Vector[], double Temp_Vector[],
                        int& node_index,  int& temp_index)
{
	temp_index++; node_index++;
	Temp_Vector[ temp_index ] = Node_Vector[ node_index ];
}

void CNode::Transfer_node(const int Node_Vector[], int Temp_Vector[],
                        int& node_index,  int& temp_index)
{
	temp_index++; node_index++;
	Temp_Vector[ temp_index ] = Node_Vector[ node_index ];
}

void CNode::Concatenate_Elements(int Concatenation_count,
						const double Node_Vector[], double Temp_Vector[],
						int& node_index, int& temp_index)
{
	node_index += Concatenation_count;
	Transfer_node(Node_Vector, Temp_Vector, node_index, temp_index);
}

void CNode::Concatenate_Elements(int Concatenation_count,
						const int Node_Vector[], int Temp_Vector[],
						int& node_index, int& temp_index)
{
	node_index += Concatenation_count;
	Transfer_node(Node_Vector, Temp_Vector, node_index, temp_index);
}

// DAC 19/5/97: The old LOG section of this code was buggy.
// if R is tiny, would make the new value be equal to L instead of R.
// We can do much better than this, esp. when L is tiny and R is big.
/***
			if (L<SMALL_REAL || R<SMALL_REAL) quotient = 1;
			else quotient = R / L;
			if (quotient < SMALL_REAL) quotient = SMALL_REAL;
			Temp_Vector[ temp_index ] = L * CMath::exp( frac * log(quotient) );
****/
void CNode::Subdivide_Element(int Subdivision_count,
						const double Node_Vector[], double Temp_Vector[],
						int& node_index,  int& temp_index,
						CScaleType Interpolation)
{
	double frac,L,R;
	double base;
	base = 1.0 / (Subdivision_count + 1.0);

   	L = Node_Vector[node_index];
    R = Node_Vector[node_index+1];

	double logR_L, logL_R;
	if (Interpolation==LOG) {
		if (L>SMALL_REAL)	{ logR_L=log(R/L); }
		else if (R>SMALL_REAL) { logL_R=log(L/R); }
/***
		else for (int k=1; k<=Subdivision_count; k++) { 
			temp_index++;
			Temp_Vector[ temp_index ] = L + frac * (R - L);
		}
****/
	}

	for (int k=1; k<=Subdivision_count; k++)
	{
	    temp_index++;
		frac = k*base;		//    	frac = ((double)k) / (Subdivision_count + 1);
		if (Interpolation==LOG)
		{
		if (L>SMALL_REAL)	
//			Temp_Vector[ temp_index ] = L * CMath::exp( frac * CMath::log(R/L) );
			Temp_Vector[ temp_index ] = L * exp( frac * logR_L );
		else if (R>SMALL_REAL) 
			Temp_Vector[ temp_index ] = R * exp( (1 - frac) * logL_R );
		else Temp_Vector[ temp_index ] = L;  // both L and R are tiny

		}
		else Temp_Vector[ temp_index ] = L + frac * (R - L);
	}
	Transfer_node(Node_Vector, Temp_Vector, node_index, temp_index);
}

void CNode::Subdivide_Element(int Subdivision_count,
						const int Node_Vector[], int Temp_Vector[],
						int& node_index,  int& temp_index)
{
	for (int k=1; k<=Subdivision_count; k++)
	{
	    temp_index++;
		Temp_Vector[ temp_index ] = Node_Vector[ node_index ];
	}
	Transfer_node(Node_Vector, Temp_Vector, node_index, temp_index);
}

void CNode::Adjust_Nodes(double Node_Vector[], CScaleType Interpolation)
{
	double Temp_Vector[MAX_ELEMENTS+1];
	for (int k=0; k<=MAX_ELEMENTS; k++) Temp_Vector[k] = 0;
	int node_index=0, temp_index=0;
	do 
		if (m_RenodeList[ node_index ] < 0)
	        Concatenate_Elements(-m_RenodeList[ node_index ],
                              Node_Vector, Temp_Vector,
                              node_index,  temp_index);
    	else if (m_RenodeList[ node_index ] > 0)
        	Subdivide_Element(m_RenodeList[ node_index ],
                          Node_Vector, Temp_Vector,
                          node_index,  temp_index,
                          Interpolation);
    	else
        	Transfer_node(Node_Vector, Temp_Vector, node_index, temp_index);
	while (node_index < m_OldLastNode);
	for (k=1; k<=last_node; k++) Node_Vector[k] = Temp_Vector[k];
}

void CNode::Adjust_Nodes(int Node_Vector[])
{
	int Temp_Vector[MAX_ELEMENTS+1];
	for (int k=0; k<=MAX_ELEMENTS; k++) Temp_Vector[k] = 0;
	int node_index=0, temp_index=0;
	do 
		if (m_RenodeList[ node_index ] < 0)
	        Concatenate_Elements(-m_RenodeList[ node_index ],
                              Node_Vector, Temp_Vector,
                              node_index,  temp_index);
    	else if (m_RenodeList[ node_index ] > 0)
        	Subdivide_Element(m_RenodeList[ node_index ],
                          Node_Vector, Temp_Vector,
                          node_index,  temp_index);
    	else
        	Transfer_node(Node_Vector, Temp_Vector, node_index, temp_index);
	while (node_index < m_OldLastNode);
	for (k=1; k<=last_node; k++) Node_Vector[k] = Temp_Vector[k];
}

// Adjust_count(): implement the renoding rules for current_node, determining whether to
// concatenate or subdivide it.
// New rule: [9/7/96] if there is a change in phi n or phi p > 30Vt, it should be divided. This
// avoids numerical inaccuracies causing nonconvergence.
// 'whichattempt' is the number of times renode has been attempted. If >0, we don't have as many nodes
// as we'd like. This lets us prioritize the renoding.
int CNode::Adjust_count(int& current_node, int whichattempt)
{
	int		start_node, next_node, loop_count;
	double	Curv_Term, current_rho, next_rho, ND_Term,
			SteepPhinTerm, SteepPhipTerm,
			PsiL, PhinL, PhipL, PsiR, PhinR, PhipR,
			d_x, dx, max_dx, charge, Size_Term, Gen_Term_Front, Gen_Term_Rear, Big_Term;

	start_node  = current_node;
	d_x         = 0;
	charge      = 0;
	if (m_bUseEQ) {
		PsiL = Psi_eq[start_node]; PhinL = 0; PhipL = 0;
		current_rho = GetPeq_nir(start_node)-GetNeq_nir(start_node)+Nd[start_node]-Na[start_node];
	}else {
		PsiL = Psi[start_node]; PhinL = Phi_n[start_node]; PhipL = Phi_p[start_node];
		current_rho = GetP_nir(start_node)-GetN_nir(start_node)+Nd[start_node]-Na[start_node];
	}

	max_dx = x[last_node]/40;
	double ralpha=1/m_alpha;	// reciprocal of alpha
	double logNdStart=log(Nd[start_node]);
	double logNaStart=log(Na[start_node]);

	// The element will be concatenated unless one of the following tests are true
	// for the two elements combined.
	do
	{
		next_node = current_node + 1;
		dx = x[next_node]-x[current_node];
		d_x += dx;
		if (m_bUseEQ) {
			PsiR = Psi_eq[next_node]; PhinR = 0; PhipR = 0;
			next_rho = GetPeq_nir(next_node)-GetNeq_nir(next_node)+Nd[next_node]-Na[next_node];
		}else {
			PsiR = Psi[next_node]; PhinR = Phi_n[next_node]; PhipR = Phi_p[next_node];
			next_rho = GetP_nir(next_node) - GetN_nir(next_node)+ Nd[next_node] - Na[next_node];
		}
		charge += 0.5*(fabs(current_rho) + fabs(next_rho))*dx;

		// TEST #1: Average volume charge density causes electrostatic potential to 
		// deviate from linearity by an excessive amount
		Curv_Term = sqrt(charge*d_x/(2*eps[next_node]))*ralpha;

		// TEST #2: The majority-carrier doping concentration changes too rapidly
		if (Nd[current_node]*Nd[start_node]>Na[current_node]*Na[start_node])
			ND_Term = fabs(log(Nd[current_node])-logNdStart)*ralpha;
		else
			ND_Term = fabs(log(Na[current_node])-logNaStart)*ralpha;
		
		// TEST #3: Too large for its proximity to surface (in case rapidly absorbing light is subsequently applied)
		if (x[start_node]>0) Gen_Term_Front = d_x/x[start_node];
		else Gen_Term_Front = 1.0;	// first element and front optical elements retained
		if (x[current_node]<x[last_node]) Gen_Term_Rear = d_x/(x[last_node]-x[current_node]);
		else Gen_Term_Rear = 1.0;	// last element and rear optical elements retained

		// TEST #4: The element consumes too large a fraction of the width of the region
        Size_Term = ralpha*d_x/max_dx;

		// TEST #5: Spike in Phin or Phip
		if (!m_bUseEQ){
			SteepPhinTerm = fabs(PhinL-PhinR)/30;
			SteepPhipTerm = fabs(PhipL-PhipR)/30;
			if (whichattempt>5) {
					// Abandon hope beyond this point!
					// if it gets here, it probably won't converge. But we have to make sure we
				    // don't ever cause an infinite loop by wanting too many nodes.
					SteepPhinTerm =	SteepPhipTerm = 0.0;
			}			
		} else {
			SteepPhinTerm=SteepPhipTerm=0.0;
		}
		
		current_node = next_node;
		current_rho = next_rho;
	}
	while (	Curv_Term < 1.0 &&
			ND_Term < 1.0 &&
			Gen_Term_Front < 1.0 &&
			Gen_Term_Rear < 1.0 &&
			Size_Term < 1.0 &&
			SteepPhinTerm < 1.0 && SteepPhipTerm < 1.0 &&
			!InFixedNodeArray(x[current_node], m_pFixed_nodes) &&
			current_node < last_node );
	loop_count = current_node - start_node;
	if (loop_count==1)
	{
		Big_Term = Curv_Term;
		if (Size_Term>Big_Term) Big_Term = Size_Term;
		if (SteepPhinTerm>Big_Term) Big_Term = SteepPhinTerm;
		if (SteepPhipTerm>Big_Term) Big_Term = SteepPhipTerm;
		if (Big_Term > 0.2*MAX_ELEMENTS) Big_Term = 0.2*MAX_ELEMENTS;
		if (Big_Term >= 1.0) return (int)Big_Term; else return 0;
	}
	else
	{
		current_node--;
		return 2-loop_count;
	}
}

/*******
int CNode::Adjust_count(int& current_node, int whichattempt)
{
	int		start_node, next_node, loop_count;
	double	Curv_Term, current_rho, next_rho, ND_Term,
			SteepPhinTerm, SteepPhipTerm,
			PsiL, PhinL, PhipL, PsiR, PhinR, PhipR,
			d_x, dx, max_dx, charge, Size_Term, Gen_Term_Front, Gen_Term_Rear, Big_Term;

	start_node  = current_node;
	d_x         = 0;
	charge      = 0;
	if (m_bUseEQ) 
		{PsiL = Psi_eq[start_node]; PhinL = 0; PhipL = 0;}
	else
		{PsiL = Psi[start_node]; PhinL = Phi_n[start_node]; PhipL = Phi_p[start_node];}
	current_rho = CMath::exp(-PsiL + Vp[start_node] + PhipL)
				- CMath::exp(PsiL + Vn[start_node] - PhinL)
				+ Nd[start_node] - Na[start_node];
	max_dx = x[last_node]/40;
	do
	{
		next_node = current_node + 1;
		dx = x[next_node]-x[current_node];
		d_x += dx;
		if (m_bUseEQ)
			{PsiR = Psi_eq[next_node]; PhinR = 0; PhipR = 0;}
		else
			{PsiR = Psi[next_node]; PhinR = Phi_n[next_node]; PhipR = Phi_p[next_node];}
		next_rho = CMath::exp(-PsiR+Vp[next_node]+PhipR)
				- CMath::exp(PsiR+Vn[next_node]-PhinR)
				+ Nd[next_node] - Na[next_node];
		charge += 0.5*(fabs(current_rho) + fabs(next_rho))*dx;
		Curv_Term = sqrt(charge*d_x/(2*eps[next_node]))/m_alpha;
		if (Nd[current_node]*Nd[start_node]>Na[current_node]*Na[start_node])
			ND_Term = fabs(log(Nd[current_node]/Nd[start_node]))/m_alpha;
		else
			ND_Term = fabs(log(Na[current_node]/Na[start_node]))/m_alpha;
//        Size_Term = d_x/max_ddx/m_alpha;
        Size_Term = d_x/max_dx;
		if (x[start_node]>0) Gen_Term_Front = d_x/x[start_node];
		else Gen_Term_Front = 1.0;	// first element and front optical elements retained
		if (x[current_node]<x[last_node]) Gen_Term_Rear = d_x/(x[last_node]-x[current_node]);
		else Gen_Term_Rear = 1.0;	// last element and rear optical elements retained

		if (!m_bUseEQ){
			SteepPhinTerm = fabs(PhinL-PhinR)/30;
			SteepPhipTerm = fabs(PhipL-PhipR)/30;
			if (whichattempt>10 && m_alpha>5) {
					// Abandon hope beyond this point!
					// if it gets here, it probably won't converge. But we have to make sure we
				    // don't ever cause an infinite loop by wanting too many nodes - put in alpha term.
					SteepPhinTerm = fabs(PhinL-PhinR)/30/m_alpha;
					SteepPhipTerm = fabs(PhipL-PhipR)/30/m_alpha;
			}			
		} else {
			SteepPhinTerm=SteepPhipTerm=0.0;
		}
		
		current_node = next_node;
		current_rho = next_rho;
	}
	while (	Curv_Term < 1.0 &&
			ND_Term < 1.0 &&
			Gen_Term_Front < 1.0 &&
			Gen_Term_Rear < 1.0 &&
			Size_Term < 1.0 &&
			SteepPhinTerm < 1.0 && SteepPhipTerm < 1.0 &&
			!InFixedNodeArray(x[current_node], m_pFixed_nodes) &&
			current_node < last_node );
	loop_count = current_node - start_node;
	if (loop_count==1)
	{
		Big_Term = Curv_Term;
		if (Size_Term>Big_Term) Big_Term = Size_Term;
		if (SteepPhinTerm>Big_Term) Big_Term = SteepPhinTerm;
		if (SteepPhipTerm>Big_Term) Big_Term = SteepPhipTerm;
		if (Big_Term > 0.2*MAX_ELEMENTS) Big_Term = 0.2*MAX_ELEMENTS;
		if (Big_Term >= 1.0) return (int)Big_Term; else return 0;
	}
	else
	{
		current_node--;
		return 2-loop_count;
	}
}
*******/

void CNode::Renode(BOOL use_eq, double alpha, const double fixed_nodes[])
{
	m_pFixed_nodes = fixed_nodes;	// Load into private member variables
	m_bUseEQ = use_eq;				// Likewise
	m_alpha = alpha;				// Likewise
	Setup_Renode_List();			// modifies m_alpha
    Adjust_Nodes (x, LINEAR);
	Adjust_Nodes (Reg);
    Adjust_Nodes (Area, LINEAR);
    Adjust_Nodes (Vn, LINEAR);
    Adjust_Nodes (Vp, LINEAR);
    Adjust_Nodes (Nd, LOG);
    Adjust_Nodes (Na, LOG);
    Adjust_Nodes (Un, LINEAR);
    Adjust_Nodes (Up, LINEAR);
	Adjust_Nodes (gamma_n, LOG);
	Adjust_Nodes (gamma_p, LOG);
	Adjust_Nodes (taun, LOG);
	Adjust_Nodes (taup, LOG);
    Adjust_Nodes (G, LINEAR);
    Adjust_Nodes (eps, LINEAR);
    Adjust_Nodes (Esat_n, LINEAR);
    Adjust_Nodes (Esat_p, LINEAR);
    Adjust_Nodes (Psi_eq, LINEAR);
    Adjust_Nodes (Phi_n, LINEAR);
    Adjust_Nodes (Psi, LINEAR);
    Adjust_Nodes (Phi_p, LINEAR);
    Adjust_Nodes (I_n, LINEAR);
    Adjust_Nodes (I_p, LINEAR);

	Adjust_Nodes (n_Old, LOG);
	Adjust_Nodes (p_Old, LOG);
	PrecalculateNie_Neq_Peq();
	PrecalculateReciprocal_dx();
}

BOOL CNode::InFixedNodeArray(double x, const double a[])
{
	int array_size = FIXED_NODE_ARRAY_SIZE;		// declared in PC1D header
	for (int k=0; k<array_size; k++)
		if (x==a[k]) return TRUE;
	return FALSE;
}

void CNode::CopyPsi_eqIntoPsi() // Set Psi[k]=Psi_eq
{
	for (int k=0; k<GetNodeCount(); k++) {
		Psi[k]=Psi_eq[k];
	}
}

// save n, p into nOld, pOld
void CNode::SaveNintoNOld()
{
	int k;
	for (k=0; k<=last_node; k++)
    {
		n_Old[k] = GetN_nir(k);	// exp( Psi[k]+Vn[k]-Phi_n[k]);
		p_Old[k] = GetP_nir(k);	// exp(-Psi[k]+Vp[k]+Phi_p[k]);
    }
}

// Used to speed up the GetNie(), GetNeq(), GetPeq() functions,
// which require calculation of an exponential which doesn't change after equilibrium!
void CNode::PrecalculateNie_Neq_Peq()
{
	int k;
	for (k=0; k<=last_node; k++) {
		nieFAST[k] = exp( (Vn[k]+Vp[k]) * 0.5  );
		neqFAST[k] = exp(Psi_eq[k] + Vn[k]);
		peqFAST[k] = exp(-Psi_eq[k] + Vp[k]);
	}
}

void CNode::PrecalculateReciprocal_dx()
{
	int k;
	for (k=0; k<last_node; k++) {
		m_reciprocal_dx[k]= 1/(x[k+1] - x[k]);
	}
}


// These functions added by DAC 29/1/97, lifted from PlotLoad.cpp
// They are commonly used throughout the code and should only occur in one place.

/// calculate E using a weighted average of the E field at either side of
/// the element.
/// This streamlined version was made 26/11/02
double CNode::GetE_Vt(int k) const
{
	int leftk, rightk;
	if (k==0) { 				leftk=k+1; rightk=k+2; 
	} else if (k<last_node) {	leftk=k-1; rightk=k+1; 
	} else {					leftk=k-2; rightk=k-1;
	}
	double dxR = x[rightk]-x[k];
	double dxL = x[k] - x[leftk];
	// A huge speedup is possible in the very common case where both sides are equal:
	if (dxR==dxL) {
			return -( Psi[rightk]-Psi[leftk] ) / (x[rightk] - x[leftk]);
	}
	return -(	(Psi[rightk]-Psi[k])*dxL / dxR +
				(Psi[k]-Psi[leftk])*dxR / dxL
			) / (x[rightk] - x[leftk]);
}

#if 0
/***
double CNode::GetE_Vt(int k) const
{
	double leftPsi, centrePsi, rightPsi, leftX, centreX, rightX;
	if (k==0) {
		leftPsi=Psi[k+1]; centrePsi=Psi[k]; rightPsi=Psi[k+2];
		leftX=x[k+1]; centreX=x[k]; rightX=x[k+2]; }
	else if (k<last_node) {
		leftPsi=Psi[k-1]; centrePsi=Psi[k]; rightPsi=Psi[k+1];
		leftX=x[k-1]; centreX=x[k]; rightX=x[k+1]; }
	else {
		leftPsi=Psi[k-2]; centrePsi=Psi[k]; rightPsi=Psi[k-1];
		leftX=x[k-2]; centreX=x[k]; rightX=x[k-1]; }	
	return -(	(rightPsi-centrePsi)*(centreX-leftX) / (rightX-centreX) +
				(centrePsi-leftPsi)*(rightX-centreX) / (centreX-leftX)
			) / (rightX-leftX);
}
***/
#endif

// Excess pn Product Ratio  "pn'/eq"
double CNode::GetPNratio(int k) const
{
	return exp(Phi_p[k]-Phi_n[k]) - 1;
}


// must multiply by nir/Area
double CNode::GetGenerationRate(int k) const
{
	if (k==0) return (G[1]-G[0]) / (x[1]-x[0]);
	else if (k<last_node) 
		return (  (G[k+1]-G[k]) *(x[k]-x[k-1]) / (x[k+1]-x[k]) 
				+ (G[k]-G[k-1]) *(x[k+1]-x[k]) / (x[k]-x[k-1])
			   )/(x[k+1]-x[k-1]);
	else return (G[k]-G[k-1]) / (x[k]-x[k-1]);
}

// questionable - used only in plotload
double CNode::GetMun_times_Neq(int k) const
{	return exp(Psi_eq[k]+Un[k]);
}

double CNode::GetMup_times_Peq(int k) const
{	return exp(-Psi_eq[k]+Up[k]);
}

// functions not in main loop

double CNode::GetMun_times_Vt(int k) const
{	return exp(Un[k]-Vn[k])/Area[k];
}

double CNode::GetMup_times_Vt(int k) const
{	return exp(Up[k]-Vp[k])/Area[k];
}

double CNode::GetMup(int k) const 
{ return exp(Up[k]-Vp[k]); 
}; // must divide by Vt*Area

double CNode::GetMun(int k) const 
{ 	return exp(Un[k]-Vn[k]); 
}; // must divide by Vt*Area

// must divide by Vt*Area
double CNode::GetMup_includinghighfieldeffect(int k) const
{
	double tmp1;
	double mup = GetMup(k);	// low-field mobility
	if (GetEsat_p(k)>0) {	// correct for high-field effect
		tmp1 = GetI_p(k) / ( GetMup_times_P(k) * GetEsat_p(k) );
		mup /= sqrt(1 + tmp1*tmp1);
	}
	return mup;
}

// must divide by Vt*Area
double CNode::GetMun_includinghighfieldeffect(int k) const
{
	double tmp1;
	double mun = GetMun(k);	// low-field mobility
	if (GetEsat_n(k)>0) {	// correct for high-field effect
		tmp1 = GetI_n(k) / ( GetMun_times_N(k) * GetEsat_n(k) );
		mun /= sqrt(1 + tmp1*tmp1);
	}
	return mun;
}
