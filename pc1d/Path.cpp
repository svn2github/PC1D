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
#include "path.h"
#include <direct.h>		// needed for _getcwd()
#include "pathdlg.h"



//////////////////////////////////////////////////////////////////
// CPath

CPath::CPath():
	m_INIheading("Paths"),
	m_INIprm("prm"), m_INImat("mat"), m_INIdev("dev"), m_INIexc("exc"),
	m_INIabs("abs"), m_INIdop("dop"), m_INIinr("inr"), m_INIref("ref"),
	m_INIgen("gen"), m_INIlgt("lgt"), m_INIspc("spc"), m_INIvlt("vlt"),
	m_INIbat("txt")
{
	Initialize();	
}

void CPath::LoadFromINIfile(void)
{
	Initialize();
	prm = AfxGetApp()->GetProfileString(m_INIheading, m_INIprm);
	mat = AfxGetApp()->GetProfileString(m_INIheading, m_INImat);
	dev = AfxGetApp()->GetProfileString(m_INIheading, m_INIdev);
	exc = AfxGetApp()->GetProfileString(m_INIheading, m_INIexc);
	abs = AfxGetApp()->GetProfileString(m_INIheading, m_INIabs);
	dop = AfxGetApp()->GetProfileString(m_INIheading, m_INIdop);
	inr = AfxGetApp()->GetProfileString(m_INIheading, m_INIinr);
	ref = AfxGetApp()->GetProfileString(m_INIheading, m_INIref);
	gen = AfxGetApp()->GetProfileString(m_INIheading, m_INIgen);
	lgt = AfxGetApp()->GetProfileString(m_INIheading, m_INIlgt);
	spc = AfxGetApp()->GetProfileString(m_INIheading, m_INIspc);
	vlt = AfxGetApp()->GetProfileString(m_INIheading, m_INIvlt);
	bat = AfxGetApp()->GetProfileString(m_INIheading, m_INIbat);
	// if there's no entry in INI file, or if there is but it's NULL, we set the directory
	// to the current working directory, as specified in file manager properties.
	CString cwdbuff;
	char *s =cwdbuff.GetBuffer(_MAX_PATH+1);
	_getcwd(s, _MAX_PATH);
	cwdbuff.ReleaseBuffer();
	cwdbuff.MakeLower();
	if (prm.IsEmpty()) prm=cwdbuff;
	if (mat.IsEmpty()) mat=cwdbuff;
	if (dev.IsEmpty()) dev=cwdbuff;
	if (exc.IsEmpty()) exc=cwdbuff;
	if (abs.IsEmpty()) abs=cwdbuff;
	if (dop.IsEmpty()) dop=cwdbuff;
	if (inr.IsEmpty()) inr=cwdbuff;
	if (ref.IsEmpty()) ref=cwdbuff;
	if (gen.IsEmpty()) gen=cwdbuff;
	if (lgt.IsEmpty()) lgt=cwdbuff;
	if (spc.IsEmpty()) spc=cwdbuff;
	if (vlt.IsEmpty()) vlt=cwdbuff;
	if (bat.IsEmpty()) bat=cwdbuff;
}

void CPath::Initialize()
{
	prm.Empty(); mat.Empty(); dev.Empty(); exc.Empty();
	abs.Empty(); dop.Empty(); inr.Empty(); ref.Empty();
	gen.Empty(); lgt.Empty(); spc.Empty(); vlt.Empty();
	bat.Empty();
}

void CPath::SetPaths()
{
	CPathDlg dlg;
	dlg.m_prm = prm;
	dlg.m_mat = mat;
	dlg.m_dev = dev;
	dlg.m_exc = exc;
	dlg.m_abs = abs;
	dlg.m_dop = dop;
	dlg.m_inr = inr;
	dlg.m_ref = ref;
	dlg.m_gen = gen;
	dlg.m_lgt = lgt;
	dlg.m_spc = spc;
	dlg.m_vlt = vlt;
	dlg.m_bat = bat;
	if (dlg.DoModal()==IDOK)
	{
		prm = dlg.m_prm; prm.MakeLower(); AfxGetApp()->WriteProfileString(m_INIheading,m_INIprm,prm);
		mat = dlg.m_mat; mat.MakeLower(); AfxGetApp()->WriteProfileString(m_INIheading,m_INImat,mat);
		dev = dlg.m_dev; dev.MakeLower(); AfxGetApp()->WriteProfileString(m_INIheading,m_INIdev,dev);
		exc = dlg.m_exc; exc.MakeLower(); AfxGetApp()->WriteProfileString(m_INIheading,m_INIexc,exc);
		abs = dlg.m_abs; abs.MakeLower(); AfxGetApp()->WriteProfileString(m_INIheading,m_INIabs,abs);
		dop = dlg.m_dop; dop.MakeLower(); AfxGetApp()->WriteProfileString(m_INIheading,m_INIdop,dop);
		inr = dlg.m_inr; inr.MakeLower(); AfxGetApp()->WriteProfileString(m_INIheading,m_INIinr,inr);
		ref = dlg.m_ref; ref.MakeLower(); AfxGetApp()->WriteProfileString(m_INIheading,m_INIref,ref);
		gen = dlg.m_gen; gen.MakeLower(); AfxGetApp()->WriteProfileString(m_INIheading,m_INIgen,gen);
		lgt = dlg.m_lgt; lgt.MakeLower(); AfxGetApp()->WriteProfileString(m_INIheading,m_INIlgt,lgt);
		spc = dlg.m_spc; spc.MakeLower(); AfxGetApp()->WriteProfileString(m_INIheading,m_INIspc,spc);
		vlt = dlg.m_vlt; vlt.MakeLower(); AfxGetApp()->WriteProfileString(m_INIheading,m_INIvlt,vlt);
		bat = dlg.m_bat; bat.MakeLower(); AfxGetApp()->WriteProfileString(m_INIheading,m_INIbat,bat);
	}
}

// returns the minimum string required to specify the file.
// If the path for the file is the same as the default path, only the filename is returned.
// Otherwise, the full pathname is returned.
CString CPath::MinimumNecessaryFilename(CString pathname, CString defaultpath)
{ 
 		char drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
		CString path;
		CString filename;
		_splitpath(pathname, drive, dir, fname, ext);
		path=drive+(CString)dir;
		filename=fname+(CString)ext;
		if (path == defaultpath+"\\") return filename; else return pathname;
}

// Returns the full pathname for filename. If filename didn't include a path, the default
// path is used.
// assumes that if the second char is a :, it's a full path, else it's filename only. 
CString CPath::MakeFullPathname(CString filename, CString defaultpath)
{
	if (filename[1]!=':') return defaultpath+"\\"+filename;
	else return filename;
}
