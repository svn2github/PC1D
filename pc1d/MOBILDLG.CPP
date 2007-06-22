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
#include "mobildlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMobilDlg dialog


CMobilDlg::CMobilDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMobilDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMobilDlg)
	m_ElecB1 = 0;
	m_ElecB2 = 0;
	m_ElecB3 = 0;
	m_ElecB4 = 0;
	m_ElecMajAlpha = 0;
	m_ElecMajMin = 0;
	m_ElecMax = 0;
	m_ElecMinAlpha = 0;
	m_ElecMinMin = 0;
	m_HoleB1 = 0;
	m_HoleB2 = 0;
	m_HoleB3 = 0;
	m_HoleB4 = 0;
	m_HoleMajAlpha = 0;
	m_HoleMajMin = 0;
	m_HoleMax = 0;
	m_HoleMinAlpha = 0;
	m_HoleMinMin = 0;
	m_ElecVmax = 0;
	m_HoleVmax = 0;
	m_ElecMajNref = 0;
	m_ElecMinNref = 0;
	m_HoleMajNref = 0;
	m_HoleMinNref = 0;
	//}}AFX_DATA_INIT
}

void CMobilDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMobilDlg)
	DDX_Text(pDX, IDC_ELEC_B1, m_ElecB1);
	DDV_MinMaxDouble(pDX, m_ElecB1, -10., 10.);
	DDX_Text(pDX, IDC_ELEC_B2, m_ElecB2);
	DDV_MinMaxDouble(pDX, m_ElecB2, -10., 10.);
	DDX_Text(pDX, IDC_ELEC_B3, m_ElecB3);
	DDV_MinMaxDouble(pDX, m_ElecB3, -10., 10.);
	DDX_Text(pDX, IDC_ELEC_B4, m_ElecB4);
	DDV_MinMaxDouble(pDX, m_ElecB4, -10., 10.);
	DDX_Text(pDX, IDC_ELEC_MAJ_ALPHA, m_ElecMajAlpha);
	DDV_MinMaxDouble(pDX, m_ElecMajAlpha, -10., 10.);
	DDX_Text(pDX, IDC_ELEC_MAJ_MIN, m_ElecMajMin);
	DDV_MinMaxDouble(pDX, m_ElecMajMin, 0., 100000.);
	DDX_Text(pDX, IDC_ELEC_MAX, m_ElecMax);
	DDV_MinMaxDouble(pDX, m_ElecMax, 1.e-006, 100000.);
	DDX_Text(pDX, IDC_ELEC_MIN_ALPHA, m_ElecMinAlpha);
	DDV_MinMaxDouble(pDX, m_ElecMinAlpha, -10., 10.);
	DDX_Text(pDX, IDC_ELEC_MIN_MIN, m_ElecMinMin);
	DDV_MinMaxDouble(pDX, m_ElecMinMin, 0., 100000.);
	DDX_Text(pDX, IDC_HOLE_B1, m_HoleB1);
	DDV_MinMaxDouble(pDX, m_HoleB1, -10., 10.);
	DDX_Text(pDX, IDC_HOLE_B2, m_HoleB2);
	DDV_MinMaxDouble(pDX, m_HoleB2, -10., 10.);
	DDX_Text(pDX, IDC_HOLE_B3, m_HoleB3);
	DDV_MinMaxDouble(pDX, m_HoleB3, -10., 10.);
	DDX_Text(pDX, IDC_HOLE_B4, m_HoleB4);
	DDV_MinMaxDouble(pDX, m_HoleB4, -10., 10.);
	DDX_Text(pDX, IDC_HOLE_MAJ_ALPHA, m_HoleMajAlpha);
	DDV_MinMaxDouble(pDX, m_HoleMajAlpha, -10., 10.);
	DDX_Text(pDX, IDC_HOLE_MAJ_MIN, m_HoleMajMin);
	DDV_MinMaxDouble(pDX, m_HoleMajMin, 0., 100000.);
	DDX_Text(pDX, IDC_HOLE_MAX, m_HoleMax);
	DDV_MinMaxDouble(pDX, m_HoleMax, 1.e-006, 100000.);
	DDX_Text(pDX, IDC_HOLE_MIN_ALPHA, m_HoleMinAlpha);
	DDV_MinMaxDouble(pDX, m_HoleMinAlpha, -10., 10.);
	DDX_Text(pDX, IDC_HOLE_MIN_MIN, m_HoleMinMin);
	DDV_MinMaxDouble(pDX, m_HoleMinMin, 0., 100000.);
	DDX_ScientificDouble(pDX, IDC_ELEC_VMAX, m_ElecVmax);
	DDX_ScientificDouble(pDX, IDC_HOLE_VMAX, m_HoleVmax);
	DDX_ScientificDouble(pDX, IDC_ELEC_MAJ_NREF, m_ElecMajNref);
	DDV_MinMaxSciDouble(pDX, m_ElecMajNref, 1., 1.e+030);
	DDX_ScientificDouble(pDX, IDC_ELEC_MIN_NREF, m_ElecMinNref);
	DDV_MinMaxSciDouble(pDX, m_ElecMinNref, 1., 1.e+030);
	DDX_ScientificDouble(pDX, IDC_HOLE_MAJ_NREF, m_HoleMajNref);
	DDV_MinMaxSciDouble(pDX, m_HoleMajNref, 1., 1.e+030);
	DDX_ScientificDouble(pDX, IDC_HOLE_MIN_NREF, m_HoleMinNref);
	DDV_MinMaxSciDouble(pDX, m_HoleMinNref, 1., 1.e+030);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMobilDlg, CDialog)
	//{{AFX_MSG_MAP(CMobilDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMobilDlg message handlers
