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
#include "ProgramLimits.h"
#include "math.h"
#include "mathstat.h"	// CMath
#include "physics.h"	// CPhysics
#include "RegionData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegionData

void CRegionData::Initialize(double endx) 
{
	for (int k=0; k<MAX_REGIONS; k++) {
		m_xRegionFront[k] = 0;
		m_xRegionRear[k] = endx;
		m_Cn[k] = m_Cp[k] = m_Cnp[k] = m_B[k] = m_Et[k] = m_Aff[k] = 0;
		m_FrontRec[k].Initialize(); m_RearRec[k].Initialize();
//		m_SnFront[k] = m_SpFront[k] = m_SnRear[k] = m_SpRear[k] = 0;
//		m_EtFront[k] = m_EtRear[k] = 0; m_JoFront[k] = m_JoRear[k] = FALSE;
		m_Nc[k] = m_Nv[k] = 1;
	}
}

void CSurfaceRecomData::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << m_Sn << m_Sp << m_Et << (WORD)m_Jo;
	} else {
		ar >> m_Sn >> m_Sp >> m_Et >> (WORD&)m_Jo;
	}
}

void CRegionData::Serialize(CArchive& ar)
{
	int k;
	if (ar.IsStoring())
	{
			for (k=0; k<MAX_REGIONS; k++)
			{
				ar << m_xRegionFront[k] << m_xRegionRear[k] << m_Cn[k] << m_Cp[k] << m_Cnp[k] << m_B[k];
				ar << m_Et[k] << m_Eref[k] << m_Egamma[k];
				m_FrontRec[k].Serialize(ar);
				m_RearRec[k].Serialize(ar);
				ar << m_Nc[k] << m_Nv[k] << m_Aff[k];
			}
	}else {
			for (k=0; k<MAX_REGIONS; k++)
			{
				ar >> m_xRegionFront[k] >> m_xRegionRear[k] >> m_Cn[k] >> m_Cp[k] >> m_Cnp[k] >> m_B[k];
				ar >> m_Et[k] >> m_Eref[k] >> m_Egamma[k];
				m_FrontRec[k].Serialize(ar);
				m_RearRec[k].Serialize(ar);
				ar >> m_Nc[k] >> m_Nv[k] >> m_Aff[k];
			}				
	}
}
