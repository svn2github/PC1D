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
#include "mathstat.h"	// CMath
#include "physics.h"	// CPhysics
#include "device.h"

#include "resource.h"      // for dialog resource IDs
#include "refldlg.h"
#include "thickdlg.h"
#include "mobildlg.h"
#include "fxmobdlg.h"
#include "indexdlg.h"
#include "absdlg.h"
#include "bkdopdlg.h"
#include "diffdlg.h"
#include "bulkdlg.h"
#include "srecdlg.h"
#include "areadlg.h"
#include "textrdlg.h"                                                                                           
#include "intrndlg.h"
#include "lumpdlg.h"
#include "cntctdlg.h"
#include "ascfile.h" 
#include "surfdlg.h"
#include "matdlgs.h"

#include "matrec.h"

#include "path.h"
#include "pc1d.h" // to get the current paths (singleton)


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

const WORD DEVICEFILE_VERSION=1;
const WORD MATERIALFILE_VERSION=3;  // PC1D 4.2 used 0 PC1D 4.5 used 1 PC1D 5.0.1 used 2

const CString DEVICE_EXT="dev";
const CString MATERIAL_EXT="mat";
const CString REFLECTANCE_EXT="ref";
const CString INDEX_EXT="inr";
const CString ABSORPTION_EXT="abs";
const CString DOPING_EXT="dop";

/////////////////////////////////////////////////////////
// CMobility (Two per Material)

IMPLEMENT_SERIAL(CMobility, CObject, 0)

CMobility::CMobility() { }

void CMobility::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << Fixed << Max << MajMin << MajNref << MajAlpha;
		ar << MinMin << MinNref << MinAlpha << B1 << B2 << B3 << B4 << Vmax;
	}
	else
	{
		ar >> Fixed >> Max >> MajMin >> MajNref >> MajAlpha;
		ar >> MinMin >> MinNref >> MinAlpha >> B1 >> B2 >> B3 >> B4 >> Vmax;
	}
}

void CMobility::Copy(CMobility* pMob)
{
	Fixed = pMob->Fixed; Max = pMob->Max; MajMin = pMob->MajMin;
	MajNref = pMob->MajNref; MajAlpha = pMob->MajAlpha; MinMin = pMob->MinMin; 
	MinNref = pMob->MinNref; MinAlpha = pMob->MinAlpha; 
	B1 = pMob->B1; B2 = pMob->B2; B3 = pMob->B3; B4 = pMob->B4; Vmax = pMob->Vmax;
}

///////////////////////////////////////////////////////////////////////////
// CDiffusion (Four per Region)

IMPLEMENT_SERIAL(CDiffusion, CObject, 0)

CDiffusion::CDiffusion()
{
	Initialize(NULL);
}

void CDiffusion::Initialize(CRegion* pR)
{
	m_pRegion = pR;
	m_Enable = FALSE; m_Type = N_TYPE; m_Npeak = 1e20; 
	m_Depth = 1e-4; m_Xpeak = 0; m_Profile = ERFC_PROFILE;
}

void CDiffusion::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);
	WORD ver_num;
	if (ar.IsStoring())
	{
		ver_num=1;
		ar << (WORD)ver_num;
		ar << (WORD)m_Enable << (WORD)m_Type << (WORD)m_Profile;
		ar << m_Npeak << m_Depth << m_Xpeak;
	}
	else
	{
		ar >> (WORD &)ver_num;
		if (ver_num != 1) 
		{
			AfxThrowArchiveException(CArchiveException::badIndex);
			return;
		}
		ar >> (WORD&)m_Enable >> (WORD&)m_Type >> (WORD&)m_Profile;
		ar >> m_Npeak >> m_Depth >> m_Xpeak;
	}
}

BOOL CDiffusion::SetDiffusion(CString title)
{
	CDiffDlg dlg;
	dlg.m_title = title;
	dlg.pElec = &(m_pRegion->GetMaterial()->m_Elec);
	dlg.pHole = &(m_pRegion->GetMaterial()->m_Hole);
	dlg.pMat = m_pRegion->GetMaterial();
	dlg.m_BkgndDoping = m_pRegion->m_BkgndDop;
	dlg.m_Enable = m_Enable;
	dlg.m_Type = (int)m_Type;
	dlg.m_Npeak = m_Npeak;
	dlg.m_Xpeak = 1e4*m_Xpeak;		// convert cm to um
	dlg.m_Depth = 1e4*m_Depth;		// convert cm to um
	dlg.m_Profile = (int)m_Profile;
	if (dlg.DoModal()==IDOK)
	{
		m_Enable = dlg.m_Enable;
		m_Type = (CMatType)dlg.m_Type;
		m_Npeak = dlg.m_Npeak;
		m_Xpeak = 1e-4*dlg.m_Xpeak;	// convert um to cm
		m_Depth = 1e-4*dlg.m_Depth;	// convert um to cm
		m_Profile = (CProfile)dlg.m_Profile;
		return TRUE;
	}
	else return FALSE;
}

//////////////////////////////////////////////////////////////////////////
// CMaterial (One per Region)

IMPLEMENT_SERIAL(CMaterial, CObject, 0)

void CMaterial::SetFileVersion(int PC1DVersion)
{
	// Material file version:
	// PC1D 4.2 used 0 PC1D 4.3-4.6 used 1 PC1D 5.0.1 used 2
	if (PC1DVersion<43) m_FileVersion=0;
	if (PC1DVersion<50) m_FileVersion=1;
					// fileversion 2 is defunct; was 5_internal_1.
	else m_FileVersion=3;
}

// the earliest version it could be saved as
int CMaterial::GetFileVersion()
{
	// Material file version:
	// PC1D 4.2 used 0 PC1D 4.3-4.6 used 1 PC1D 5.0.1 used 2
	if (!m_HurkxEnable) {
		// if we're using hurkx, has to be ver 5 at least, don't even try
		if (m_FileVersion==0) return 40;
		if (m_FileVersion<2) return 43;
	}
	return 50;	// fileversion 2 & 3 both are 5.0
}

void CMaterial::Initialize()
{
	m_Filename.Empty();
	SetFileVersion(PC1DVERSION);
	m_bModified = FALSE; 
	m_FixedMobility = FALSE;
	m_Elec.Fixed = 1200; m_Elec.Max = 1417; m_Elec.MajMin = 60.0; m_Elec.MajNref = 9.64E16;
	m_Elec.MajAlpha = 0.664; m_Elec.MinMin = 160; m_Elec.MinNref = 5.6e16; 
	m_Elec.MinAlpha = 0.647; m_Elec.B1 = -0.57; m_Elec.B2 = -2.33; m_Elec.B3 = 2.4; 
	m_Elec.B4 = -0.146; m_Elec.Vmax = 1e7;
	m_Hole.Fixed = 400; m_Hole.Max = 470; m_Hole.MajMin = 37.4; m_Hole.MajNref = 2.82e17;
	m_Hole.MajAlpha = 0.642; m_Hole.MinMin = 155; m_Hole.MinNref = 1.0E17;
	m_Hole.MinAlpha = 0.9; m_Hole.B1 = -0.57; m_Hole.B2 = -2.23; m_Hole.B3 = 2.4;
	m_Hole.B4 = -0.146; m_Hole.Vmax = 1e7;
	m_BandGap = 1.124; m_Affinity = 4.05; m_NcNv = 1.06;  m_Permittivity = 11.9;
	m_ni200 = 4.7e4; m_ni300 = 1.0e10; m_ni400 = 5.6e12;
	m_Cn = 2.2e-31; m_Cp = 9.9e-32; m_Cnp = 1.66e-30; 
	m_BB = 9.5e-15; // Ref: H.Schlangenotto et. al. Physica Status Solidi A21, 357-367 (1974).
	m_BulkNrefN = 1e15; m_BulkNalphaN = 0;
	m_BulkNrefP = 1e15; m_BulkNalphaP = 0;
	m_BulkTalpha = 0; m_BulkEref = 3.7E5;
	m_SurfNrefN = 1e18; m_SurfNalphaN = 0;
	m_SurfNrefP = 1e18; m_SurfNalphaP = 0;
	m_SurfTalpha = 0; m_BulkEgamma = 3.46;
	m_BGNnNref = 1.4e17; m_BGNnSlope = 0.014;
	m_BGNpNref = 1.4e17; m_BGNpSlope = 0.014;
	m_IndexFilename.Empty(); m_IndexExternal = FALSE;
	m_FixedIndex = 3.58;
	m_AbsFilename.Empty(); m_AbsExternal = FALSE;
	m_AbsEd1 = 3.177; m_AbsEd2 = 4.00; m_AbsEi1 = 1.102; m_AbsEi2 = 2.489;
	m_AbsEp1 = 0.01826; m_AbsEp2 = 0.06494; m_AbsAd1 = 1.29e6;
	m_AbsAd2 = 3.746e4; m_AbsA11 = 1.468e3; m_AbsA12 = 1.561e3;
	m_AbsA21 = 2.890e4; m_AbsA22 = 8.797e4; m_AbsTcoeff = 4.73e-4;
	m_AbsToffset = 636;
	m_nAbsorb = m_nIndex = 0;
	for (int k=0; k<MAX_WAVELENGTHS; k++)
	{
		m_AbsLambda[k] = m_Absorption[k] = 0;
		m_IdxLambda[k] = 0; m_Index[k] = 1;
	}
	m_FreeCarrEnable=TRUE;
	m_FreeCarrCoeffN=2.6e-27;	m_FreeCarrPowerN=3; // use Schmidt's Si data, Green's fit.
	m_FreeCarrCoeffP=2.7e-24;   m_FreeCarrPowerP=2;
	m_HurkxFgamma = 3.688e5; 	// V/cm use Hurkx paper.
	m_HurkxPrefactor = 6.14;	// 2*sqrt(3*PI)
	m_HurkxEnable=TRUE;
}

void CMaterial::Copy(CMaterial* pM)
{
	m_Filename = pM->m_Filename;
	m_FileVersion= pM->m_FileVersion;
	m_bModified = pM->m_bModified;

	m_FixedMobility = pM->m_FixedMobility;
	m_Elec.Copy(&pM->m_Elec);
	m_Hole.Copy(&pM->m_Hole);
	m_BandGap = pM->m_BandGap; m_Affinity = pM->m_Affinity; m_NcNv = pM->m_NcNv;
	m_Permittivity = pM->m_Permittivity;
	m_ni200 = pM->m_ni200; m_ni300 = pM->m_ni300; m_ni400 = pM->m_ni400;
	m_Cn = pM->m_Cn; m_Cp = pM->m_Cp; m_Cnp = pM->m_Cnp; m_BB = pM->m_BB;
	m_BulkNrefN = pM->m_BulkNrefN; m_BulkNalphaN = pM->m_BulkNalphaN;
	m_BulkNrefP = pM->m_BulkNrefP; m_BulkNalphaP = pM->m_BulkNalphaP;
	m_BulkTalpha = pM->m_BulkTalpha; m_BulkEref = pM->m_BulkEref;
	m_SurfNrefN = pM->m_SurfNrefN; m_SurfNalphaN = pM->m_SurfNalphaN;
	m_SurfNrefP = pM->m_SurfNrefP; m_SurfNalphaP = pM->m_SurfNalphaP;
	m_SurfTalpha = pM->m_SurfTalpha; m_BulkEgamma = pM->m_BulkEgamma;
	m_BGNnNref = pM->m_BGNnNref; m_BGNnSlope = pM->m_BGNnSlope;
	m_BGNpNref = pM->m_BGNpNref; m_BGNpSlope = pM->m_BGNpSlope;
	m_IndexFilename = pM->m_IndexFilename; m_IndexExternal = pM->m_IndexExternal;
	m_FixedIndex = pM->m_FixedIndex;
	m_AbsFilename = pM->m_AbsFilename; m_AbsExternal = pM->m_AbsExternal;
	m_AbsEd1 = pM->m_AbsEd1; m_AbsEd2 = pM->m_AbsEd2; m_AbsEi1 = pM->m_AbsEi1;
	m_AbsEi2 = pM->m_AbsEi2; m_AbsEp1 = pM->m_AbsEp1; m_AbsEp2 = pM->m_AbsEp2;
	m_AbsAd1 = pM->m_AbsAd1; m_AbsAd2 = pM->m_AbsAd2; m_AbsA11 = pM->m_AbsA11;
	m_AbsA12 = pM->m_AbsA12; m_AbsA22 = pM->m_AbsA22;
	m_AbsTcoeff = pM->m_AbsTcoeff; m_AbsToffset = pM->m_AbsToffset;
	m_nAbsorb = pM->m_nAbsorb; m_nIndex = pM->m_nIndex;
	for (int k=0; k<MAX_WAVELENGTHS; k++)
	{
		m_AbsLambda[k] = pM->m_AbsLambda[k]; m_Absorption[k] = pM->m_Absorption[k];
		m_IdxLambda[k] = pM->m_IdxLambda[k]; m_Index[k] = pM->m_Index[k];
	}
	m_FreeCarrCoeffN=pM->m_FreeCarrCoeffN;	m_FreeCarrPowerN=pM->m_FreeCarrPowerN;
	m_FreeCarrCoeffP=pM->m_FreeCarrCoeffP;  m_FreeCarrPowerP=pM->m_FreeCarrPowerP;
	m_HurkxEnable = pM->m_HurkxEnable;
	m_HurkxFgamma = pM->m_HurkxFgamma;
	m_HurkxPrefactor=pM->m_HurkxPrefactor;
}

CMaterial::CMaterial()
{
	Initialize();
}

CMaterial::~CMaterial()
{
}

/////////////////////////////////////////////////////////////////////////////
// CMaterial serialization

void CMaterial::Serialize(CArchive& ar)
{
	WORD version;
	CObject::Serialize(ar);
	m_Elec.Serialize(ar); m_Hole.Serialize(ar);
	if (ar.IsStoring())
	{
//		version=MATERIALFILE_VERSION;
		version=m_FileVersion;
		ar << CPath::MinimumNecessaryFilename(m_Filename, ((CPc1dApp *)AfxGetApp())->m_Path.mat );
		if (version>0) {
			if (m_bModified) ar << (WORD)0xDC01; else ar << (WORD)0xDC00;
			ar << version;
		} else	ar << (WORD)m_bModified;
		ar << (WORD)m_FixedMobility;
		ar << m_Permittivity << m_BandGap << m_Affinity << m_NcNv;
		ar << m_ni200 << m_ni300 << m_ni400;
		ar << m_BGNnNref << m_BGNnSlope << m_BGNpNref << m_BGNpSlope;
		ar << m_Cn << m_Cp << m_Cnp << m_BB;
		ar << m_BulkNrefN << m_BulkNalphaN;
		ar << m_BulkNrefP << m_BulkNalphaP;
		ar << m_BulkTalpha << m_BulkEref << m_BulkEgamma;
		ar << m_SurfNrefN << m_SurfNalphaN;
		ar << m_SurfNrefP << m_SurfNalphaP;
		ar << m_SurfTalpha;

		ar << CPath::MinimumNecessaryFilename(m_IndexFilename, ((CPc1dApp *)AfxGetApp())->m_Path.inr );				
		ar << (WORD)m_IndexExternal << m_FixedIndex;
		ar << CPath::MinimumNecessaryFilename(m_AbsFilename, ((CPc1dApp *)AfxGetApp())->m_Path.abs );
		ar << (WORD)m_AbsExternal;
		
		ar << m_AbsEd1 << m_AbsEd2 << m_AbsEi1 << m_AbsEi2;
		ar << m_AbsEp1 << m_AbsEp2 << m_AbsAd1 << m_AbsAd2;
		ar << m_AbsA11 << m_AbsA12 << m_AbsA21 << m_AbsA22;
		ar << m_AbsTcoeff << m_AbsToffset;
		if (version>0) {
			ar << (WORD)m_FreeCarrEnable;
			ar << m_FreeCarrCoeffN << m_FreeCarrPowerN << m_FreeCarrCoeffP << m_FreeCarrPowerP;
		}
		if (version==2) ar << m_HurkxPrefactor << m_HurkxFgamma;
		if (version>2) {
			ar << (WORD)m_HurkxEnable << m_HurkxPrefactor << m_HurkxFgamma;
		}
	}
	else
	{		
		ar >> m_Filename; 
		ar >> (WORD&)m_bModified;
		if ((m_bModified & 0xFF00) == 0xDC00) { ar >> version; m_bModified &=0x00FF;} 
		else version=0;		
		m_FileVersion=version;
		ar >> (WORD&)m_FixedMobility;
		ar >> m_Permittivity >> m_BandGap >> m_Affinity >> m_NcNv;
		ar >> m_ni200 >> m_ni300 >> m_ni400;
		ar >> m_BGNnNref >> m_BGNnSlope >> m_BGNpNref >> m_BGNpSlope;
		ar >> m_Cn >> m_Cp >> m_Cnp >> m_BB;
		ar >> m_BulkNrefN >> m_BulkNalphaN;
		ar >> m_BulkNrefP >> m_BulkNalphaP;
		ar >> m_BulkTalpha >> m_BulkEref >> m_BulkEgamma;
		ar >> m_SurfNrefN >> m_SurfNalphaN;
		ar >> m_SurfNrefP >> m_SurfNalphaP;
		ar >> m_SurfTalpha;
		ar >> m_IndexFilename >> (WORD&)m_IndexExternal >> m_FixedIndex;
		ar >> m_AbsFilename >> (WORD&)m_AbsExternal;
		ar >> m_AbsEd1 >> m_AbsEd2 >> m_AbsEi1 >> m_AbsEi2;
		ar >> m_AbsEp1 >> m_AbsEp2 >> m_AbsAd1 >> m_AbsAd2;
		ar >> m_AbsA11 >> m_AbsA12 >> m_AbsA21 >> m_AbsA22;
		ar >> m_AbsTcoeff >> m_AbsToffset;
		if (version>0) {
			ar >> (WORD &)m_FreeCarrEnable ;
			ar >> m_FreeCarrCoeffN >> m_FreeCarrPowerN >> m_FreeCarrCoeffP >> m_FreeCarrPowerP;
		} else {
			m_FreeCarrEnable=FALSE;
			m_FreeCarrCoeffN=2.6e-27;	m_FreeCarrPowerN=3; // use Schmidt's Si data, Green's fit.
			m_FreeCarrCoeffP=2.7e-18*1e-6;  m_FreeCarrPowerP=2;
		}
		switch (version) {
		case 2:
			ar >> m_HurkxPrefactor >> m_HurkxFgamma;
			m_HurkxEnable=TRUE;
			break;
		case 0:
		case 1:
			m_HurkxFgamma = 3.688e5; 	// V/cm use Hurkx paper.
			m_HurkxPrefactor = 6.14;	// 2*sqrt(3*PI)
			m_HurkxEnable = FALSE;
			break;
		case 3:
		default:
			ar >> (WORD &)m_HurkxEnable >> m_HurkxPrefactor >> m_HurkxFgamma;
			break;
		}

		// finished reading from material file, now must act on the data...
		// Now load data from external files if required

		m_Filename=CPath::MinimumNecessaryFilename(m_Filename, ((CPc1dApp *)AfxGetApp())->m_Path.mat );
		m_IndexFilename=CPath::MinimumNecessaryFilename(m_IndexFilename, ((CPc1dApp *)AfxGetApp())->m_Path.inr );
		m_AbsFilename=CPath::MinimumNecessaryFilename(m_AbsFilename, ((CPc1dApp *)AfxGetApp())->m_Path.abs );
		m_nAbsorb = MAX_WAVELENGTHS;
		if (m_AbsExternal)
			if (!CAscFile::Read(CPath::MakeFullPathname(m_AbsFilename, ((CPc1dApp *)AfxGetApp())->m_Path.abs ),
					m_nAbsorb, m_AbsLambda, m_Absorption))
				AfxMessageBox("Error reading Absorption file "+m_AbsFilename);
		m_nIndex = MAX_WAVELENGTHS;
		if (m_IndexExternal)
			if (!CAscFile::Read(CPath::MakeFullPathname(m_IndexFilename, ((CPc1dApp *)AfxGetApp())->m_Path.inr ),
					m_nIndex, m_IdxLambda, m_Index))
				AfxMessageBox("Error reading Refractive Index file "+m_IndexFilename);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMaterial implementation

BOOL CMaterial::OnMaterialOpen()
{
	BOOL FileOpen = TRUE;
	CString Ext = MATERIAL_EXT;
	CString Filter = "Material (*.mat)|*.mat|All Files (*.*)|*.*||";
	CFileDialog dlg(FileOpen, Ext, m_Filename, OFN_OVERWRITEPROMPT, Filter);
	dlg.m_ofn.lpstrTitle = "Open Material File";
	dlg.m_ofn.lpstrInitialDir = ((CPc1dApp *)AfxGetApp())->m_Path.mat;
	if (dlg.DoModal()==IDOK)
	{
		CFile f;
		if (!f.Open(dlg.GetPathName(), CFile::modeRead))
		{
			AfxMessageBox("Unable to open material file");
			return FALSE;
		}
		else 
		{
			CArchive ar(&f, CArchive::load); 
			Serialize(ar);
			m_Filename = dlg.GetPathName();
			m_Filename.MakeLower();
			m_Filename=CPath::MinimumNecessaryFilename(m_Filename, ((CPc1dApp *)AfxGetApp())->m_Path.mat );
			m_bModified = FALSE;
			return TRUE;
		}
	}
	else return FALSE;
}

void CMaterial::OnMaterialSaveas()
{
	BOOL FileOpen = FALSE;
	CString Ext = MATERIAL_EXT;
	CString Filter = "Material (*.mat)|*.mat|PC1D 4.5 Material (*.mat)|*.mat|All Files (*.*)|*.*||";
	CFileDialog dlg(FileOpen, Ext, m_Filename, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, Filter);
	dlg.m_ofn.lpstrTitle = "Save Material File";
	dlg.m_ofn.lpstrInitialDir = ((CPc1dApp *)AfxGetApp())->m_Path.mat;	
	if (GetFileVersion()<50) dlg.m_ofn.nFilterIndex=2;
	else dlg.m_ofn.nFilterIndex=1;

	if (dlg.DoModal()==IDOK)
	{
		CFile f;
		if (!f.Open(dlg.GetPathName(), CFile::modeCreate | CFile::modeWrite))
		{
			AfxMessageBox("Unable to create material file");
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

BOOL CMaterial::SetMobilityModel()
{
	CMobilDlg dlg;
	dlg.m_ElecMax = m_Elec.Max;
	dlg.m_ElecMajMin = m_Elec.MajMin;
	dlg.m_ElecMajNref = m_Elec.MajNref;
	dlg.m_ElecMajAlpha = m_Elec.MajAlpha;
	dlg.m_ElecMinMin = m_Elec.MinMin;
	dlg.m_ElecMinNref = m_Elec.MinNref;
	dlg.m_ElecMinAlpha = m_Elec.MinAlpha;
	dlg.m_ElecB1 = m_Elec.B1;
	dlg.m_ElecB2 = m_Elec.B2;
	dlg.m_ElecB3 = m_Elec.B3;
	dlg.m_ElecB4 = m_Elec.B4;
	dlg.m_ElecVmax = m_Elec.Vmax;
	dlg.m_HoleMax = m_Hole.Max;
	dlg.m_HoleMajMin = m_Hole.MajMin;
	dlg.m_HoleMajNref = m_Hole.MajNref;
	dlg.m_HoleMajAlpha = m_Hole.MajAlpha;
	dlg.m_HoleMinMin = m_Hole.MinMin;
	dlg.m_HoleMinNref = m_Hole.MinNref;
	dlg.m_HoleMinAlpha = m_Hole.MinAlpha;
	dlg.m_HoleB1 = m_Hole.B1;
	dlg.m_HoleB2 = m_Hole.B2;
	dlg.m_HoleB3 = m_Hole.B3;
	dlg.m_HoleB4 = m_Hole.B4;
	dlg.m_HoleVmax = m_Hole.Vmax;
	if (dlg.DoModal()==IDOK)
	{
		m_Elec.Max = dlg.m_ElecMax;
		m_Elec.MajMin = dlg.m_ElecMajMin;
		m_Elec.MajNref = dlg.m_ElecMajNref;
		m_Elec.MajAlpha = dlg.m_ElecMajAlpha;
		m_Elec.MinMin = dlg.m_ElecMinMin;
		m_Elec.MinNref = dlg.m_ElecMinNref;
		m_Elec.MinAlpha = dlg.m_ElecMinAlpha;
		m_Elec.B1 = dlg.m_ElecB1;
		m_Elec.B2 = dlg.m_ElecB2;
		m_Elec.B3 = dlg.m_ElecB3;
		m_Elec.B4 = dlg.m_ElecB4;
		m_Elec.Vmax = dlg.m_ElecVmax;
		m_Hole.Max = dlg.m_HoleMax;
		m_Hole.MajMin = dlg.m_HoleMajMin;
		m_Hole.MajNref = dlg.m_HoleMajNref;
		m_Hole.MajAlpha = dlg.m_HoleMajAlpha;
		m_Hole.MinMin = dlg.m_HoleMinMin;
		m_Hole.MinNref = dlg.m_HoleMinNref;
		m_Hole.MinAlpha = dlg.m_HoleMinAlpha;
		m_Hole.B1 = dlg.m_HoleB1;
		m_Hole.B2 = dlg.m_HoleB2;
		m_Hole.B3 = dlg.m_HoleB3;
		m_Hole.B4 = dlg.m_HoleB4;
		m_Hole.Vmax = dlg.m_HoleVmax;
		m_FixedMobility = FALSE;
		return TRUE;
	}
	else return FALSE;
}

BOOL CMaterial::SetMobilityFixed()
{
	CFxMobDlg dlg;
	dlg.m_Elec = m_Elec.Fixed;
	dlg.m_Hole = m_Hole.Fixed;
	if (dlg.DoModal()==IDOK)
	{
		m_Elec.Fixed = dlg.m_Elec;
		m_Hole.Fixed = dlg.m_Hole;
		m_FixedMobility = TRUE;
		return TRUE;
	}
	else return FALSE;
}

BOOL CMaterial::SetPermittivity()
{
	CPermDlg dlg;
	dlg.m_Permittivity = m_Permittivity;
	if (dlg.DoModal()==IDOK)
	{
		m_Permittivity = dlg.m_Permittivity;
		return TRUE;
	}
	else return FALSE;
}

BOOL CMaterial::SetBandStructure()
{
	CBandDlg dlg;
	dlg.m_BandGap = m_BandGap;
	dlg.m_Affinity = m_Affinity;
	dlg.m_NcNv = m_NcNv;
	dlg.m_Ni200 = m_ni200;
	dlg.m_Ni300 = m_ni300;
	dlg.m_Ni400 = m_ni400;
	if (dlg.DoModal()==IDOK)
	{
		m_BandGap = dlg.m_BandGap;
		m_Affinity = dlg.m_Affinity;
		m_NcNv = dlg.m_NcNv;
		m_ni200 = dlg.m_Ni200;
		m_ni300 = dlg.m_Ni300;
		m_ni400 = dlg.m_Ni400;
		return TRUE;
	}
	else return FALSE;
}

BOOL CMaterial::DoRecombinationDlg()
{
	CPropertySheet dlg("Recombination");
	CMatRecAuger AugerPage;
	CMatRecDoping DopingPage;
	CMatRecTemperature TempPage;
	CMatRecHurkx HurkxPage;
	
	AugerPage.m_Cn = m_Cn;
	AugerPage.m_Cp = m_Cp;
	AugerPage.m_Cnp = m_Cnp;
    AugerPage.m_BB = m_BB;
    
	DopingPage.m_BulkNrefN = m_BulkNrefN;
	DopingPage.m_BulkNalphaN = m_BulkNalphaN;
	DopingPage.m_BulkNrefP = m_BulkNrefP;
	DopingPage.m_BulkNalphaP = m_BulkNalphaP;
	TempPage.m_BulkTalpha = m_BulkTalpha;
	DopingPage.m_SurfNrefN = m_SurfNrefN;
	DopingPage.m_SurfNalphaN = m_SurfNalphaN;
	DopingPage.m_SurfNrefP = m_SurfNrefP;
	DopingPage.m_SurfNalphaP = m_SurfNalphaP;
	TempPage.m_SurfTalpha = m_SurfTalpha;
	
	HurkxPage.m_Enable = m_HurkxEnable;
	HurkxPage.m_Fgamma = m_HurkxFgamma;
	HurkxPage.m_prefactor = m_HurkxPrefactor;
    
	dlg.AddPage(&DopingPage);
	dlg.AddPage(&AugerPage);
	dlg.AddPage(&TempPage);
	dlg.AddPage(&HurkxPage);
	if (dlg.DoModal()==IDOK) {
		m_Cn = AugerPage.m_Cn;
		m_Cp = AugerPage.m_Cp;
		m_Cnp = AugerPage.m_Cnp;
		m_BB = AugerPage.m_BB;
		
		m_BulkNrefN = DopingPage.m_BulkNrefN;
		m_BulkNalphaN = DopingPage.m_BulkNalphaN;
		m_BulkNrefP = DopingPage.m_BulkNrefP;
		m_BulkNalphaP = DopingPage.m_BulkNalphaP;
		m_BulkTalpha = TempPage.m_BulkTalpha;
		m_SurfNrefN = DopingPage.m_SurfNrefN;
		m_SurfNalphaN = DopingPage.m_SurfNalphaN;
		m_SurfNrefP = DopingPage.m_SurfNrefP;
		m_SurfNalphaP = DopingPage.m_SurfNalphaP;
		m_SurfTalpha = TempPage.m_SurfTalpha;

		m_HurkxEnable = HurkxPage.m_Enable;
		m_HurkxFgamma = HurkxPage.m_Fgamma;
		m_HurkxPrefactor = HurkxPage.m_prefactor;
		
		return TRUE;
	} else return FALSE;
}
	

BOOL CMaterial::SetBandgapNarrowing()
{
	CBGNDlg dlg;
	dlg.m_nNref = m_BGNnNref;
	dlg.m_nSlope =  1000 * m_BGNnSlope;	// Convert eV to meV
	dlg.m_pNref = m_BGNpNref;
	dlg.m_pSlope =  1000 * m_BGNpSlope;	// Likewise
	if (dlg.DoModal()==IDOK)
	{
		m_BGNnNref = dlg.m_nNref;
		m_BGNnSlope =  0.001 * dlg.m_nSlope;	// Convert meV back to eV
		m_BGNpNref = dlg.m_pNref;
		m_BGNpSlope =  0.001 * dlg.m_pSlope;	// Likewise
		return TRUE;
	}
	else return FALSE;
}

BOOL CMaterial::DoOpticalDlg(int toppage)
{
	CPropertySheet sheet("Optical properties", NULL, toppage);
	CAbsDlg abspage;
	CParasiticsDlg freecarrpage;
	CIndexDlg indexpage;
	
	abspage.m_Radio = 0;
	if (m_AbsExternal) abspage.m_Radio=1;
	
	abspage.m_Ed1 = m_AbsEd1;
	abspage.m_Ed2 = m_AbsEd2;
	abspage.m_Ei1 = m_AbsEi1;
	abspage.m_Ei2 = m_AbsEi2;
	abspage.m_Ep1 = 1000 * m_AbsEp1;		// convert eV to meV
	abspage.m_Ep2 = 1000 * m_AbsEp2;		// convert eV to meV
	abspage.m_Ad1 = m_AbsAd1;
	abspage.m_Ad2 = m_AbsAd2;
	abspage.m_A11 = m_AbsA11;
	abspage.m_A12 = m_AbsA12;
	abspage.m_A21 = m_AbsA21;
	abspage.m_A22 = m_AbsA22;
	abspage.m_Tcoeff = 1000*m_AbsTcoeff;	// convert eV/K to meV/K
	abspage.m_Toffset = m_AbsToffset;
	
	abspage.m_Filename = m_AbsFilename;
	abspage.m_Ext = ABSORPTION_EXT;
	abspage.m_Path = ((CPc1dApp *)AfxGetApp())->m_Path.abs;
	abspage.m_OpenDlgTitle = "Open Absorption Coefficients File";
	
	
	freecarrpage.m_nCoeff = m_FreeCarrCoeffN;
	freecarrpage.m_pCoeff = m_FreeCarrCoeffP;
	freecarrpage.m_nLambdaPower = m_FreeCarrPowerN;
	freecarrpage.m_pLambdaPower = m_FreeCarrPowerP;
	freecarrpage.m_Enable = m_FreeCarrEnable;

	indexpage.m_Index = m_FixedIndex;
	indexpage.m_Radio = m_IndexExternal ? 1 : 0;
	indexpage.m_Filename = m_IndexFilename;
	indexpage.m_Ext=INDEX_EXT;
	indexpage.m_Path=((CPc1dApp *)AfxGetApp())->m_Path.inr;


	sheet.AddPage(&abspage);
	sheet.AddPage(&freecarrpage);
	sheet.AddPage(&indexpage);
	if (sheet.DoModal()) {
		m_AbsEd1 = abspage.m_Ed1;
		m_AbsEd2 = abspage.m_Ed2;
		m_AbsEi1 = abspage.m_Ei1;
		m_AbsEi2 = abspage.m_Ei2;
		m_AbsEp1 = 0.001 * abspage.m_Ep1;	// convert meV to eV
		m_AbsEp2 = 0.001 * abspage.m_Ep2;	// convert meV to eV
		m_AbsAd1 = abspage.m_Ad1;
		m_AbsAd2 = abspage.m_Ad2;
		m_AbsA11 = abspage.m_A11;
		m_AbsA12 = abspage.m_A12;
		m_AbsA21 = abspage.m_A21;
		m_AbsA22 = abspage.m_A22;
		m_AbsTcoeff = 0.001*abspage.m_Tcoeff;	// convert meV/K to eV/K
		m_AbsToffset = abspage.m_Toffset;

		m_AbsFilename = abspage.m_Filename;
		
		m_AbsExternal=FALSE;
		if (abspage.m_Radio==1) {
			m_AbsExternal=TRUE;
			m_nAbsorb = MAX_WAVELENGTHS;
			if (!CAscFile::Read(CPath::MakeFullPathname(m_AbsFilename, ((CPc1dApp *)AfxGetApp())->m_Path.abs), m_nAbsorb, m_AbsLambda, m_Absorption))
				AfxMessageBox("Error reading Absorption file "+m_AbsFilename);
			m_AbsFilename=CPath::MinimumNecessaryFilename(m_AbsFilename, ((CPc1dApp *)AfxGetApp())->m_Path.abs );
		}
		
		m_FreeCarrEnable = freecarrpage.m_Enable;
		m_FreeCarrCoeffN = freecarrpage.m_nCoeff;
		m_FreeCarrCoeffP = freecarrpage.m_pCoeff;
		m_FreeCarrPowerN = freecarrpage.m_nLambdaPower;
		m_FreeCarrPowerP = freecarrpage.m_pLambdaPower;

		m_FixedIndex = indexpage.m_Index;
		m_IndexExternal= (indexpage.m_Radio==1);
		m_IndexFilename = indexpage.m_Filename;
		if (m_IndexExternal) {
			m_nIndex = MAX_WAVELENGTHS;
			if (!CAscFile::Read(CPath::MakeFullPathname(m_IndexFilename, ((CPc1dApp *)AfxGetApp())->m_Path.inr), m_nIndex, m_IdxLambda, m_Index))
				AfxMessageBox("Error reading Refractive Index file "+m_IndexFilename);
		}
		m_IndexFilename=CPath::MinimumNecessaryFilename(m_IndexFilename, ((CPc1dApp *)AfxGetApp())->m_Path.inr );

		return TRUE;
	}
	return FALSE;
}

BOOL CMaterial::SetAbsFile(CString path)
{
	BOOL FileOpen = TRUE;
	CString Ext = ABSORPTION_EXT;
	CString Filter = "Absorption (*."+Ext+")|*."+Ext+"|All Files (*.*)|*.*||";
	CFileDialog dlg(FileOpen, Ext, m_AbsFilename, OFN_OVERWRITEPROMPT, Filter);
	dlg.m_ofn.lpstrTitle = "Open Absorption Coefficients File";
	dlg.m_ofn.lpstrInitialDir = path;
	if (dlg.DoModal()==IDOK)
	{
		m_AbsFilename = dlg.GetPathName();
		m_AbsFilename.MakeLower();
		m_nAbsorb = MAX_WAVELENGTHS;
		if (!CAscFile::Read(CPath::MakeFullPathname(m_AbsFilename, ((CPc1dApp *)AfxGetApp())->m_Path.abs), m_nAbsorb, m_AbsLambda, m_Absorption))
			AfxMessageBox("Error reading Absorption file "+m_AbsFilename);
		m_AbsFilename=CPath::MinimumNecessaryFilename(m_AbsFilename, ((CPc1dApp *)AfxGetApp())->m_Path.abs );
		return TRUE;
	}
	else return FALSE;
}

double CMaterial::GetAbsorptionAtWavelength(double lambda, double T) const
{
	if (m_AbsExternal)
			return CMath::LogInterp(lambda, m_nAbsorb, m_AbsLambda, m_Absorption);
	else return CPhysics::Absorption(lambda, m_AbsEd1, m_AbsEd2, 
	    		m_AbsEi1, m_AbsEi2, m_AbsEp1, m_AbsEp2, 
	    		m_AbsAd1, m_AbsAd2, m_AbsA11, m_AbsA12,
	    		m_AbsA21, m_AbsA22, m_AbsTcoeff, m_AbsToffset, T);
}

double CMaterial::GetRefractiveIndexAtWavelength(double lambda) const
{
	if (m_IndexExternal) return CMath::LinearInterp(lambda, m_nIndex, m_IdxLambda, m_Index);
	else return m_FixedIndex;
}


/////////////////////////////////////////////////////////
// CRegion (Up to 5 per Device)

IMPLEMENT_SERIAL(CRegion, CObject, 0)

CRegion::CRegion()
{
	Initialize();
}

void CRegion::Initialize()
{
	m_Mat.Initialize();
	m_FrontDiff1.Initialize(this);
	m_FrontDiff2.Initialize(this);
	m_RearDiff1.Initialize(this);
	m_RearDiff2.Initialize(this);
	m_Thickness = 10E-4;
	m_FrontFilename.Empty(); m_FrontExternal = FALSE;
	m_RearFilename.Empty(); m_RearExternal = FALSE;
	m_BkgndDop = 1e16; m_BkgndType = P_TYPE;
	m_TauN = 1e-3; m_TauP = 1e-3; m_BulkEt = 0;
	m_FrontJo = FALSE;
	m_FrontSn = 0; m_FrontSp = 0; m_FrontEt = 0;
	m_RearJo = FALSE;
	m_RearSn = 0; m_RearSp = 0; m_RearEt = 0;
	m_nFront = m_nRear = 0;
	for (int k=0; k<MAX_ELEMENTS; k++)
	{
		m_FrontPosition[k] = 0; m_FrontDopingDonor[k] = m_FrontDopingAcceptor[k] = 1;
		m_RearPosition[k] = 0; m_RearDopingDonor[k] = m_RearDopingAcceptor[k] = 1;
	}
}

void CRegion::Copy(CRegion* pR)
{
	// External & Diffusion doping not copied to new regions
	// Sn and Sp not copied either, nor is Jo preference
	m_Mat.Copy(&pR->m_Mat);
	m_Thickness = pR->m_Thickness;
	m_BkgndDop = pR->m_BkgndDop; m_BkgndType = pR->m_BkgndType;	
	m_TauN = pR->m_TauN; m_TauP = pR->m_TauP; m_BulkEt = pR->m_BulkEt;
}

/////////////////////////////////////////////////////////////////////////////
// CRegion serialization

void CRegion::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);
	m_Mat.Serialize(ar);
	m_FrontDiff1.Serialize(ar);
	m_FrontDiff2.Serialize(ar);
	m_RearDiff1.Serialize(ar);
	m_RearDiff2.Serialize(ar);
	if (ar.IsStoring())
	{
		ar << m_Thickness;
		ar << CPath::MinimumNecessaryFilename(m_FrontFilename,((CPc1dApp *)AfxGetApp())->m_Path.dop);
		ar << (WORD)m_FrontExternal;
		ar << CPath::MinimumNecessaryFilename(m_RearFilename, ((CPc1dApp *)AfxGetApp())->m_Path.dop);
		ar << (WORD)m_RearExternal;		
		ar << m_BkgndDop << (WORD)m_BkgndType;
		ar << m_TauN << m_TauP << m_BulkEt;
		ar << m_FrontSn << m_FrontSp << m_FrontEt << (WORD)m_FrontJo;
		ar << m_RearSn << m_RearSp << m_RearEt << (WORD)m_RearJo;
	}
	else
	{
		ar >> m_Thickness;
		ar >> m_FrontFilename >> (WORD&)m_FrontExternal;
		ar >> m_RearFilename >> (WORD&)m_RearExternal;
		ar >> m_BkgndDop >> (WORD&)m_BkgndType;
		ar >> m_TauN >> m_TauP >> m_BulkEt;
		ar >> m_FrontSn >> m_FrontSp >> m_FrontEt >> (WORD&)m_FrontJo;
		ar >> m_RearSn >> m_RearSp >> m_RearEt >> (WORD&)m_RearJo;
		m_FrontFilename=CPath::MinimumNecessaryFilename(m_FrontFilename,((CPc1dApp *)AfxGetApp())->m_Path.dop);
		m_RearFilename=CPath::MinimumNecessaryFilename(m_RearFilename, ((CPc1dApp *)AfxGetApp())->m_Path.dop);
		m_nFront = MAX_ELEMENTS;
		if (m_FrontExternal)
		{
			if (!CAscFile::Read(CPath::MakeFullPathname(m_FrontFilename, ((CPc1dApp *)AfxGetApp())->m_Path.dop),
					m_nFront, m_FrontPosition, m_FrontDopingDonor, m_FrontDopingAcceptor))
				AfxMessageBox("Error reading Doping file "+m_FrontFilename);
			for (int k=0; k<m_nFront; k++) m_FrontPosition[k] *= 1E-4;	// um to cm
		}
		m_nRear = MAX_ELEMENTS;
		if (m_RearExternal)
		{
			if (!CAscFile::Read(CPath::MakeFullPathname(m_RearFilename,((CPc1dApp *)AfxGetApp())->m_Path.dop),
			 		m_nRear, m_RearPosition, m_RearDopingDonor, m_RearDopingAcceptor))
				AfxMessageBox("Error reading Doping file "+m_RearFilename);
			for (int k=0; k<m_nRear; k++) m_RearPosition[k] *= 1E-4;	// um to cm
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CRegion implementation

BOOL CRegion::SetRegionThickness()
{
	CThickDlg dlg;
	dlg.m_Thick = m_Thickness *  1E4;	// convert cm to um
	if (dlg.DoModal()==IDOK)
	{
		m_Thickness = dlg.m_Thick *  1E-4;	// convert um to cm
		return TRUE;
	}
	else return FALSE;
}

BOOL CRegion::SetBackgroundDoping()
{
	CBkDopDlg dlg;
	dlg.pMat = &m_Mat;
	dlg.pElec = &m_Mat.m_Elec;
	dlg.pHole = &m_Mat.m_Hole;
	dlg.ni300 = m_Mat.m_ni300;
	dlg.m_Doping = m_BkgndDop;
	dlg.m_Type = (int)m_BkgndType;
	if (dlg.DoModal()==IDOK)
	{
		m_BkgndDop = dlg.m_Doping;
		m_BkgndType = (CMatType)dlg.m_Type;
		return TRUE;
	}
	else return FALSE;
}

BOOL CRegion::DoRecombinationDlg(int regionnum, int toppage)
{   
	CString title="Region "+(CString)(char)(regionnum+'1')+" Recombination";
	CPropertySheet sheet(title, NULL, toppage);
	CBulkDlg BulkPage;
	CSrecDlg FrontPage(IDS_FRONTSURFACECHARGE);
	CSrecDlg RearPage(IDS_REARSURFACECHARGE);
              
    // Bulk recombination
    
	BulkPage.pElec = &m_Mat.m_Elec;
	BulkPage.pHole = &m_Mat.m_Hole;
	BulkPage.pMat = &m_Mat;
	BulkPage.m_Type = m_BkgndType;
	BulkPage.m_BkgndDoping = CMath::round(m_BkgndDop,4);
	BulkPage.m_ni200 = m_Mat.m_ni200;
	BulkPage.m_ni300 = m_Mat.m_ni300;
	BulkPage.m_ni400 = m_Mat.m_ni400;
	BulkPage.m_BGNnNref = m_Mat.m_BGNnNref;
	BulkPage.m_BGNnSlope = m_Mat.m_BGNnSlope;
	BulkPage.m_BGNpNref = m_Mat.m_BGNpNref;
	BulkPage.m_BGNpSlope = m_Mat.m_BGNpSlope;
	BulkPage.m_Cn = m_Mat.m_Cn;
	BulkPage.m_Cp = m_Mat.m_Cp;
	BulkPage.m_Cnp = m_Mat.m_Cnp;
	BulkPage.m_B = m_Mat.m_BB;
	if (m_BkgndType==N_TYPE)
		{BulkPage.m_Nalpha = m_Mat.m_BulkNalphaN; BulkPage.m_Nref = m_Mat.m_BulkNrefN;}
	else
		{BulkPage.m_Nalpha = m_Mat.m_BulkNalphaP; BulkPage.m_Nref = m_Mat.m_BulkNrefP;}
	BulkPage.m_Et = m_BulkEt;
	BulkPage.m_TauN =  1e6 * m_TauN;	// s to us
	BulkPage.m_TauP =  1e6 * m_TauP;	// s to us
	BulkPage.m_Talpha = m_Mat.m_BulkTalpha;
	
	// Front surface recombination
	double Nbk = m_BkgndDop;
	double N1=0, N2=0;
	if (m_FrontDiff1.IsEnabled())
		N1 = CPhysics::Doping(0,m_FrontDiff1.m_Profile,m_FrontDiff1.m_Npeak,
						m_FrontDiff1.m_Depth,m_FrontDiff1.m_Xpeak);
	if (m_FrontDiff2.IsEnabled())
		N2 = CPhysics::Doping(0,m_FrontDiff2.m_Profile,m_FrontDiff2.m_Npeak,
						m_FrontDiff2.m_Depth,m_FrontDiff2.m_Xpeak);
	FrontPage.m_Type = m_BkgndType;
	if (N1>Nbk) {FrontPage.m_Type = m_FrontDiff1.m_Type;}
	if (N2>Nbk && N2>N1) {FrontPage.m_Type = m_FrontDiff2.m_Type;}
	FrontPage.m_Doping = CMath::round(Nbk+N1+N2,4);
	FrontPage.m_ni200 = m_Mat.m_ni200;
	FrontPage.m_ni300 = m_Mat.m_ni300;
	FrontPage.m_ni400 = m_Mat.m_ni400;
	FrontPage.m_BGNnNref = m_Mat.m_BGNnNref;
	FrontPage.m_BGNnSlope = m_Mat.m_BGNnSlope;
	FrontPage.m_BGNpNref = m_Mat.m_BGNpNref;
	FrontPage.m_BGNpSlope = m_Mat.m_BGNpSlope;
	if (FrontPage.m_Type == N_TYPE)
		{FrontPage.m_Nalpha = m_Mat.m_SurfNalphaN; FrontPage.m_Nref = m_Mat.m_SurfNrefN;}
	else
		{FrontPage.m_Nalpha = m_Mat.m_SurfNalphaP; FrontPage.m_Nref = m_Mat.m_SurfNrefP;}
	FrontPage.m_Et = m_FrontEt;
	FrontPage.m_Sn =  m_FrontSn;
	FrontPage.m_Sp =  m_FrontSp;
	FrontPage.m_Talpha = m_Mat.m_SurfTalpha;
	FrontPage.m_UseJo = m_FrontJo;
	
	// Rear surface recombination

	Nbk = m_BkgndDop;
	N1=0; N2=0;
	if (m_RearDiff1.IsEnabled())
		N1 = CPhysics::Doping(0,m_RearDiff1.m_Profile,m_RearDiff1.m_Npeak,
						m_RearDiff1.m_Depth,m_RearDiff1.m_Xpeak);
	if (m_RearDiff2.IsEnabled())
		N2 = CPhysics::Doping(0,m_RearDiff2.m_Profile,m_RearDiff2.m_Npeak,
						m_RearDiff2.m_Depth,m_RearDiff2.m_Xpeak);
	RearPage.m_Type = m_BkgndType;
	if (N1>Nbk) {RearPage.m_Type = m_RearDiff1.m_Type;}
	if (N2>Nbk && N2>N1) {RearPage.m_Type = m_RearDiff2.m_Type;}
	RearPage.m_Doping = CMath::round(Nbk+N1+N2,4);
	RearPage.m_ni200 = m_Mat.m_ni200;
	RearPage.m_ni300 = m_Mat.m_ni300;
	RearPage.m_ni400 = m_Mat.m_ni400;
	RearPage.m_BGNnNref = m_Mat.m_BGNnNref;
	RearPage.m_BGNnSlope = m_Mat.m_BGNnSlope;
	RearPage.m_BGNpNref = m_Mat.m_BGNpNref;
	RearPage.m_BGNpSlope = m_Mat.m_BGNpSlope;
	if (RearPage.m_Type == N_TYPE)
		{RearPage.m_Nalpha = m_Mat.m_SurfNalphaN; RearPage.m_Nref = m_Mat.m_SurfNrefN;}
	else
		{RearPage.m_Nalpha = m_Mat.m_SurfNalphaP; RearPage.m_Nref = m_Mat.m_SurfNrefP;}
	RearPage.m_Et = m_RearEt;
	RearPage.m_Sn =  m_RearSn;
	RearPage.m_Sp =  m_RearSp;
	RearPage.m_Talpha = m_Mat.m_SurfTalpha;
	RearPage.m_UseJo = m_RearJo;
	
	sheet.AddPage(&BulkPage);
	sheet.AddPage(&FrontPage);
	sheet.AddPage(&RearPage);
	if (sheet.DoModal()==IDOK) {
		m_BulkEt = BulkPage.m_Et;
		m_TauN =  1e-6 * BulkPage.m_TauN;	// us to s
		m_TauP =  1e-6 * BulkPage.m_TauP;	// us to s

		m_FrontEt = FrontPage.m_Et;
		m_FrontSn = FrontPage.m_Sn;
		m_FrontSp = FrontPage.m_Sp;
		m_FrontJo = FrontPage.m_UseJo;
		
		m_RearEt = RearPage.m_Et;
		m_RearSn = RearPage.m_Sn;
		m_RearSp = RearPage.m_Sp;
		m_RearJo = RearPage.m_UseJo;

		return TRUE;
	}
	else return FALSE;
}

BOOL CRegion::SetFrontDopingFile(CString path)
{
	BOOL FileOpen = TRUE;
	CString Ext = DOPING_EXT;
	CString Filter = "Doping (*."+Ext+")|*."+Ext+"|All Files (*.*)|*.*||";
	CFileDialog dlg(FileOpen, Ext, m_FrontFilename, OFN_OVERWRITEPROMPT, Filter);
	dlg.m_ofn.lpstrTitle = "Open Front Doping File";
	dlg.m_ofn.lpstrInitialDir = path;
	if (dlg.DoModal()==IDOK)
	{
		m_FrontFilename = dlg.GetPathName();
		m_FrontFilename.MakeLower();
		m_nFront = MAX_ELEMENTS;
		if (!CAscFile::Read(CPath::MakeFullPathname(m_FrontFilename, ((CPc1dApp *)AfxGetApp())->m_Path.dop), m_nFront, m_FrontPosition, m_FrontDopingDonor, m_FrontDopingAcceptor))
			AfxMessageBox("Error reading Doping file "+m_FrontFilename);
		for (int k=0; k<m_nFront; k++) m_FrontPosition[k] *= 1E-4;	// um to cm
		m_FrontFilename=CPath::MinimumNecessaryFilename(m_FrontFilename,((CPc1dApp *)AfxGetApp())->m_Path.dop);
		return TRUE;
	}
	else return FALSE;
}

BOOL CRegion::SetRearDopingFile(CString path)
{
	BOOL FileOpen = TRUE;
	CString Ext = DOPING_EXT;
	CString Filter = "Doping (*."+Ext+")|*."+Ext+"|All Files (*.*)|*.*||";
	CFileDialog dlg(FileOpen, Ext, m_RearFilename, OFN_OVERWRITEPROMPT, Filter);
	dlg.m_ofn.lpstrTitle = "Open Rear Doping File";
	dlg.m_ofn.lpstrInitialDir = path;
	if (dlg.DoModal()==IDOK)
	{
		m_RearFilename = dlg.GetPathName();
		m_RearFilename.MakeLower();
		m_nRear = MAX_ELEMENTS;
		if (!CAscFile::Read(CPath::MakeFullPathname(m_RearFilename, ((CPc1dApp *)AfxGetApp())->m_Path.dop), m_nRear, m_RearPosition, m_RearDopingDonor, m_RearDopingAcceptor))
			AfxMessageBox("Error reading Doping file "+m_RearFilename);
		for (int k=0; k<m_nRear; k++) m_RearPosition[k] *= 1E-4;	// um to cm
		m_RearFilename=CPath::MinimumNecessaryFilename(m_RearFilename, ((CPc1dApp *)AfxGetApp())->m_Path.dop);
		return TRUE;
	}
	else return FALSE;
}

///////////////////////////////////////////////////////////////////////////
// CReflectance (Two per Device)

IMPLEMENT_SERIAL(CReflectance, CObject, 0)

CReflectance::CReflectance()
{
	Initialize();
}

void CReflectance::Initialize()
{
	int k;
	m_bFixed=TRUE; m_bCoated=FALSE; m_bExternal=FALSE;
	m_Fixed = 0; m_Broadband = 0;
	for (k=0; k<MAX_LAYERS; k++) 
		{m_Thick[k]=0; m_Index[k]=1;}
	m_Filename.Empty();
	m_Internal1 = 0; m_Internal2 = 0; m_Rough = PLANAR_SURFACE;
	m_nLambda = 0;
	for (k=0; k<MAX_WAVELENGTHS; k++) m_Lambda[k] = m_Reflectance[k] = 0;
}

void CReflectance::Serialize(CArchive& ar)
{
	int k, num_layers=MAX_LAYERS;
	CObject::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << (WORD)m_bCoated << (WORD)m_bExternal;
		ar << CPath::MinimumNecessaryFilename(m_Filename, ((CPc1dApp *)AfxGetApp())->m_Path.ref );		
		ar << m_Fixed << m_Broadband << m_Internal1 << m_Internal2 << (WORD)m_Rough;
		ar << (WORD)num_layers;
		for (k=0; k<num_layers; k++) ar << m_Thick[k] << m_Index[k];
	}
	else
	{
		ar >> (WORD&)m_bCoated >> (WORD&)m_bExternal >> m_Filename;
		ar >> m_Fixed >> m_Broadband >> m_Internal1 >> m_Internal2 >> (WORD&)m_Rough;
		ar >> (WORD&)num_layers;
		for (k=0; k<num_layers; k++) ar >> m_Thick[k] >> m_Index[k];
		m_Filename=CPath::MinimumNecessaryFilename(m_Filename, ((CPc1dApp *)AfxGetApp())->m_Path.ref );
		m_bFixed=!(m_bCoated || m_bExternal);
		m_nLambda = MAX_WAVELENGTHS;
		if (m_bExternal)
			if (!CAscFile::Read( CPath::MakeFullPathname(m_Filename, ((CPc1dApp *)AfxGetApp())->m_Path.ref),
				m_nLambda, m_Lambda, m_Reflectance))
				AfxMessageBox("Error reading Reflectance file "+m_Filename);
	}
}

void CReflectance::TransferDataToDlg(CReflectDlg &dlg, CString OpenDlgTitle)
{
	dlg.m_Radio = 0;
	if (m_bCoated) dlg.m_Radio=1;
	if (m_bExternal) dlg.m_Radio=2;

	dlg.m_Fixed = (100*m_Fixed);	// normalized to percent

	dlg.m_Broadband = (100*m_Broadband);	// normalized to percent
	dlg.m_OuterThick = m_Thick[2];
	dlg.m_OuterIndex = m_Index[2];
	dlg.m_MiddleThick = m_Thick[1];
	dlg.m_MiddleIndex = m_Index[1];
	dlg.m_InnerThick = m_Thick[0];
	dlg.m_InnerIndex = m_Index[0];
	
	dlg.m_Filename = m_Filename;
	dlg.m_Ext = REFLECTANCE_EXT;
	dlg.m_Path = ((CPc1dApp *)AfxGetApp())->m_Path.ref;
	dlg.m_OpenDlgTitle = OpenDlgTitle;
}

void CReflectance::TransferDataFromDlg(CReflectDlg &dlg)
{	
	m_Fixed = (dlg.m_Fixed/100);	// percent to normalized
		
	m_Broadband = (dlg.m_Broadband/100);	// percent to normalized
	m_Thick[2] = dlg.m_OuterThick;
	m_Index[2] = dlg.m_OuterIndex;
	m_Thick[1] = dlg.m_MiddleThick;
	m_Index[1] = dlg.m_MiddleIndex;
	m_Thick[0] = dlg.m_InnerThick;
	m_Index[0] = dlg.m_InnerIndex;
		
	m_Filename = dlg.m_Filename;  

	m_bFixed=m_bCoated=m_bExternal=FALSE;		

	switch (dlg.m_Radio) {
	case 0: m_bFixed=TRUE;
			break; 
	case 1:	m_bCoated=TRUE;
			break;
	case 2:	m_bExternal=TRUE;
			m_nLambda = MAX_WAVELENGTHS;
			if (!CAscFile::Read(m_Filename, m_nLambda, m_Lambda, m_Reflectance))
				AfxMessageBox("Error reading Reflectance file "+m_Filename);
			break;
	}		
}


///////////////////////////////////////////////////////////////////////////
// CLumped (Four per Device)

IMPLEMENT_SERIAL(CLumped, CObject, 0)

CLumped::CLumped()
{
	Initialize();
}

void CLumped::Initialize()
{
	m_Enable = FALSE;
	m_Xa = 0; m_Xc = 0; m_n = 1;
	m_Type = CONDUCTOR;
	m_Value = 0;
	for (int k=0; k<MAX_TIME_STEPS+2; k++) m_Volts[k] = m_Amps[k] = 0;
}

void CLumped::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << (WORD)m_Enable << m_Xa << m_Xc << (WORD)m_Type;
		ar << m_n << m_Value;
	}
	else
	{
		ar >> (WORD&)m_Enable >> m_Xa >> m_Xc >> (WORD&)m_Type;
		ar >> m_n >> m_Value;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDevice (One per Document)

IMPLEMENT_SERIAL(CDevice, CObject, 0)

CDevice::CDevice()
{
	pList = new CObList(1);
	Initialize();
}

CDevice::~CDevice()
{
	while (!pList->IsEmpty()) delete (CRegion*)pList->RemoveTail();
	delete pList;
}

void CDevice::Initialize()
{
	while (!pList->IsEmpty()) delete (CRegion*)pList->RemoveTail();
	pR = new CRegion;
	pList->AddTail(pR);
	m_CurrentRegion = 0;
	m_NumRegions = 1;
	m_bModified = FALSE;
	m_Filename.Empty();
	m_FrontRfl.Initialize();
	m_RearRfl.Initialize();
	m_Area = 1;
	m_Aunit = CM2;
	m_FrontTexture = m_RearTexture = FALSE;
	m_FrontAngle = m_RearAngle = 54.74;	// deg
	m_FrontDepth = m_RearDepth = 3e-4;	// cm
	m_FrontSurface = m_RearSurface = NEUTRAL_SURFACE;
	m_FrontBarrier = m_RearBarrier = 0;
	m_FrontCharge = m_RearCharge = 0;
	m_EnableEmitter = TRUE;
	m_EnableBase = TRUE;
	m_EnableCollector = FALSE;
	m_EnableInternal = FALSE;
	m_EmitterR = m_BaseR = m_CollectorR = 1e-6;	// ohms
	m_EmitterX = 0;
	m_BaseX = m_CollectorX = 1;	// cm
	for (int k=0; k<MAX_LUMPED; k++) m_Lumped[k].Initialize();

	SetFileVersion(PC1DVERSION);
}

/////////////////////////////////////////////////////////////////////////////
// CDevice serialization

void CDevice::SetFileVersion(int PC1DVersion)
{
	int i;
	for (i=0; i<m_NumRegions; i++) {
		this->GetMaterialForRegion(i)->SetFileVersion(PC1DVersion);
	}
	m_FileVersion=1;
}

// the earliest version it could be saved as
// This will be the earliest form the material can be saved as.
int CDevice::GetFileVersion()
{
	int ver_num;
	ver_num=41;	//lowest PC1D which actually has a version number
	int i;
	int tmp;
	for (i=0; i<m_NumRegions; i++) {
		tmp=GetMaterialForRegion(i)->GetFileVersion();
		if (tmp>ver_num) ver_num=tmp;
	}
	return ver_num;
}

void CDevice::Serialize(CArchive& ar)
{
	WORD version;
	POSITION pos;
	int i, k, num_lumped=MAX_LUMPED;
	CObject::Serialize(ar);
	if (ar.IsStoring())
	{
		m_FrontRfl.Serialize(ar);	// Can't be earlier due to Initialize()
		m_RearRfl.Serialize(ar);
		ar << (WORD)num_lumped;
		for (k=0; k<num_lumped; k++) m_Lumped[k].Serialize(ar);
		ar << CPath::MinimumNecessaryFilename(m_Filename, ((CPc1dApp *)AfxGetApp())->m_Path.dev );
		version=DEVICEFILE_VERSION;
		if (version>0) {
			if (m_bModified) ar << (WORD)0xDC01; else ar << (WORD)0xDC00;
			ar << version;
		} else	ar << (WORD)m_bModified;
		// anything after this can use version number. Everything before, we're stuck with!
		ar << (WORD)m_CurrentRegion << (WORD)m_NumRegions;
		pos = pList->GetHeadPosition();
		for (i=0; i<m_NumRegions; i++) pList->GetNext(pos)->Serialize(ar);
		ar << m_Area << (WORD)m_Aunit;
		ar << (WORD)m_FrontTexture << m_FrontAngle << m_FrontDepth;
		ar << (WORD)m_RearTexture << m_RearAngle << m_RearDepth;
		ar << (WORD)m_FrontSurface << m_FrontBarrier << m_FrontCharge;
		ar << (WORD)m_RearSurface << m_RearBarrier << m_RearCharge;
		ar << (WORD)m_EnableEmitter << (WORD)m_EnableBase;
		ar << (WORD)m_EnableCollector << (WORD)m_EnableInternal;
		ar << m_EmitterR << m_BaseR << m_CollectorR;
		ar << m_EmitterX << m_BaseX << m_CollectorX;
	}
	else
	{
		Initialize();	// Necessary to free region objects
		m_FrontRfl.Serialize(ar);	// Must come after Initialize()
		m_RearRfl.Serialize(ar);
		ar >> (WORD&)num_lumped;
		for (k=0; k<num_lumped; k++) m_Lumped[k].Serialize(ar);
		ar >> m_Filename >> (WORD&)m_bModified;
		if ((m_bModified & 0xFF00) == 0xDC00) { ar >> version; m_bModified &=0x00FF;} 
		else version=0;		
		ar >> (WORD&)m_CurrentRegion >> (WORD&)m_NumRegions;
		pR->Serialize(ar);	// Read first region into existing object
		for (i=1; i<m_NumRegions; i++)	// Create any remaining regions
			{pR = new CRegion; pList->AddTail(pR); pR->Serialize(ar);}
		pos = pList->FindIndex(m_CurrentRegion);
		pR = (CRegion*)pList->GetAt(pos);
		ar >> m_Area >> (WORD&)m_Aunit;
		ar >> (WORD&)m_FrontTexture >> m_FrontAngle >> m_FrontDepth;
		ar >> (WORD&)m_RearTexture >> m_RearAngle >> m_RearDepth;
		ar >> (WORD&)m_FrontSurface >> m_FrontBarrier >> m_FrontCharge;
		ar >> (WORD&)m_RearSurface >> m_RearBarrier >> m_RearCharge;
		ar >> (WORD&)m_EnableEmitter >> (WORD&)m_EnableBase;
		ar >> (WORD&)m_EnableCollector >> (WORD&)m_EnableInternal;
		ar >> m_EmitterR >> m_BaseR >> m_CollectorR;
		ar >> m_EmitterX >> m_BaseX >> m_CollectorX;
		m_Filename=CPath::MinimumNecessaryFilename(m_Filename, ((CPc1dApp *)AfxGetApp())->m_Path.dev );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDevice functions

double CDevice::GetThickness()
{
	double thick=0;
	// total thickness of device
	for (POSITION pos=pList->GetHeadPosition(); pos!=NULL; ) {
		CRegion *pR =(CRegion*)pList->GetNext(pos);
		thick+=pR->m_Thickness;
	}
	return thick;
}


void CDevice::InsertRegion(int region_num)
{
	CRegion* pRnew = new CRegion;	// allocate space for a new region on the heap
	pRnew->Copy(pR);				// copy contents of current region into it
	pR = pRnew;			// make the new region the current region
	if (region_num>m_NumRegions-1) pList->AddTail(pR);
	else
	{
		POSITION pos = pList->FindIndex(region_num);	// index is zero-based
		pList->InsertBefore(pos, pR);	// add the new region to the linked list
	}
	m_NumRegions++;
	m_CurrentRegion = region_num;
}

void CDevice::RemoveRegion(int region_num)
{
	POSITION pos = pList->FindIndex(region_num);	// Index is zero-based
	ASSERT(pos!=NULL);
	delete (CRegion*)pList->GetAt(pos);
	pList->RemoveAt(pos);
	m_NumRegions--;
	if (m_CurrentRegion+1>m_NumRegions) SetCurrentRegionNumber(m_CurrentRegion-1);
}

void CDevice::SetCurrentRegionNumber(int region_num)
{
	if (region_num>m_NumRegions-1) region_num=m_NumRegions-1; // added by DAC 10/4/96 
	
	POSITION pos = pList->FindIndex(region_num);	// Index is zero-based
	pR = (CRegion*)pList->GetAt(pos);
	m_CurrentRegion = region_num;	
}

BOOL CDevice::OnDeviceOpen()
{
	BOOL FileOpen = TRUE;
	CString Ext = DEVICE_EXT;
	CString Filter = "Device (*."+Ext+")|*."+Ext+"|All Files (*.*)|*.*||";
	CFileDialog dlg(FileOpen, Ext, m_Filename, OFN_OVERWRITEPROMPT, Filter);
	dlg.m_ofn.lpstrTitle = "Open Device File";
	dlg.m_ofn.lpstrInitialDir = ((CPc1dApp *)AfxGetApp())->m_Path.dev;	
	if (dlg.DoModal()==IDOK)
	{
		CFile f;
		if (!f.Open(dlg.GetPathName(), CFile::modeRead))
		{
			AfxMessageBox("Unable to open device file");
			return FALSE;
		}
		else 
		{
			CArchive ar(&f, CArchive::load); 
			Serialize(ar);
			m_Filename = dlg.GetPathName();
			m_Filename.MakeLower();
			m_bModified = FALSE;
			return TRUE;
		}
	}
	else return FALSE;
}

void CDevice::OnDeviceSaveas()
{
	CString Ext = DEVICE_EXT;
	CString Filter = "Device (*.dev)|*.dev|PC1D 4.5 Device (*.dev)|*.dev|All Files (*.*)|*.*||";
	CFileDialog dlg(FALSE, Ext, m_Filename, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, Filter);
	dlg.m_ofn.lpstrTitle = "Save Device File";
	dlg.m_ofn.lpstrInitialDir = ((CPc1dApp *)AfxGetApp())->m_Path.dev;	
	if (GetFileVersion()<=46) dlg.m_ofn.nFilterIndex=2;
						else dlg.m_ofn.nFilterIndex=1;
	if (dlg.DoModal()==IDOK)
	{
		CFile f;
		if (!f.Open(dlg.GetPathName(), CFile::modeCreate | CFile::modeWrite))
		{
			AfxMessageBox("Unable to create device file");
		}
		else 
		{
			if (dlg.m_ofn.nFilterIndex==2) {
				SetFileVersion(45);
			} else {
				SetFileVersion(PC1DVERSION);
			}
			CArchive ar(&f, CArchive::store); 
			Serialize(ar);
			m_Filename = dlg.GetPathName();
			m_Filename.MakeLower();
			m_bModified = FALSE;
		}
	}
}

BOOL CDevice::DoAreaDlg()
{
	CAreaDlg dlg;
	if (m_Aunit==MM2) dlg.m_Area = (100*m_Area);
	else if (m_Aunit==UM2) dlg.m_Area = (1e8*m_Area);
	else dlg.m_Area = m_Area;
	dlg.m_Aunit = (int)m_Aunit;
	if (dlg.DoModal()==IDOK)
	{
		m_Aunit = (CAreaUnit)dlg.m_Aunit;
		if (m_Aunit==MM2) m_Area = (dlg.m_Area/100);
		else if (m_Aunit==UM2) m_Area = (dlg.m_Area/1e8);
		else m_Area = dlg.m_Area;
		return TRUE;
	}
	return FALSE;
}


BOOL CDevice::DoTextureDlg()
{
	CTextrDlg dlg;
	dlg.m_CheckFrontTexture=m_FrontTexture;
	dlg.m_FrontAngle = m_FrontAngle;
	dlg.m_FrontDepth = 1e4*m_FrontDepth;	// cm to um
	dlg.m_CheckRearTexture=m_RearTexture;
	dlg.m_RearAngle = m_RearAngle;
	dlg.m_RearDepth = 1e4*m_RearDepth;		// cm to um
	if (dlg.DoModal()==IDOK)
	{
		m_FrontTexture=dlg.m_CheckFrontTexture;
		m_FrontAngle = dlg.m_FrontAngle;
		m_FrontDepth = 1e-4*dlg.m_FrontDepth;	// um to cm
		m_RearTexture=dlg.m_CheckRearTexture;
		m_RearAngle = dlg.m_RearAngle;
		m_RearDepth = 1e-4*dlg.m_RearDepth;	// um to cm
		return TRUE;
	}
	return FALSE;
}

BOOL CDevice::DoSurfaceDlg(int toppage)
{
	CPropertySheet sheet("Surface Charge", NULL, toppage);
	CSurfaceDlg FrontPage(IDS_FRONTSURFACECHARGE), RearPage(IDS_REARSURFACECHARGE);
	FrontPage.m_Barrier = m_FrontBarrier;
	FrontPage.m_Charge  = m_FrontCharge;
	switch(m_FrontSurface) {
		case NEUTRAL_SURFACE: FrontPage.m_SurfaceType=0; break;
		case CHARGED_SURFACE: FrontPage.m_SurfaceType=1; break;
		case BARRIER_SURFACE: FrontPage.m_SurfaceType=2; break;
	}

	RearPage.m_Barrier = m_RearBarrier;
	RearPage.m_Charge  = m_RearCharge;
	switch(m_RearSurface) {
		case NEUTRAL_SURFACE: RearPage.m_SurfaceType=0; break;
		case CHARGED_SURFACE: RearPage.m_SurfaceType=1; break;
		case BARRIER_SURFACE: RearPage.m_SurfaceType=2; break;
	}
	
	sheet.AddPage(&FrontPage);
	sheet.AddPage(&RearPage);
	if (sheet.DoModal()==IDOK) {
	
		m_FrontCharge  = FrontPage.m_Charge;
		m_FrontBarrier = FrontPage.m_Barrier;
		switch (FrontPage.m_SurfaceType) {
			case 0: m_FrontSurface=NEUTRAL_SURFACE; break;
			case 1: m_FrontSurface=CHARGED_SURFACE; break;
			case 2: m_FrontSurface=BARRIER_SURFACE; break;
		}	
	
		m_RearCharge  = RearPage.m_Charge;
		m_RearBarrier = RearPage.m_Barrier;
		switch (RearPage.m_SurfaceType) {
			case 0: m_RearSurface=NEUTRAL_SURFACE; break;
			case 1: m_RearSurface=CHARGED_SURFACE; break;
			case 2: m_RearSurface=BARRIER_SURFACE; break;
		}	
		return TRUE;
	}
	return FALSE;
}

BOOL	CDevice::DoReflectanceDlg(int toppage)
{
	CPropertySheet sheet("Reflectance", NULL, toppage);
	CIntrnDlg InternalPage;
	CReflectDlg FrontPage(IDS_FRONTREFL);
	CReflectDlg RearPage(IDS_REARREFL);

	InternalPage.m_Front1 = (100*m_FrontRfl.m_Internal1);	// normalized to percent
	InternalPage.m_Front2 = (100*m_FrontRfl.m_Internal2);
	InternalPage.m_Rear1 = (100*m_RearRfl.m_Internal1);
	InternalPage.m_Rear2 = (100*m_RearRfl.m_Internal2);
	InternalPage.m_FrontRough = (int)m_FrontRfl.m_Rough;
	InternalPage.m_RearRough = (int)m_RearRfl.m_Rough;
	m_FrontRfl.TransferDataToDlg(FrontPage, "Open Front Reflectance File");
	m_RearRfl.TransferDataToDlg(RearPage, "Open Rear Reflectance File");
	
    sheet.AddPage(&FrontPage);
    sheet.AddPage(&RearPage);
	sheet.AddPage(&InternalPage);
	if (sheet.DoModal()==IDOK) {
		m_FrontRfl.m_Internal1 = (InternalPage.m_Front1/100);	// percent to normalized
		m_FrontRfl.m_Internal2 = (InternalPage.m_Front2/100);
		m_RearRfl.m_Internal1 = (InternalPage.m_Rear1/100);
		m_RearRfl.m_Internal2 = (InternalPage.m_Rear2/100);
		m_FrontRfl.m_Rough = (CTexture)InternalPage.m_FrontRough;
		m_RearRfl.m_Rough = (CTexture)InternalPage.m_RearRough;
		
		m_FrontRfl.TransferDataFromDlg(FrontPage);
		m_RearRfl.TransferDataFromDlg(RearPage);
		return TRUE;
	}
	else return FALSE;
}	


BOOL CDevice::DoContactsDlg()
{
	CCntctDlg dlg;
	dlg.m_EmitterEnable = m_EnableEmitter;
	dlg.m_EmitterResistance=m_EmitterR;
	dlg.m_EmitterPosition=1e4*m_EmitterX;	// cm to um
	dlg.m_BaseEnable = m_EnableBase;
	dlg.m_BaseResistance=m_BaseR;
	dlg.m_BasePosition=1e4*m_BaseX;			// cm to um
	dlg.m_CollectorEnable = m_EnableCollector;
	dlg.m_CollectorResistance=m_CollectorR;
	dlg.m_CollectorPosition=1e4*m_CollectorX;	// cm to um
	if (dlg.DoModal()==IDOK)
	{
		m_EnableEmitter = dlg.m_EmitterEnable;
		m_EmitterR = dlg.m_EmitterResistance;
		m_EmitterX = 1e-4*dlg.m_EmitterPosition;	// um to cm
		m_EnableBase = dlg.m_BaseEnable;
		m_BaseR = dlg.m_BaseResistance;
		m_BaseX = 1e-4*dlg.m_BasePosition;			// um to cm
		m_EnableCollector = dlg.m_CollectorEnable;
		m_CollectorR = dlg.m_CollectorResistance;
		m_CollectorX = 1e-4*dlg.m_CollectorPosition;	// um to cm
		return TRUE;
	}
	return FALSE;
}

BOOL CDevice::DoInternalElementsDlg()
{
	CLumpDlg dlg;
	dlg.m_Enable1 = m_Lumped[0].m_Enable;
	dlg.m_Enable2 = m_Lumped[1].m_Enable;
	dlg.m_Enable3 = m_Lumped[2].m_Enable;
	dlg.m_Enable4 = m_Lumped[3].m_Enable;
	dlg.m_Type1 = (int)m_Lumped[0].m_Type;
	dlg.m_Type2 = (int)m_Lumped[1].m_Type;
	dlg.m_Type3 = (int)m_Lumped[2].m_Type;
	dlg.m_Type4 = (int)m_Lumped[3].m_Type;
	dlg.m_Value1 = m_Lumped[0].m_Value;
	dlg.m_Value2 = m_Lumped[1].m_Value;
	dlg.m_Value3 = m_Lumped[2].m_Value;
	dlg.m_Value4 = m_Lumped[3].m_Value;
	dlg.m_n1 = m_Lumped[0].m_n;
	dlg.m_n2 = m_Lumped[1].m_n;
	dlg.m_n3 = m_Lumped[2].m_n;
	dlg.m_n4 = m_Lumped[3].m_n;
	dlg.m_Xa1 = (1e4*m_Lumped[0].m_Xa);	// cm to um
	dlg.m_Xa2 = (1e4*m_Lumped[1].m_Xa);
	dlg.m_Xa3 = (1e4*m_Lumped[2].m_Xa);
	dlg.m_Xa4 = (1e4*m_Lumped[3].m_Xa);
	dlg.m_Xc1 = (1e4*m_Lumped[0].m_Xc);
	dlg.m_Xc2 = (1e4*m_Lumped[1].m_Xc);
	dlg.m_Xc3 = (1e4*m_Lumped[2].m_Xc);
	dlg.m_Xc4 = (1e4*m_Lumped[3].m_Xc);
	if (dlg.DoModal()==IDOK)
	{
		m_Lumped[0].m_Enable = dlg.m_Enable1;
		m_Lumped[1].m_Enable = dlg.m_Enable2;
		m_Lumped[2].m_Enable = dlg.m_Enable3;
		m_Lumped[3].m_Enable = dlg.m_Enable4;
		m_Lumped[0].m_Type = (CElemType)dlg.m_Type1;
		m_Lumped[1].m_Type = (CElemType)dlg.m_Type2;
		m_Lumped[2].m_Type = (CElemType)dlg.m_Type3;
		m_Lumped[3].m_Type = (CElemType)dlg.m_Type4;
		m_Lumped[0].m_Value = dlg.m_Value1;
		m_Lumped[1].m_Value = dlg.m_Value2;
		m_Lumped[2].m_Value = dlg.m_Value3;
		m_Lumped[3].m_Value = dlg.m_Value4;
		m_Lumped[0].m_n = dlg.m_n1;
		m_Lumped[1].m_n = dlg.m_n2;
		m_Lumped[2].m_n = dlg.m_n3;
		m_Lumped[3].m_n = dlg.m_n4;
		m_Lumped[0].m_Xa = (dlg.m_Xa1/1e4);	// um to cm
		m_Lumped[1].m_Xa = (dlg.m_Xa2/1e4);
		m_Lumped[2].m_Xa = (dlg.m_Xa3/1e4);
		m_Lumped[3].m_Xa = (dlg.m_Xa4/1e4);
		m_Lumped[0].m_Xc = (dlg.m_Xc1/1e4);
		m_Lumped[1].m_Xc = (dlg.m_Xc2/1e4);
		m_Lumped[2].m_Xc = (dlg.m_Xc3/1e4);
		m_Lumped[3].m_Xc = (dlg.m_Xc4/1e4);
		m_EnableInternal = 
			dlg.m_Enable1 || dlg.m_Enable2 || dlg.m_Enable3 || dlg.m_Enable4;
		return TRUE;
	}
	else return FALSE;
}
