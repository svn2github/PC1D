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
#include "ProgramLimits.h" // For MAX_LAYERS
#include "math.h"
#include "mathstat.h"	// Complex, CMath
#include "physics.h"	// CPhysics

//////////////////////////////////////////////////////////////////////////
// CPhysics

double CPhysics::Mobility(double N, double mu_max, double mu_min, double Nref, 
			double alpha, double B1, double B2, double B3, double B4, double T)
{
	double Tn = T/300.0;
	return
		mu_min*pow(Tn,B1) 
		+ (mu_max-mu_min)*pow(Tn,B2) /
		  (1 + pow(N/(Nref*pow(Tn,B3)),alpha*pow(Tn,B4)));
}
			  
double CPhysics::Resistivity(double n, double e_mobility, double p, double h_mobility)
{
	return 1/(Q*n*e_mobility + Q*p*h_mobility);
}

double CPhysics::BGN(double N, double Nref, double Slope)
{
	if (Nref>0 && N>Nref) return Slope * log(N/Nref);
	else return 0;
}

double CPhysics::Nc(double ni, double NcNv, double Eg, double T)
{
	double Vt = VT300 * T/300;
	return ni*sqrt(NcNv)*CMath::exp(Eg/(2*Vt));
}

double CPhysics::Nv(double ni, double NcNv, double Eg, double T)
{
	double Vt = VT300 * T/300;
	return ni/sqrt(NcNv)*CMath::exp(Eg/(2*Vt));
}

double CPhysics::Neutral_Eq_Psi(double ni, double D, double BGNc, double BGNv, double T)
{	// D = Nd - Na
    double neq;
    double Vt = VT300 * T/300;
    double nie2 = ni*ni*CMath::exp((BGNc+BGNv)/Vt);
	if (D>0) neq = (D + sqrt(D*D + 4*nie2))/2;
	else neq = nie2 / ((-D + sqrt(D*D + 4*nie2))/2);
//	if (D>0) neq = (D + sqrt(pow(D,2) + 4*nie2))/2;
//	else neq = nie2 / ((-D + sqrt(pow(D,2) + 4*nie2))/2);
	return Vt*log(neq/ni) - BGNc;
}

double CPhysics::ni(double ni200, double ni300, double ni400, double T)
{
	double x[3] = {1.0/200, 1.0/300, 1.0/400};
	double y[3] = {log(ni200), log(ni300), log(ni400)};
	double c[3];
	double y0 = CMath::ParabolicInterp(1/T, x, y, c);
	return CMath::exp(y0);
}

double CPhysics::neq(double ni, double Psi, double BGNc, double T)
{
	double Vt = VT300*T/300;
	return ni * CMath::exp((Psi+BGNc)/Vt);
}

double CPhysics::peq(double ni, double Psi, double BGNv, double T)
{
	double Vt = VT300*T/300;
	return ni * CMath::exp((-Psi+BGNv)/Vt);
}

double CPhysics::Bulk_Recomb(double n, double p, double nie,
			double taun, double taup, double Et_Vt, 
			double Nd, double Na, double Cn, double Cp, double Cnp, double B)
{
	double n1 = nie * CMath::exp(Et_Vt);
	double p1 = nie * CMath::exp(-Et_Vt);
	double Cnp2 = Cnp/2;
	return (n*p-nie*nie) / ((p+p1)*taun + (n+n1)*taup)
			+ ( (Cn*Nd + Cnp2*p)*n/(p+Nd) + (Cp*Na + Cnp2*n)*p/(n+Na) + B )*(p*n-nie*nie);
}

double CPhysics::Bulk_Recomb_Hurkx(double n, double p, double nie,
			double taun, double taup, double Et_Vt, 
			double Nd, double Na, double Cn, double Cp, double Cnp, double B,
			double localEfield_Vt, double prefactor, double Fgamma_Vt)
{
	double n1 = nie * CMath::exp(Et_Vt);
	double p1 = nie * CMath::exp(-Et_Vt);
	double Cnp2 = Cnp/2;
	
	double gamma;
	gamma=prefactor*CMath::exp( localEfield_Vt*localEfield_Vt/(Fgamma_Vt*Fgamma_Vt) )*fabs(localEfield_Vt)/Fgamma_Vt;
	
	return (n*p-nie*nie) / ((p+p1)*taun/(1+gamma) + (n+n1)*taup/(1+gamma))
			+ ( (Cn*Nd + Cnp2*p)*n/(p+Nd) + (Cp*Na + Cnp2*n)*p/(n+Na) + B )*(p*n-nie*nie);			
}

// ----------------------------------------
//			Bulk_Recomb_3()
//-----------------------------------------
// The derivatives of surface + bulk recombination are calculated numerically 
// based on a 1% change in the excess carrier densities.

// The Bulk_Recomb_3() function calculates bulk recombination and the derivatives of 
// the bulk recombination with respect to phi_n and phi_p.

// This works out the 3 bulk recombination values required for each element in LoadLocalValues().
// It turns out this calculation was one of the worst bottlenecks in the code.
// By taking advantage of the similarities in the 3 calculations, the whole program
// runs 6% faster!

const double EXP01 = exp(+1E-2);
const double EXP_01 = exp(-1E-2);

// in this function, the exponential of Et_Vt takes much of the time.
// BUT it is constant across a region, so cache it. For Hurkx, cache 1/Fgamma also
double lastEt_Vt=0;
double expEtVt=1;
double rexpEtVt=1;
double lastFgamma=1;
double rFGamma=1;


void CPhysics::Bulk_Recomb_3(double &RecR, double &dRdPhinR, double &dRdPhipR,
			double n, double p, double nie,	double taun, double taup, double Et_Vt, 
			double Nd, double Na, double Cn, double Cp, double Cnp, double B)
{
	if (Et_Vt!=lastEt_Vt) {
		lastEt_Vt=Et_Vt;
		expEtVt=exp(Et_Vt);	// cache these values
		rexpEtVt=1/expEtVt;
	}
	double n1 = nie*expEtVt;
	double p1 = nie*rexpEtVt;  // p1=nie/expEtVt;
	double n01 = n*EXP_01;
	double p01 = p*EXP01;

	double Cnp2 = Cnp/2;
	double CnNd   = (Cn*Nd+Cnp2*p)*n/(p+Nd);
	double CpNa   = (Cp*Na+Cnp2*n)*p/(n+Na);
	double Cn01Nd = CnNd*EXP_01;	// n replaced by n01
	double Cp01Na = CpNa*EXP01;	    // p replaced by p01
	double np_nie2   =n*p-nie*nie;
	double n01p_nie2 =n01*p-nie*nie;
	double np01_nie2 =n*p01-nie*nie;

	RecR     = np_nie2  / ((p+p1)*taun + (n+n1)*taup) 
		     + np_nie2  * (CnNd + CpNa + B);
	dRdPhinR = n01p_nie2 / ((p+p1)*taun + (n01+n1)*taup)
			 + n01p_nie2 * (Cn01Nd + CpNa + B)	// ignore effect on CpNa
		     - RecR;
	dRdPhipR = np01_nie2 / ((p01+p1)*taun + (n+n1)*taup)
		     + np01_nie2 * (CnNd + Cp01Na + B)	// ignore effect on CnNd
		     - RecR;
	dRdPhinR*=1E2;		dRdPhipR*=1E2;
}

// Exactly as Bulk_Recomb_3, 
// but includes the Hurkx model for field-enhanced recombination.
// Thus, it depends on the local electric field.


void CPhysics::Bulk_Recomb_Hurkx_3(double &RecR, double &dRdPhinR, double &dRdPhipR,
			double n, double p, double nie,	double taun, double taup, double Et_Vt, 
			double Nd, double Na, double Cn, double Cp, double Cnp, double B,
			double localEfield_Vt, double prefactor, double Fgamma_Vt)
{
	if (Et_Vt!=lastEt_Vt) {
		lastEt_Vt=Et_Vt;
		expEtVt=exp(Et_Vt);	// cache these values
		rexpEtVt=1/expEtVt;
	}
	if (Fgamma_Vt!=lastFgamma) {
		lastFgamma=Fgamma_Vt;
		rFGamma=1/Fgamma_Vt;
	}
	double n1 = nie*expEtVt;
	double p1 = nie*rexpEtVt;  // p1=nie/expEtVt;
	double n01 = n*EXP_01;
	double p01 = p*EXP01;

	double Cnp2 = Cnp/2;
	double CnNd   = (Cn*Nd+Cnp2*p)*n/(p+Nd);
	double CpNa   = (Cp*Na+Cnp2*n)*p/(n+Na);
//	double Cn01Nd = (Cn*Nd+Cnp2*p)*n01/(p+Nd);	1
//	double Cp01Na = (Cp*Na+Cnp2*n)*p01/(n+Na);
	double Cn01Nd = CnNd*EXP_01;	// n replaced by n01
	double Cp01Na = CpNa*EXP01;	    // p replaced by p01
	double tmp1=n*p-nie*nie;
	double tmp2=n01*p-nie*nie;
	double tmp3=n*p01-nie*nie;
	
	double gamma=prefactor*exp( pow(localEfield_Vt*rFGamma, 2) )*fabs(localEfield_Vt)*rFGamma;

	RecR     = tmp1 * (1+gamma) / ((p+p1)*taun + (n+n1)*taup) 
		     + tmp1 * (CnNd + CpNa + B);
	dRdPhinR = tmp2 * (1+gamma) / ((p+p1)*taun + (n01+n1)*taup)
			 + tmp2 * (Cn01Nd + CpNa + B)	// ignore effect on CpNa
		     - RecR;
	dRdPhipR = tmp3 * (1+gamma) / ((p01+p1)*taun + (n+n1)*taup)
		     + tmp3 * (CnNd + Cp01Na + B)	// ignore effect on CnNd
		     - RecR;
	dRdPhinR*=1E2;		dRdPhipR*=1E2;
}

double CPhysics::tau(double tau0, double N, double Nref, double Nalpha,
				double T, double Talpha)
{
	double Tref = 300;
	if (N>Nref) 
		return tau0 * pow(T/Tref,Talpha) * pow(N/Nref,Nalpha);
	else    	
		return tau0 * pow(T/Tref,Talpha);
}

double CPhysics::Surf_Recomb(double n, double p, double nie,
				double Sn, double Sp, double Et_Vt, BOOL JoModel)
{
	if (Sn==0 && Sp==0) return 0;
	double n1 = nie * CMath::exp(Et_Vt);
	double p1 = nie * CMath::exp(-Et_Vt);
	double neq, peq;
	if (JoModel)
	{
		if (n>p) 
		{
//			neq = 0.5*((n-p) + sqrt(pow(n-p,2)+4*pow(nie,2)));
//			peq = pow(nie,2) / neq;
			neq = 0.5*((n-p) + sqrt( (n-p)*(n-p)+4*nie*nie ));
			peq = nie*nie / neq;
		}
		else
		{
//			peq = 0.5*((p-n) + sqrt(pow(p-n,2)+4*pow(nie,2)));
//			neq = pow(nie,2) / peq;
			peq = 0.5*((p-n) + sqrt( (p-n)*(p-n)+4*nie*nie ));
			neq = nie*nie / peq;
		}
		return Sn*Sp*(n*p-nie*nie) / ((peq+p1)*Sp + (neq+n1)*Sn);
	}
	else return Sn*Sp*(n*p-nie*nie) / ((p+p1)*Sp + (n+n1)*Sn);
}

// ----------------------------------------
//			Surf_Recomb_3()
//-----------------------------------------
// The derivatives of surface + bulk recombination are calculated numerically 
// based on a 1% change in the excess carrier densities.

void CPhysics::Surf_Recomb_3(double &Recomb, double &dRs_dPhin, double &dRs_dPhip, double AreaC,
				double n, double p, double nie,
				double Sn, double Sp, double Et_Vt, BOOL JoModel)
{
	Recomb =		 AreaC * Surf_Recomb(n,        p, nie, Sn, Sp, Et_Vt, JoModel);

	dRs_dPhin = 1E2*(AreaC * Surf_Recomb(n*EXP_01, p, nie, Sn, Sp, Et_Vt, JoModel) 
				- Recomb);
	dRs_dPhip = 1E2*(AreaC * Surf_Recomb(n, EXP01*p, nie, Sn, Sp, Et_Vt, JoModel) 
				- Recomb);
}



double CPhysics::S(double S0, double N, double Nref, double Nalpha,
				double T, double Talpha)
{
	double Tref = 300;
	if (N>Nref)
		return S0 * pow(T/Tref,Talpha) * pow(N/Nref,Nalpha);
	else
		return S0 * pow(T/Tref,Talpha);
}

double CPhysics::Doping(double x, CProfile Profile, double Npeak, double Depth, double Xpeak)
{
	switch (Profile)
	{
		case UNIFORM_PROFILE: 
			if (x<=Xpeak+Depth && x>=Xpeak) return Npeak;
			else return 0;
		case EXPONENTIAL_PROFILE:
			return Npeak*CMath::exp(-fabs(x-Xpeak)/Depth);
		case GAUSSIAN_PROFILE:
			return Npeak*CMath::exp(-pow(x-Xpeak,2)/(pow(Depth,2)));
		case ERFC_PROFILE:
			return Npeak*CMath::erfc((x-Xpeak)/Depth);
	}
	return 0;
}

double CPhysics::Absorption(double wavelength, double Ed1, double Ed2, double Ei1, double Ei2,
				double Ep1, double Ep2, double Ad1, double Ad2, 
				double A11, double A12, double A21, double A22, 
				double Tcoeff, double Toffset, double T)
{
	if (wavelength==0 || T==0) return 0;
	double A = 0;
	double Vt = VT300*T/300;
	double hw = HC/wavelength;
    double Delta_E  = Tcoeff * (T*T/(T + Toffset) - (300.0*300.0)/(300 + Toffset));
	Ed1 -= Delta_E; Ed2 -= Delta_E; Ei1 -= Delta_E; Ei2 -= Delta_E;
	if (Ei1>0)
		{
			if (hw>Ed1 && Ed1>Ei1) hw = Ed1;
			else if (hw>Ed2 && Ed2>Ei1) hw = Ed2;
			if (hw+Ep1 > Ei1) A += A11*(hw-Ei1+Ep1)*(hw-Ei1+Ep1)/(CMath::exp(Ep1/Vt)-1);
			if (hw-Ep1 > Ei1) A += A11*(hw-Ei1-Ep1)*(hw-Ei1-Ep1)/(1-CMath::exp(-Ep1/Vt));
			if (hw+Ep2 > Ei1) A += A12*(hw-Ei1+Ep2)*(hw-Ei1+Ep2)/(CMath::exp(Ep2/Vt)-1);
			if (hw-Ep2 > Ei1) A += A12*(hw-Ei1-Ep2)*(hw-Ei1-Ep2)/(1-CMath::exp(-Ep2/Vt));
		}
	hw = HC/wavelength;
	if (Ei2>0)
	{
		if (hw>Ed1 && Ed1>Ei2) hw = Ed1;
		else if (hw>Ed2 && Ed2>Ei2) hw = Ed2;
		if (hw+Ep1 > Ei2) A += A21*(hw-Ei2+Ep1)*(hw-Ei2+Ep1)/(CMath::exp(Ep1/Vt)-1);
		if (hw-Ep1 > Ei2) A += A21*(hw-Ei2-Ep1)*(hw-Ei2-Ep1)/(1-CMath::exp(-Ep1/Vt));
		if (hw+Ep2 > Ei2) A += A22*(hw-Ei2+Ep2)*(hw-Ei2+Ep2)/(CMath::exp(Ep2/Vt)-1);
		if (hw-Ep2 > Ei2) A += A22*(hw-Ei2-Ep2)*(hw-Ei2-Ep2)/(1-CMath::exp(-Ep2/Vt));
	}
	hw = HC/wavelength;
	if (Ed1>0 && hw>Ed1) A += Ad1*sqrt(hw-Ed1);
	if (Ed2>0 && hw>Ed2) A += Ad2*sqrt(hw-Ed2);
	
	return A;
}

/**
double CPhysics::Absorption(double wavelength, double Ed1, double Ed2, double Ei1, double Ei2,
				double Ep1, double Ep2, double Ad1, double Ad2, 
				double A11, double A12, double A21, double A22, 
				double Tcoeff, double Toffset, double T)
{
	if (wavelength==0 || T==0) return 0;
	double A = 0;
	double Vt = VT300*T/300;
	double hw = HC/wavelength;
    double Delta_E  = Tcoeff * (pow(T,2)/(T + Toffset) - pow(300,2)/(300 + Toffset));
	Ed1 -= Delta_E; Ed2 -= Delta_E; Ei1 -= Delta_E; Ei2 -= Delta_E;
	if (Ei1>0)
		{
			if (hw>Ed1 && Ed1>Ei1) hw = Ed1;
			else if (hw>Ed2 && Ed2>Ei1) hw = Ed2;
			if (hw+Ep1 > Ei1) A += A11*pow(hw-Ei1+Ep1,2)/(CMath::exp(Ep1/Vt)-1);
			if (hw-Ep1 > Ei1) A += A11*pow(hw-Ei1-Ep1,2)/(1-CMath::exp(-Ep1/Vt));
			if (hw+Ep2 > Ei1) A += A12*pow(hw-Ei1+Ep2,2)/(CMath::exp(Ep2/Vt)-1);
			if (hw-Ep2 > Ei1) A += A12*pow(hw-Ei1-Ep2,2)/(1-CMath::exp(-Ep2/Vt));
		}
	hw = HC/wavelength;
	if (Ei2>0)
	{
		if (hw>Ed1 && Ed1>Ei2) hw = Ed1;
		else if (hw>Ed2 && Ed2>Ei2) hw = Ed2;
		if (hw+Ep1 > Ei2) A += A21*pow(hw-Ei2+Ep1,2)/(CMath::exp(Ep1/Vt)-1);
		if (hw-Ep1 > Ei2) A += A21*pow(hw-Ei2-Ep1,2)/(1-CMath::exp(-Ep1/Vt));
		if (hw+Ep2 > Ei2) A += A22*pow(hw-Ei2+Ep2,2)/(CMath::exp(Ep2/Vt)-1);
		if (hw-Ep2 > Ei2) A += A22*pow(hw-Ei2-Ep2,2)/(1-CMath::exp(-Ep2/Vt));
	}
	hw = HC/wavelength;
	if (Ed1>0 && hw>Ed1) A += Ad1*sqrt(hw-Ed1);
	if (Ed2>0 && hw>Ed2) A += Ad2*sqrt(hw-Ed2);
	
	return A;
}
**/

Complex CPhysics::gamma_i_1(Complex n_i_1, Complex n_i)
{
	return (n_i_1 - n_i)/(n_i_1 + n_i);
}

Complex CPhysics::gamma_i_prime(Complex gamma_i_1, Complex gamma_i, double phi)
{
	Complex one(1.0, 0.0), result;
	if (phi<20*PI)
		result = (gamma_i_1 + (gamma_i * Complex::expj(phi))) /
                   (one + (gamma_i_1 * (gamma_i * Complex::expj(phi))));
	else
	{
		result = Complex(sqrt((Complex::mag2(gamma_i_1)+Complex::mag2(gamma_i)
                          -2*Complex::mag2(gamma_i_1)*Complex::mag2(gamma_i))
                         /(1 - Complex::mag2(gamma_i_1)*Complex::mag2(gamma_i))), 0);
	if (gamma_i_1.r<0) result.r = -result.r;
	}
	return result;
}

double CPhysics::ARCreflectance(double lambda, double index, double absorb,
								double d[], double n[])
{	// layer thickness d, index n starting with layer adjacent to substrate
	Complex n_i, n_i_1, gamma(0,0);
	double phi, d_i;
	// substrate
	n_i.r = index;
	n_i.i = lambda * 1e-7 / (4*PI) * absorb;
	phi = 0;
	for (int k=0; k<MAX_LAYERS; k++)	// previously required d[k]>0
	{
		// underlying interface
		n_i_1.r = n[k];
  		n_i_1.i = 0;
		if (k==0) gamma = gamma_i_1(n_i_1, n_i);
		else gamma = gamma_i_prime(gamma_i_1(n_i_1, n_i), gamma, phi);
		// layer
		n_i = n_i_1;
		d_i = d[k];
		phi = 4 * PI * d_i * n_i.r / lambda;
	}
	// air interface
	n_i_1.r = 1;
	n_i_1.i = 0;
	gamma = gamma_i_prime(gamma_i_1(n_i_1, n_i), gamma, phi);
	return Complex::mag2(gamma);
}
