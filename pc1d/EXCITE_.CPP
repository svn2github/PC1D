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
#include "resource.h"
#include "lightdlg.h"
#include "spectdlg.h"
#include "modedlg.h"
#include "tempdlg.h"
#include "PhysicalConstants.h"
#include "ProgramLimits.h"
#include "excite.h"
#include "ascfile.h"
#include "path.h"
#include "pc1d.h" // to get the current paths (singleton)

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

const CString INTENSITY_EXT="lgt";
const CString SPECTRUM_EXT="spc";
const CString GENERATION_EXT="gen";
const CString VOLTAGE_EXT="vlt";

///////////////////////////////////////////////////////////////////////////
// CLight

IMPLEMENT_SERIAL(CLight, CObject, 0)

CLight::CLight()
{
	Initialize();
}

void CLight::Initialize()
{
	m_On = FALSE; m_IntensityExternal = FALSE; m_Back = FALSE;
	m_SpectrumExternal = FALSE; m_SpectrumBlack = FALSE;
	m_SpectrumMono = TRUE; m_BlackNum = 50;
	m_LambdaSS = 300; m_LambdaTR1 = 300; m_LambdaTR2 = 1200;
	m_IntensitySS = 0.1; m_IntensityTR1 = 0.1; m_IntensityTR2 = 0.1;
	m_SpectrumFile.Empty(); m_IntensityFile.Empty();
	m_BlackTemperature = 2000; m_BlackMin = 0; m_BlackMax = 10000;
	int k;
	m_nSpectrum = 0;
	for (k=0; k<MAX_WAVELENGTHS; k++)
		m_SpcLambda[k]= m_Spectrum[k] = 0;
	m_nSource = 0;
	for (k=0; k<MAX_TIME_STEPS; k++) 
		m_SrcTime[k] = m_SrcIntensity[k] = 0;
	for (k=0; k<MAX_TIME_STEPS+2; k++)
		m_Lambda[k] = m_Intensity[k] = m_Reflectance[k] = m_Escape[k] = 0;
}

void CLight::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << (WORD)m_On << (WORD)m_IntensityExternal << (WORD)m_Back;
		ar << (WORD)m_SpectrumExternal << (WORD)m_SpectrumBlack;
		ar << (WORD)m_SpectrumMono << (WORD)m_BlackNum;
		ar << m_LambdaSS << m_LambdaTR1 << m_LambdaTR2;
		ar << m_IntensitySS << m_IntensityTR1 << m_IntensityTR2;
		ar << CPath::MinimumNecessaryFilename(m_SpectrumFile, ((CPc1dApp *)AfxGetApp())->m_Path.spc);
		ar << CPath::MinimumNecessaryFilename(m_IntensityFile, ((CPc1dApp *)AfxGetApp())->m_Path.lgt);
		ar << m_BlackTemperature << m_BlackMin << m_BlackMax;
	}
	else
	{
		ar >> (WORD&)m_On >> (WORD&)m_IntensityExternal >> (WORD&)m_Back;
		ar >> (WORD&)m_SpectrumExternal >> (WORD&)m_SpectrumBlack;
		ar >> (WORD&)m_SpectrumMono >> (WORD&)m_BlackNum;
		ar >> m_LambdaSS >> m_LambdaTR1 >> m_LambdaTR2;
		ar >> m_IntensitySS >> m_IntensityTR1 >> m_IntensityTR2;
		ar >> m_SpectrumFile >> m_IntensityFile;
		ar >> m_BlackTemperature >> m_BlackMin >> m_BlackMax;
		m_SpectrumFile=  CPath::MinimumNecessaryFilename(m_SpectrumFile,  ((CPc1dApp *)AfxGetApp())->m_Path.spc);
		m_IntensityFile= CPath::MinimumNecessaryFilename(m_IntensityFile, ((CPc1dApp *)AfxGetApp())->m_Path.lgt);
		m_nSource = MAX_TIME_STEPS;
		if (m_IntensityExternal)
			if (!CAscFile::Read(CPath::MakeFullPathname(m_IntensityFile, ((CPc1dApp *)AfxGetApp())->m_Path.lgt),
					 m_nSource, m_SrcTime, m_SrcIntensity))
				AfxMessageBox("Error reading Light Intensity file "+m_IntensityFile);
		m_nSpectrum = MAX_WAVELENGTHS;
		if (m_SpectrumExternal)
			if (!CAscFile::Read(CPath::MakeFullPathname(m_SpectrumFile, ((CPc1dApp *)AfxGetApp())->m_Path.spc),
			 		m_nSpectrum, m_SpcLambda, m_Spectrum))
				AfxMessageBox("Error reading Spectrum file "+m_SpectrumFile);
	}
}

////////////////////////////////////////////////////////////////////////////
// CLight commands

BOOL CLight::DoLightPropertySheet(CString prisec, int toppage)
{
	CString title=prisec+" Illumination";
	CPropertySheet PropSheet(title, NULL, toppage);
	CString specOpenDlgTitle="Open "+prisec+" Spectrum File";
	CString  intOpenDlgTitle="Open "+prisec+" Light Intensity File";
	
	CSpectrumDlg SpectrumPage;
	CLightDlg IntensityPage;
	
	SpectrumPage.m_title = title; 
	IntensityPage.m_title = title;
	
	SpectrumPage.m_Radio = 0;
	if (m_SpectrumBlack) SpectrumPage.m_Radio=1;
	if (m_SpectrumExternal) SpectrumPage.m_Radio=2;
	
	SpectrumPage.m_LambdaSS = m_LambdaSS;
	SpectrumPage.m_LambdaTR1 = m_LambdaTR1;
	SpectrumPage.m_LambdaTR2 = m_LambdaTR2;
	
	SpectrumPage.m_Temperature = m_BlackTemperature;
	SpectrumPage.m_Minimum = m_BlackMin;
	SpectrumPage.m_Maximum = m_BlackMax;
	SpectrumPage.m_Number = m_BlackNum;
	
	SpectrumPage.m_Filename = m_SpectrumFile;
	SpectrumPage.m_Ext = SPECTRUM_EXT;
	SpectrumPage.m_Path = ((CPc1dApp *)AfxGetApp())->m_Path.spc;
	SpectrumPage.m_OpenDlgTitle = specOpenDlgTitle;
	
	IntensityPage.m_Enable = m_On;
	IntensityPage.m_Radio = 0;
	if (m_IntensityExternal) IntensityPage.m_Radio=1;
	
	IntensityPage.m_IntensitySS = m_IntensitySS;
	IntensityPage.m_IntensityTR1 = m_IntensityTR1;
	IntensityPage.m_IntensityTR2 = m_IntensityTR2;
	IntensityPage.m_Back = (int)m_Back;
	
	IntensityPage.m_Filename = m_IntensityFile;
	IntensityPage.m_Ext = INTENSITY_EXT;
	IntensityPage.m_Path = ((CPc1dApp *)AfxGetApp())->m_Path.lgt;
	IntensityPage.m_OpenDlgTitle = intOpenDlgTitle;

	
	PropSheet.AddPage(&IntensityPage);
	PropSheet.AddPage(&SpectrumPage);
	
	if (PropSheet.DoModal()==IDOK) { 
	
		// Save Spectrum settings...
		m_LambdaSS = SpectrumPage.m_LambdaSS;
		m_LambdaTR1= SpectrumPage.m_LambdaTR1;
		m_LambdaTR2= SpectrumPage.m_LambdaTR2;
		
		m_BlackTemperature = SpectrumPage.m_Temperature;
		m_BlackMin = SpectrumPage.m_Minimum;
		m_BlackMax = SpectrumPage.m_Maximum;
		m_BlackNum = SpectrumPage.m_Number;
		
		m_SpectrumFile = SpectrumPage.m_Filename;  
		
		m_SpectrumMono = m_SpectrumBlack = m_SpectrumExternal = FALSE;
		
		switch (SpectrumPage.m_Radio) {
		case 0: m_SpectrumMono = TRUE;
				break; 
		case 1:	m_SpectrumBlack = TRUE;
				break;
		case 2:	m_SpectrumExternal = TRUE;
				if (!m_On) break;
				m_nSpectrum = MAX_WAVELENGTHS;
				if (!CAscFile::Read(m_SpectrumFile, m_nSpectrum, m_SpcLambda, m_Spectrum))
					AfxMessageBox("Error reading Spectrum file "+m_SpectrumFile);
				break;
		}		
		m_SpectrumFile=  CPath::MinimumNecessaryFilename(m_SpectrumFile,  ((CPc1dApp *)AfxGetApp())->m_Path.spc);
		                    
		// Save Intensity settings...
		m_IntensitySS = IntensityPage.m_IntensitySS;
		m_IntensityTR1 = IntensityPage.m_IntensityTR1;
		m_IntensityTR2 = IntensityPage.m_IntensityTR2;
		m_Back = (BOOL)IntensityPage.m_Back;
		
		m_IntensityFile = IntensityPage.m_Filename;
		
		m_On=IntensityPage.m_Enable;
		
		m_IntensityExternal=FALSE;
		if (IntensityPage.m_Radio==1) {
			m_IntensityExternal=TRUE;
			if (m_On) {
			 m_nSource = MAX_TIME_STEPS;
			 if (!CAscFile::Read(CPath::MakeFullPathname(m_IntensityFile, ((CPc1dApp *)AfxGetApp())->m_Path.lgt), m_nSource, m_SrcTime, m_SrcIntensity))
				AfxMessageBox("Error reading Light Intensity file "+m_IntensityFile);
			}
		}
		m_IntensityFile= CPath::MinimumNecessaryFilename(m_IntensityFile, ((CPc1dApp *)AfxGetApp())->m_Path.lgt);

		return TRUE;
	} else return FALSE;
}		


/////////////////////////////////////////////////////////////////////////////
// CCircuit

IMPLEMENT_SERIAL(CCircuit, CObject, 0)

CCircuit::CCircuit()
{
	Initialize();
}

void CCircuit::Initialize()
{
	m_File.Empty();
	m_External = FALSE;
	m_ConnectSS = TRUE; m_ConnectTR = TRUE;
	m_OhmsSS = 1; m_OhmsTR = 1;
	m_RSS = 0; m_RTR = 0;
	m_VSS = 0; m_VTR1 = 0; m_VTR2 = 0;
	m_nSource = 0;
	for (int k=0; k<MAX_TIME_STEPS; k++) 
		m_SourceTime[k] = m_SourceVolts[k] = m_Resistance[k] = 0;
	for (k=0; k<MAX_TIME_STEPS+2; k++)
		m_Volts[k] = m_Amps[k] = 0;
}

void CCircuit::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << CPath::MinimumNecessaryFilename(m_File, ((CPc1dApp *)AfxGetApp())->m_Path.vlt);
		ar << (WORD)m_External;
		ar << (WORD)m_ConnectSS << (WORD)m_OhmsSS << m_RSS << m_VSS;
		ar << (WORD)m_ConnectTR << (WORD)m_OhmsTR << m_RTR << m_VTR1 << m_VTR2;
	}
	else
	{
		ar >> m_File;
		ar >> (WORD&)m_External;
		ar >> (WORD&)m_ConnectSS >> (WORD&)m_OhmsSS >> m_RSS >> m_VSS;
		ar >> (WORD&)m_ConnectTR >> (WORD&)m_OhmsTR >> m_RTR >> m_VTR1 >> m_VTR2;
		m_ConnectSS = TRUE; m_ConnectTR = TRUE;
		m_File=CPath::MinimumNecessaryFilename(m_File, ((CPc1dApp *)AfxGetApp())->m_Path.vlt);
		m_nSource = MAX_TIME_STEPS;
		if (m_External)
			if (!CAscFile::Read(CPath::MakeFullPathname(m_File, ((CPc1dApp *)AfxGetApp())->m_Path.vlt),
					 m_nSource, m_SourceTime, m_SourceVolts, m_Resistance))
				AfxMessageBox("Error reading Voltage file "+m_File);
	}
}

////////////////////////////////////////////////////////////////////////////
// CCircuit commands

void CCircuit::TransferDataToDlg(CCircDlg &dlg, CString OpenDlgTitle)
{
	if (m_External) dlg.m_Radio=1; else dlg.m_Radio=0;

	if (m_OhmsSS) dlg.m_OhmsSS=1; else dlg.m_OhmsSS=0;
	dlg.m_RSS = m_RSS;
	dlg.m_VSS = m_VSS;
	if (m_OhmsTR) dlg.m_OhmsTR=1; else dlg.m_OhmsTR=0;
	dlg.m_RTR = m_RTR;
	dlg.m_VTR1 = m_VTR1;
	dlg.m_VTR2 = m_VTR2;
	
	dlg.m_Filename = m_File;
	dlg.m_Ext = VOLTAGE_EXT;
	dlg.m_Path = ((CPc1dApp *)AfxGetApp())->m_Path.vlt;
	dlg.m_OpenDlgTitle = OpenDlgTitle;
}

void CCircuit::TransferDataFromDlg(CCircDlg &dlg)
{
		m_ConnectSS = TRUE;
		m_ConnectTR = TRUE;
		m_OhmsSS = (BOOL)dlg.m_OhmsSS;
		m_RSS = dlg.m_RSS;
		m_VSS = dlg.m_VSS;
		m_OhmsTR = (BOOL)dlg.m_OhmsTR;
		m_RTR = dlg.m_RTR;
		m_VTR1 = dlg.m_VTR1;
		m_VTR2 = dlg.m_VTR2;
		m_File = dlg.m_Filename;
		if (dlg.m_Radio==0) m_External=FALSE;
		else {
				m_External=TRUE;
				m_nSource = MAX_TIME_STEPS;
				if (!CAscFile::Read(m_File, m_nSource, m_SourceTime, m_SourceVolts, m_Resistance))
					AfxMessageBox("Error reading Voltage file "+m_File);
		}
		m_File=CPath::MinimumNecessaryFilename(m_File, ((CPc1dApp *)AfxGetApp())->m_Path.vlt);
}

/*******************
BOOL CCircuit::SetCircuit(CString title, CString path, CString OpenDlgTitle)
{
	CCircDlg dlg;
	dlg.m_title = title;
	
	if (m_External) dlg.m_Radio=1; else dlg.m_Radio=0;

	if (m_OhmsSS) dlg.m_OhmsSS=1; else dlg.m_OhmsSS=0;
	dlg.m_RSS = m_RSS;
	dlg.m_VSS = m_VSS;
	if (m_OhmsTR) dlg.m_OhmsTR=1; else dlg.m_OhmsTR=0;
	dlg.m_RTR = m_RTR;
	dlg.m_VTR1 = m_VTR1;
	dlg.m_VTR2 = m_VTR2;
	
	dlg.m_Filename = m_File;
	dlg.m_Ext = VOLTAGE_EXT;
	dlg.m_Path = path;
	dlg.m_OpenDlgTitle = OpenDlgTitle;

	if (dlg.DoModal()==IDOK)
	{
		m_ConnectSS = TRUE;
		m_ConnectTR = TRUE;
		m_OhmsSS = (BOOL)dlg.m_OhmsSS;
		m_RSS = dlg.m_RSS;
		m_VSS = dlg.m_VSS;
		m_OhmsTR = (BOOL)dlg.m_OhmsTR;
		m_RTR = dlg.m_RTR;
		m_VTR1 = dlg.m_VTR1;
		m_VTR2 = dlg.m_VTR2;
		m_File = dlg.m_Filename;
		if (dlg.m_Radio==0) m_External=FALSE;
		else {
				m_External=TRUE;
				m_nSource = MAX_TIME_STEPS;
				if (!CAscFile::Read(m_File, m_nSource, m_SourceTime, m_SourceVolts, m_Resistance))
					AfxMessageBox("Error reading Voltage file "+m_File);
		}
		m_File=CPath::MinimumNecessaryFilename(m_File, ((CPc1dApp *)AfxGetApp())->m_Path.vlt);
		return TRUE;
	}
	else return FALSE;
}
****************/
/////////////////////////////////////////////////////////////////////////////
// CExcite

IMPLEMENT_SERIAL(CExcite, CObject, 0 /* schema number*/ )

CExcite::CExcite()
{
	Initialize();
}

CExcite::~CExcite()
{
}

void CExcite::SetFileVersion(int PC1DVersion)
{
	if (PC1DVersion<50) m_FileVersion=1; // PC1D 4.6 was ver=1.
	else m_FileVersion=2;
}

// returns the lowest possible PC1D version compatible with this file
int CExcite::GetFileVersion()
{
	if (m_FileVersion==0) return 40;
	if (m_FileVersion==1) return 41;
	return 50;
}


void CExcite::Initialize()
{
	SetFileVersion(PC1DVERSION);
	m_LightPri.Initialize();
	m_LightSec.Initialize();
	m_Base.Initialize();
	m_Coll.Initialize();
	m_Filename.Empty(); m_bModified = FALSE;
	m_Mode = SS_MODE;
	m_TranNum = 1; m_TranStep = 1; m_FirstStep = 1e-9;
	m_LightExternal = FALSE; m_LightExternalFile.Empty();
	m_Temp = 300; m_Tunit = KELVIN;
	m_nPhoto = 0;
	for (int k=0; k<MAX_ELEMENTS; k++) m_Position[k] = m_Photogen[k] = 0;
}

/////////////////////////////////////////////////////////////////////////////
// CExcite serialization

void CExcite::Serialize(CArchive& ar)
{
	int i;
	CObject::Serialize(ar);
	m_LightPri.Serialize(ar);
	m_LightSec.Serialize(ar);
	m_Base.Serialize(ar);
	m_Coll.Serialize(ar);
	if (ar.IsStoring())
	{
		ar << (WORD)m_FileVersion;		
		ar << CPath::MinimumNecessaryFilename(m_Filename, ((CPc1dApp *)AfxGetApp())->m_Path.exc);		
		ar << (WORD)m_bModified << (WORD)m_Mode;
		ar << (WORD)m_TranNum << m_TranStep << m_FirstStep;
		ar << (WORD)m_LightExternal;
		ar << CPath::MinimumNecessaryFilename(m_LightExternalFile, ((CPc1dApp *)AfxGetApp())->m_Path.exc);		 		
		ar << m_Temp << (WORD)m_Tunit;
		if (m_FileVersion>1) {
			m_InteractiveGraph.Serialize(ar);
			for (i=0; i<4; i++) { m_nostategraphs[i].Serialize(ar); }
			for (i=0; i<4; i++) { m_eqstategraphs[i].Serialize(ar); }
			for (i=0; i<4; i++) { m_ssstategraphs[i].Serialize(ar); }
		}
	}
	else
	{
		ar >> (WORD &)m_FileVersion;
		if (m_FileVersion > 2) 
		{
			AfxThrowArchiveException(CArchiveException::badIndex);
			return;
		}
		ar >> m_Filename;
		ar >> (WORD&)m_bModified >> (WORD&)m_Mode;
		ar >> (WORD&)m_TranNum >> m_TranStep >> m_FirstStep;
		ar >> (WORD&)m_LightExternal >> m_LightExternalFile;
		ar >> m_Temp >> (WORD&)m_Tunit;
		if (m_FileVersion>1) {
			m_InteractiveGraph.Serialize(ar);
			for (i=0; i<4; i++) { m_nostategraphs[i].Serialize(ar); }
			for (i=0; i<4; i++) { m_eqstategraphs[i].Serialize(ar); }
			for (i=0; i<4; i++) { m_ssstategraphs[i].Serialize(ar); }
		}
		m_Filename=CPath::MinimumNecessaryFilename(m_Filename, ((CPc1dApp *)AfxGetApp())->m_Path.exc);
		m_LightExternalFile=CPath::MinimumNecessaryFilename(m_Filename, ((CPc1dApp *)AfxGetApp())->m_Path.exc);
		m_nPhoto = MAX_ELEMENTS;
		if (m_LightExternal) 
		{
			if (!CAscFile::Read(CPath::MakeFullPathname(m_LightExternalFile, ((CPc1dApp *)AfxGetApp())->m_Path.exc),
					 m_nPhoto, m_Position, m_Photogen))
				AfxMessageBox("Error reading Photogeneration file "+m_LightExternalFile);
			for (int k=0; k<m_nPhoto; k++) m_Position[k] *= 1E-4;	// um to cm
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CExcite commands

BOOL CExcite::OnExcitationOpen()
{
	BOOL FileOpen = TRUE;
	CString Ext = EXCITATION_EXT;
	CString Filter = "Excitation (*.exc)|*.exc|All Files (*.*)|*.*||";
	CFileDialog dlg(FileOpen, Ext, m_Filename, OFN_OVERWRITEPROMPT, Filter);
	dlg.m_ofn.lpstrTitle = "Open Excitation File";
	dlg.m_ofn.lpstrInitialDir = ((CPc1dApp *)AfxGetApp())->m_Path.exc;
	if (dlg.DoModal()==IDOK)
	{
		CFile f;
		if (!f.Open(dlg.GetPathName(), CFile::modeRead))
		{
			AfxMessageBox("Unable to open excitation file");
			return FALSE;
		}
		else 
		{
			CArchive ar(&f, CArchive::load); 
			Serialize(ar);
			m_Filename = dlg.GetPathName();
			m_Filename.MakeLower();
			m_Filename=CPath::MinimumNecessaryFilename(m_Filename, ((CPc1dApp *)AfxGetApp())->m_Path.exc);
			m_bModified = FALSE;
			return TRUE;
		}
	}
	else return FALSE;
}

void CExcite::OnExcitationSaveas()
{
	BOOL FileOpen = FALSE;
	CString Ext = EXCITATION_EXT;
	CString Filter = "Excitation (*.exc)|*.exc|PC1D 4.5 Excitation (*.exc)|*.exc|All Files (*.*)|*.*||";
	CFileDialog dlg(FileOpen, Ext, m_Filename, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, Filter);
	dlg.m_ofn.lpstrTitle = "Save Excitation File";
	dlg.m_ofn.lpstrInitialDir = ((CPc1dApp *)AfxGetApp())->m_Path.exc;
	if (GetFileVersion()<50) dlg.m_ofn.nFilterIndex=2;
	else dlg.m_ofn.nFilterIndex=1;
	if (dlg.DoModal()==IDOK)
	{
		CFile f;
		if (!f.Open(dlg.GetPathName(), CFile::modeCreate | CFile::modeWrite))
		{
			AfxMessageBox("Unable to create excitation file");
		}
		else 
		{			
			if (dlg.m_ofn.nFilterIndex==2) {
				SetFileVersion(45);
			} else {
				SetFileVersion(50);
			}
			CArchive ar(&f, CArchive::store); 
			Serialize(ar);
			m_Filename = dlg.GetPathName();
			m_Filename.MakeLower();
			m_bModified = FALSE;
		}
	}
}

BOOL CExcite::OnMode()
{
	CModeDlg dlg;
	dlg.m_TranNum = m_TranNum;
	dlg.m_TranStep = m_TranStep;
	dlg.m_FirstStep = m_FirstStep;
	switch (m_Mode) {
	 case EQ_MODE: dlg.m_Mode=0; break;
	 case SS_MODE: dlg.m_Mode=1; break;
	 case TR_MODE: dlg.m_Mode=2; break;
	}
	if (dlg.DoModal()==IDOK)
	{
		m_TranNum = dlg.m_TranNum;
		m_TranStep = dlg.m_TranStep;
		m_FirstStep = dlg.m_FirstStep;
		switch (dlg.m_Mode) {
		 case 0: m_Mode=EQ_MODE; break;
		 case 1: m_Mode=SS_MODE; break;
		 case 2: m_Mode=TR_MODE; break;
		}
		return TRUE;
	}
	else return FALSE;
}

BOOL CExcite::SetDeviceTemperature()
{
	CTempDlg dlg;
	dlg.m_Tunit = (int)m_Tunit;
	if (m_Tunit==KELVIN)
		dlg.m_TempSS = m_Temp;
	else
		dlg.m_TempSS = (float)(m_Temp - TKC);
	if (dlg.DoModal()==IDOK)
	{
		m_Tunit = (CTempUnit)dlg.m_Tunit;
		if (dlg.m_Tunit==KELVIN)
			m_Temp = dlg.m_TempSS;
		else
			m_Temp = dlg.m_TempSS + TKC;
		return TRUE;
	}
	else return FALSE;
}

BOOL CExcite::SetFilename(CString path)
{
	BOOL FileOpen = TRUE;
	CString Ext = GENERATION_EXT;
	CString Filter = "Photogeneration (*."+Ext+")|*."+Ext+"|All Files (*.*)|*.*||";
	CFileDialog dlg(FileOpen, Ext, m_LightExternalFile, OFN_OVERWRITEPROMPT, Filter);
	dlg.m_ofn.lpstrTitle = "Open Photogeneration File";
	dlg.m_ofn.lpstrInitialDir = path;
	if (dlg.DoModal()==IDOK)
	{
		m_LightExternalFile = dlg.GetPathName();
		m_LightExternalFile.MakeLower();
		m_nPhoto = MAX_ELEMENTS;
		if (!CAscFile::Read(m_LightExternalFile, m_nPhoto, m_Position, m_Photogen))
			AfxMessageBox("Error reading Photogeneration file "+m_LightExternalFile);
		for (int k=0; k<m_nPhoto; k++) m_Position[k] *= 1E-4;	// um to cm
		m_LightExternalFile=CPath::MinimumNecessaryFilename(m_LightExternalFile, ((CPc1dApp *)AfxGetApp())->m_Path.exc);
		return TRUE;
	}
	else return FALSE;
}

BOOL CExcite::DoCircuitDlg(int toppage)
{
	CPropertySheet sheet("Circuit", NULL, toppage);
	CCircDlg BasePage(IDS_BASECIRCUIT);
	CCircDlg CollectorPage(IDS_COLLECTORCIRCUIT);
	m_Base.TransferDataToDlg(BasePage, "Open Base Voltage File");
	m_Coll.TransferDataToDlg(CollectorPage, "Open Collector Voltage File");
	sheet.AddPage(&BasePage);
	sheet.AddPage(&CollectorPage);
	if (sheet.DoModal()==IDOK) {
		m_Base.TransferDataFromDlg(BasePage);
		m_Coll.TransferDataFromDlg(CollectorPage);
		return TRUE;
	}else return FALSE;
}