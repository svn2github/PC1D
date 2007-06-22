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
////////////////////////////////////////////////////////////////////////////
// CPath

class CPath : public CObject
{
// Attributes
private:
	const CString m_INIheading;
	const CString m_INIprm, m_INIdev, m_INImat, m_INIexc;
	const CString m_INIabs, m_INIdop, m_INIinr, m_INIref;
	const CString m_INIgen, m_INIlgt, m_INIspc, m_INIvlt;
	const CString m_INIbat;
public:
	CString prm;	// Problem parameters
	CString dev;	// Device parameters
	CString mat;	// Material parameters
	CString exc;	// Excitation parameters
	CString abs;	// Optical absorption
	CString dop;	// Doping vs. position
	CString inr;	// Index of refraction
	CString ref;	// Surface reflectance
	CString gen;	// Photogeneration profile
	CString lgt;	// Light source intensity
	CString spc;	// Light source spectrum
	CString vlt;	// Voltage source
	CString bat;	// Batch parameters
// Operations
public:
	CPath();
	void Initialize();
	void LoadFromINIfile();
	void SetPaths();
	static CString MinimumNecessaryFilename(CString pathname, CString defaultpath);
	static CString MakeFullPathname(CString filename, CString defaultpath);
};
