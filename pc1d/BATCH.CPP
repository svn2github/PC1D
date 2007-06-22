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
// BATCH.CPP
//	CBatch implementation file
// Nomenclature: 
// sim : solving of a system for a given fixed set of parameters
//     = one line in a batch job
// batch : a list of several sims to be solved.
//
//	To add more loopable parameters: You only need to change batch.cpp and batch.h (and the help file!)
// <1> Extend the "BatchParameter" list in batch.h
//	   Make sure that LastRegionParameter is updated if necessary
// <2> Add the text to m_BatchParamTextList in batch.cpp
//	   The order must be the same as <1>!!!!
// <3> Add the appropriate calculation (input or output) in batch.cpp
//     Input parameters: add the calculation to SetBatchParameter() and GetBatchParameter()
//		 Make sure unit conversions are correct.
//	   Non-spatial results parameters: add calculation to GetResultValue()
//	   Spatial results: add to array in GetSpatialResultValue()
//-------------------------------------------------------------------------------

#include "stdafx.h"
#include "path.h"
#include "pc1d.h"
#include "mathstat.h" 	// CMath
#include "problem.h"
#include "plotload.h"
#include "useful.h"		// Clipboard functions
#include "QuickBatchDlg.h"

char *CBatch::m_BatchParamDescriptList[]={
"Area",			"Area of device",					"cm2",
"FrTxAngle",	"Front texture angle",				"deg",
"FrTxDepth",	"Front texture depth",				"um",
"RrTxAngle",	"Rear texture angle",				"deg",
"RrTxDepth",	"Rear texture depth",				"um",
"FrBarrier",	"Height of front surface barrier (+ve = bands bend up)",	"eV",
"FrCharge",		"Front surface charge",				"cm-2",
"RrBarrier",	"Height of rear surface barrier (+ve= bands bend up)",		"eV",
"RrCharge",		"Rear surface charge",				"cm-2",
"EmitterR",		"Emitter internal resistance",		"ohms",
"BaseR",		"Base internal resistance",			"ohms",
"CollectorR",	"Collector internal resistance",	"ohms",
"EmitterX",		"Emitter distance from front",		"um",
"BaseX",		"Base distance from front",			"um",
"CollectorX",	"Collector distance from front",	"um",
"Shunt1",		"Value of 1st shunt element",		"ohm-1",
"Shunt2",		"Value of 2nd shunt element",		"ohm-1",
"Shunt3",		"Value of 3rd shunt element",		"ohm-1",
"Shunt4",		"Value of 4th shunt element",		"ohm-1",
"Shunt1Xa",		"Anode position of 1st shunt",		"um",
"Shunt1Xc",		"Cathode position of 1st shunt",	"um",
"Shunt2Xa",		"Anode position of 2nd shunt",		"um",
"Shunt2Xc",		"Cathode position of 2nd shunt",	"um",
"Shunt3Xa",		"Anode position of 3rd shunt",		"um",
"Shunt3Xc",		"Cathode position of 3rd shunt",	"um",
"Shunt4Xa",		"Anode position of 4th shunt",		"um",
"Shunt4Xc",		"Cathode position of 4th shunt",	"um",
"FrRefl",		"Front reflectance (fixed)",		"%",
"RrRefl",		"Rear reflectance (fixed)",			"%",
"FrBroadRef",	"Front broadband reflectance",		"%",
"RrBroadRef",	"Rear broadband reflectance",		"%",
"FrOutThick",	"Thickness of outer AR coating",				"nm",
"FrMidThick",	"Thickness of front middle AR coating",			"nm",
"FrInThick",	"Thickness of front inner AR coating",			"nm",
"FrOutIndex",	"Refractive index of front outer AR coating",	"",
"FrMidIndex",	"Refractive index of front middle AR coating",	"",
"FrInIndex",	"Refractive index of front inner AR coating",	"",
"RrOutThick",	"Thickness of rear outer AR coating",			"nm",
"RrMidThick",	"Thickness of rear middle AR coating",			"nm",
"RrInThick",	"Thickness of rear inner AR coating",			"nm",
"RrOutIndex",	"Refractive index of rear outer AR coating",	"",
"RrMidIndex",	"Refractive index of rear middle AR coating",	"",
"RrInIndex",	"Refractive index of rear inner AR coating",	"",
"FrIntRefl1",	"Front:First internal reflection",				"%",
"FrIntRefl2",	"Subsequent internal reflection",				"%",
"RrIntRefl1",	"Rear: First internal reflection",				"%",
"RrIntRefl2",	"Subsequent internal reflection",				"%",
"FrIntRefl",	"Front internal reflection FrIntRefl1=FrIntRefl2=FrIntRefl", "%",
"RrIntRefl",	"Rear internal reflection RrIntRefl1=RrIntRefl2=RrIntRefl",	"%",

// Region parameters

"Thickness",	"Thickness of region",							"um",
"BkgndDop",		"Background doping",							"cm-3",
"FrDopPeak1",	"1st Front diffusion - peak doping",			"cm-3",
"FrDopDpth1",	"1st Front diffusion - depth factor",			"um",
"FrDopPos1",	"1st Front diffusion - peak position",          "um",          
"FrDopPeak2",	"2nd front diffusion - peak doping",            "cm-3",
"FrDopDpth2",	"2nd front diffusion - depth factor",			"um",
"FrDopPos2",	"2nd front diffusion - peak position",			"um",
"RrDopPeak1",	"1st rear diffusion - peak doping",				"cm-3",
"RrDopDpth1",	"1st rear diffusion - depth factor",			"um",
"RrDopPos1",	"1st rear diffusion - peak position",			"um",
"RrDopPeak2",	"2nd rear diffusion - peak doping",				"cm-3",
"RrDopDpth2",	"2nd rear diffusion - depth factor",			"um",
"RrDopPos2",	"2nd rear diffusion - peak position",			"um",
"BulkTaun",		"Bulk recombination: electron lifetime",		"us",
"BulkTaup",		"Bulk recombination: hole lifetime",			"us",
"BulkTau",		"Bulk recombination, taun=taup=tau",			"us",
"BulkEt",		"Bulk trap energy level",						"eV",
"FrSn",			"Front surface electron recombination velocity","cm/s",
"FrSp",			"Front surface hole recombination velocity",	"cm/s",
"FrEt",			"Front surface trap energy level",				"eV",
"RrSn",			"Rear surface electron recombination velocity",	"cm/s",
"RrSp",			"Rear surface hole recombination velocity",		"cm/s",
"RrEt",			"Rear surface trap energy level",				"eV",
"FrS",			"Front surface recomb. velocity FrSn=FrSp=FrS",	"cm/s",
"RrS",			"Rear surface recomb. velocity RrSn=RrSp=RrS",	"cm/s",

"BandGap",		"BandGap",										"eV",
"AbsEd1",		"1st Direct BandGap (for absorption)",			"eV",
"AbsEd2",		"2nd Direct Bandgap",							"eV",
"AbsEi1",		"1st Indirect BandGap",							"eV",
"AbsEi2",		"2nd Indirect BandGap",							"eV",
"Ni200", 		"Intrinsic concentation at 200K",				"",
"Ni300",		"Intrinsic concentation at 300K",				"",
"Ni400",		"Intrinsic concentation at 400K",				"",

// Excitation parameters

"Temp",			"Temperature of device",                     	"kelvin",
"BaseResSS",	"Base steady-state resistance",              	"ohms",  
"BaseResTR",	"Base transient resistance",                 	"ohms",  
"BaseVltSS",	"Base steady-state voltage",                 	"V",     
"BaseVltTR1",	"Base transient initial voltage",            	"V",     
"BaseVltTR2",	"Base transient final voltage",              	"V",     
"CollResSS",	"Collector steady-state resistance",         	"ohms",  
"CollResTR",	"Collector transient resistance",            	"ohms",  
"CollVltSS",	"Collector steady-state voltage",            	"V",     
"CollVltTR1",	"Collector transient initial voltage",       	"V",     
"CollVltTR2",	"Collector transient final voltage",         	"V",     
"PriInsySS",	"Primary source steady-state intensity",     	"Wcm-2", 
"PriInsyTR1",	"Primary source initial transient intensity",	"Wcm-2", 
"PriInsyTR2",	"Primary source final transient intensity",  	"Wcm-2", 
"PriMonoSS",	"Pri: Wavelength - steady state",            	"nm",    
"PriMonoTR1",	"Pri: initial transient wavelength",         	"nm",    
"PriMonoTR2",	"Pri: final transient wavelength",           	"nm",    
"PriBlackT",	"Pri: Blackbody temperature",                	"kelvin",
"SecInsySS",	"Secondary source steady-state intensity",   	"Wcm-2", 
"SecInsyTR1",	"Sec: Initial transient intensity",          	"Wcm-2", 
"SecInsyTR2",	"Sec: Final transient intensity",            	"Wcm-2", 
"SecMonoSS",	"Sec: Wavelength - steady state",            	"nm",    
"SecMonoTR1",	"Sec: Initial transient wavelength",         	"nm",    
"SecMonoTR2",	"Sec: Final transient wavelength",           	"nm",    
"SecBlackT",	"Sec: Blackbody temperature",                	"kelvin",

// Numerical parameters

"ElemSize",		"Element size factor",			"",
"ErrorLimit",	"Normalized error limit",		"",
"Clamp",		"Normalized potential clamp",	""

};

char *CBatch::m_BatchResultDescriptList[] = {

	// Result parameters

"BaseVoc", 		"Voc, base contact",							"V",
"BaseIsc", 		"Isc, base contact",							"V",
"BasePmax",		"Pmax, base contact",							"A",
"CollVoc", 		"Voc, collector contact",						"A",
"CollIsc",		"Isc, collector contact",						"V",
"CollPmax",		"Pmax, collector contact",						"A",
"Vb", 			"Base voltage",									"W",
"Ib", 			"Base current",									"V",
"Vc",			"Collector voltage",							"A",
"Ic",			"Collector current",							"W",
"V1",			"Voltage across 1st shunt element",				"V",
"I1",			"Current through 1st shunt element",			"A",
"V2",			"Voltage across 2nd shunt element",				"V",
"I2",			"Current through 1st shunt element",			"A",
"V3",			"Voltage across 3rd shunt element",				"V",
"I3",			"Current through 1st shunt element",			"A",
"V4",			"Voltage across 4th shunt element",				"V",
"I4",			"Current through 1st shunt element",			"A",
"RmsExptDiff",	"RMS of BiasedIQE-expt data",					"",
"NumElements",	"Number of finite elements used",				"",
	
	// Spatial graphs

"Na",			"Acceptor Doping Density",						"cm-3",
"RBulk",		"Bulk Recombination Rate",						"cm-3/s",
"Rho",			"Charge Density",								"C/cm3",
"Ec",			"Conduction Band Edge",							"eV",
"Cond",			"Conductivity",									"S/cm",
"CCum",			"Cumulative Conductivity",						"S",
"CCum_",		"Cumulative Excess Conductivity",				"S",
"Gcum",			"Cumulative Photogeneration",					"s-1",
"Rcum",			"Cumulative Recombination",						"s-1",
"Perm",			"Dielectric Constant (Permeability)",			"",
"Ld",			"Diffusion Length",								"m",
"Nd",			"Donor Doping Density",							"cm-3",
"Eg",			"Effective Energy Gap",							"eV",
"Nie",			"Effective Intrinsic Concentration",			"cm-3",
"E",			"Electric Field",								"V/cm",
"In",			"Electron Current",								"A",
"Jn",			"Electron Current Density",						"A/cm2",
"N",			"Electron Density",								"cm-3",
"MuN",			"Electron Mobility",							"cm2/Vs",
"PhiN",			"Electron Quasi-Fermi Energy",					"eV",
"Vn",			"Electron Velocity",							"cm/s",
"Psi",			"Electrostatic Potential",						"V",
"Rho_",			"Excess Charge Density",						"C/cm3",
"Cond_",		"Excess Conductivity",							"S/cm",
"N_",			"Excess Electron Density",						"cm-3",
"Psi_",			"Excess Electrostatic Potential",				"V",
"P_",			"Excess Hole Density",							"cm-3",
"G",			"Generation Rate",								"cm-3/s",
"Ip",			"Hole Current",									"A",
"Jp",			"Hole Current Density",							"A/cm2",
"P",			"Hole Density",									"cm-3",
"MuP",			"Hole Mobility",								"cm2/Vs",
"PhiP",			"Hole Quasi-Fermi Energy",						"eV",
"Vp",			"Hole Velocity",								"cm/s",
"Tau",			"LLI Carrier Lifetime",							"s",
"Res",			"Resistivity",									"ohm*cm",
"It",			"Total Current",								"A",
"Jt",			"Total Current Density",						"A/cm2",
"Evac",			"Vacuum Energy",								"eV",
"Ev",			"Valence Band Edge",							"eV",
"Nratio",		"Excess Electron Density Ratio",				"",
"Pratio",		"Excess Hole Density Ratio",					"",
"PNratio",		"Excess pn Product Ratio",						"",
"PNnorm",		"Normalized Excess pn Product",					"",
"Ndrift",		"Electron Drift Current Density",				"A/cm2",
"Ndiff",		"Electron Diffusion Current Density",			"A/cm2",
"Pdrift",		"Hole Drift Current Density",					"A/cm2",
"Pdiff",		"Hole Diffusion Current Density",				"A/cm2"
	
};

BOOL CBatch::IsResultsParameter(BatchParameter p)
{
	if (p >= FirstResultsParameter) return TRUE;
	return FALSE;
}

BOOL CBatch::IsRegionParameter(BatchParameter p)
{
	return (p>=FirstRegionParameter && p<=LastRegionParameter);
}

BOOL CBatch::IsSpatialParameter(BatchParameter p)
{
	return (p>=FirstSpatialResult);
}

CString CBatch::BatchParam2Str(BatchParameter bp)
{
	if (bp<0) return "";
	else if (bp<FirstResultsParameter) return m_BatchParamDescriptList[bp*3];
	else return m_BatchResultDescriptList[(bp-FirstResultsParameter)*3];
}

CString CBatch::GetUnitsStr(BatchParameter bp)
{
	if (bp<0) return "";
	else if (bp<FirstResultsParameter) return m_BatchParamDescriptList[bp*3+2];
	else return m_BatchResultDescriptList[(bp-FirstResultsParameter)*3+2];
}

CString CBatch::GetDescriptionStr(BatchParameter bp)
{
	if (bp<0) return "";
	else if (bp<FirstResultsParameter) return m_BatchParamDescriptList[bp*3+1];
	else return m_BatchResultDescriptList[(bp-FirstResultsParameter)*3+1];
}

CBatch::BatchParameter CBatch::Str2BatchParam(CString str)
{
	BatchParameter i;
	for (i=FirstBatchParameter; i<=LastResultsParameter; i++) {
		if (!str.CompareNoCase(BatchParam2Str(i)) ) return i;
	}
/**
	for (i=FirstBatchParameter; i<FirstResultsParameter; i++) {
		if ( !str.CompareNoCase(m_BatchParamDescriptList[i*3]) ) return i;
	}
	for (i=FirstResultsParameter; i<=LastResultsParameter; i++) {
		if ( !str.CompareNoCase(m_BatchResultDescriptList[(i-FirstResultsParameter)*3]) ) return i;
	}
***/
	return NoBatchParameter;
}

////////////////////////////////////////////////////////////////////////////////////////////
// CBatch

IMPLEMENT_SERIAL(CBatch, CObject,1)

CBatch::CBatch()
{
	Initialize();
}

void CBatch::SetFileVersion(int PC1DVersion)
{
	// Batch file version:
	// 5.3 was the first version with internal batch
	if (PC1DVersion<53) m_FileVersion=1;
	else m_FileVersion=2;
}

// the earliest version it could be saved as
int CBatch::GetFileVersion()
{
	// Material file version:
	// 5.3 was the first version with internal batch
	if (m_nTotalQuickSims==0) return 40;
	return 53;
}


void CBatch::Serialize(CArchive& ar)
{
	int num_parms;
	CString strname;
	CString reservedstr="";
	int i;

	CObject::Serialize(ar);
	if (ar.IsStoring())
	{
		if (m_FileVersion>1 && m_nTotalQuickSims==0) m_FileVersion=1;	// don't save internal batch if don't need to
		ar << (WORD)m_FileVersion;
		ar << (WORD)m_Enable;
		ar << CPath::MinimumNecessaryFilename(m_Filename, ((CPc1dApp *)AfxGetApp())->m_Path.bat );
		if (m_FileVersion==2) {
			ar << (WORD) m_bExternal;
			ar << reservedstr;
			num_parms=8;
			ar << num_parms;
			for (i=0; i<num_parms; i++) {
				strname = BatchParam2Str(m_Quickfieldparam[i]);
				ar << strname;
				ar << m_Quickregionnum[i];
				ar << m_Quickfieldxcoord[i];
				ar << m_QuickFrom[i];
				ar << m_QuickTo[i];
				ar << (WORD) m_bQuickIsLog[i];

				int junk_int=0;
				if (i<MAXNUMBEROFPERMUTES) ar << m_nQuickSteps[i];
				else ar << junk_int;		

				ar << (WORD) m_bQuickIsSame[i];
				ar << reservedstr;
			}
		}
	} else {
		Initialize();
		ar >> (WORD &)m_FileVersion; // version number
		if (m_FileVersion > 2) 
		{
			AfxThrowArchiveException(CArchiveException::badIndex);
			return;
		}
		ar >> (WORD &)m_Enable;
		ar >> m_Filename;
		m_Filename=CPath::MinimumNecessaryFilename(m_Filename, ((CPc1dApp *)AfxGetApp())->m_Path.bat );
		if (m_FileVersion==2) {
			ar >> (WORD &)m_bExternal;
			ar >> reservedstr;
			ar >> num_parms;
			for (i=0; i<num_parms; i++) {
				ar >> strname;
				m_Quickfieldparam[i] = Str2BatchParam(strname);

				ar >> m_Quickregionnum[i];
				ar >> m_Quickfieldxcoord[i];
				ar >> m_QuickFrom[i];
				ar >> m_QuickTo[i];
				ar >> (WORD &) m_bQuickIsLog[i];
				int junk_int;
				if (i<MAXNUMBEROFPERMUTES) ar >> m_nQuickSteps[i];
				else ar >> junk_int;

				ar >> (WORD &)m_bQuickIsSame[i];
				ar >> reservedstr;		// for formulae

			}
		}
		if (m_Enable) {
			if (m_bExternal) Read();
			else GenerateQuickBatch();
		}
	}
}

void CBatch::Initialize()
{
	m_Enable=FALSE;
	m_bExternal=TRUE;
	m_Filename.Empty();
	m_numfields=0;
	m_numlines=0;
	m_simnum=0;
	m_bStartOfSim=TRUE;
	int i;
	for (i=0; i<MAXBATCHFIELDS; i++){
		m_QuickFrom[i]=m_QuickTo[i]=0;
		m_bQuickIsLog[i]=FALSE;
		m_bQuickIsSame[i]=TRUE;
		m_Quickregionnum[i]=1;
		m_Quickfieldparam[i]=(BatchParameter)-1;	// = empty
		m_Quickfieldxcoord[i]=0;
	}
	m_bQuickIsSame[0]=FALSE;

	for (i=0; i<MAXNUMBEROFPERMUTES; i++) {
		m_nQuickSteps[i]=2;
	}	
	InvalidateBatchResults();
}

// Like load from file, except it's from string. Return FALSE if error
// E.g. used for Edit:Paste
BOOL CBatch::LoadFromString(CString InputSring)
{
	LPSTR buff=InputSring.GetBuffer(10);
	char *p;
	p=strtok(buff, "\t \n");
    while (m_numfields<MAXBATCHFIELDS && p!=NULL){ 
		char *q=p;
		BOOL hasnumber=FALSE;
		while (*q!=0 && *q!='(') q++;
		if (*q=='(') { *q=0; q++; hasnumber=TRUE; }

		m_fieldparam[m_numfields] = Str2BatchParam(p);
//		(int &)m_fieldparam[m_numfields] = FindStringInArray(p,m_BatchParamTextList);
    	if (m_fieldparam[m_numfields]==-1) {
			return FALSE;
    	} else if (IsRegionParameter(m_fieldparam[m_numfields])) {
    		m_regionnum[m_numfields]=atoi(q);
    		if (!hasnumber) return FALSE;
    	} else if (IsSpatialParameter(m_fieldparam[m_numfields])) {
    		m_fieldxcoord[m_numfields]=atof(q);
			if (!hasnumber) return FALSE;
    	}
//        m_paramname[m_numfields]=p;
        m_numfields++; 
        p=strtok(NULL, "\t \n");
	}
	m_ParamLine.SetSize(0);
	m_numlines=0;
	while (p!=NULL) {
		int n=0;
		CBatchLine sim;
		while (n<m_numfields && p!=NULL) {
			sim.field[n]=atof(p);
        	p=strtok(NULL, "\t \n");
        	n++;
		}
		while (n<m_numfields){ sim.field[n]=0; n++; }
		sim.m_bConverged=FALSE;
        m_numlines++;
		m_ParamLine.Add(sim);
		p=strtok(NULL, "\t \n");
	}
	InputSring.ReleaseBuffer();

	InvalidateBatchResults();
    return (m_numlines>0);
}

BOOL CBatch::Read(void)
{
	CString str;							   
	CStdioFile f;
	if (!f.Open(CPath::MakeFullPathname(m_Filename, ((CPc1dApp *)AfxGetApp())->m_Path.bat),
		CFile::modeRead | CFile::typeText)) { m_Enable=FALSE; return FALSE; };
	
	if (!f.ReadString(str)) return FALSE;

	ConvertToTabbedString(str);
	CString fieldstr;
	m_numfields=GetNumFieldsInString(str);
	int n;
	for (n=0; n<m_numfields; n++) {
		fieldstr=GetNthTabbedField(str, n);
		// need to split up into:   paramname ( paramnum )
		CString paramname=fieldstr;  //  name of the parameter
		double paramnum;	//  region or position
		BOOL hasnumber=FALSE;
		for (int q=0; q<fieldstr.GetLength(); q++) {
			if (fieldstr[q]=='(') {
				paramname=fieldstr.Left(q);
				paramnum=atof(fieldstr.Mid(q+1));
				hasnumber=TRUE;
				break;
			}
		}
		m_fieldparam[n] = Str2BatchParam(paramname);
    	if (m_fieldparam[n]==-1) {
    		AfxMessageBox("Unrecognized parameter:"+paramname);
    	} else if (IsRegionParameter(m_fieldparam[n])) {
    		if (!hasnumber) {
				AfxMessageBox("Must specify a region number for "+paramname+". Region 1 assumed.");
				paramnum=1;
			}
    		m_regionnum[n]=(int)paramnum;
    	} else if (IsSpatialParameter(m_fieldparam[n])) {
    		if (!hasnumber) {
				AfxMessageBox("Must specify a position for " +paramname);
				paramnum=0;
			}
    		m_fieldxcoord[n]=paramnum;
    	}
	}
	m_ParamLine.SetSize(0);
	m_numlines=0;
	while (f.ReadString(str)) {
		ConvertToTabbedString(str);

		CBatchLine sim;
		for (n=0; n<m_numfields; n++) {
			fieldstr=GetNthTabbedField(str, n);
			sim.field[n]=atof(fieldstr);
		}
		sim.m_bConverged=FALSE;
        m_numlines++;
		m_ParamLine.Add(sim);
	}

	InvalidateBatchResults();
    return (m_numlines>0);
}

// if in (NULL-terminated) array, returns the index; if not found, returns -1.              
int CBatch::FindStringInArray(CString str, char *strlist[])
{
	int i;
	for (i=0; strlist[i]!=NULL; i++) {
		if ( !str.CompareNoCase(strlist[i]) ) return i;
	}
	return -1;
}


// get name of field n
CString CBatch::GetFieldName(int fieldnum)
{
	CString str;
	BatchParameter p=m_fieldparam[fieldnum];
	if (p==-1) return "<Error>";
	else {
		if (IsRegionParameter(p)) {
			str.Format("(%d)",m_regionnum[fieldnum]);
			return BatchParam2Str(p) +str;
//			return m_BatchParamTextList[p]+(CString)str;
		} if (IsSpatialParameter(p)) {
			str.Format("(%.4g)", m_fieldxcoord[fieldnum]);
			return BatchParam2Str(p) +str;
//			return m_BatchParamTextList[p]+(CString)str;		
		}else return BatchParam2Str(p);
//			return m_BatchParamTextList[p];
	}
}

CBatch::BatchParameter CBatch::GetFieldType(int fieldnum)
{
	return (BatchParameter)m_fieldparam[fieldnum];
}

int CBatch::GetRegionNumForField(int fieldnum)
{
	if (IsRegionParameter(m_fieldparam[fieldnum]))	return m_regionnum[fieldnum];
	else return 0;
}


double CBatch::GetValue(int fieldnum)
{
	  return m_ParamLine[m_simnum].field[fieldnum];
}

// For displaying values in parmview, you must be able to access any element
double CBatch::GetValue(int linenum, int fieldnum)
{
	return m_ParamLine[linenum].field[fieldnum];
}

BOOL CBatch::IsValueValid(int linenum, int fieldnum)
{
	if (m_ParamLine[linenum].m_bConverged) return TRUE; 
//	if (m_simnum>linenum) return TRUE;
	return !IsResultsParameter(GetFieldType(fieldnum));
}

void CBatch::SetValue(int fieldnum, double value)
{
	  m_ParamLine[m_simnum].field[fieldnum]=value;
}

void CBatch::OnStartBatch()
{
	if (!m_bStartOfSim || (m_simnum!=0 && m_simnum<GetNumberOfLines())) {
		// we were stopped halfway through a batch last time
		// m_OriginalWindowTitle has already been set
	} else 	AfxGetMainWnd()->GetWindowText(m_OriginalWindowTitle);
	m_simnum=0;
	m_bStartOfSim=TRUE;
	InvalidateBatchResults();
}

// display [ Batch 1 of n ] in title
void CBatch::ShowProgress(void)
{
	char str[100];
	sprintf(str, " [Batch %d of %d]", m_simnum+1, m_numlines);
	AfxGetMainWnd()->SetWindowText(m_OriginalWindowTitle+str);		
}

void CBatch::TerminateBatch(void)
{
	AfxGetMainWnd()->SetWindowText(m_OriginalWindowTitle);
}

void CBatch::SaveOriginalParams(CProblem *pProb)
{
	int fnum;
	BatchParameter p;
	for (fnum=0; fnum<GetNumberOfFields(); fnum++) {
		p=GetFieldType(fnum);
		if (!IsResultsParameter(p)) {
			if (IsRegionParameter(p)) {
//				int SaveRegion = pDoc->pD->m_CurrentRegion;
				int newreg=GetRegionNumForField(fnum);
//				if (newreg>0) pDoc->pD->SelectRegion(newreg-1);
				m_savedparamvalue[fnum]=GetParameterValue(pProb, p, newreg-1);				
//				pDoc->pD->SelectRegion(SaveRegion);
			} else m_savedparamvalue[fnum]=GetParameterValue(pProb, p, 0);
		}
	}
}
		
void CBatch::RestoreOriginalParams(CProblem *pProb)
{
	int fnum;
	BatchParameter p;
	for (fnum=0; fnum<GetNumberOfFields(); fnum++) {
		p=GetFieldType(fnum);
		if (!IsResultsParameter(p)) {
			if (IsRegionParameter(p)) {
//				int SaveRegion = pDoc->pD->m_CurrentRegion;
				int newreg=GetRegionNumForField(fnum);
//				if (newreg>0) pDoc->pD->SelectRegion(newreg-1);
				SetParameterValue(pProb, p, m_savedparamvalue[fnum], newreg-1);				
//				pDoc->pD->SelectRegion(SaveRegion);
			} else SetParameterValue(pProb, p, m_savedparamvalue[fnum],0);
		}
	}
}

void CBatch::InvalidateBatchResults(void)
{
	int i, fnum;
	for (fnum=0; fnum<GetNumberOfFields(); fnum++) {
		BatchParameter p=GetFieldType(fnum);
		if (IsResultsParameter(p)) {
			for (i=0; i<GetNumberOfLines(); i++) m_ParamLine[i].field[fnum]=0;
		}
	}
	for (i=0; i<GetNumberOfLines(); i++) m_ParamLine[i].m_bConverged=FALSE;
}

void CBatch::OnSimFinished(CProblem *pProb)
{
	int fnum;
	for (fnum=0; fnum<GetNumberOfFields(); fnum++) {
		BatchParameter p=GetFieldType(fnum);
		if (IsResultsParameter(p)) {
			if (IsSpatialParameter(p)) 
					 SetValue(fnum, GetSpatialResultValue(pProb, p, m_fieldxcoord[fnum])); 
				else SetValue(fnum, GetResultValue(pProb, p));
		}
	}
	m_ParamLine[m_simnum].m_bConverged=TRUE;
	m_simnum++;
	m_bStartOfSim=TRUE;
	RestoreOriginalParams(pProb);
	if (IsBatchFinished()) TerminateBatch();
}

void CBatch::OnSimFailed(CProblem *pProb)
{
	m_ParamLine[m_simnum].m_bConverged=FALSE;
	m_simnum++;
	m_bStartOfSim=TRUE;
	RestoreOriginalParams(pProb);
	if (IsBatchFinished()) TerminateBatch();
}

void CBatch::OnSimBegin(CProblem *pProb)
{
	m_bStartOfSim=FALSE;
	SaveOriginalParams(pProb);	
	int fnum;
	for (fnum=0; fnum<GetNumberOfFields(); fnum++) {
			BatchParameter p=GetFieldType(fnum);
		if (!IsResultsParameter(p)) {
			if (IsRegionParameter(p)) {
//				int SaveRegion = pDoc->pD->m_CurrentRegion;
				int newreg=GetRegionNumForField(fnum);
//				if (newreg>0) pDoc->pD->SelectRegion(newreg-1);
				SetParameterValue(pProb, p, GetValue(fnum), newreg-1);				
//				pDoc->pD->SelectRegion(SaveRegion);
			} else SetParameterValue(pProb, p, GetValue(fnum), 0);
		}
	}
	ShowProgress();
}

BOOL CBatch::IsBatchFinished(void)
{   
	ASSERT(m_Enable);
	return m_simnum>=GetNumberOfLines();
}

BOOL CBatch::IsAtStartOfNewSim(void)
{ 
	return m_bStartOfSim;
}

int CBatch::GetNumberOfFailedSimulations(void)
{
	int num;
	int i;
	num=0;
	for (i=0; i<GetNumberOfLines(); i++) if (!m_ParamLine[i].m_bConverged) num++;
	return num;
}

// Puts the data for each batch onto the clipboard using the CF_TEXT format
// i.e. num TAB num TAB num CRLF 
//		num TAB num TAB num CRLF
//		0
void CBatch::CopyEntireBatchToClipboard(CWnd *)
{
    int n;
    int i;
    char outstr[200]; 
    CString myString;
	CString sxunit,syunit;

	myString="Batch run from:"+m_Filename+"\r\n";
	myString+=GetFieldName(0);
	for (i=1; i<GetNumberOfFields(); i++) {
		myString+="\t"+GetFieldName(i);
	}
	myString+="\r\n";
	    
	for(n=0; n<GetNumberOfLines(); n++) {
		if (IsValueValid(n,0))	{ 
			sprintf(outstr, "%g", GetValue(n,0));
			myString+=outstr;
		} // else value isn't calculated yet
		for (i=1; i<GetNumberOfFields(); i++) {
			if (IsValueValid(n,i)) {	
				sprintf(outstr, "%g", GetValue(n,i));
				myString+="\t"+(CString)outstr;
			} else { // value hasn't been calculated yet
				myString+="\t";
			}
		}
    	myString+="\r\n";
	}
	CopyStringToClipboard(myString);
}

/***
BOOL CBatch::DoBatchDlg()
{
	CBatchDlg dlg;
	dlg.m_Filename = m_Filename;
	dlg.m_Enable = m_Enable;
	dlg.m_Path = ((CPc1dApp *)AfxGetApp())->m_Path.bat;
	if (dlg.DoModal()==IDOK) 
	{
		m_Filename=CPath::MinimumNecessaryFilename(dlg.m_Filename, ((CPc1dApp *)AfxGetApp())->m_Path.bat );
		m_Enable=dlg.m_Enable;
		m_bExternal=TRUE;
		if (dlg.m_Enable) if (!Read()) {
			m_Enable=FALSE;
			AfxMessageBox("Error reading batch parameter file. Batch mode has been disabled.");
			return FALSE;
		}
		return TRUE;
	} else return FALSE;
}
***/

BOOL CBatch::DoQuickBatchDlg()
{
	int k;
	CQuickBatchDlg dlg;
//	dlg.m_maxallowedregion=5;
	dlg.m_Reg1=m_Quickregionnum[0];
	dlg.m_Reg2=m_Quickregionnum[1];
	dlg.m_Reg3=m_Quickregionnum[2];
	dlg.m_Reg4=m_Quickregionnum[3];
	dlg.m_From1=m_QuickFrom[0];
	dlg.m_From2=m_QuickFrom[1];
	dlg.m_From3=m_QuickFrom[2];
	dlg.m_From4=m_QuickFrom[3];
	dlg.m_To1=m_QuickTo[0];
	dlg.m_To2=m_QuickTo[1];
	dlg.m_To3=m_QuickTo[2];
	dlg.m_To4=m_QuickTo[3];
	dlg.m_Log1=m_bQuickIsLog[0];
	dlg.m_Log2=m_bQuickIsLog[1];
	dlg.m_Log3=m_bQuickIsLog[2];
	dlg.m_Log4=m_bQuickIsLog[3];
	dlg.m_Steps1=m_nQuickSteps[0];
	dlg.m_Steps2=m_nQuickSteps[1];
	dlg.m_Steps3=m_nQuickSteps[2];
	dlg.m_Steps4=m_nQuickSteps[3];
	// first entry is never same as previous
	dlg.m_Same1=!m_bQuickIsSame[1];
	dlg.m_Same2=!m_bQuickIsSame[2];
	dlg.m_Same3=!m_bQuickIsSame[3];
	for (k=0; k<4; k++) dlg.m_Parm[k] = BatchParam2Str(m_Quickfieldparam[k]);
	for (k=0; k<4; k++) dlg.m_Result[k] = BatchParam2Str(m_Quickfieldparam[4+k]);

	dlg.m_Filename = m_Filename;
	dlg.m_Path = ((CPc1dApp *)AfxGetApp())->m_Path.bat;
	int rad;
	rad=0;
	if (!m_Enable) rad=0; 
	else if (m_bExternal) rad=1; else rad=2; 
	dlg.m_Radio = rad;

	if (dlg.DoModal()==IDOK) {
		m_Quickregionnum[0]=dlg.m_Reg1;
		m_Quickregionnum[1]=dlg.m_Reg2;
		m_Quickregionnum[2]=dlg.m_Reg3;
		m_Quickregionnum[3]=dlg.m_Reg4;
		m_QuickFrom[0]=dlg.m_From1;
		m_QuickFrom[1]=dlg.m_From2;
		m_QuickFrom[2]=dlg.m_From3;
		m_QuickFrom[3]=dlg.m_From4;
		m_QuickTo[0]=dlg.m_To1;
		m_QuickTo[1]=dlg.m_To2;
		m_QuickTo[2]=dlg.m_To3;
		m_QuickTo[3]=dlg.m_To4;
		m_bQuickIsLog[0]=dlg.m_Log1;
		m_bQuickIsLog[1]=dlg.m_Log2;
		m_bQuickIsLog[2]=dlg.m_Log3;
		m_bQuickIsLog[3]=dlg.m_Log4;
		m_nQuickSteps[0]=dlg.m_Steps1;
		m_nQuickSteps[1]=dlg.m_Steps2;
		m_nQuickSteps[2]=dlg.m_Steps3;
		m_nQuickSteps[3]=dlg.m_Steps4;

		for (k=0; k<4; k++)	m_Quickfieldparam[k]=Str2BatchParam(dlg.m_Parm[k]);
		for (k=0; k<4; k++)	m_Quickfieldparam[k+4]=Str2BatchParam(dlg.m_Result[k]);
		m_bQuickIsSame[0]=FALSE;
		m_bQuickIsSame[1]=!dlg.m_Same1;
		m_bQuickIsSame[2]=!dlg.m_Same2;
		m_bQuickIsSame[3]=!dlg.m_Same3;

		m_Filename=CPath::MinimumNecessaryFilename(dlg.m_Filename, ((CPc1dApp *)AfxGetApp())->m_Path.bat );
		rad=dlg.m_Radio;
		if (rad==1) {
			m_Enable=TRUE;
			m_bExternal=TRUE;
			if (!Read()) {
				m_Enable=FALSE;
				AfxMessageBox("Error reading batch parameter file. Batch mode has been disabled.");
				return FALSE;
			}
		}else if (rad==2) {
			m_Enable=TRUE;
			m_bExternal=FALSE;
			GenerateQuickBatch();
		} else {
			m_Enable=FALSE;
		}
		return TRUE;
	} else return FALSE;
}

double GetIntermediateValue(double frac, double left, double right, BOOL bUseLog)
{
	if (bUseLog) {
		if (left>SMALL_REAL)		return left  * exp(      frac  * log(right/left) );
		else if (right>SMALL_REAL) 	return right * exp( (1 - frac) * log(left/right) );
		else return left;  // both L and R are tiny
	}
	else return left + frac * (right - left);	// linear
}


void CBatch::GenerateQuickBatch()
{
	m_Enable=TRUE;
	m_bExternal=FALSE;

	m_numfields=0;
	int numpermutes=0;
	int permuteindex[MAXBATCHFIELDS];	// which field does each permutation belong to?
	int nPermuteSteps[MAXNUMBEROFPERMUTES];


	// set up headings
	BOOL bShouldIncrement;
	int i;
	int k;
	bShouldIncrement=TRUE;
	numpermutes=-1;

	for (i=0; i<4; i++) {
		if (m_Quickfieldparam[i]>=0) {
			if (bShouldIncrement) {
				numpermutes++;
				nPermuteSteps[numpermutes]=m_nQuickSteps[i];
			}
			m_fieldparam[m_numfields]=m_Quickfieldparam[i];
			m_regionnum[m_numfields]=m_Quickregionnum[i];
			permuteindex[m_numfields]=numpermutes;
			m_numfields++;
			if (!m_bQuickIsSame[i+1]) bShouldIncrement=TRUE;
			else bShouldIncrement=FALSE;
		}
	}
	if (bShouldIncrement || m_numfields>0) numpermutes++;
	for (i=4; i<8; i++) {
		if (m_Quickfieldparam[i]>=0) {
			m_fieldparam[m_numfields]=m_Quickfieldparam[i];
			permuteindex[m_numfields]=-1;
			m_numfields++;
		}
	}
	if (numpermutes==0) {
		// There are no sims to perform!
		m_nTotalQuickSims=0;
		m_Enable=FALSE;
		return;
	}
	m_nTotalQuickSims=1;
	for (i=0; i<numpermutes; i++) {
//		m_nTotalQuickSims*=m_nQuickSteps[i];
		m_nTotalQuickSims*=nPermuteSteps[i];
	}
	if (m_nTotalQuickSims>200) {
		CString warningstr;
		if (m_nTotalQuickSims<=1000) {
			warningstr.Format("Warning! This will create %d simulations. Do you still want to continue?", m_nTotalQuickSims);
		} else {
			warningstr.Format("WARNING!! This will create %d simulations, which may exceed the capacity of PC1D. Do you still want to continue?", m_nTotalQuickSims); 
			MessageBeep(MB_ICONEXCLAMATION);
		}
		if (IDOK!=AfxMessageBox(warningstr, MB_OKCANCEL)) {
			m_Enable=FALSE;
			return;
		}

	}


	int permute[MAXNUMBEROFPERMUTES];
	for (i=0; i<numpermutes; i++) permute[i]=0;

	m_ParamLine.RemoveAll();
	m_ParamLine.SetSize(m_nTotalQuickSims);
	m_numlines=0;

	BOOL bAllDone=FALSE;
	do {
		// Add sim line for current permutation
		CBatchLine sim;
		for (k=0; k<m_numfields; k++) {
			if (permuteindex[k]<0) sim.field[k]=0; // input field
			else {
				double frac;				
				if (nPermuteSteps[permuteindex[k]]==1) frac=0;
				else frac = ((double)permute[permuteindex[k]]) / ((double)nPermuteSteps[permuteindex[k]]-1);

//				if (m_nQuickSteps[permuteindex[k]]==1) frac=0;
//				else frac = ((double)permute[permuteindex[k]]) / ((double)m_nQuickSteps[permuteindex[k]]-1);
				sim.field[k]=GetIntermediateValue(frac,  m_QuickFrom[k], m_QuickTo[k], m_bQuickIsLog[k]);
			}
		}
		sim.m_bConverged=FALSE;
//		m_ParamLine.Add(sim);
		m_ParamLine.SetAt(m_numlines, sim);
		m_numlines++;
		
		// Move to the next permutation of the permute[] array
		BOOL bDone=FALSE;
		i = numpermutes-1;
		do {
			permute[i]++;
//			if (permute[i]>=m_nQuickSteps[i]) {
			if (permute[i]>=nPermuteSteps[i]) {
				permute[i]=0;
				i--;
				if (i<0) bAllDone=bDone=TRUE;
			} else bDone=TRUE;
		} while (!bDone);

	} while (!bAllDone);
}

/***********

// given 'fieldstr', sets up parameter #n with paramtype, region number.
void CBatch::ParseParameterName(CString fieldstr, int n)
{
		// need to split up into:   paramname ( paramnum )
		CString paramname=fieldstr;  //  name of the parameter
		double paramnum;	//  region or position
		BOOL hasnumber=FALSE;
		for (int q=0; q<fieldstr.GetLength(); q++) {
			if (fieldstr[q]=='(') {
				paramname=fieldstr.Left(q);
				paramnum=atof(fieldstr.Mid(q+1));
				hasnumber=TRUE;
				break;
			}
		}
		(int &)m_fieldparam[n] = FindStringInArray(paramname,m_BatchParamTextList);
    	if (m_fieldparam[n]==-1) {
//    		AfxMessageBox("Unrecognized parameter:"+paramname);
m_fieldparam[n]=FrontTextureAngle;
    	} else if (IsRegionParameter(m_fieldparam[n])) {
    		if (!hasnumber) {
//				AfxMessageBox("Must specify a region number for "+paramname+". Region 1 assumed.");
				paramnum=1;
			}
    		m_regionnum[n]=(int)paramnum;
    	} else if (IsSpatialParameter(m_fieldparam[n])) {
    		if (!hasnumber) {
//				AfxMessageBox("Must specify a position for " +paramname);
				paramnum=0;
			}
    		m_fieldxcoord[n]=paramnum;
    	}
}
********/
/*******
void CBatch::CreateQuickBatchFromText()
{
	m_Enable=TRUE;
	m_Filename="QuickBatch";
	m_numfields=2;
	int numinputfields=1;
	CStringArray paramnamearray;
	paramnamearray.SetSize(0);
	paramnamearray.Add("FrTxDepth");
	paramnamearray.Add("BaseVoc");
	int n=0;
	BOOL bIsLog;
	double from, to;
	char *instr;
	char buff[80]; 
	for (n=0; n<m_numfields; n++) {
			if (n==0) instr="LOG(BkDop(1),1e16,1e18)";
			else instr="BaseVoc";

			if (instr[0]=='L' || instr[2]=='O' && instr[2]=='G') bIsLog=TRUE; else bIsLog=FALSE;
			// skip '('
			char *q=instr+4;
			char *p=buff;
			for (q=instr+4; *q!=','; q++) *p++=*q;
			*p=0;
			// now parse name
			ParseParameterName(buff, n);
			// now
			q++; // skip comma
			p=buff;
			for (; *q!=','; q++) *p++=*q;
			from = atof(q);
			q++; // skip comma
			p=buff;
			for (; *q!=')'; q++) *p++=*q;
			to=atof(q);
			q++; // skip )
		CString paramname=paramnamearray[n];
		int paramnum=0;
		(int &)m_fieldparam[n] = FindStringInArray(paramname,m_BatchParamTextList);
   		if (IsRegionParameter(m_fieldparam[n])) {
   			m_regionnum[n]=(int)paramnum;
   		} else if (IsSpatialParameter(m_fieldparam[n])) {
   			m_fieldxcoord[n]=paramnum;
		}
	}

	m_ParamLine.SetSize(0);
	m_numlines=0;
	while (m_numlines<10) {
		CBatchLine sim;
		for (n=0; n<numinputfields; n++) {
			sim.field[n]=m_numlines*10.5;
		}
		sim.m_bConverged=FALSE;
        m_numlines++;
		m_ParamLine.Add(sim);
	}
}
***/

void CBatch::SetParameterValue(CProblem *pProb, BatchParameter p, double value, int region_num)
{
	CDevice *pD=pProb->GetDevice();
	CExcite *pE=pProb->GetExcite();
	CRegion *pR=pD->GetRegion(region_num);
	CMaterial *pMat = pD->GetMaterialForRegion(region_num);
	CSolve *pS=pProb->GetSolve();
	switch (p)
	{
	// Device parameters
	case CBatch::Area: 		pD->m_Area=value;		break;
		// Texturing - front & rear
	case CBatch::FrontTextureAngle:		pD->m_FrontAngle=value;			break;
	case CBatch::FrontTextureDepth:		pD->m_FrontDepth=value*1e-4;	break; // um to cm
	case CBatch::RearTextureAngle:		pD->m_RearAngle=value;			break;
	case CBatch::RearTextureDepth:		pD->m_RearDepth=value*1e-4;		break; // um to cm
		// Surface charge - front & rear
	case CBatch::FrontBarrier:		pD->m_FrontBarrier=value;		break;
	case CBatch::FrontCharge:		pD->m_FrontCharge=value;		break;
	case CBatch::RearBarrier:		pD->m_RearBarrier=value;		break;
	case CBatch::RearCharge:		pD->m_RearCharge=value;			break;
		// Contacts - Internal resistance +  position
	case CBatch::EmitterInternRes:	pD->m_EmitterR=value;			break;
	case CBatch::BaseInternRes:		pD->m_BaseR=value;				break;
	case CBatch::CollInternRes:		pD->m_CollectorR=value;			break;
	case EmitterX:					pD->m_EmitterX=value*1e-4;		break;
	case BaseX:						pD->m_BaseX=value*1e-4;			break;
	case CollectorX:				pD->m_CollectorX=value*1e-4;	break;
		// Shunt elements
	case CBatch::Shunt1Value:		pD->GetLumpedElement(0)->m_Value=value;	break;
	case CBatch::Shunt2Value: 		pD->GetLumpedElement(1)->m_Value=value;	break;
	case CBatch::Shunt3Value:		pD->GetLumpedElement(2)->m_Value=value;	break;
	case CBatch::Shunt4Value:		pD->GetLumpedElement(3)->m_Value=value;	break;

	case CBatch::Shunt1Anode:		pD->GetLumpedElement(0)->m_Xa=value*1e-4; break; // um to cm
	case CBatch::Shunt1Cathode:		pD->GetLumpedElement(0)->m_Xc=value*1e-4; break; 
	case CBatch::Shunt2Anode:		pD->GetLumpedElement(1)->m_Xa=value*1e-4; break; 
	case CBatch::Shunt2Cathode:		pD->GetLumpedElement(1)->m_Xc=value*1e-4; break; 
	case CBatch::Shunt3Anode:		pD->GetLumpedElement(2)->m_Xa=value*1e-4; break; 
	case CBatch::Shunt3Cathode:		pD->GetLumpedElement(2)->m_Xc=value*1e-4; break; 
	case CBatch::Shunt4Anode:		pD->GetLumpedElement(3)->m_Xa=value*1e-4; break; 
	case CBatch::Shunt4Cathode:		pD->GetLumpedElement(3)->m_Xc=value*1e-4; break; 

		// Reflectance-front & rear
	case CBatch::FrontFixedReflectance:		pD->m_FrontRfl.m_Fixed=value/100;	break; // % to normalized
	case CBatch::FrontBroadbandReflectance:	pD->m_FrontRfl.m_Broadband=value/100; break; // % to normalized
	case CBatch::FrOuterThickness:			pD->m_FrontRfl.m_Thick[2]=value;	break;
	case CBatch::FrMiddleThickness:			pD->m_FrontRfl.m_Thick[1]=value;	break;
	case CBatch::FrInnerThickness:			pD->m_FrontRfl.m_Thick[0]=value;	break;
	case CBatch::FrOuterIndex:				pD->m_FrontRfl.m_Index[2]=value;	break;
	case CBatch::FrMiddleIndex:				pD->m_FrontRfl.m_Index[1]=value;	break;
	case CBatch::FrInnerIndex:				pD->m_FrontRfl.m_Index[0]=value;	break;
	case CBatch::FrontInternReflFirst:		pD->m_FrontRfl.m_Internal1=value/100;	break; // % to normalized
	case CBatch::FrontInternReflSubsequent:	pD->m_FrontRfl.m_Internal2=value/100;	break; // % to normalized
	
	case CBatch::RearFixedReflectance:		pD->m_RearRfl.m_Fixed=value/100;	break;
	case CBatch::RearBroadbandReflectance:	pD->m_RearRfl.m_Broadband=value/100; break;
	case CBatch::RrOuterThickness:			pD->m_RearRfl.m_Thick[2]=value;		break;
	case CBatch::RrMiddleThickness:			pD->m_RearRfl.m_Thick[1]=value;		break;
	case CBatch::RrInnerThickness:			pD->m_RearRfl.m_Thick[0]=value;		break;
	case CBatch::RrOuterIndex:				pD->m_RearRfl.m_Index[2]=value;		break;
	case CBatch::RrMiddleIndex:				pD->m_RearRfl.m_Index[1]=value;		break;
	case CBatch::RrInnerIndex:				pD->m_RearRfl.m_Index[0]=value;		break;
	case CBatch::RearInternReflFirst:		pD->m_RearRfl.m_Internal1=value/100;	break; // % to normalized
	case CBatch::RearInternReflSubsequent:	pD->m_RearRfl.m_Internal2=value/100;	break; // % to normalized

	case CBatch::FrontInternRefl:		pD->m_FrontRfl.m_Internal1 = pD->m_FrontRfl.m_Internal2 = value/100; break; // % to normalized
	case CBatch::RearInternRefl:		pD->m_RearRfl.m_Internal1 = pD->m_RearRfl.m_Internal2 = value/100; break; // % to normalized
		
	// Region parameters		
	case CBatch::Thickness:				pR->m_Thickness=value*1e-4;  break; // um to cm
	
	case CBatch::BackgroundDoping:		pR->m_BkgndDop=value;		break;
		// Doping -front
	case CBatch::FrontDopingPeak1: 		pR->m_FrontDiff1.m_Npeak=value;			break;
	case CBatch::FrontDopingDepth1: 	pR->m_FrontDiff1.m_Depth=value*1e-4;	break; // um to cm
	case CBatch::FrontDopingPos1: 		pR->m_FrontDiff1.m_Xpeak=value*1e-4;	break;
	case CBatch::FrontDopingPeak2: 		pR->m_FrontDiff2.m_Npeak=value;			break;
	case CBatch::FrontDopingDepth2: 	pR->m_FrontDiff2.m_Depth=value*1e-4;	break;
	case CBatch::FrontDopingPos2: 		pR->m_FrontDiff2.m_Xpeak=value*1e-4;	break;
		// Doping - rear		
	case CBatch::RearDopingPeak1: 		pR->m_RearDiff1.m_Npeak=value;			break;
	case CBatch::RearDopingDepth1: 		pR->m_RearDiff1.m_Depth=value*1e-4;		break;
	case CBatch::RearDopingPos1: 		pR->m_RearDiff1.m_Xpeak=value*1e-4;		break;
	case CBatch::RearDopingPeak2: 		pR->m_RearDiff2.m_Npeak=value;		break;
	case CBatch::RearDopingDepth2: 		pR->m_RearDiff2.m_Depth=value*1e-4;		break;
	case CBatch::RearDopingPos2: 		pR->m_RearDiff2.m_Xpeak=value*1e-4;		break;
		// Recombination	
	case CBatch::BulkTaun:		pR->m_TauN=value*1e-6;		break; // us to s
	case CBatch::BulkTaup:		pR->m_TauP=value*1e-6;		break; // us to s
	case CBatch::BulkTau:		pR->m_TauP=pR->m_TauN=value*1e-6;	break;
	case CBatch::BulkEt:		pR->m_BulkEt=value;		break;
	case CBatch::FrontSn:		pR->m_FrontSn=value;	break;
	case CBatch::FrontSp:  		pR->m_FrontSp=value;	break;
	case CBatch::FrontEt:		pR->m_FrontEt=value;	break;
	case CBatch::RearSn:		pR->m_RearSn=value;		break;
	case CBatch::RearSp:		pR->m_RearSp=value;		break;
	case CBatch::RearEt:		pR->m_RearEt=value;		break;
	case CBatch::FrontS:		pR->m_FrontSn = pR->m_FrontSp = value; break;
	case CBatch::RearS:			pR->m_RearSn = pR->m_RearSp = value; break;

		// Material
	case CBatch::BandgapIndirect1:	pMat->m_AbsEi1=value;	break;
	case CBatch::BandgapIndirect2:	pMat->m_AbsEi2=value;	break;
	case CBatch::BandgapDirect1:	pMat->m_AbsEd1=value;	break;
	case CBatch::BandgapDirect2:	pMat->m_AbsEd2=value;	break;
	case CBatch::Bandgap:			pMat->m_BandGap=value;	break;
	case CBatch::Ni200:				pMat->m_ni200=value;	break;
	case CBatch::Ni300:				pMat->m_ni300=value;	break;
	case CBatch::Ni400:				pMat->m_ni400=value;	break;

	// Excitation parameters
	case CBatch::Temperature:		pE->m_Temp=value;		break;
		// Circuit
	case CBatch::BaseResSS:			pE->m_Base.m_RSS=value;		break;
	case CBatch::BaseResTR:			pE->m_Base.m_RTR=value;		break;
	case CBatch::BaseVoltageSS:		pE->m_Base.m_VSS=value;		break;
	case CBatch::BaseVoltageTR1:	pE->m_Base.m_VTR1=value;	break;
	case CBatch::BaseVoltageTR2:	pE->m_Base.m_VTR2=value;	break;
		
	case CBatch::CollResSS:			pE->m_Coll.m_RSS=value;		break;
	case CBatch::CollResTR:			pE->m_Coll.m_RTR=value;		break;
	case CBatch::CollVoltageSS:		pE->m_Coll.m_VSS=value;		break;
	case CBatch::CollVoltageTR1:	pE->m_Coll.m_VTR1=value;	break;
	case CBatch::CollVoltageTR2:	pE->m_Coll.m_VTR2=value;	break;
		// Light
	case CBatch::PriIntensitySS:	pE->m_LightPri.m_IntensitySS=value;		break;
	case CBatch::PriIntensityTR1:	pE->m_LightPri.m_IntensityTR1=value;	break;
	case CBatch::PriIntensityTR2:	pE->m_LightPri.m_IntensityTR2=value;	break;
	case CBatch::PriMonoSS:			pE->m_LightPri.m_LambdaSS=value;		break;
	case CBatch::PriMonoTR1:		pE->m_LightPri.m_LambdaTR1=value;		break;
	case CBatch::PriMonoTR2:		pE->m_LightPri.m_LambdaTR2=value;		break;
	case CBatch::PriBlackTemp:		pE->m_LightPri.m_BlackTemperature=value;	break;

	case CBatch::SecIntensitySS:	pE->m_LightSec.m_IntensitySS=value;		break;
	case CBatch::SecIntensityTR1:	pE->m_LightSec.m_IntensityTR1=value;	break;
	case CBatch::SecIntensityTR2:	pE->m_LightSec.m_IntensityTR2=value;	break;
	case CBatch::SecMonoSS:			pE->m_LightSec.m_LambdaSS=value;		break;
	case CBatch::SecMonoTR1:		pE->m_LightSec.m_LambdaTR1=value;		break;
	case CBatch::SecMonoTR2:		pE->m_LightSec.m_LambdaTR2=value;		break;
	case CBatch::SecBlackTemp:		pE->m_LightSec.m_BlackTemperature=value;	break;
	// Numerical
	case CBatch::ElementSizeFactor:	pS->m_SizeFactor=value;		break;
	case CBatch::ErrorLimit:		pS->m_ErrorLimit=value;		break;
	case CBatch::Clamp:				pS->m_Clamp=value;			break;			
	}
}

// this is used for saving the original values so they can be restored after a
// batch run.
double CBatch::GetParameterValue(CProblem *pProb, BatchParameter p, int region_num)
{
	CDevice *pD=pProb->GetDevice();
	CExcite *pE=pProb->GetExcite();
	const CRegion *pR=pD->GetRegion(region_num);
	const CMaterial *pMat = pD->GetMaterialForRegion(region_num);
	CSolve *pS=pProb->GetSolve();
	switch (p)
	{
	// Device parameters
	case CBatch::Area: 		return pD->m_Area;
		// Texturing - front & rear
	case CBatch::FrontTextureAngle:	return pD->m_FrontAngle;		
	case CBatch::FrontTextureDepth:	return pD->m_FrontDepth*1e4;	 // cm to um
	case CBatch::RearTextureAngle:	return pD->m_RearAngle;			
	case CBatch::RearTextureDepth:	return pD->m_RearDepth*1e4;		 // cm to um
		// Surface charge - front & rear
	case CBatch::FrontBarrier:		return pD->m_FrontBarrier;		
	case CBatch::FrontCharge:		return pD->m_FrontCharge;		
	case CBatch::RearBarrier:		return pD->m_RearBarrier;		
	case CBatch::RearCharge:		return pD->m_RearCharge;		
		// Contacts
	case CBatch::EmitterInternRes:	return pD->m_EmitterR;			
	case CBatch::BaseInternRes:		return pD->m_BaseR;				
	case CBatch::CollInternRes:		return pD->m_CollectorR;			
	case EmitterX:					return pD->m_EmitterX*1e4;		// cm to um
	case BaseX:						return pD->m_BaseX*1e4;
	case CollectorX:				return pD->m_CollectorX*1e4;

		// Shunt elements
	case CBatch::Shunt1Value:		return pD->GetLumpedElement(0)->m_Value;
	case CBatch::Shunt2Value:		return pD->GetLumpedElement(1)->m_Value;
	case CBatch::Shunt3Value:		return pD->GetLumpedElement(2)->m_Value;
	case CBatch::Shunt4Value:		return pD->GetLumpedElement(3)->m_Value;

	case CBatch::Shunt1Anode:		return pD->GetLumpedElement(0)->m_Xa*1e4;  // cm to um
	case CBatch::Shunt1Cathode:		return pD->GetLumpedElement(0)->m_Xc*1e4; 
	case CBatch::Shunt2Anode:		return pD->GetLumpedElement(1)->m_Xa*1e4;  // cm to um
	case CBatch::Shunt2Cathode:		return pD->GetLumpedElement(1)->m_Xc*1e4; 
	case CBatch::Shunt3Anode:		return pD->GetLumpedElement(2)->m_Xa*1e4;  // cm to um
	case CBatch::Shunt3Cathode:		return pD->GetLumpedElement(2)->m_Xc*1e4; 
	case CBatch::Shunt4Anode:		return pD->GetLumpedElement(3)->m_Xa*1e4;  // cm to um
	case CBatch::Shunt4Cathode:		return pD->GetLumpedElement(3)->m_Xc*1e4; 

		// Reflectance-front & rear
	case CBatch::FrontFixedReflectance:		return pD->m_FrontRfl.m_Fixed*100;	 // % to normalized
	case CBatch::FrontBroadbandReflectance:	return pD->m_FrontRfl.m_Broadband*100;  // % to normalized
	case CBatch::FrOuterThickness:			return pD->m_FrontRfl.m_Thick[2];	
	case CBatch::FrMiddleThickness:			return pD->m_FrontRfl.m_Thick[1];	
	case CBatch::FrInnerThickness:			return pD->m_FrontRfl.m_Thick[0];	
	case CBatch::FrOuterIndex:				return pD->m_FrontRfl.m_Index[2];	
	case CBatch::FrMiddleIndex:				return pD->m_FrontRfl.m_Index[1];	
	case CBatch::FrInnerIndex:				return pD->m_FrontRfl.m_Index[0];	
	case CBatch::FrontInternReflFirst:		return pD->m_FrontRfl.m_Internal1*100;	// % to normalized
	case CBatch::FrontInternReflSubsequent:	return pD->m_FrontRfl.m_Internal2*100;	// % to normalized
	
	case CBatch::RearFixedReflectance:		return pD->m_RearRfl.m_Fixed*100;	
	case CBatch::RearBroadbandReflectance:	return pD->m_RearRfl.m_Broadband*100; 
	case CBatch::RrOuterThickness:			return pD->m_RearRfl.m_Thick[2];		
	case CBatch::RrMiddleThickness:			return pD->m_RearRfl.m_Thick[1];		
	case CBatch::RrInnerThickness:			return pD->m_RearRfl.m_Thick[0];		
	case CBatch::RrOuterIndex:				return pD->m_RearRfl.m_Index[2];		
	case CBatch::RrMiddleIndex:				return pD->m_RearRfl.m_Index[1];		
	case CBatch::RrInnerIndex:				return pD->m_RearRfl.m_Index[0];		
	case CBatch::RearInternReflFirst:		return pD->m_RearRfl.m_Internal1*100;	// % to normalized
	case CBatch::RearInternReflSubsequent:	return pD->m_RearRfl.m_Internal2*100;	// % to normalized

	case CBatch::FrontInternRefl:		return pD->m_FrontRfl.m_Internal1*100;	// % to normalized
	case CBatch::RearInternRefl:		return pD->m_RearRfl.m_Internal1*100;	// % to normalized

		
	// Region parameters		
	case CBatch::Thickness:				return pR->m_Thickness/1e-4;  // um to cm
	
	case CBatch::BackgroundDoping:		return pR->m_BkgndDop;		
		// Doping -front
	case CBatch::FrontDopingPeak1: 		return pR->m_FrontDiff1.m_Npeak;			
	case CBatch::FrontDopingDepth1: 	return pR->m_FrontDiff1.m_Depth/1e-4;	// um to cm
	case CBatch::FrontDopingPos1: 		return pR->m_FrontDiff1.m_Xpeak/1e-4;	
	case CBatch::FrontDopingPeak2: 		return pR->m_FrontDiff2.m_Npeak;		
	case CBatch::FrontDopingDepth2: 	return pR->m_FrontDiff2.m_Depth/1e-4;	
	case CBatch::FrontDopingPos2: 		return pR->m_FrontDiff2.m_Xpeak/1e-4;	
		// Doping - rear		
	case CBatch::RearDopingPeak1: 		return pR->m_RearDiff1.m_Npeak;			
	case CBatch::RearDopingDepth1: 		return pR->m_RearDiff1.m_Depth/1e-4;	
	case CBatch::RearDopingPos1: 		return pR->m_RearDiff1.m_Xpeak/1e-4;
	case CBatch::RearDopingPeak2: 		return pR->m_RearDiff2.m_Npeak;		
	case CBatch::RearDopingDepth2: 		return pR->m_RearDiff2.m_Depth/1e-4;
	case CBatch::RearDopingPos2: 		return pR->m_RearDiff2.m_Xpeak/1e-4;
		// Recombination	
	case CBatch::BulkTaun:		return pR->m_TauN/1e-6; 	// us to s
	case CBatch::BulkTaup:		return pR->m_TauP/1e-6;		// us to s
	case CBatch::BulkTau:		return pR->m_TauN/1e-6;
	case CBatch::BulkEt:		return pR->m_BulkEt;
	case CBatch::FrontSn:		return pR->m_FrontSn;
	case CBatch::FrontSp:  		return pR->m_FrontSp;
	case CBatch::FrontEt:		return pR->m_FrontEt;
	case CBatch::RearSn:		return pR->m_RearSn;
	case CBatch::RearSp:		return pR->m_RearSp;
	case CBatch::RearEt:		return pR->m_RearEt;

	case CBatch::FrontS:		return pR->m_FrontSn;
	case CBatch::RearS:			return pR->m_RearSn;
		// Material
	case CBatch::BandgapIndirect1:	return pMat->m_AbsEi1;
	case CBatch::BandgapIndirect2:	return pMat->m_AbsEi2;
	case CBatch::BandgapDirect1:	return pMat->m_AbsEd1;
	case CBatch::BandgapDirect2:	return pMat->m_AbsEd2;
	case CBatch::Bandgap:			return pMat->m_BandGap;
	case CBatch::Ni200:				return pMat->m_ni200;
	case CBatch::Ni300:				return pMat->m_ni300;
	case CBatch::Ni400:				return pMat->m_ni400;
	
	// Excitation parameters
	case CBatch::Temperature:		return pE->m_Temp;
		// Circuit
	case CBatch::BaseResSS:			return pE->m_Base.m_RSS;
	case CBatch::BaseResTR:			return pE->m_Base.m_RTR;
	case CBatch::BaseVoltageSS:		return pE->m_Base.m_VSS;
	case CBatch::BaseVoltageTR1:	return pE->m_Base.m_VTR1;
	case CBatch::BaseVoltageTR2:	return pE->m_Base.m_VTR2;
		
	case CBatch::CollResSS:			return pE->m_Coll.m_RSS;
	case CBatch::CollResTR:			return pE->m_Coll.m_RTR;
	case CBatch::CollVoltageSS:		return pE->m_Coll.m_VSS;
	case CBatch::CollVoltageTR1:	return pE->m_Coll.m_VTR1;
	case CBatch::CollVoltageTR2:	return pE->m_Coll.m_VTR2;
		// Light
	case CBatch::PriIntensitySS:	return pE->m_LightPri.m_IntensitySS;
	case CBatch::PriIntensityTR1:	return pE->m_LightPri.m_IntensityTR1;
	case CBatch::PriIntensityTR2:	return pE->m_LightPri.m_IntensityTR2;
	case CBatch::PriMonoSS:			return pE->m_LightPri.m_LambdaSS;
	case CBatch::PriMonoTR1:		return pE->m_LightPri.m_LambdaTR1;
	case CBatch::PriMonoTR2:		return pE->m_LightPri.m_LambdaTR2;
	case CBatch::PriBlackTemp:		return pE->m_LightPri.m_BlackTemperature;

	case CBatch::SecIntensitySS:	return pE->m_LightSec.m_IntensitySS;
	case CBatch::SecIntensityTR1:	return pE->m_LightSec.m_IntensityTR1;
	case CBatch::SecIntensityTR2:	return pE->m_LightSec.m_IntensityTR2;
	case CBatch::SecMonoSS:			return pE->m_LightSec.m_LambdaSS;
	case CBatch::SecMonoTR1:		return pE->m_LightSec.m_LambdaTR1;
	case CBatch::SecMonoTR2:		return pE->m_LightSec.m_LambdaTR2;
	case CBatch::SecBlackTemp:		return pE->m_LightSec.m_BlackTemperature;
	// Numerical
	case CBatch::ElementSizeFactor:	return pS->m_SizeFactor;
	case CBatch::ErrorLimit:		return pS->m_ErrorLimit;
	case CBatch::Clamp:				return pS->m_Clamp;
	}
	return 0;	
}

double CBatch::GetResultValue(CProblem *pProb, CBatch::BatchParameter p)
{
	CDevice *pD=pProb->GetDevice();
	CExcite *pE=pProb->GetExcite();
	CStatus *pStatus = pProb->GetStatus();
	int timestep=pStatus->time_step;
	
	switch (p)
	{
	case CBatch::Vb:
		if (!pD->m_EnableBase) return 0;
		return pE->m_Base.m_Volts[timestep+1];
		break;
	case CBatch::Ib:
		if (!pD->m_EnableBase) return 0;
		return pE->m_Base.m_Amps[timestep+1];
		break;
	case CBatch::Vc:
		if (!pD->m_EnableCollector) return 0;
		return pE->m_Coll.m_Volts[timestep+1];
		break;
	case CBatch::Ic:
		if (!pD->m_EnableCollector) return 0;
		return pE->m_Coll.m_Amps[timestep+1];
		break;	
 	case CBatch::BaseVoc:
 		if (!pD->m_EnableBase) return 0;
 		switch(pStatus->state) {
 			case SS_STATE: return pE->m_Base.m_Volts[timestep+1];
 			case TR_STATE:
 						return CMath::Intcpt(timestep+2, 
						pE->m_Base.m_Amps, pE->m_Base.m_Volts);
		};
		break;
 	case CBatch::BaseIsc:
 		if (!pD->m_EnableBase) return 0;
 		switch(pStatus->state) {
 			case SS_STATE: return pE->m_Base.m_Amps[timestep+1];
 			case TR_STATE:
 						return CMath::Intcpt(timestep+2, 
						pE->m_Base.m_Volts, pE->m_Base.m_Amps);
		};
 		break;
 	case CBatch::BasePmax:
 		if (!pD->m_EnableBase) return 0;
 		if (pStatus->state==TR_STATE) {
 			int k;
			double* z = new double[timestep+2];	// intermediate result holder
 			for (k=0; k<timestep+2; k++) 
				z[k]=pE->m_Base.m_Volts[k]*pE->m_Base.m_Amps[k];
			double result=-CMath::InterpMin(timestep+2, z);
			delete z;
			return result;
		}
		break;
 	case CBatch::CollVoc:
 		if (!pD->m_EnableCollector) return 0;
 		switch(pStatus->state) {
 			case SS_STATE: return pE->m_Coll.m_Volts[timestep+1];
 			case TR_STATE:
 						return CMath::Intcpt(timestep+2, 
						pE->m_Coll.m_Amps, pE->m_Coll.m_Volts);
		};
		break;
 	case CBatch::CollIsc:
 		if (!pD->m_EnableCollector) return 0;
 		switch(pStatus->state) {
 			case SS_STATE: return pE->m_Coll.m_Amps[timestep+1];
 			case TR_STATE:
 						return CMath::Intcpt(timestep+2, 
						pE->m_Coll.m_Volts, pE->m_Coll.m_Amps);
		};
 		break;
 	case CBatch::CollPmax:
 		if (!pD->m_EnableCollector) return 0;
 		if (pStatus->state==TR_STATE) {
 			int k;
			double* z = new double[timestep+2];	// intermediate result holder
 			for (k=0; k<timestep+2; k++) 
				z[k]=pE->m_Coll.m_Volts[k]*pE->m_Coll.m_Amps[k];
			double result=-CMath::InterpMin(timestep+2, z);
			delete z;
			return result;
		}
		break;
	case CBatch::V1: return pD->GetLumpedElement(0)->m_Volts[timestep+1];
	case CBatch::I1: return pD->GetLumpedElement(0)->m_Amps[timestep+1];
	case CBatch::V2: return pD->GetLumpedElement(1)->m_Volts[timestep+1];
	case CBatch::I2: return pD->GetLumpedElement(1)->m_Amps[timestep+1];
	case CBatch::V3: return pD->GetLumpedElement(2)->m_Volts[timestep+1];
	case CBatch::I3: return pD->GetLumpedElement(2)->m_Amps[timestep+1];
	case CBatch::V4: return pD->GetLumpedElement(3)->m_Volts[timestep+1];
	case CBatch::I4: return pD->GetLumpedElement(3)->m_Amps[timestep+1];
	case CBatch::NumElements:
		return pProb->GetNode()->NumElements();
	case CBatch::RmsExptDiff: 
		int elementsrequired=timestep+2;
		double *lambda = new double[elementsrequired];
		double *iqe = new double[elementsrequired];
		double *expt = new double[elementsrequired];
		CGraph gr;
		gr.SetQuantities(DataWp, DataIQEp, DataExperimental);
		gr.LoadQuantities(pProb, elementsrequired, lambda, iqe, expt);
		double rms=CMath::ArrayRmsDifference(elementsrequired, iqe, expt);
		delete [] lambda;
		delete [] iqe;
		delete [] expt;
		return rms;	
	}
	return 0;
}

double CBatch::GetSpatialResultValue(CProblem *pProb, CBatch::BatchParameter p, double xposition)
{
	GraphableQuantity grlist[]={ DataPsi, DataNd, DataNa, DataN, DataP,
		DataE, DataRho, DataJn, DataJp, DataJt,
		DataGcum, DataEg, DataNie, DataTau, DataRbulk, 
		DataRcum, DataG, DataVn, DataVp, DataMuN,
		DataMuP, DataN_, DataP_, DataPsi_, DataRho_, 
		DataPhiN, DataPhiP,	DataEc, DataEv, DataEvac, 
		DataRes, DataCond, DataCcum, DataCond_, DataCcum_, 
		DataLd, DataIn, DataIp, DataIt, DataPerm, 
		DataNratio, DataPratio, DataPNratio, DataPNnorm, 
		DataNdrift, DataNdiff, DataPdrift, DataPdiff
	};
	BatchParameter batlist[]={ Psi, Nd, Na, N, P, 
		E, Rho, Jn, Jp, Jt,
		Gcum, Eg, Nie, Tau, Rbulk, 
		Rcum, G, Vn, Vp, MuN, 
		MuP, N_, P_, Psi_, Rho_,
		PhiN, PhiP,	Ec, Ev, Evac,
		Res, Cond, Ccum, Cond_, Ccum_,
		Ld, In, Ip, It, Perm, 
		Nratio, Pratio, PNratio, PNnorm, 
		Ndrift, Ndiff, Pdrift, Pdiff, (BatchParameter)0};
		
	GraphableQuantity g=DataNothing;
	for (int i=0; batlist[i]!=0; i++) if (p==batlist[i]) g=grlist[i];
	
	int elementsrequired=pProb->GetNode()->GetNodeCount();
	double *z=new double [elementsrequired+1];
	CData::LoadQuantityIntoArray(pProb, elementsrequired, z, g); // Spatial basis
	int k=0;
	while (k<pProb->GetNode()->GetLastNode()
			&& xposition*1e-4 >pProb->GetNode()->GetX(k)) k++;		// find location (um to cm)
	double result = z[k];			// time array
	delete [] z;
	return result;
}
