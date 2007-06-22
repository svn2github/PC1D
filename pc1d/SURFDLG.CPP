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
#include "surfdlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSurfaceDlg dialog


CSurfaceDlg::CSurfaceDlg(UINT tabnameID)
	: CPropertyPage(CSurfaceDlg::IDD, tabnameID)
{
	//{{AFX_DATA_INIT(CSurfaceDlg)
	m_Barrier = 0;
	m_SurfaceType = -1;
	m_Charge = 0;
	//}}AFX_DATA_INIT
}

void CSurfaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSurfaceDlg)
	DDX_Text(pDX, IDC_BARRIER, m_Barrier);
	DDX_Radio(pDX, IDC_RADIONEUTRAL, m_SurfaceType);
	DDX_Text(pDX, IDC_CHARGE, m_Charge);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSurfaceDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CSurfaceDlg)
	ON_BN_CLICKED(IDC_RADIOBARRIER, OnRadiobarrier)
	ON_BN_CLICKED(IDC_RADIOCHARGE, OnRadiocharge)
	ON_BN_CLICKED(IDC_RADIONEUTRAL, OnRadioneutral)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSurfaceDlg message handlers

BOOL CSurfaceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	DoDisabling(m_SurfaceType);
	return TRUE;
}

void CSurfaceDlg::DoDisabling(int n)
{
	GetDlgItem(IDC_CHARGE)->EnableWindow(n==1);
	GetDlgItem(IDC_STATIC_C1)->EnableWindow(n==1);
	GetDlgItem(IDC_BARRIER)->EnableWindow(n==2);
	GetDlgItem(IDC_STATIC_B1)->EnableWindow(n==2);
	GetDlgItem(IDC_STATIC_B2)->EnableWindow(n==2);
}


void CSurfaceDlg::OnRadiobarrier()
{
	DoDisabling(2);	
}

void CSurfaceDlg::OnRadiocharge()
{
	DoDisabling(1);
	
}

void CSurfaceDlg::OnRadioneutral()
{
	DoDisabling(0);	
}
