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

#include "ProgramLimits.h"

class CRegion;		// forward reference declaration needed by CDiffusion

/////////////////////////////////////////////////////////////////////////////
// CMobility (Two per Material)

class CMobility : public CObject
{
	DECLARE_SERIAL(CMobility)
// Attributes	
public:
	double	Fixed, Max, MajMin, MajNref, MajAlpha, MinMin, MinNref, MinAlpha;
	double	B1, B2, B3, B4, Vmax;
// Operations
public:
	CMobility();
	void	Copy(CMobility* pMob);		// CObject blocks use of = operator
	void	Serialize(CArchive& ar);
};	

/////////////////////////////////////////////////////////////////////////////
// CDiffusion (Four per Region)

class CDiffusion : public CObject
{
	DECLARE_SERIAL(CDiffusion)
// Attributes	
private:
	CRegion* m_pRegion;	// pointer to parent region object, set by Initialize
	BOOL	m_Enable;
public:
	BOOL IsEnabled() { return m_Enable; };
	void SetEnabled(BOOL bOn) { m_Enable=bOn; };
	CMatType m_Type;
	double	m_Npeak;	// cm-3
	double	m_Depth;	// cm
	double	m_Xpeak;	// cm
	CProfile m_Profile;
// Operations
public:
	BOOL SetDiffusion(CString title="");
	void Initialize(CRegion* pR);
public:
	CDiffusion();
	void	Serialize(CArchive& ar);
//  Note that diffusions are not copied from one region to another
};	

//////////////////////////////////////////////////////////////////////////
// CMaterial (One per Region)

class CMaterial : public CObject
{
	DECLARE_SERIAL(CMaterial)

// Attributes
public:
	WORD m_FileVersion;	// what version is it saved as on disk?
// Serialized
	double	m_Permittivity;	// relative to free space
	double	m_BandGap;		// eV
	double	m_Affinity;		// eV
	double	m_NcNv;			// ratio
	double	m_ni200, m_ni300, m_ni400;	// cm-3
	double	m_BGNnNref, m_BGNpNref;	// cm-3
	double	m_BGNnSlope, m_BGNpSlope;	// eV
	double	m_Cn, m_Cp, m_Cnp;	// cm6/s
	double	m_BB;				// cm3/s
	double	m_BulkNrefN, m_BulkNrefP;	// cm-3
	double	m_BulkNalphaN, m_BulkNalphaP;	// dimensionless
	double	m_BulkTalpha;		// dimensionless, relative to 300 K
	double	m_BulkEref;			// V/cm		!! not used in version 4.0
	double	m_BulkEgamma;		// dimensionless	!! not used in version 4.0
	double	m_SurfNrefN, m_SurfNrefP;	// cm-3
	double	m_SurfNalphaN, m_SurfNalphaP;	// dimensionless
	double	m_SurfTalpha;		// dimensionless, relative to 300 K
	BOOL	m_HurkxEnable;
	double	m_HurkxFgamma;		// V/cm, relative to 300 K
	double	m_HurkxPrefactor;	// dimensionless
	double	m_FixedIndex;		// free space = 1
	double	m_AbsEd1, m_AbsEd2, m_AbsEi1, m_AbsEi2, m_AbsEp1, m_AbsEp2;	// eV
	double	m_AbsAd1, m_AbsAd2, m_AbsA11, m_AbsA12, m_AbsA21, m_AbsA22;	// cm-1
	double	m_AbsTcoeff;	// eV/K
	double	m_AbsToffset;	// K
	BOOL	m_FixedMobility, m_IndexExternal, m_AbsExternal;
	CString	m_Filename;		// for the material file
	CString m_IndexFilename;	// for the refractive index file
	CString m_AbsFilename;	// for the absorption coefficient file
	CMobility m_Elec, m_Hole;		// defined in physics.h
	BOOL 	m_FreeCarrEnable;
	double	m_FreeCarrCoeffN, m_FreeCarrCoeffP; // cm/s for lambda in nm
	double  m_FreeCarrPowerN, m_FreeCarrPowerP; 
public:
	bool IsHurkxEnabled() const { return m_HurkxEnable!=0; };
// From external files
private:
	int		m_nAbsorb;
	double	m_AbsLambda[MAX_WAVELENGTHS];	// nm
	double	m_Absorption[MAX_WAVELENGTHS];	// cm-1
	int		m_nIndex;
	double	m_IdxLambda[MAX_WAVELENGTHS];	// nm
	double	m_Index[MAX_WAVELENGTHS];		// unitless
// Modified-from-file flag
	BOOL	m_bModified;

// Operations
public:
	void	SetModifiedFlag() {m_bModified = TRUE;}
	BOOL	IsModified() const {return m_bModified;}
	void	Copy(CMaterial* pM);		// CObject blocks use of = operator

// Implementation
public:
	void Initialize();
	void SetFileVersion(int PC1DVersion);
	int GetFileVersion(void);
	BOOL SetMobilityModel();
	BOOL SetMobilityFixed();
	BOOL SetPermittivity();
	BOOL SetBandStructure();
	BOOL SetBandgapNarrowing();
	BOOL DoOpticalDlg(int toppage=0);
	BOOL DoRecombinationDlg();
	BOOL SetBkgndDoping();
	BOOL OnMaterialOpen();
	void OnMaterialSaveas();
	BOOL SetAbsFile(CString path);
	double GetAbsorptionAtWavelength(double lambda, double T) const;
	double GetRefractiveIndexAtWavelength(double lambda) const;
public:
	CMaterial();
	virtual ~CMaterial();
	virtual void Serialize(CArchive& ar);

};

/////////////////////////////////////////////////////////
// CRegion (Up to Five per Device)

class CRegion : public CObject
{
	DECLARE_SERIAL(CRegion)

// Attributes
private:
// Serialized
	CMaterial m_Mat;
public:
	double	m_Thickness;	// cm
	double	m_BkgndDop;		// cm-3
	CMatType m_BkgndType;
	BOOL	m_FrontExternal, m_RearExternal;
	CString	m_FrontFilename, m_RearFilename;	// for the doping files
	CDiffusion m_FrontDiff1;
	CDiffusion m_FrontDiff2;
	CDiffusion m_RearDiff1;
	CDiffusion m_RearDiff2;
	double	m_TauN, m_TauP;	// s
	double	m_BulkEt;		// eV (relative to Ei)
	BOOL	m_FrontJo;		// beyond LLI, use Jo model
	double	m_FrontSn, m_FrontSp;	// cm/s
	double	m_FrontEt;		// eV (relative to Ei)
	BOOL	m_RearJo;		// beyond LLI, use Jo model
	double	m_RearSn, m_RearSp;	// cm/s
	double	m_RearEt;		// eV (relative to Ei)
// From external files
	int		m_nFront;
	double	m_FrontPosition[MAX_ELEMENTS];
	double	m_FrontDopingDonor[MAX_ELEMENTS];
	double	m_FrontDopingAcceptor[MAX_ELEMENTS];
	int		m_nRear;
	double	m_RearPosition[MAX_ELEMENTS];
	double	m_RearDopingDonor[MAX_ELEMENTS];
	double	m_RearDopingAcceptor[MAX_ELEMENTS];
// Operations
public:
	void	Copy(CRegion* pR);	// Copies contents of pR into this region
// Implementation
public:
	void Initialize();
	BOOL SetRegionThickness();
	BOOL SetBackgroundDoping();
	BOOL DoRecombinationDlg(int regionnum, int toppage=0);
	BOOL SetFrontDopingFile(CString path);
	BOOL SetRearDopingFile(CString path);
	CMaterial *GetMaterial() { return &m_Mat; };
public:
	CRegion();		// Constructor
	virtual void Serialize(CArchive& ar);
};

/////////////////////////////////////////////////////////////////////////////
// CReflectance (Two per Device)

class CReflectDlg;

class CReflectance : public CObject
{
	DECLARE_SERIAL(CReflectance)
// Attributes	
public:
// Serialized
	BOOL	m_bFixed, m_bCoated, m_bExternal;
	double	m_Fixed, m_Broadband;	// normalized (not %)
	double	m_Thick[MAX_LAYERS];	// nm, layer next to substrate first
	double	m_Index[MAX_LAYERS];	// unitless, layer next to substrate first
	CString	m_Filename;
	double	m_Internal1, m_Internal2;	// normalized (not %)
	CTexture m_Rough;
// From external file
	int		m_nLambda;
	double	m_Lambda[MAX_WAVELENGTHS];		// nm
	double	m_Reflectance[MAX_WAVELENGTHS];	// unitless
// Operations
public:
	void TransferDataToDlg(CReflectDlg &dlg, CString OpenDlgTitle);
	void TransferDataFromDlg(CReflectDlg &dlg);
	void Initialize();
public:
	CReflectance();
	void	Serialize(CArchive& ar);
};	
                                                    
////////////////////////////////////////////////////////////////////////////
// CLumped (Four per Device)

class CLumped : public CObject
{
	DECLARE_SERIAL(CLumped)
// Attributes	
public:
	BOOL	m_Enable;
	CElemType m_Type;	// Conductor, Diode, or Capacitor
	double	m_Xa, m_Xc;	// anode, cathode distance from front, cm
	double	m_n;		// ideality factor (Diode only)
	double	m_Value;	// Conductor: siemens, Diode: amps, Capacitor: farads
// Operations
public:
	void Initialize();
public:
	CLumped();
	void	Serialize(CArchive& ar);
// Implementation
public:
// Calculated Results
	double	m_Volts[MAX_TIME_STEPS+2];	// volts across lumped element
	double	m_Amps[MAX_TIME_STEPS+2];		// amps through lumped element
};	

/////////////////////////////////////////////////////////////////////////////
// CDevice (One per Document)

class CDevice : public CObject
{   
	DECLARE_SERIAL(CDevice)

// Attributes
public:
	WORD m_FileVersion;	// what version is it saved as on disk?

	CString	m_Filename;			// Filename used to load device (if any)
	int		m_NumRegions;		// Interface-limited 1..5
	double	m_Area;				// cm2
	CAreaUnit m_Aunit;			// For display of area. Storage always cm2.
	BOOL	m_FrontTexture;
	double	m_FrontAngle;		// degrees
	double	m_FrontDepth;		// cm
	BOOL	m_RearTexture;
	double	m_RearAngle;		// degrees
	double	m_RearDepth;		// cm
	CSurface m_FrontSurface, m_RearSurface;
	double	m_FrontBarrier, m_RearBarrier;	// eV (Positive bends bands upward)
	double	m_FrontCharge, m_RearCharge;	// cm-2
	CReflectance m_FrontRfl, m_RearRfl;
	BOOL	m_EnableEmitter, m_EnableBase, m_EnableCollector, m_EnableInternal;
	double	m_EmitterR, m_BaseR, m_CollectorR;	// ohms series resistance
	double	m_EmitterX, m_BaseX, m_CollectorX;	// cm

private:
	CRegion* pR;			// The current region
	CObList* pList;			// Linked list of regions
	int		m_CurrentRegion;	// Interface-limited 0..4
	CLumped m_Lumped[MAX_LUMPED];
public:
	BOOL 	m_bModified;

// Operations
public:
	void SetFileVersion(int PC1DVersion);
	int GetFileVersion(void);

	void	Initialize();
	void	SetModifiedFlag() {m_bModified = TRUE;}
	void	InsertRegion(int region_num);	// 0-4
	void	RemoveRegion(int region_num);	// 0-4
	void	SetCurrentRegionNumber(int region_num);	// 0-4
	CRegion *GetCurrentRegion() { return pR; };
	CMaterial *GetCurrentMaterial() { return GetCurrentRegion()->GetMaterial(); };
	CMaterial *GetMaterialForRegion(int region_num) { 
		return GetRegion(region_num)->GetMaterial(); };
	CRegion *GetRegion(int region_num) { 
			POSITION pos = pList->FindIndex(region_num);	// Index is zero-based
			return (CRegion*)pList->GetAt(pos);
	};
	int GetCurrentRegionNumber() { return m_CurrentRegion; };
	CLumped *GetLumpedElement(int lump_num) { return &m_Lumped[lump_num]; };

	double GetThickness(); // total thickness of device
// Implementation
public:
	BOOL	OnDeviceOpen();
	void	OnDeviceSaveas();
	BOOL	DoAreaDlg();
	BOOL	DoTextureDlg();
	BOOL 	DoSurfaceDlg(int toppage=0);
	BOOL	DoReflectanceDlg(int toppage=0);
	BOOL	DoContactsDlg();
	BOOL	DoInternalElementsDlg();

public:
	CDevice();
	virtual ~CDevice();
	virtual void Serialize(CArchive& ar);
};

/////////////////////////////////////////////////////////////////////////////
