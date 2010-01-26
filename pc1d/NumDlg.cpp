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
#include "numdlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNumDlg dialog


CNumDlg::CNumDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNumDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNumDlg)
	m_Clamp = 0;
	m_bClampPhi = FALSE;
	m_bClampPsi = FALSE;
	m_bRenodeEQ = FALSE;
	m_bRenodeSS = FALSE;
	m_bRenodeTR = FALSE;
	m_ErrorLimit = 0;
	m_SizeFactor = 0;
	m_nMaxTime = 0;
	m_bGraphsEveryIteration = FALSE;
	m_bTotalSat = FALSE;
	//}}AFX_DATA_INIT
}

void CNumDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNumDlg)
	DDX_Text(pDX, IDC_CLAMP, m_Clamp);
	DDV_MinMaxDouble(pDX, m_Clamp, 1.e-001, 10.);
	DDX_Check(pDX, IDC_PHI, m_bClampPhi);
	DDX_Check(pDX, IDC_PSI, m_bClampPsi);
	DDX_Check(pDX, IDC_EQ, m_bRenodeEQ);
	DDX_Check(pDX, IDC_SS, m_bRenodeSS);
	DDX_Check(pDX, IDC_TR, m_bRenodeTR);
	DDX_Text(pDX, IDC_ERROR, m_ErrorLimit);
	DDV_MinMaxDouble(pDX, m_ErrorLimit, 1.e-012, 1.e-003);
	DDX_Text(pDX, IDC_SIZEFACTOR, m_SizeFactor);
	DDV_MinMaxDouble(pDX, m_SizeFactor, 1.e-002, 1.);
	DDX_Text(pDX, IDC_MAXTIME, m_nMaxTime);
	DDV_MinMaxInt(pDX, m_nMaxTime, 10, 1800);
	DDX_Check(pDX, IDC_EVERYITERATION, m_bGraphsEveryIteration);
	DDX_Check(pDX, IDC_VELSAT, m_bTotalSat);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CNumDlg, CDialog)
	//{{AFX_MSG_MAP(CNumDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CNumDlg message handlers
