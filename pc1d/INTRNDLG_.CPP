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
#include "intrndlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIntrnDlg dialog


CIntrnDlg::CIntrnDlg()
	: CPropertyPage(CIntrnDlg::IDD)
{
	//{{AFX_DATA_INIT(CIntrnDlg)
	m_Front1 = 0;
	m_Front2 = 0;
	m_Rear1 = 0;
	m_Rear2 = 0;
	m_FrontRough = -1;
	m_RearRough = -1;
	//}}AFX_DATA_INIT
}

void CIntrnDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIntrnDlg)
	DDX_Text(pDX, IDC_FRONT1, m_Front1);
	DDV_MinMaxDouble(pDX, m_Front1, 0., 100.);
	DDX_Text(pDX, IDC_FRONT2, m_Front2);
	DDV_MinMaxDouble(pDX, m_Front2, 0., 100.);
	DDX_Text(pDX, IDC_REAR1, m_Rear1);
	DDV_MinMaxDouble(pDX, m_Rear1, 0., 100.);
	DDX_Text(pDX, IDC_REAR2, m_Rear2);
	DDV_MinMaxDouble(pDX, m_Rear2, 0., 100.);
	DDX_Radio(pDX, IDC_FRONTSPEC, m_FrontRough);
	DDX_Radio(pDX, IDC_REARSPEC, m_RearRough);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CIntrnDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CIntrnDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CIntrnDlg message handlers
