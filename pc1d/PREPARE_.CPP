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
//#include "pc1d.h"
#include "problem.h"
#include "math.h"
#include "mathstat.h"	// CMath
#include "physics.h"	// CPhysics
#include "useful.h"

//////////////////////////////////////////////////////////////////////////////
// CProblem problem preparation functions

void CProblem::SetReferenceValues(int region_num)
{
	CMaterial* pM = pD->GetMaterialForRegion(region_num);
	T = pE->m_Temp;
	Vt = VT300*T/300;
	Aref = pD->m_Area;
	Eg = pM->m_BandGap;
	nir = CPhysics::ni(	pM->m_ni200, pM->m_ni300, pM->m_ni400, T);
	double Nc0 = CPhysics::Nc(nir, pM->m_NcNv, pM->m_BandGap, T);
	double Nv0 = CPhysics::Nv(nir, pM->m_NcNv, pM->m_BandGap, T);
	Vn0 = Vt*CMath::log(Nc0) + pM->m_Affinity;
	Vp0 = Vt*CMath::log(Nv0) - pM->m_BandGap - pM->m_Affinity;
}

void CProblem::LoadOpticParms(BOOL m_Back, double lambda,
				double& cosTheta1, double& cosTheta2, double& ThetaDepth, 
				double absorb[], double index[], double w[],
				double nfreecarrcoeff[], double pfreecarrcoeff[])
{
	CMaterial* pM;
	double f, n_r, n_i;
	int k;
//	int SaveRegion = pD->m_CurrentRegion;
	for (int reg=0; reg<pD->m_NumRegions; reg++)
	{
		if (m_Back) k=pD->m_NumRegions-reg-1; else k=reg;
//		pD->SelectRegion(reg); 
		pM=pD->GetMaterialForRegion(reg);
		w[k] = pD->GetRegion(reg)->m_Thickness;

		absorb[k] = pM->GetAbsorptionAtWavelength(lambda, T);
		index[k] = pM->GetRefractiveIndexAtWavelength(lambda);
/***
		if (pM->m_AbsExternal)
			absorb[k] = CMath::LogInterp(lambda, pM->m_nAbsorb, 
							pM->m_AbsLambda, pM->m_Absorption);
		else
		    absorb[k] =  
		    	CPhysics::Absorption(lambda, pM->m_AbsEd1, pM->m_AbsEd2, 
		    		pM->m_AbsEi1, pM->m_AbsEi2, pM->m_AbsEp1, pM->m_AbsEp2, 
		    		pM->m_AbsAd1, pM->m_AbsAd2, pM->m_AbsA11, pM->m_AbsA12,
		    		pM->m_AbsA21, pM->m_AbsA22, pM->m_AbsTcoeff, pM->m_AbsToffset, T);
		if (pM->m_IndexExternal)
    		index[k] = CMath::LinearInterp(lambda, pM->m_nIndex, 
    						pM->m_IdxLambda, pM->m_Index);
		else
			index[k] = pM->m_FixedIndex;
***/

		if (pM->m_FreeCarrEnable) {
			nfreecarrcoeff[k] =	pM->m_FreeCarrCoeffN*pow(lambda,pM->m_FreeCarrPowerN); 	
			pfreecarrcoeff[k] =	pM->m_FreeCarrCoeffP*pow(lambda,pM->m_FreeCarrPowerP); 	
		} else nfreecarrcoeff[k]=pfreecarrcoeff[k]=0;
	}
// 	pD->SelectRegion(SaveRegion);
 	double sinf;
	if (!m_Back && pD->m_FrontTexture)
	{	// front texture
    	f = pD->m_FrontAngle*PI/180;
		n_r = index[0];
		n_i = absorb[0] * lambda * 1e-7 / (4*PI);
		sinf=sin(f);
		cosTheta1=sqrt(1-sinf*sinf/(n_r*n_r + n_i*n_i));
		cosTheta2=cos(f)*cosTheta1 +sinf*sqrt(1-cosTheta1*cosTheta1);
	    ThetaDepth = pD->m_FrontDepth/6;
	}
	else if (m_Back && pD->m_RearTexture)
	{	// rear texture
    	f = pD->m_RearAngle*PI/180;
		n_r = index[0];		// note that 0 means first surface, not front surface
		n_i = absorb[0] * lambda * 1e-7 / (4*PI);
		sinf=sin(f);
		cosTheta1=sqrt(1-sinf*sinf/(n_r*n_r + n_i*n_i));
		cosTheta2=cos(f)*cosTheta1 +sinf*sqrt(1-cosTheta1*cosTheta1);
	    ThetaDepth = pD->m_RearDepth/6;
	}
	else {cosTheta1 = 1; cosTheta2 = 1; ThetaDepth = 0;}
}

double CProblem::Reflectance(double lambda, CLight* pL)
{	// This function gives the fraction of the incident reflected from the surface
	double absorb[MAX_REGIONS], index[MAX_REGIONS], w[MAX_REGIONS];
	double cosTheta1, cosTheta2, ThetaDepth;
	double nfreecarrier[MAX_REGIONS], pfreecarrier[MAX_REGIONS];
	CReflectance* pRfl;
	if (pL->m_Back) pRfl = &pD->m_RearRfl; else pRfl = &pD->m_FrontRfl;
	if (pRfl->m_bExternal)
		return CMath::LinearInterp(lambda, pRfl->m_nLambda,
						pRfl->m_Lambda, pRfl->m_Reflectance);
	else if (pRfl->m_bCoated)
		{
			LoadOpticParms(pL->m_Back, lambda, cosTheta1, cosTheta2, ThetaDepth, 
							absorb, index, w, nfreecarrier, pfreecarrier);
			return CPhysics::ARCreflectance(lambda, index[0], absorb[0], 
							pRfl->m_Thick, pRfl->m_Index) + pRfl->m_Broadband;
		}
	else return pRfl->m_Fixed;
}


//#pragma optimize("t", on) // these next two functions are time-critical
#pragma intrinsic(exp, sqrt)

// Speedup for free-carrier absorption (FCA).
// effectiven, effectivep are independent of wavelength & only need to
// be calculated once per spectrum - could be stored. 
// This makes free carrier calc almost as fast as no F.C.A.
// Note: If you wanted to make FCA not linear with n,p, you could do it here.
// e.g. suppose 				   2.5 		 3
//				alpha_fc = 5e-10. n    lambda
// you would put effectiven = pow(n, 2.5)

void CProblem::CalculateCarrierConcentrationsForFCA(double *effectiven, double *effectivep)
{
	BOOL bUseFreeCarrierAbs=FALSE;
//	int SaveRegion = pD->m_CurrentRegion;
	for (int xregion=0; xregion<pD->m_NumRegions; xregion++){		
//		pD->SelectRegion(xregion); 
//		if (pD->pR->GetMaterial()->m_FreeCarrEnable) bUseFreeCarrierAbs=TRUE;
		if (pD->GetMaterialForRegion(xregion)->m_FreeCarrEnable) bUseFreeCarrierAbs=TRUE;
	}
//	pD->SelectRegion(SaveRegion); 
	if (!bUseFreeCarrierAbs) return; // speedup - don't bother if no FCA.

	CNode* pN = pS->GetNodeData();
	double logn, logp, logprevn, logprevp;
	double n, p, prevn, prevp;
	
	logprevn =  pN->GetPsi(0) + pN->GetVn(0) - pN->GetPhi_n(0);
	logprevp = -pN->GetPsi(0) + pN->GetVp(0) + pN->GetPhi_p(0);
	prevn=nir*exp(logprevn);
	prevp=nir*exp(logprevp);

	int k;
	for (k=0; k<=pN->GetLastNode(); k++) {
		logn =  pN->GetPsi(k) + pN->GetVn(k) - pN->GetPhi_n(k);
		logp = -pN->GetPsi(k) + pN->GetVp(k) + pN->GetPhi_p(k);
		
		n=nir*exp(logn);
		p=nir*exp(logp);

		if (fabs(logn-logprevn)>SMALL_REAL) 
			 effectiven[k]=(n-prevn)/(logn-logprevn); // use exponential assumption if possible
		else effectiven[k]=(n+prevn)/2; 		  // otherwise, use linear approximation
		if (fabs(logp-logprevp)>SMALL_REAL)		// ditto for p
			 effectivep[k]=(p-prevp)/(logp-logprevp); 
		else effectivep[k]=(p+prevp)/2;
		
		logprevn=logn;	logprevp=logp;
		prevn=n;    	prevp=p;
		// effectiven[k] = pow(effectiven[k], m_FCA_nPower);
		// effectivep[k] = pow(effectivep[k], m_FCA_pPower);
	  }
}


// Gives cumulative generation between INCIDENT SURFACE and x, where x is 
// distance from INCIDENT SURFACE (not from front), for 1 W/cm2, in units cm-2/s.
// Assumes that cosTheta1 is valid for 0<x<ThetaDepth, and cosTheta2 otherwise.
// Uses cosTheta2 for reflections from posterior surface if it is planar.
// Internal index of refraction mismatch is ignored!
// The methodology is: Total generation=
//	 	Generation from front to x (first pass)
// 		-Generation from back to x (return pass)
//		+Generation from back to front (return pass)
//		+ (aw_x/aw_tot)*remaining generation (subsequent passes)

// 		...Generation With Parasitics...
// Gen[k]=generation from x=node.x[k-1] to x=node.x[k]
// Includes parasitic absorption such as free carrier absorption
// puts generation into Gen[].
// It's complicated a little by the need to be able to do rear light sources. Hence the firstk,
// lastk, k_inc variables.
// Returns Escape:The fraction of the energy that initially gets into the device that 
// subsequently escapes out the incident surface.
// POSSIBLE SPEEDUP: if !posteriorTex, exp(-alphadx/cosTheta) is same for both passes. It could be stored.
// this would only require one exp() per element per lambda! (ver 4.1 needed 11 & had no F.C.A.!)
void CProblem::GenerationWithParasitics(double lambda, CLight *pL, double Gen[], double & escape, double *effectiven, double *effectivep)
{
	double absorb[MAX_REGIONS], index[MAX_REGIONS], w[MAX_REGIONS];
	double nfreecarrier[MAX_REGIONS], pfreecarrier[MAX_REGIONS];
	double cosTheta1, cosTheta2, ThetaDepth;
	double incident1, incident2, posterior1, posterior2;
	CTexture incidentTex, posteriorTex;
	
	int firstk, lastk, k_inc;
	
	LoadOpticParms(pL->m_Back, lambda, cosTheta1, cosTheta2, ThetaDepth, 
						absorb, index, w, nfreecarrier, pfreecarrier);
	CNode* pN = pS->GetNodeData();

	if (pL->m_Back) 
	{
		incident1 = pD->m_RearRfl.m_Internal1;
		incident2 = pD->m_RearRfl.m_Internal2;
		posterior1 = pD->m_FrontRfl.m_Internal1;
		posterior2 = pD->m_FrontRfl.m_Internal2;
		incidentTex = pD->m_RearRfl.m_Rough;
		posteriorTex = pD->m_FrontRfl.m_Rough;
		
		firstk=pN->GetLastNode();
		lastk=0;
		k_inc=-1;
	}
	else
	{
		incident1 = pD->m_FrontRfl.m_Internal1;
		incident2 = pD->m_FrontRfl.m_Internal2;
		posterior1 = pD->m_RearRfl.m_Internal1;
		posterior2 = pD->m_RearRfl.m_Internal2;
		incidentTex = pD->m_FrontRfl.m_Rough;
		posteriorTex = pD->m_RearRfl.m_Rough;
		
		firstk=0;
		lastk=pN->GetLastNode();
		k_inc=+1;
	}	
	
	int k;
	int xregion;
	double dx;
	double xregionstart;
	double remaining;

	BOOL bUseFreeCarrierAbs=FALSE;

	// check for special case for speedup
	// (a) no free carrier absorption
	for (xregion=0; xregion<pD->m_NumRegions; xregion++){		
		if (pfreecarrier[xregion]!=0 || nfreecarrier[xregion]!=0) {
			// must be non-zero FCA, also, if less than 1e-5 of direct absorption, it's completely negligible.
			// Just take n=p=1e20 as an upper limit for carrier concentrations to work this out.
			if (pfreecarrier[xregion]*1e20+ nfreecarrier[xregion]*1e20>1e-5*absorb[xregion])
				bUseFreeCarrierAbs=TRUE;
		}
	}

	// For each element across the device, 
	// * a certain fraction of light will be absorbed, generating carriers;
	// * a certain fraction will be parasitically absorbed (lost)
	// * the remainder will be transmitted


	double * alphadx = new double [pN->GetNodeCount()];		// alpha * dx for element k. ( alpha includes all absorption)
	double aw_tot; 	// total absorption (of all kinds) across entire device = sum of alphadx[]	

	double * fractAbsorb;
	if (bUseFreeCarrierAbs)	fractAbsorb = new double [pN->GetNodeCount()]; // fraction of absorbed photons which generate carriers

	
	double totalwid=0;
	for (xregion=0; xregion<pD->m_NumRegions; xregion++) totalwid+=w[xregion];
	
	// STEP 1: Calculate the fractions absorbed, lost and transmitted

	double ncoeff, pcoeff;
	double alpha_fc;	

//	alphadx[firstk]=0;
//	if (bUseFreeCarrierAbs) fractAbsorb[firstk]=0;
	aw_tot=0;

	for (k=firstk; k!=lastk+k_inc; k+=k_inc) {
//		if (k>0) dx=pS->node.x[k]-pS->node.x[k-1]; else dx=0;
		if (k>0) dx=pN->Get_dxR(k-1); else dx=0;
		double distancein;
		if (pL->m_Back) distancein=totalwid-pN->GetX(k);
		else distancein=pN->GetX(k);

		xregion=0; 
		xregionstart=0; 
		while (distancein>xregionstart+w[xregion] && xregion<pD->m_NumRegions-1) { 
			xregionstart+=w[xregion]; 
			xregion++; 
		}

	  if (bUseFreeCarrierAbs) {

		//		Calculate alpha_fc = free carrier absorption across element		

		pcoeff=pfreecarrier[xregion];
		ncoeff=nfreecarrier[xregion];

		alpha_fc=ncoeff*effectiven[k]+pcoeff*effectivep[k];

		alphadx[k]=dx *( alpha_fc + absorb[xregion] );

		if (alpha_fc+absorb[xregion]>SMALL_REAL)	// total absorption might be 0.
				fractAbsorb[k]=absorb[xregion]/(alpha_fc+absorb[xregion]);
		else fractAbsorb[k]=0;

	  } else {
		// this is the non-parasitic version.
		alphadx[k]=absorb[xregion]*dx;
//		fractAbsorb[k]=1;
	  }
		aw_tot+=alphadx[k];
	}

	// check for special case for speedup
	// (b) no absorption of any kind
	if (aw_tot==0) {	// don't waste time if no absorption
		if (posterior2*incident2==1) escape = 1-incident1;	// Undefined. Arbitrarily, we say that no light escapes.
		else escape = (1-incident1) + posterior2*(1-incident2)/(1 - posterior2*incident2);
		for (k=0; k<=pN->GetLastNode(); k++) Gen[k]=0;
		delete [] alphadx;
		if (bUseFreeCarrierAbs) delete [] fractAbsorb;
		return;
	}
	
// STEP 2: Calculate the generation at all points across the device.
	
	double T=1; // Intensity of light remaining - start at 1 (full transmission)
	
	// generation from front to back (first pass) 

	double rcosTheta1=1/cosTheta1;	// save many divisions!
	double rcosTheta2=1/cosTheta2;
	
	if (pL->m_Back) { // light from back
      for (k=pN->GetLastNode(); k>=0 && pN->GetX(k)>=totalwid-ThetaDepth; k--) {
			remaining=T*exp(-alphadx[k]*rcosTheta1);
			Gen[k]=(T-remaining);
			T=remaining;
	  }
	  if (k>0) { // >0 not >=0 bcos if k==0, dx=0 so no absorption anyway
  			dx=pN->Get_dxR(k-1); // pS->node.x[k]-pS->node.x[k-1];
			remaining=T*exp(-alphadx[k]*(pN->GetX(k)-(totalwid-ThetaDepth))/dx/cosTheta1)
			 * exp(-alphadx[k]*((totalwid-ThetaDepth)-pN->GetX(k-1))/dx/cosTheta2);
			Gen[k]=(T-remaining);
			T=remaining;
			k--;			
	  }
	  for ( ;k>=0; k--) {
		  if (T>1e-50) { // Don't bother once all light has been absorbed 
			remaining=T*exp(-alphadx[k]*rcosTheta2);
			Gen[k]=(T-remaining);
			T=remaining;
		  } else Gen[k]=0;
	  }
	} else {	// light from front
      
      for (k=0; k<=pN->GetLastNode() && pN->GetX(k)<=ThetaDepth; k++) {
			remaining=T*exp(-alphadx[k]*rcosTheta1);
			Gen[k]=(T-remaining);
			T=remaining;
	  }
	  if (k<=pN->GetLastNode() && k>0) {
  			dx=pN->GetX(k)-pN->GetX(k-1);
			remaining=T*exp(-alphadx[k]*(ThetaDepth-pN->GetX(k-1))/dx/cosTheta1)
			 * exp(-alphadx[k]*(pN->GetX(k)-ThetaDepth)/dx/cosTheta2);
			Gen[k]=(T-remaining);
			T=remaining;
			k++;
	  }
	  for ( ;k<=pN->GetLastNode(); k++) {
		  if (T>1e-50) { // Don't bother once all light has been abosrbed 
			remaining=T*exp(-alphadx[k]*rcosTheta2);
			Gen[k]=(T-remaining);
			T=remaining;
		  } else Gen[k]=0;
	  }
	  
	}

	// generation from back to front (return pass)
	T*=posterior1;

	double cosTheta=cosTheta2;
	double aw_sofar=0;
	if (T>1e-50) { // Don't bother once all light has been abosrbed 
		if (posteriorTex) {
			for (k=lastk; k!=firstk-k_inc; k-=k_inc) {
					aw_sofar+=alphadx[k];
					cosTheta = 0.5 + 0.15*sqrt(aw_sofar); 		
					if (cosTheta>1) cosTheta=1; 
					remaining=T*exp(-alphadx[k]/cosTheta); 
					Gen[k]+=(T-remaining);
					T=remaining;
			}
		} else {
			double rcosTheta=1/cosTheta;
			for (k=lastk; k!=firstk-k_inc; k-=k_inc) {
				remaining=T*exp(-alphadx[k]*rcosTheta); 
				Gen[k]+=(T-remaining);
				T=remaining;
			}
		}
	}

	// Determine energy escaping after first double-pass
	escape = T * (1 - incident1);
	
	// Assume that remaining absorption occurs uniformly across thickness of
	// each region, weighted by absorption value in that region.
	T*=incident1; // energy available for second pass
	if (incidentTex||posteriorTex){
		cosTheta = 0.5 + 0.15*sqrt(aw_tot);
		if (cosTheta>1) cosTheta = 1;
	} else cosTheta=cosTheta2;
	
	if (aw_tot>SMALL_REAL && T>1e-50)	{
		double TransBackFr = exp(-aw_tot/cosTheta);
		double RemainingGeneration=  T * (1-TransBackFr)*(1+posterior2*TransBackFr)
	  				/(1-posterior2*incident2*TransBackFr*TransBackFr);

		escape += T * posterior2 * TransBackFr*TransBackFr *(1-incident2)
			/(1 - posterior2*incident2*TransBackFr*TransBackFr); // Add energy lost through subsequent passes

		RemainingGeneration/=aw_tot; // saves a division
		for (k=0; k<=pN->GetLastNode(); k++) {
			Gen[k]+=alphadx[k]*RemainingGeneration;
		}
	} else { // there is no absorption, but we must calculate escape.
		if (posterior2*incident2==1) escape +=0;	// Undefined. Arbitrarily, we say that no light escapes.
		else escape += T * posterior2 *(1-incident2)	
			/(1 - posterior2*incident2); // Add energy lost through subsequent passes
	}

	if (escape<1e-50) escape=0;

	// Now convert results to cm-2/sec.
	// Also, we must discard the parisitics from the generation.
	// To do this, multiply everything by fractAbsorb[k].

	double lambda_QHC= lambda/(Q*HC);
	if (bUseFreeCarrierAbs) {

		for (k=0; k<=pN->GetLastNode(); k++) {
			Gen[k] *= fractAbsorb[k]
					* lambda_QHC;	// Convert to cm-2/sec 
		}
		delete [] fractAbsorb;

	} else {

		for (k=0; k<=pN->GetLastNode(); k++) {
	//		Gen[k] *= lambda/(Q*HC);	// Convert to cm-2/sec 
			Gen[k] *= lambda_QHC;	// Convert to cm-2/sec 
		}
	}
	delete [] alphadx;
}	

void CProblem::LoadBlackbodySpectrum(CLight* pL)
{	// Uses blackbody parameters to load a spectrum
	double lambda;
	if (pL->m_BlackNum>MAX_WAVELENGTHS) pL->m_BlackNum=MAX_WAVELENGTHS;
	double delta_lambda = fabs(pL->m_BlackMax - pL->m_BlackMin)/pL->m_BlackNum;
	double Tb = pL->m_BlackTemperature;	// K
	const double C1 = 3.7405e16;	// W/cm2/nm
	const double C2 = 1.43879e7;	// K*nm
	for (int k=0; k<pL->m_BlackNum; k++)
	{
		lambda = pL->m_BlackMin + (k+0.5)*delta_lambda;
		pL->m_SpcLambda[k] = lambda;
		pL->m_Spectrum[k] = ( delta_lambda*C1
						/pow(lambda,5) /(exp(C2/(lambda*Tb))-1) );
	}
	pL->m_nSpectrum = pL->m_BlackNum;
}

#pragma optimize("", on) // normal, non-critical optimisation for the rest

void CProblem::SourcePhotogeneration(int time_step, CLight* pL)
{
	double Int, Lambda, Rfl;
	double escape;
	int k;
	if (pL->m_IntensityExternal) 
    	Int = CMath::LinearInterp(m_Status.time, pL->m_nSource,
	    				pL->m_SrcTime, pL->m_SrcIntensity);
	else if (time_step<0)
		Int = pL->m_IntensitySS;
	else
		Int = pL->m_IntensityTR1 + ((double)time_step/pE->m_TranNum)*
				(pL->m_IntensityTR2 - pL->m_IntensityTR1);
	pL->m_Intensity[time_step+1] = Int;
	CNode *pN = GetNode();
	if (pL->m_SpectrumMono)
	{	// monochromatic
		if (time_step<0)
			Lambda = pL->m_LambdaSS;
		else
			Lambda = pL->m_LambdaTR1 + ((double)time_step/pE->m_TranNum)*
				(pL->m_LambdaTR2 - pL->m_LambdaTR1);
		Rfl = Reflectance(Lambda,pL);

		double *TempGen= new double[pN->GetNodeCount()];
		double *effectiven= new double[pN->GetNodeCount()];
		double *effectivep= new double[pN->GetNodeCount()];
		CalculateCarrierConcentrationsForFCA(effectiven, effectivep);
		GenerationWithParasitics(Lambda, pL, TempGen, escape, effectiven, effectivep);
		double Gensofar=0;
		for (k=0; k<=pN->GetLastNode(); k++) {
			Gensofar+=TempGen[k]; // convert to cumulative generation
//			pS->node.G[k]+= Int * (1-Rfl) *Gensofar/nir;
			pN->SetGcum(k, pN->GetGcum(k) + Int * (1-Rfl) *Gensofar/nir);
		}
		delete [] TempGen;
		delete [] effectiven;
		delete [] effectivep;
		pL->m_Lambda[time_step+1] = Lambda;
		pL->m_Reflectance[time_step+1] = Rfl;
		pL->m_Escape[time_step+1] = escape;
	}
	else
	{	// spectrum
		m_Status.message = "Photogeneration";
		CDonStatusBar::UpdateStatusBar();
		if (pL->m_SpectrumBlack) LoadBlackbodySpectrum(pL);
		CMath::NormalizeArray(pL->m_nSpectrum, pL->m_Spectrum);
		double *TempGen= new double[pN->GetNodeCount()];

		double *effectiven= new double[pN->GetNodeCount()];
		double *effectivep= new double[pN->GetNodeCount()];
		CalculateCarrierConcentrationsForFCA(effectiven, effectivep);

		for (int j=0; j<pL->m_nSpectrum; j++)
		{
			Lambda = pL->m_SpcLambda[j];
			Rfl = Reflectance(Lambda,pL);
			
			GenerationWithParasitics(Lambda, pL, TempGen, escape, effectiven, effectivep);
			double Gensofar=0;
			for (k=0; k<=pN->GetLastNode(); k++) {
				Gensofar+=TempGen[k];	// convert to cumulative generation
//				pS->node.G[k]+= Int * pL->m_Spectrum[j] * (1-Rfl) *Gensofar/nir;
				pN->SetGcum(k, pN->GetGcum(k)+ Int * pL->m_Spectrum[j] * (1-Rfl) *Gensofar/nir);
			}
		}
		delete [] TempGen;
		delete [] effectiven;
		delete [] effectivep;
		pL->m_Lambda[time_step+1] = 0;	// external representation for "spectrum"
		pL->m_Reflectance[time_step+1] = 0;
		pL->m_Escape[time_step+1] = 0;
	}
}

void CProblem::SetPhotogeneration(int time_step)
{
	CNode *pN = GetNode();
	ASSERT(pN->GetLastNode()<=MAX_ELEMENTS);
	for (int k=0; k<=pN->GetLastNode(); k++) pN->SetGcum(k, 0);
	if (pE->m_LightExternal)
	{
		for (k=0; k<=pN->GetLastNode(); k++)
			pN->SetGcum(k, CMath::LinearInterp(pN->GetX(k), pE->m_nPhoto,
							pE->m_Position, pE->m_Photogen) / nir);
		return;
	}			
	if (pE->m_LightPri.m_On) SourcePhotogeneration(time_step, &pE->m_LightPri);
	if (pE->m_LightSec.m_On) SourcePhotogeneration(time_step, &pE->m_LightSec);
}

void CProblem::SetExcitation(int time_step, BOOL initialize)
{
	if (time_step<0)	// steady-state
	{
		SetPhotogeneration(time_step);
		if (pE->m_Base.m_External)
		{
			pS->m_bSource[BASE]=pD->m_EnableBase;
			pS->m_vSource[BASE]=CMath::LinearInterp
					(m_Status.time, pE->m_Base.m_nSource, 
						pE->m_Base.m_SourceTime, pE->m_Base.m_SourceVolts) / Vt; 
			pS->m_rSource[BASE]=CMath::LinearInterp
					(m_Status.time, pE->m_Base.m_nSource,
						pE->m_Base.m_SourceTime, pE->m_Base.m_Resistance) *Q*nir*Aref/Vt;
		}
		else
		{
			pS->m_bSource[BASE] = pD->m_EnableBase && pE->m_Base.m_ConnectSS;
			pS->m_vSource[BASE] = pE->m_Base.m_VSS /Vt;
			pS->m_rSource[BASE] = pE->m_Base.m_RSS *Q*nir*Aref/Vt;
			if (!pE->m_Base.m_OhmsSS) pS->m_rSource[BASE] /= Aref;
		}
		if (pE->m_Coll.m_External)
		{
			pS->m_bSource[COLLECTOR]=pD->m_EnableCollector;
			pS->m_vSource[COLLECTOR]=CMath::LinearInterp
					(m_Status.time, pE->m_Coll.m_nSource,
						pE->m_Coll.m_SourceTime, pE->m_Coll.m_SourceVolts) / Vt;
			pS->m_rSource[COLLECTOR]=CMath::LinearInterp
					(m_Status.time, pE->m_Coll.m_nSource,
						pE->m_Coll.m_SourceTime, pE->m_Coll.m_Resistance) *Q*nir*Aref/Vt;
		}
		else
		{
			pS->m_bSource[COLLECTOR] = pD->m_EnableCollector && pE->m_Coll.m_ConnectSS;
			pS->m_vSource[COLLECTOR] = pE->m_Coll.m_VSS /Vt;
			pS->m_rSource[COLLECTOR] = pE->m_Coll.m_RSS *Q*nir*Aref/Vt;
			if (!pE->m_Coll.m_OhmsSS) pS->m_rSource[COLLECTOR] /= Aref;
		}			
	}
	else if (time_step==0)	// begin transient
	{
		if ( initialize ||
		    (pE->m_LightPri.m_On && pE->m_LightPri.m_SpectrumMono) ||
		    (pE->m_LightPri.m_On &&
		    	pE->m_LightPri.m_IntensityTR1!=pE->m_LightPri.m_IntensitySS) ||
		    (pE->m_LightPri.m_On && pE->m_LightPri.m_IntensityExternal) ||
			(pE->m_LightSec.m_On && pE->m_LightSec.m_SpectrumMono) ||
			(pE->m_LightSec.m_On &&
				pE->m_LightSec.m_IntensityTR1!=pE->m_LightSec.m_IntensitySS) ||
			(pE->m_LightSec.m_On && pE->m_LightSec.m_IntensityExternal) )
			SetPhotogeneration(time_step);
		if (pE->m_Base.m_External)
		{
			pS->m_bSource[BASE]=pD->m_EnableBase;
			pS->m_vSource[BASE]=CMath::LinearInterp
					(m_Status.time, pE->m_Base.m_nSource, 
						pE->m_Base.m_SourceTime, pE->m_Base.m_SourceVolts) / Vt; 
			pS->m_rSource[BASE]=CMath::LinearInterp
					(m_Status.time, pE->m_Base.m_nSource,
						pE->m_Base.m_SourceTime, pE->m_Base.m_Resistance) * Q*nir*Aref/Vt;
		}
		else
		{
			pS->m_bSource[BASE] = pD->m_EnableBase && pE->m_Base.m_ConnectTR;
			pS->m_vSource[BASE] = pE->m_Base.m_VTR1 /Vt;
			pS->m_rSource[BASE] = pE->m_Base.m_RTR *Q*nir*Aref/Vt;
			if (!pE->m_Base.m_OhmsTR) pS->m_rSource[BASE] /= Aref;
		}
		if (pE->m_Coll.m_External)
		{
			pS->m_bSource[COLLECTOR]=pD->m_EnableCollector;
			pS->m_vSource[COLLECTOR]=CMath::LinearInterp
					(m_Status.time, pE->m_Coll.m_nSource,
						pE->m_Coll.m_SourceTime, pE->m_Coll.m_SourceVolts) / Vt;
			pS->m_rSource[COLLECTOR]=CMath::LinearInterp
					(m_Status.time, pE->m_Coll.m_nSource,
						pE->m_Coll.m_SourceTime, pE->m_Coll.m_Resistance) * Q*nir*Aref/Vt;
		}
		else
		{
			pS->m_bSource[COLLECTOR] = pD->m_EnableCollector && pE->m_Coll.m_ConnectTR;
			pS->m_vSource[COLLECTOR] = pE->m_Coll.m_VTR1 /Vt;
			pS->m_rSource[COLLECTOR] = pE->m_Coll.m_RTR *Q*nir*Aref/Vt;
			if (!pE->m_Coll.m_OhmsTR) pS->m_rSource[COLLECTOR] /= Aref;
		}
	}
	else	// continue transient
	{
		if ( initialize || 
		    (pE->m_LightPri.m_On && pE->m_LightPri.m_SpectrumMono) ||
			(pE->m_LightPri.m_On &&
				pE->m_LightPri.m_IntensityTR1!=pE->m_LightPri.m_IntensityTR2) ||
			(pE->m_LightPri.m_On && pE->m_LightPri.m_IntensityExternal) ||
			(pE->m_LightSec.m_On && pE->m_LightSec.m_SpectrumMono) ||
			(pE->m_LightSec.m_On &&
				pE->m_LightSec.m_IntensityTR1!=pE->m_LightSec.m_IntensityTR2) ||
			(pE->m_LightSec.m_On && pE->m_LightSec.m_IntensityExternal) )
			SetPhotogeneration(time_step);
		if (pE->m_Base.m_External)
		{
			pS->m_bSource[BASE]=pD->m_EnableBase;
			pS->m_vSource[BASE]=CMath::LinearInterp
					(m_Status.time, pE->m_Base.m_nSource, 
						pE->m_Base.m_SourceTime, pE->m_Base.m_SourceVolts) / Vt; 
			pS->m_rSource[BASE]=CMath::LinearInterp
					(m_Status.time, pE->m_Base.m_nSource,
						pE->m_Base.m_SourceTime, pE->m_Base.m_Resistance) * Q*nir*Aref/Vt;
		}
		else
		{
			pS->m_bSource[BASE] = pD->m_EnableBase && pE->m_Base.m_ConnectTR;
			pS->m_vSource[BASE] = pE->m_Base.m_VTR1 /Vt 
				+ (double)time_step/pE->m_TranNum 
					* (pE->m_Base.m_VTR2 - pE->m_Base.m_VTR1) /Vt;
			pS->m_rSource[BASE] = pE->m_Base.m_RTR *Q*nir*Aref/Vt;
			if (!pE->m_Base.m_OhmsTR) pS->m_rSource[BASE] /= Aref;
		}
		if (pE->m_Coll.m_External)
		{
			pS->m_bSource[COLLECTOR]=pD->m_EnableCollector;
			pS->m_vSource[COLLECTOR]=CMath::LinearInterp
					(m_Status.time, pE->m_Coll.m_nSource,
						pE->m_Coll.m_SourceTime, pE->m_Coll.m_SourceVolts) / Vt;
			pS->m_rSource[COLLECTOR]=CMath::LinearInterp
					(m_Status.time, pE->m_Coll.m_nSource,
						pE->m_Coll.m_SourceTime, pE->m_Coll.m_Resistance) *Q*nir*Aref/Vt;
		}
		else
		{
			pS->m_bSource[COLLECTOR] = pD->m_EnableCollector && pE->m_Coll.m_ConnectTR;
			pS->m_vSource[COLLECTOR] = pE->m_Coll.m_VTR1 /Vt 
				+ (double)time_step/pE->m_TranNum 
					* (pE->m_Coll.m_VTR2 - pE->m_Coll.m_VTR1) /Vt;
			pS->m_rSource[COLLECTOR] = pE->m_Coll.m_RTR *Q*nir*Aref/Vt;
			if (!pE->m_Coll.m_OhmsTR) pS->m_rSource[COLLECTOR] /= Aref;
		}
	}
	pS->m_bSource[EMITTER] = pD->m_EnableEmitter;
	pS->m_vSource[EMITTER] = 0;
	pS->m_rSource[EMITTER] = 0;
	pS->SetSourceResistance(EMITTER, pD->m_EmitterR *Q*nir*Aref/Vt );
//	if (pS->m_rIntSource[EMITTER]<SMALL_REAL) pS->m_rIntSource[EMITTER] = SMALL_REAL;
	pS->SetSourceResistance(BASE, pD->m_BaseR *Q*nir*Aref/Vt);
//	if (pS->m_rIntSource[BASE]<SMALL_REAL) pS->m_rIntSource[BASE] = SMALL_REAL;
	pS->SetSourceResistance(COLLECTOR, pD->m_CollectorR *Q*nir*Aref/Vt);
//	if (pS->m_rIntSource[COLLECTOR]<SMALL_REAL) pS->m_rIntSource[COLLECTOR] = SMALL_REAL;
}

void CProblem::SetNeutralEQ(CNode* pN)
{
	for (int k=0; k<=pN->GetLastNode(); k++)
	{
		pN->SetPsi_eq(k, CPhysics::Neutral_Eq_Psi(nir, nir*(pN->GetNd(k)-pN->GetNa(k)),
							pN->GetVn(k)*Vt, pN->GetVp(k)*Vt, T)/Vt);
	}
}

void CProblem::SetSurfacePotentials(CNode* pN)
{	// positive barrier bends bands upward
	// for Barrier, assumes Nd, Na, Vn, Vp already set up
	int k;
	pS->m_FrontSurface = pD->m_FrontSurface;
	pS->m_RearSurface = pD->m_RearSurface;
	k=0;
	if (pS->m_FrontSurface==BARRIER_SURFACE)
		pN->SetPsi_eq(k,  CPhysics::Neutral_Eq_Psi(nir, nir*(pN->GetNd(k)-pN->GetNa(k)),
							pN->GetVn(k)*Vt, pN->GetVp(k)*Vt, T)/Vt
						+ pD->m_FrontBarrier /Vt);
	k=pN->GetLastNode();
	if (pS->m_RearSurface==BARRIER_SURFACE)
		pN->SetPsi_eq(k, CPhysics::Neutral_Eq_Psi(nir, nir*(pN->GetNd(k)-pN->GetNa(k)),
							pN->GetVn(k)*Vt, pN->GetVp(k)*Vt, T)/Vt
						+ pD->m_RearBarrier /Vt);
	if (pS->m_FrontSurface==CHARGED_SURFACE)
		pS->m_NssFront = pD->m_FrontCharge /nir;
	if (pS->m_RearSurface==CHARGED_SURFACE)
		pS->m_NssRear = pD->m_RearCharge /nir;
}

int CProblem::SetNodePositions(CNode* pN, int first, int region_num)
{
	const int MAX_LOGS = 13;
	const double log_node[MAX_LOGS] = {1E-7, 2E-7, 5E-7, 1E-6, 2E-6, 5E-6,
	                           			1E-5, 2E-5, 5E-5, 1E-4, 2E-4, 5E-4, 1E-3};
	// first is leftmost node that still needs to be assigned
	// last is the rightmost node that still needs to be assigned
	// return the rightmost node that WAS assigned
	int k, n, nz, nf, nr, nbk, zones, last_log, last, return_value;
    double xj, jf1, jf2, jr1, jr2, jf, jr, z, xL, xR, w;
	CRegion* pR = pD->GetRegion(region_num);
	if (first==0) first++;	// node zero is always at x=0
	w = pR->m_Thickness;
	xj = sqrt(2 * EPS0 * pR->GetMaterial()->m_Permittivity / (Q * pR->m_BkgndDop) );
	if (pR->m_FrontExternal)
	{
		jf = 2*xj + 1E-4;
		if (jf>=w/2) jf = 0;
	}
	else
	{
		jf1 = jf2 = 0;
		if (pR->m_FrontDiff1.IsEnabled())
		{
			jf1 = pR->m_FrontDiff1.m_Xpeak;
			do jf1 += pR->m_FrontDiff1.m_Depth; 
			while (CPhysics::Doping(jf1, pR->m_FrontDiff1.m_Profile, 
					pR->m_FrontDiff1.m_Npeak, pR->m_FrontDiff1.m_Depth,
					pR->m_FrontDiff1.m_Xpeak) > pR->m_BkgndDop);
			jf1 += 2*xj;
		}
		if (pR->m_FrontDiff2.IsEnabled())
		{
			jf2 = pR->m_FrontDiff2.m_Xpeak;
			do jf2 += pR->m_FrontDiff2.m_Depth; 
			while (CPhysics::Doping(jf2, pR->m_FrontDiff2.m_Profile, 
					pR->m_FrontDiff2.m_Npeak, pR->m_FrontDiff2.m_Depth,
					pR->m_FrontDiff2.m_Xpeak) > pR->m_BkgndDop);
			jf2 += 2*xj;
		}
		if (jf1>=w/2) jf1 = 0;
		if (jf2>=w/2) jf2 = 0;
	    if (jf2>jf1) jf = jf2; else jf = jf1;
	}
	if (pR->m_RearExternal)
	{
		jr = 2*xj + 1E-4;
		if (jr>=w/2) jr = 0;
	}
	else
	{
		jr1 = jr2 = 0;
		if (pR->m_RearDiff1.IsEnabled())
		{
			jr1 = pR->m_RearDiff1.m_Xpeak;
			do jr1 += pR->m_RearDiff1.m_Depth;
			while (CPhysics::Doping(jr1, pR->m_RearDiff1.m_Profile, 
					pR->m_RearDiff1.m_Npeak, pR->m_RearDiff1.m_Depth,
					pR->m_RearDiff1.m_Xpeak) > pR->m_BkgndDop);
			jr1 += 2*xj;
		}
		if (pR->m_RearDiff2.IsEnabled())
		{
			jr2 = pR->m_RearDiff2.m_Xpeak;
			do jr2 += pR->m_RearDiff2.m_Depth; 
			while (CPhysics::Doping(jr2, pR->m_RearDiff2.m_Profile, 
					pR->m_RearDiff2.m_Npeak, pR->m_RearDiff2.m_Depth,
					pR->m_RearDiff2.m_Xpeak) > pR->m_BkgndDop);
			jr2 += 2*xj;
		}
		if (jr1>=w/2) jr1 = 0;
		if (jr2>=w/2) jr2 = 0;	
	    if (jr2>jr1) jr = jr2; else jr = jr1;
	}
	zones = 1;
	if (jf>0) zones++; if (jr>0) zones++;
	n = MAX_ELEMENTS / MAX_REGIONS;			// total elements in this region
	last = first + n - 1;					// index for last node in region
	return_value = last;
	pN->SetX(last, pN->GetX(first-1) + w); last--;	// makes right side like left side
	xL=pN->GetX(first-1);						// xL was last assigned left pos
	xR=pN->GetX(last+1); 						// xR was last assigned right pos
	nz = n / zones;							// approximate elements per zone
	// front zone nodes
	if (jf>0) z=jf; else z=w-jr;	// front zone is diffusion, if one exists
	last_log = 0;
	while (last_log<MAX_LOGS && log_node[last_log]<z/nz) last_log++;
	if (last_log>n/2) last_log = n/4;	// don't use all of the nodes for log nodes!
	for (k=0; k<last_log; k++) pN->SetX(first+k, xL + log_node[k]);
	first += last_log; xL = pN->GetX(first-1);
	if (jf>0)	// assign remaining diffusion nodes
	{
		nf = nz - last_log;
		for (k=0; k<nf; k++) pN->SetX(first+k, xL + (k+1)*(jf-log_node[last_log-1])/nf);
		first += nf; xL = pN->GetX(first-1);
	}
	// rear zone nodes
	if (jr>0) z=jr; else z=w-jf;	// rear zone is diffusion, if one exists
	last_log = 0;
	while (last_log<MAX_LOGS && log_node[last_log]<z/nz) last_log++;
	if (last_log>n/2) last_log = n/4;	// don't use all of the nodes for log nodes!
	for (k=0; k<last_log; k++) pN->SetX(last-k, xR - log_node[k]);
	last -= last_log; xR = pN->GetX(last+1);
	if (jr>0)	// assign remaining diffusion nodes
	{
		nr = nz - last_log;
		for (k=0; k<nr; k++) pN->SetX(last-k, xR - (k+1)*(jr-log_node[last_log-1])/nr);
		last -= nr; xR = pN->GetX(last+1);
	}
	// bulk zone
	nbk = last - first + 1;
	ASSERT(xR>xL);
	for (k=0; k<nbk; k++) pN->SetX(first+k, xL + (k+1)*(xR-xL)/(nbk+1));
	return return_value;
}

int CProblem::GetLastNode(CNode* pN, int first)
{
	// return the last node in the current region
	int reg = pN->GetRegion(first);
	int node = first;
	while (node<pN->GetLastNode() && pN->GetRegion(node+1)==reg) node++;
	return node;
}

void CProblem::SetNodeDoping(CNode* pN, int first, int last, int region_num)
{
	double xL, xR;	// location of left, right edges of region
	if (first==0) xL = pN->GetX(first); else xL = pN->GetX(first-1);
	xR = pN->GetX(last);

	double dop;
	CRegion* pR = pD->GetRegion(region_num);
	for (int k=first; k<=last; k++)
	{
		double Na, Nd;
		Nd = 1; Na = 1;
		if (pR->m_BkgndType==N_TYPE) Nd += pR->m_BkgndDop;
		if (pR->m_BkgndType==P_TYPE) Na += pR->m_BkgndDop;
		if (pR->m_FrontExternal) {
			Nd += CMath::LogInterp
				(pN->GetX(k)-xL, pR->m_nFront, pR->m_FrontPosition, pR->m_FrontDopingDonor);
			Na += CMath::LogInterp
				(pN->GetX(k)-xL, pR->m_nFront, pR->m_FrontPosition, pR->m_FrontDopingAcceptor);
		} else {
			if (pR->m_FrontDiff1.IsEnabled()) {
				dop =CPhysics::Doping(pN->GetX(k)-xL, pR->m_FrontDiff1.m_Profile,
								pR->m_FrontDiff1.m_Npeak, pR->m_FrontDiff1.m_Depth,
								pR->m_FrontDiff1.m_Xpeak);
				if (pR->m_FrontDiff1.m_Type==N_TYPE) Nd+=dop; else Na+=dop;
			}
			if (pR->m_FrontDiff2.IsEnabled()) {
				dop = CPhysics::Doping(pN->GetX(k)-xL, pR->m_FrontDiff2.m_Profile,
								pR->m_FrontDiff2.m_Npeak, pR->m_FrontDiff2.m_Depth,
								pR->m_FrontDiff2.m_Xpeak);
				if (pR->m_FrontDiff2.m_Type==N_TYPE) Nd+=dop; else Na+=dop;
			}
		}
		if (pR->m_RearExternal)
		{
			Nd += CMath::LogInterp
				(xR-pN->GetX(k), pR->m_nRear, pR->m_RearPosition, pR->m_RearDopingDonor);
			Na += CMath::LogInterp
				(xR-pN->GetX(k), pR->m_nRear, pR->m_RearPosition, pR->m_RearDopingAcceptor);
		}
		else
		{
			if (pR->m_RearDiff1.IsEnabled()){			
				dop = CPhysics::Doping(xR-pN->GetX(k), pR->m_RearDiff1.m_Profile,
								pR->m_RearDiff1.m_Npeak, pR->m_RearDiff1.m_Depth,
								pR->m_RearDiff1.m_Xpeak);
				if (pR->m_RearDiff1.m_Type==N_TYPE) Nd+=dop; else Na+=dop;
			}
			if (pR->m_RearDiff2.IsEnabled()) {
				dop = CPhysics::Doping(xR-pN->GetX(k), pR->m_RearDiff2.m_Profile,
								pR->m_RearDiff2.m_Npeak, pR->m_RearDiff2.m_Depth,
								pR->m_RearDiff2.m_Xpeak);
				if (pR->m_RearDiff2.m_Type==N_TYPE) Nd+=dop; else Na+=dop;
			}
		}
		pN->SetNaNd(k, Na/nir, Nd/nir);
	}
}

/***		
void CProblem::SetNodeDoping(CNode* pN, int first, int last)
{
	double xL, xR;	// location of left, right edges of region
	if (first==0) xL = pN->GetX(first); else xL = pN->GetX(first-1);
	xR = pN->GetX(last);

	CRegion* pR = pD->pR;
	for (int k=first; k<=last; k++)
	{
		double Na, Nd;
		Nd = 1 /nir; Na = 1/ nir;
		if (pR->m_BkgndType==N_TYPE) Nd += pR->m_BkgndDop /nir;
		if (pR->m_BkgndType==P_TYPE) Na += pR->m_BkgndDop /nir;
		if (pR->m_FrontExternal) {
			Nd += CMath::LogInterp
				(pN->GetX(k)-xL, pR->m_nFront, pR->m_FrontPosition, pR->m_FrontDopingDonor) / nir;
			Na += CMath::LogInterp
				(pN->GetX(k)-xL, pR->m_nFront, pR->m_FrontPosition, pR->m_FrontDopingAcceptor) / nir;
		} else {
			if (pR->m_FrontDiff1.m_Enable && pR->m_FrontDiff1.m_Type==N_TYPE)
				Nd += CPhysics::Doping(pN->GetX(k)-xL, pR->m_FrontDiff1.m_Profile,
								pR->m_FrontDiff1.m_Npeak, pR->m_FrontDiff1.m_Depth,
								pR->m_FrontDiff1.m_Xpeak) / nir;
			if (pR->m_FrontDiff2.m_Enable && pR->m_FrontDiff2.m_Type==N_TYPE)
				Nd += CPhysics::Doping(pN->GetX(k)-xL, pR->m_FrontDiff2.m_Profile,
								pR->m_FrontDiff2.m_Npeak, pR->m_FrontDiff2.m_Depth,
								pR->m_FrontDiff2.m_Xpeak) / nir;
			if (pR->m_FrontDiff1.m_Enable && pR->m_FrontDiff1.m_Type==P_TYPE)
				Na += CPhysics::Doping(pN->GetX(k)-xL, pR->m_FrontDiff1.m_Profile,
								pR->m_FrontDiff1.m_Npeak, pR->m_FrontDiff1.m_Depth,
								pR->m_FrontDiff1.m_Xpeak) / nir;
			if (pR->m_FrontDiff2.m_Enable && pR->m_FrontDiff2.m_Type==P_TYPE)
				Na += CPhysics::Doping(pN->GetX(k)-xL, pR->m_FrontDiff2.m_Profile,
								pR->m_FrontDiff2.m_Npeak, pR->m_FrontDiff2.m_Depth,
								pR->m_FrontDiff2.m_Xpeak) / nir;
		}
		if (pR->m_RearExternal)
		{
			Nd += CMath::LogInterp
				(xR-pN->GetX(k), pR->m_nRear, pR->m_RearPosition, pR->m_RearDopingDonor) / nir;
			Na += CMath::LogInterp
				(xR-pN->GetX(k), pR->m_nRear, pR->m_RearPosition, pR->m_RearDopingAcceptor) / nir;
		}
		else
		{
			if (pR->m_RearDiff1.m_Enable && pR->m_RearDiff1.m_Type==N_TYPE)
				Nd += CPhysics::Doping(xR-pN->GetX(k), pR->m_RearDiff1.m_Profile,
								pR->m_RearDiff1.m_Npeak, pR->m_RearDiff1.m_Depth,
								pR->m_RearDiff1.m_Xpeak) / nir;
			if (pR->m_RearDiff2.m_Enable && pR->m_RearDiff2.m_Type==N_TYPE)
				Nd += CPhysics::Doping(xR-pN->GetX(k), pR->m_RearDiff2.m_Profile,
								pR->m_RearDiff2.m_Npeak, pR->m_RearDiff2.m_Depth,
								pR->m_RearDiff2.m_Xpeak) / nir;
			if (pR->m_RearDiff1.m_Enable && pR->m_RearDiff1.m_Type==P_TYPE)
				Na += CPhysics::Doping(xR-pN->GetX(k), pR->m_RearDiff1.m_Profile,
								pR->m_RearDiff1.m_Npeak, pR->m_RearDiff1.m_Depth,
								pR->m_RearDiff1.m_Xpeak) / nir;
			if (pR->m_RearDiff2.m_Enable && pR->m_RearDiff2.m_Type==P_TYPE)
				Na += CPhysics::Doping(xR-pN->GetX(k), pR->m_RearDiff2.m_Profile,
								pR->m_RearDiff2.m_Npeak, pR->m_RearDiff2.m_Depth,
								pR->m_RearDiff2.m_Xpeak) / nir;
		}
		pN->SetNaNd(k, Na, Nd);
***/
/**
		pN->Nd[k] = 1 /nir; pN->Na[k] = 1/ nir;
		if (pR->m_BkgndType==N_TYPE) pN->Nd[k] += pR->m_BkgndDop /nir;
		if (pR->m_BkgndType==P_TYPE) pN->Na[k] += pR->m_BkgndDop /nir;
		if (pR->m_FrontExternal)
		{
			pN->Nd[k] += CMath::LogInterp
				(pN->GetX(k)-xL, pR->m_nFront, pR->m_FrontPosition, pR->m_FrontDopingDonor) / nir;
			pN->Na[k] += CMath::LogInterp
				(pN->GetX(k)-xL, pR->m_nFront, pR->m_FrontPosition, pR->m_FrontDopingAcceptor) / nir;
		}
		else
		{
			if (pR->m_FrontDiff1.m_Enable && pR->m_FrontDiff1.m_Type==N_TYPE)
				pN->Nd[k] += CPhysics::Doping(pN->x[k]-xL, pR->m_FrontDiff1.m_Profile,
								pR->m_FrontDiff1.m_Npeak, pR->m_FrontDiff1.m_Depth,
								pR->m_FrontDiff1.m_Xpeak) / nir;
			if (pR->m_FrontDiff2.m_Enable && pR->m_FrontDiff2.m_Type==N_TYPE)
				pN->Nd[k] += CPhysics::Doping(pN->x[k]-xL, pR->m_FrontDiff2.m_Profile,
								pR->m_FrontDiff2.m_Npeak, pR->m_FrontDiff2.m_Depth,
								pR->m_FrontDiff2.m_Xpeak) / nir;
			if (pR->m_FrontDiff1.m_Enable && pR->m_FrontDiff1.m_Type==P_TYPE)
				pN->Na[k] += CPhysics::Doping(pN->x[k]-xL, pR->m_FrontDiff1.m_Profile,
								pR->m_FrontDiff1.m_Npeak, pR->m_FrontDiff1.m_Depth,
								pR->m_FrontDiff1.m_Xpeak) / nir;
			if (pR->m_FrontDiff2.m_Enable && pR->m_FrontDiff2.m_Type==P_TYPE)
				pN->Na[k] += CPhysics::Doping(pN->x[k]-xL, pR->m_FrontDiff2.m_Profile,
								pR->m_FrontDiff2.m_Npeak, pR->m_FrontDiff2.m_Depth,
								pR->m_FrontDiff2.m_Xpeak) / nir;
		}
		if (pR->m_RearExternal)
		{
			pN->Nd[k] += CMath::LogInterp
				(xR-pN->x[k], pR->m_nRear, pR->m_RearPosition, pR->m_RearDopingDonor) / nir;
			pN->Na[k] += CMath::LogInterp
				(xR-pN->x[k], pR->m_nRear, pR->m_RearPosition, pR->m_RearDopingAcceptor) / nir;
		}
		else
		{
			if (pR->m_RearDiff1.m_Enable && pR->m_RearDiff1.m_Type==N_TYPE)
				pN->Nd[k] += CPhysics::Doping(xR-pN->x[k], pR->m_RearDiff1.m_Profile,
								pR->m_RearDiff1.m_Npeak, pR->m_RearDiff1.m_Depth,
								pR->m_RearDiff1.m_Xpeak) / nir;
			if (pR->m_RearDiff2.m_Enable && pR->m_RearDiff2.m_Type==N_TYPE)
				pN->Nd[k] += CPhysics::Doping(xR-pN->x[k], pR->m_RearDiff2.m_Profile,
								pR->m_RearDiff2.m_Npeak, pR->m_RearDiff2.m_Depth,
								pR->m_RearDiff2.m_Xpeak) / nir;
			if (pR->m_RearDiff1.m_Enable && pR->m_RearDiff1.m_Type==P_TYPE)
				pN->Na[k] += CPhysics::Doping(xR-pN->x[k], pR->m_RearDiff1.m_Profile,
								pR->m_RearDiff1.m_Npeak, pR->m_RearDiff1.m_Depth,
								pR->m_RearDiff1.m_Xpeak) / nir;
			if (pR->m_RearDiff2.m_Enable && pR->m_RearDiff2.m_Type==P_TYPE)
				pN->Na[k] += CPhysics::Doping(xR-pN->x[k], pR->m_RearDiff2.m_Profile,
								pR->m_RearDiff2.m_Npeak, pR->m_RearDiff2.m_Depth,
								pR->m_RearDiff2.m_Xpeak) / nir;
		}
	}
}
***/

void CProblem::SetNodeParameters(CNode* pN, int first, int last, int region_num)
{
	// assumes reference values already set up
	CMaterial* pM = pD->GetMaterialForRegion(region_num);
	CRegion* pR = pD->GetRegion(region_num);
	int reg;		// current region
	if (first==0) reg=0; else reg = pN->GetRegion(first-1) + 1;
	SetNodeDoping(pN, first, last, region_num);
	double ni = CPhysics::ni(pM->m_ni200, pM->m_ni300, pM->m_ni400, T);
	double e_mob, h_mob;	// local electron and hole mobilities

	double endx;
	endx=pD->GetThickness();
	for (int k=first; k<=last; k++)
	{
		pN->SetRegion(k, reg);
		pN->SetEps(k, pM->m_Permittivity*EPS0 *Vt/(Q*nir));
		double taun, taup;
		double nref, nalpha;
		if (pR->m_BkgndType==N_TYPE) {
			nref = pM->m_BulkNrefN;
			nalpha = pM->m_BulkNalphaN;
		} else {
			nref = pM->m_BulkNrefP;
			nalpha = pM->m_BulkNalphaN;
		}
		taun= CPhysics::tau(pR->m_TauN, pR->m_BkgndDop,
						nref,	nalpha, T, pM->m_BulkTalpha);
		taup= CPhysics::tau(pR->m_TauP, pR->m_BkgndDop,
						nref,	nalpha, T, pM->m_BulkTalpha);
		pN->SetTau(k, taun, taup);
		ASSERT(pD->m_Area==Aref);

		double area=1;
		if (pD->m_FrontTexture)
			area += (1/cos(pD->m_FrontAngle*PI/180)-1) *
				CMath::exp(-pow(pN->GetX(k)/(pD->m_FrontDepth/6),2)/2);

		if (pD->m_RearTexture)
			area += (1/cos(pD->m_RearAngle*PI/180)-1) *
				CMath::exp(-pow((endx-pN->GetX(k))/(pD->m_RearDepth/6),2)/2);
		pN->SetArea(k, area);
		double Na, Nd;
		Na=pN->GetNa(k);
		Nd=pN->GetNd(k);
		double Vn, Vp;
		Vn = CPhysics::BGN(Nd*nir, pM->m_BGNnNref, pM->m_BGNnSlope)/Vt
				+ CMath::log(CPhysics::Nc(ni, pM->m_NcNv, pM->m_BandGap, T))
				+ pM->m_Affinity/Vt - Vn0/Vt;
		Vp = CPhysics::BGN(Na*nir, pM->m_BGNpNref, pM->m_BGNpSlope)/Vt
				+ CMath::log(CPhysics::Nv(ni, pM->m_NcNv, pM->m_BandGap, T))
				- pM->m_BandGap/Vt - pM->m_Affinity/Vt - Vp0/Vt;
		pN->SetVnVp(k, Vn, Vp);

		if (pM->m_FixedMobility)		// fixed mobility
			e_mob = pM->m_Elec.Fixed;
		else if (Nd>Na)	// electrons are majority
			e_mob = CPhysics::Mobility(nir*(Nd+Na), 
						pM->m_Elec.Max,
						pM->m_Elec.MajMin, pM->m_Elec.MajNref, pM->m_Elec.MajAlpha,
						pM->m_Elec.B1, pM->m_Elec.B2, pM->m_Elec.B3, pM->m_Elec.B4, T);
		else							// electrons are minority
			e_mob = CPhysics::Mobility(nir*(Nd+Na), 
						pM->m_Elec.Max,
						pM->m_Elec.MinMin, pM->m_Elec.MinNref, pM->m_Elec.MinAlpha,
						pM->m_Elec.B1, pM->m_Elec.B2, pM->m_Elec.B3, pM->m_Elec.B4, T);
		if (pM->m_FixedMobility)
			h_mob = pM->m_Hole.Fixed;
		else if (Na>Nd)	// holes are majority
			h_mob = CPhysics::Mobility(nir*(Nd+Na), 
						pM->m_Hole.Max,
						pM->m_Hole.MajMin, pM->m_Hole.MajNref, pM->m_Hole.MajAlpha,
						pM->m_Hole.B1, pM->m_Hole.B2, pM->m_Hole.B3, pM->m_Hole.B4, T);
		else							// holes are minority
			h_mob = CPhysics::Mobility(nir*(Nd+Na), 
						pM->m_Hole.Max,
						pM->m_Hole.MinMin, pM->m_Hole.MinNref, pM->m_Hole.MinAlpha,
						pM->m_Hole.B1, pM->m_Hole.B2, pM->m_Hole.B3, pM->m_Hole.B4, T);
		double Un, Up;
		Un = pN->GetVn(k) + CMath::log(Vt * e_mob * pN->GetArea(k));	// D from mu using Vt
		Up = pN->GetVp(k) + CMath::log(Vt * h_mob * pN->GetArea(k));	// assumes Einstein relation
		pN->SetUnUp(k, Un, Up);

		double esatn, esatp;
		if (e_mob<=0 || pM->m_FixedMobility) esatn = 0;
		else esatn = pM->m_Elec.Vmax/e_mob /Vt;
		if (h_mob<=0 || pM->m_FixedMobility) esatp = 0;
		else esatp = pM->m_Hole.Vmax/h_mob /Vt;
		pN->SetEsat(k, esatn, esatp);
	}
}

void CProblem::SetRegionParameters(int reg, int first, int last)
{	// assumes node doping already set up
	CRegion* pR = pD->GetRegion(reg);
	CMaterial* pM = pD->GetMaterialForRegion(reg);
	double ni = CPhysics::ni(pM->m_ni200, pM->m_ni300, pM->m_ni400, T);
	CNode *pN = GetNode();
	CRegionData *pRegData = pS->GetRegionData();
	pRegData->SetRegionLocation(reg, pN->GetX(first), pN->GetX(last));
//	pS->m_xRegionFront[reg] = pN->GetX(first); 
//	pS->m_xRegionRear[reg] = pN->GetX(last);
/**
	pS->m_Cn[reg] = pM->m_Cn *(nir*nir);
	pS->m_Cp[reg] = pM->m_Cp *(nir*nir);
	pS->m_Cnp[reg] = pM->m_Cnp *(nir*nir);
	pS->m_B[reg] = pM->m_BB *nir;
	pS->m_Et[reg] = pR->m_BulkEt/Vt;
***/
	pRegData->SetBulkRecombinationParametersForRegion(reg, pM->m_Cn *(nir*nir), pM->m_Cp *(nir*nir), pM->m_Cnp *(nir*nir), pM->m_BB *nir, pR->m_BulkEt/Vt);
//	pS->m_Eref[reg] = pM->m_BulkEref/Vt;
//	pS->m_Egamma[reg] = pM->m_BulkEgamma;
	pRegData->SetErefEgammaForRegion(reg, pM->m_BulkEref/Vt, pM->m_BulkEgamma);
//	pRegData->m_HurkxPrefactor[reg] = pM->m_HurkxPrefactor;
//	pRegData->m_HurkxFgamma_Vt[reg] = pM->m_HurkxFgamma*sqrt(T*T*T/27e6)/Vt;	// change from ref val at 300K to real val & normalise.
//	pRegData->m_HurkxEnable[reg] = pM->m_HurkxEnable;
	double gamma = pM->m_HurkxFgamma*sqrt(T*T*T/27e6)/Vt;	// change from ref val at 300K to real val & normalise.
	pRegData->SetHurkxParametersForRegion(reg, pM->IsHurkxEnabled(), pM->m_HurkxPrefactor, gamma);

	/***
	if (pN->GetNd(first)>pN->GetNa(first))
	{
		pS->m_SnFront[reg] = CPhysics::S(pR->m_FrontSn, 
							pN->GetNd(first)*nir,
							pM->m_SurfNrefN, pM->m_SurfNalphaN, 
							T, pM->m_SurfTalpha);
		pS->m_SpFront[reg] = CPhysics::S(pR->m_FrontSp,
							pN->GetNd(first)*nir,
							pM->m_SurfNrefN, pM->m_SurfNalphaN, 
							T, pM->m_SurfTalpha);
	} else {
		pS->m_SnFront[reg] = CPhysics::S(pR->m_FrontSn, 
							pN->GetNa(first)*nir,
							pM->m_SurfNrefP, pM->m_SurfNalphaP, 
							T, pM->m_SurfTalpha);
		pS->m_SpFront[reg] = CPhysics::S(pR->m_FrontSp,
							pN->GetNa(first)*nir,
							pM->m_SurfNrefP, pM->m_SurfNalphaP, 
							T, pM->m_SurfTalpha);
	}
	pS->m_EtFront[reg] = pR->m_FrontEt/Vt;
	pS->m_JoFront[reg] = pR->m_FrontJo;

	if (pN->GetNd(last)>pN->GetNa(last))
	{
		pS->m_SnRear[reg] = CPhysics::S(pR->m_RearSn,
							pN->GetNd(last)*nir,
							pM->m_SurfNrefN, pM->m_SurfNalphaN, 
							T, pM->m_SurfTalpha);
		pS->m_SpRear[reg] = CPhysics::S(pR->m_RearSp,
							pN->GetNd(last)*nir,
							pM->m_SurfNrefN, pM->m_SurfNalphaN, 
							T, pM->m_SurfTalpha);
	} else {
		pS->m_SnRear[reg] = CPhysics::S(pR->m_RearSn,
							pN->GetNa(last)*nir,
							pM->m_SurfNrefP, pM->m_SurfNalphaP, 
							T, pM->m_SurfTalpha);
		pS->m_SpRear[reg] = CPhysics::S(pR->m_RearSp,
							pN->GetNa(last)*nir,
							pM->m_SurfNrefP, pM->m_SurfNalphaP, 
							T, pM->m_SurfTalpha);
	}
	pS->m_EtRear[reg] = pR->m_RearEt/Vt;
	pS->m_JoRear[reg] = pR->m_RearJo;
	pS->m_Nc[reg] = CPhysics::Nc(ni, pM->m_NcNv, pM->m_BandGap, T)/nir;
	pS->m_Nv[reg] = CPhysics::Nv(ni, pM->m_NcNv, pM->m_BandGap, T)/nir;
	pS->m_Aff[reg] = pM->m_Affinity/Vt;
**/
	CSurfaceRecomData rec;
	if (pN->GetNd(first)>pN->GetNa(first))
	{
		rec.m_Sn = CPhysics::S(pR->m_FrontSn, 
							pN->GetNd(first)*nir,
							pM->m_SurfNrefN, pM->m_SurfNalphaN, 
							T, pM->m_SurfTalpha);
		rec.m_Sp = CPhysics::S(pR->m_FrontSp,
							pN->GetNd(first)*nir,
							pM->m_SurfNrefN, pM->m_SurfNalphaN, 
							T, pM->m_SurfTalpha);
	} else {
		rec.m_Sn = CPhysics::S(pR->m_FrontSn, 
							pN->GetNa(first)*nir,
							pM->m_SurfNrefP, pM->m_SurfNalphaP, 
							T, pM->m_SurfTalpha);
		rec.m_Sp = CPhysics::S(pR->m_FrontSp,
							pN->GetNa(first)*nir,
							pM->m_SurfNrefP, pM->m_SurfNalphaP, 
							T, pM->m_SurfTalpha);
	}
	rec.m_Et = pR->m_FrontEt/Vt;
	rec.m_Jo = pR->m_FrontJo;
	pRegData->SetFrontRecombinationParameters(reg, rec);

	if (pN->GetNd(last)>pN->GetNa(last))
	{
		rec.m_Sn = CPhysics::S(pR->m_RearSn,
							pN->GetNd(last)*nir,
							pM->m_SurfNrefN, pM->m_SurfNalphaN, 
							T, pM->m_SurfTalpha);
		rec.m_Sp = CPhysics::S(pR->m_RearSp,
							pN->GetNd(last)*nir,
							pM->m_SurfNrefN, pM->m_SurfNalphaN, 
							T, pM->m_SurfTalpha);
	} else {
		rec.m_Sn = CPhysics::S(pR->m_RearSn,
							pN->GetNa(last)*nir,
							pM->m_SurfNrefP, pM->m_SurfNalphaP, 
							T, pM->m_SurfTalpha);
		rec.m_Sp = CPhysics::S(pR->m_RearSp,
							pN->GetNa(last)*nir,
							pM->m_SurfNrefP, pM->m_SurfNalphaP, 
							T, pM->m_SurfTalpha);
	}
	rec.m_Et = pR->m_RearEt/Vt;
	rec.m_Jo = pR->m_RearJo;
	pRegData->SetRearRecombinationParameters(reg, rec);

	double nc = CPhysics::Nc(ni, pM->m_NcNv, pM->m_BandGap, T)/nir;
	double nv = CPhysics::Nv(ni, pM->m_NcNv, pM->m_BandGap, T)/nir;
	double aff = pM->m_Affinity/Vt;

	pS->GetRegionData()->SetBandEdgesAndAffinityForRegion(reg, nc, nv, aff);
}

void CProblem::SetLumpedParameters()
{
//	CLumped* pL = pD->m_Lumped;
	for (int k=0; k<MAX_LUMPED; k++)
	{
		CLumped* pL = pD->GetLumpedElement(k);
		double val;
		switch (pL->m_Type)
		{
		case CONDUCTOR:	{val = pL->m_Value *Vt/(Q*nir*Aref); break;}
		case DIODE:		{val = pL->m_Value /(Q*nir*Aref); break;}
		case CAPACITOR:	{val = pL->m_Value *Vt/(Q*nir*Aref); break;}
		}
		pS->SetLumpedElement(k, pL->m_Type, pL->m_Enable, val, pL->m_n);
/***
		pS->m_tLumped[k] = pL->m_Type;
		
		switch (pL->m_Type)
		{
		case CONDUCTOR:	{pS->m_vLumped[k] = pL->m_Value *Vt/(Q*nir*Aref); break;}
		case DIODE:		{pS->m_vLumped[k] = pL->m_Value /(Q*nir*Aref); break;}
		case CAPACITOR:	{pS->m_vLumped[k] = pL->m_Value *Vt/(Q*nir*Aref); break;}
		}
		pS->m_bLumped[k] = pL->m_Enable;
		pS->m_nLumped[k] = pL->m_n;
***/
	}
}

void CProblem::SetCircuitNodes()
{
	// contacts between nodes are assigned to the node to the right
	int k;
	CNode* pN = GetNode();
	if (pS->m_bSource[EMITTER]) {	
		k=0; while (k<pN->GetLastNode() && pN->GetX(k)<pD->m_EmitterX) k++;
		pS->SetSourceLocation(EMITTER, pN->GetX(k));
	}
	if (pS->m_bSource[BASE]){	
		k=0; while (k<pN->GetLastNode() && pN->GetX(k)<pD->m_BaseX) k++;
		pS->SetSourceLocation(BASE, pN->GetX(k));
	}	
	if (pS->m_bSource[COLLECTOR]) {
		k=0; while (k<pN->GetLastNode() && pN->GetX(k)<pD->m_CollectorX) k++;
		pS->SetSourceLocation(COLLECTOR, pN->GetX(k));
	}
	for (int elem=0; elem<MAX_LUMPED; elem++){
		CLumped *pL = pD->GetLumpedElement(elem);
		if (pL->m_Enable) {
			pS->SetLumpedElementPosition(elem, pL->m_Xa, pL->m_Xc);
		}
	}
/***
	for (int elem=0; elem<MAX_LUMPED; elem++){
		if (pS->m_bLumped[elem]){
			CLumped *pL = pD->GetLumpedElement(elem);
			k=0; while (k<pN->GetLastNode() && pN->GetX(k)<pL->m_Xa) k++;
			pS->m_xLumpedA[elem] = pN->GetX(k);
			k=0; while (k<pN->GetLastNode() && pN->GetX(k)<pL->m_Xc) k++;
			pS->m_xLumpedC[elem] = pN->GetX(k);
		}
	}
***/
}
