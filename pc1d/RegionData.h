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

/// The material-dependent values used in surface recombination calculations
class CSurfaceRecomData
{
public:
	double	m_Sn;	// cm/s
	double	m_Sp;	// cm/s
	double	m_Et;	// normalized by Vt
	BOOL	m_Jo;
public:
	void Initialize() { m_Sn = m_Sp = 0; 	m_Et= 0; m_Jo = FALSE; };
	void Serialize(CArchive& ar);
};

/// The values which are constant within a region
class CRegionData {
private:
	double  m_HurkxFgamma_Vt[MAX_REGIONS];		// the value at the temp T (not 300K). Normalised by Vt
	double	m_HurkxPrefactor[MAX_REGIONS];
	bool	m_HurkxEnable[MAX_REGIONS];

	double	m_xRegionFront[MAX_REGIONS];	// node locations at region fronts
	double	m_xRegionRear[MAX_REGIONS];		// nodes locations at regions rears

	double	m_Nc[MAX_REGIONS];		// normalized by nir, plot conduction-band edge
	double	m_Nv[MAX_REGIONS];		// normalized by nir, plot valence-band edge
	double	m_Aff[MAX_REGIONS];		// normalized by Vt, electron affinity

	double	m_Cn[MAX_REGIONS], m_Cp[MAX_REGIONS], m_Cnp[MAX_REGIONS];	// sec-1, norm by nir-2
	double	m_B[MAX_REGIONS];		// sec-1, normalized by nir-1
	double	m_Et[MAX_REGIONS];		// normalized by Vt

	double	m_Eref[MAX_REGIONS];	// normalized by Vt
	double 	m_Egamma[MAX_REGIONS];	// dimensionless

	CSurfaceRecomData m_FrontRec[MAX_REGIONS];
	CSurfaceRecomData m_RearRec[MAX_REGIONS];

public:
	void Initialize(double endx);
	void Serialize(CArchive& ar);

	double GetRegionFront(int reg) const { return m_xRegionFront[reg]; };
	double GetRegionRear(int reg) const { return m_xRegionRear[reg]; };
	void SetRegionLocation(int reg, double startx, double endx) { m_xRegionFront[reg]=startx; m_xRegionRear[reg]=endx; };
	double GetNc_nir(int reg) const { return m_Nc[reg]; }; ///< normalized by nir, plot conduction-band edge
	double GetNv_nir(int reg) const { return m_Nv[reg]; }; ///< normalized by nir, plot valence-band edge
	double GetAff_Vt(int reg) const { return m_Aff[reg]; }; ///< normalized by Vt, electron affinity
	void SetBandEdgesAndAffinityForRegion(int reg, double Nc, double Nv, double Aff) { m_Nc[reg]=Nc; m_Nv[reg]=Nv; m_Aff[reg]=Aff;};
	void SetErefEgammaForRegion(int reg, double eref, double egamma) { m_Eref[reg]=eref; m_Egamma[reg]=egamma; };
	void SetHurkxParametersForRegion(int reg, bool enable, double prefactor, double gamma) { m_HurkxEnable[reg]=enable; m_HurkxPrefactor[reg]=prefactor; m_HurkxFgamma_Vt[reg]=gamma; };
	bool IsHurkxEnabledForRegion(int reg) const { return m_HurkxEnable[reg]; };
	double GetHurkxPrefactor(int reg) const { return m_HurkxPrefactor[reg]; };
	double GetHurkxFgamma_Vt(int reg) const { return m_HurkxFgamma_Vt[reg]; };
	double GetCn(int reg) { return m_Cn[reg]; };
	double GetCp(int reg) { return m_Cp[reg]; };
	double GetCnp(int reg) { return m_Cnp[reg]; };
	double GetB(int reg) { return m_B[reg]; };
	double GetEt(int reg) { return m_Et[reg]; };
	void SetBulkRecombinationParametersForRegion(int reg, double Cn, double Cp, double Cnp, double B, double Et) {
		m_Cn[reg]=Cn; m_Cp[reg]=Cp; m_Cnp[reg]=Cnp; m_B[reg]=B; m_Et[reg]=Et; 
	}
	void SetFrontRecombinationParameters(int reg, const CSurfaceRecomData &s) {
		m_FrontRec[reg]=s;
	}
	void SetRearRecombinationParameters(int reg, const CSurfaceRecomData &s) {
		m_RearRec[reg]=s;
	}
	const CSurfaceRecomData &GetFrontRecom(int reg) {
		return m_FrontRec[reg];
	}
	const CSurfaceRecomData &GetRearRecom(int reg) {
		return m_RearRec[reg];
	}
};
