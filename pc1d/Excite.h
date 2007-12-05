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
#include "circdlg.h"  // for CCircuit::transferdatatodlg
#ifndef PLOTLOAD_HEADER
#include "plotload.h"	// for CGraph
#endif
#define EXCITATION_EXT "exc"

/////////////////////////////////////////////////////////////////////////////
// CLight

class CLight : public CObject
{
	DECLARE_SERIAL(CLight)
	
// Attributes
public:
// Serialized
	BOOL	m_On, m_IntensityExternal, m_Back;
	BOOL	m_SpectrumExternal, m_SpectrumBlack, m_SpectrumMono;
	int		m_BlackNum;
	double	m_LambdaSS, m_LambdaTR1, m_LambdaTR2;			// nm
	double	m_IntensitySS, m_IntensityTR1, m_IntensityTR2;	// W/cm2
	double	m_BlackTemperature;			// K
	double	m_BlackMin, m_BlackMax;		// nm
	CString	m_SpectrumFile, m_IntensityFile;
// From external files
	int		m_nSpectrum;
	double	m_SpcLambda[MAX_WAVELENGTHS];	// nm, for this source
	double	m_Spectrum[MAX_WAVELENGTHS];	// W/cm2 at each wavelength, for this source
	int		m_nSource;
	double	m_SrcTime[MAX_TIME_STEPS];		// sec
	double	m_SrcIntensity[MAX_TIME_STEPS];	// W/cm2
// Calculated results
	double	m_Lambda[MAX_TIME_STEPS+2];		// nm
	double	m_Intensity[MAX_TIME_STEPS+2];	// W/cm2
	double	m_Reflectance[MAX_TIME_STEPS+2];	// unitless, incident surface for this source
	double	m_Escape[MAX_TIME_STEPS+2];		// unitless, out incident surface from this source
// Operations
public:
	BOOL DoLightPropertySheet(CString prisec, int toppage=0);
	
public:
	CLight();
	void Initialize();
	virtual void Serialize(CArchive& ar);
};

///////////////////////////////////////////////////////////////////////////
// CCircuit

class CCircuit : public CObject
{
	DECLARE_SERIAL(CCircuit)
	
// Attributes
public:
// Serialized
	CString	m_File;
	BOOL	m_External;
	BOOL	m_ConnectSS, m_ConnectTR;
	int		m_OhmsSS, m_OhmsTR;			// 0(FALSE)=ohmcm2, 1(TRUE)=ohms
	double	m_RSS, m_RTR;				// ohms or ohm-cm2
	double	m_VSS, m_VTR1, m_VTR2;		// volts
// From external file
	int		m_nSource;
	double	m_SourceTime[MAX_TIME_STEPS];	// sec
	double	m_SourceVolts[MAX_TIME_STEPS];	// volts
	double	m_Resistance[MAX_TIME_STEPS];	// ohms
// Calculated results
	double	m_Volts[MAX_TIME_STEPS+2];	// volts
	double	m_Amps[MAX_TIME_STEPS+2];	// amps
// Operations
public:	
	void TransferDataToDlg(CCircDlg &dlg, CString OpenDlgTitle);
	void TransferDataFromDlg(CCircDlg &dlg);

public:
	CCircuit();
	void Initialize();
	virtual void Serialize(CArchive& ar);
};

/////////////////////////////////////////////////////////////////////////////
// CExcite

class CExcite : public CObject
{
	DECLARE_SERIAL(CExcite)

// Attributes
private:
	int m_FileVersion;	// version to save as
public:
// Serialized
	CCircuit m_Base, m_Coll;
	CString	m_Filename;
	CMode	m_Mode;
	int		m_TranNum;
	double	m_TranStep, m_FirstStep;	// sec
	BOOL	m_LightExternal;
	CString	m_LightExternalFile;
	CLight	m_LightPri, m_LightSec;
	double	m_Temp;	// kelvin
	CTempUnit m_Tunit;		// affects dialog, storage always in K

	// These are the 'file' values, used only for load & store with 
	// the excitation file. The true values are in actview.h & 4graphvw.h
	CGraph m_InteractiveGraph;
	CGraph m_nostategraphs[4];
	CGraph m_eqstategraphs[4];
	CGraph m_ssstategraphs[4];

// From external file
	int		m_nPhoto;
	double	m_Position[MAX_ELEMENTS];	// cm
	double	m_Photogen[MAX_ELEMENTS];	// cummulative from x=0, carriers per sec
public:
	BOOL	m_bModified;

// Operations
public:
	void SetModifiedFlag() {m_bModified = TRUE;}

// Implementation
public:
	void SetFileVersion(int PC1DVersion);
	int  GetFileVersion();
	void Initialize();
	BOOL OnExcitationOpen();
	void OnExcitationSaveas();
	BOOL OnMode();
	BOOL SetDeviceTemperature();
	BOOL SetFilename(CString path="");
	BOOL DoCircuitDlg(int toppage=0);

public:
	CExcite();
	virtual ~CExcite();
	virtual void Serialize(CArchive& ar);

};

