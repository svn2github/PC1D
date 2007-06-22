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
#include "lumpdlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLumpDlg dialog


CLumpDlg::CLumpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLumpDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLumpDlg)
	m_Enable1 = FALSE;
	m_Enable2 = FALSE;
	m_Enable3 = FALSE;
	m_Enable4 = FALSE;
	m_n2 = 1;
	m_n1 = 1;
	m_n3 = 1;
	m_n4 = 1;
	m_Type1 = -1;
	m_Type2 = -1;
	m_Type3 = -1;
	m_Type4 = -1;
	m_Value1 = 0;
	m_Value2 = 0;
	m_Value3 = 0;
	m_Value4 = 0;
	m_Xa1 = 0;
	m_Xa2 = 0;
	m_Xa3 = 0;
	m_Xa4 = 0;
	m_Xc1 = 0;
	m_Xc2 = 0;
	m_Xc3 = 0;
	m_Xc4 = 0;
	//}}AFX_DATA_INIT
}

void CLumpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLumpDlg)
	DDX_Check(pDX, IDC_ENABLE1, m_Enable1);
	DDX_Check(pDX, IDC_ENABLE2, m_Enable2);
	DDX_Check(pDX, IDC_ENABLE3, m_Enable3);
	DDX_Check(pDX, IDC_ENABLE4, m_Enable4);
	DDX_Text(pDX, IDC_N2, m_n2);
	DDV_MinMaxDouble(pDX, m_n2, 0.5, 10.);
	DDX_Text(pDX, IDC_N1, m_n1);
	DDV_MinMaxDouble(pDX, m_n1, 0.5, 10.);
	DDX_Text(pDX, IDC_N3, m_n3);
	DDV_MinMaxDouble(pDX, m_n3, 0.5, 10.);
	DDX_Text(pDX, IDC_N4, m_n4);
	DDV_MinMaxDouble(pDX, m_n4, 0.5, 10.);
	DDX_Radio(pDX, IDC_SHUNT1, m_Type1);
	DDX_Radio(pDX, IDC_SHUNT2, m_Type2);
	DDX_Radio(pDX, IDC_SHUNT3, m_Type3);
	DDX_Radio(pDX, IDC_SHUNT4, m_Type4);
	DDX_Text(pDX, IDC_VALUE1, m_Value1);
	DDV_MinMaxDouble(pDX, m_Value1, 0., 1.e030);
	DDX_Text(pDX, IDC_VALUE2, m_Value2);
	DDV_MinMaxDouble(pDX, m_Value2, 0., 1.e030);
	DDX_Text(pDX, IDC_VALUE3, m_Value3);
	DDV_MinMaxDouble(pDX, m_Value3, 0., 1.e030);
	DDX_Text(pDX, IDC_VALUE4, m_Value4);
	DDV_MinMaxDouble(pDX, m_Value4, 0., 1.e030);
	DDX_Text(pDX, IDC_XA1, m_Xa1);
	DDV_MinMaxDouble(pDX, m_Xa1, 0., 10000.);
	DDX_Text(pDX, IDC_XA2, m_Xa2);
	DDV_MinMaxDouble(pDX, m_Xa2, 0., 10000.);
	DDX_Text(pDX, IDC_XA3, m_Xa3);
	DDV_MinMaxDouble(pDX, m_Xa3, 0., 10000.);
	DDX_Text(pDX, IDC_XA4, m_Xa4);
	DDV_MinMaxDouble(pDX, m_Xa4, 0., 10000.);
	DDX_Text(pDX, IDC_XC1, m_Xc1);
	DDV_MinMaxDouble(pDX, m_Xc1, 0., 10000.);
	DDX_Text(pDX, IDC_XC2, m_Xc2);
	DDV_MinMaxDouble(pDX, m_Xc2, 0., 10000.);
	DDX_Text(pDX, IDC_XC3, m_Xc3);
	DDV_MinMaxDouble(pDX, m_Xc3, 0., 10000.);
	DDX_Text(pDX, IDC_XC4, m_Xc4);
	DDV_MinMaxDouble(pDX, m_Xc4, 0., 10000.);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLumpDlg, CDialog)
	//{{AFX_MSG_MAP(CLumpDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CLumpDlg message handlers
