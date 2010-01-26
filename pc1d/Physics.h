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

/////////////////////////////////////////////////////////////////////////////
// CPhysics

#include "PhysicalConstants.h"

class CPhysics
{
// Operations
public:
	CPhysics() { };
// Implementation
private:
	static Complex gamma_i_1(Complex n_i_1, Complex n_i);
	static Complex gamma_i_prime(Complex gamma_i_1, Complex gamma_i, double phi);
public:
	static double Mobility(double N,	// cm-3
				double mu_max, double mu_min,	// cm2/Vs 
				double Nref,	// cm-3
				double alpha, double B1, double B2, double B3, double B4, 
				double T);		// K
	static double Resistivity(double n,	// cm-3
				double e_mobility,	// cm2/Vs
				double p,		// cm-3
				double h_mobility);	// cm2/Vs
	static double BGN(double N, double Nref,	// cm-3
				double Slope);	// eV
	static double Nc(double ni,		// cm-3
				double NcNv,
				double Eg,		// eV
				double T);		// K
	static double Nv(double ni,		// cm-3
				double NcNv,
				double Eg,		// eV
				double T);		// K
    static double Neutral_Eq_Psi(double ni, double D,	// cm-3 (D=Nd-Na)
    			double BGNc, double BGNv,	// eV
    			double T);		// K
	static double ni(double ni200, double ni300, double ni400,	// cm-3
				double T);		// K
	static double neq(double ni, 		// cm-3
				double Psi, 	// V
				double BGNc, 	// eV
				double T);		// K
	static double peq(double ni, 		// cm-3
				double Psi, 	// V
				double BGNv, 	// eV
				double T);		// K
	static double Bulk_Recomb(double n, double p, double nie,	// cm-3
				double taun, double taup,	// s
				double Et_Vt,		// normalized by Vt (relative to Ei)
				double Nd, double Na,	// cm-3
				double Cn, double Cp, double Cnp,	// cm6/s
				double B);			// cm3/s
				
	static double Bulk_Recomb_Hurkx(double n, double p, double nie,
			double taun, double taup, double Et_Vt, 
			double Nd, double Na, double Cn, double Cp, double Cnp, double B,
			double localEfield_Vt, double prefactor, double F_gamma_Vt);
			
	static void Bulk_Recomb_Hurkx_3(double &RecR, double &dRdPhinR, double &dRdPhipR,
			double n, double p, double nie,	double taun, double taup, double Et_Vt, 
			double Nd, double Na, double Cn, double Cp, double Cnp, double B,
			double localEfield_Vt, double prefactor, double Fgamma_Vt);


	static void Bulk_Recomb_3(double &RecR, double &dRdPhinR, double &dRdPhipR,
			double n, double p, double nie,	double taun, double taup, double Et_Vt, 
			double Nd, double Na, double Cn, double Cp, double Cnp, double B);
			
	static double tau(double tau0,	// s (at T = 300 K, low doping, low field)
				double N, double Nref,	// cm-3
				double Nalpha,
				double T, 	// K
				double Talpha);
	static double Surf_Recomb(double n, double p, double nie,	// cm-3
				double Sn, double Sp,	// s
				double Et_Vt,		// normalized by Vt (relative to Ei)
				BOOL   JoModel);	// maintain Jo-behavior beyond LLI

	static void Surf_Recomb_3(double &Recomb, double &dRs_dPhin, double &dRs_dPhip,
				double AreaC, 
				double n, double p, double nie,
				double Sn, double Sp, double Et_Vt, BOOL JoModel);

	static double S(double S0,	// cm/s (at reference T, low doping)
				double N, double Nref,	// cm-3
				double Nalpha,
				double T, 	// K
				double Talpha);
	static double Doping(double x, 		// cm
				CProfile Profile, 
				double Npeak, 		// cm-3
				double Depth, 		// cm
				double Xpeak);		// cm
	static double Absorption(double wavelength, 	// nm (free space)
				double Ed1, double Ed2, double Ei1, double Ei2,	// eV
				double Ep1, double Ep2, 	// eV
				double Ad1, double Ad2, 	// cm-1
				double A11, double A12, double A21, double A22,	// cm-1
				double Tcoeff, 				// eV/K
				double Toffset, double T);	// K
	static double ARCreflectance(double lambda,	// nm
				double index,					// substrate index of refraction
				double absorb,					// substrate absorption coefficient (cm-1)
				double d[], 					// layer thickneses (nm), inside -> outside
				double n[]);					// layer index, inside -> outside
};
