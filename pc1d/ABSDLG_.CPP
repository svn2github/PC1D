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
#include "absdlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAbsDlg dialog


CAbsDlg::CAbsDlg()
	: CPropertyPage(CAbsDlg::IDD)
{
	//{{AFX_DATA_INIT(CAbsDlg)
	m_Ed1 = 0;
	m_Ed2 = 0;
	m_Ei1 = 0;
	m_Ei2 = 0;
	m_Ep1 = 0;
	m_Ep2 = 0;
	m_Tcoeff = 0;
	m_Toffset = 0;
	m_Radio = -1;
	m_A11 = 0;
	m_A12 = 0;
	m_A21 = 0;
	m_A22 = 0;
	m_Ad1 = 0;
	m_Ad2 = 0;
	//}}AFX_DATA_INIT
}

void CAbsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAbsDlg)
	DDX_Text(pDX, IDC_ED1, m_Ed1);
	DDV_MinMaxDouble(pDX, m_Ed1, 0., 20.);
	DDX_Text(pDX, IDC_ED2, m_Ed2);
	DDV_MinMaxDouble(pDX, m_Ed2, 0., 20.);
	DDX_Text(pDX, IDC_EI1, m_Ei1);
	DDV_MinMaxDouble(pDX, m_Ei1, 0., 20.);
	DDX_Text(pDX, IDC_EI2, m_Ei2);
	DDV_MinMaxDouble(pDX, m_Ei2, 0., 20.);
	DDX_Text(pDX, IDC_EP1, m_Ep1);
	DDV_MinMaxDouble(pDX, m_Ep1, 0., 1000.);
	DDX_Text(pDX, IDC_EP2, m_Ep2);
	DDV_MinMaxDouble(pDX, m_Ep2, 0., 1000.);
	DDX_Text(pDX, IDC_TCOEFF, m_Tcoeff);
	DDV_MinMaxDouble(pDX, m_Tcoeff, 0., 100.);
	DDX_Text(pDX, IDC_TOFFSET, m_Toffset);
	DDV_MinMaxDouble(pDX, m_Toffset, -10000., 10000.);
	DDX_Radio(pDX, IDC_RADIO1, m_Radio);
	DDX_ScientificDouble(pDX, IDC_A11, m_A11);
	DDX_ScientificDouble(pDX, IDC_A12, m_A12);
	DDX_ScientificDouble(pDX, IDC_A21, m_A21);
	DDX_ScientificDouble(pDX, IDC_A22, m_A22);
	DDX_ScientificDouble(pDX, IDC_AD1, m_Ad1);
	DDX_ScientificDouble(pDX, IDC_AD2, m_Ad2);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAbsDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CAbsDlg)
	ON_BN_CLICKED(IDC_RADIO1, DoDisabling)
	ON_BN_CLICKED(IDC_OPEN, OnOpen)
	ON_BN_CLICKED(IDC_RADIO3, DoDisabling)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAbsDlg message handlers

CAbsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	if (!m_title.IsEmpty()) SetWindowText(m_title);
	if (m_Filename.IsEmpty()) SetDlgItemText(IDC_FILENAME, "(No file selected)");
	else SetDlgItemText(IDC_FILENAME, m_Filename);
	DoDisabling();
	return TRUE;
}

void CAbsDlg::OnOpen()
{
	BOOL FileOpen = TRUE;
	CString Filter = "Absorption (*."+m_Ext+")|*."+m_Ext+"|All Files (*.*)|*.*||";
	CFileDialog dlg(FileOpen, m_Ext, m_Filename, OFN_OVERWRITEPROMPT, Filter);
	dlg.m_ofn.lpstrTitle = m_OpenDlgTitle;
	dlg.m_ofn.lpstrInitialDir = m_Path;
	if (dlg.DoModal()==IDOK)
	{
		m_Filename = dlg.GetPathName();
		m_Filename.MakeLower();
		SetDlgItemText(IDC_FILENAME, m_Filename);
	}
}

void CAbsDlg::DoDisabling()
{
	BOOL b1=((CButton*)GetDlgItem(IDC_RADIO1))->GetState();
	BOOL b3=!b1;
	
	int Radio1items[]={	IDC_A11, IDC_A12, IDC_A21, IDC_A22, IDC_AD1, IDC_AD2, IDC_ED1, IDC_ED2,
		IDC_EI1, IDC_EI2, IDC_EP1, IDC_EP2, IDC_TCOEFF, IDC_TOFFSET,
	    IDC_STATIC1, IDC_STATIC2, IDC_STATIC3, IDC_STATIC4, IDC_STATIC5, 
	    IDC_STATIC6, IDC_STATIC7, IDC_STATIC9,
	    IDC_STATIC_B1, IDC_STATIC_B2, IDC_STATIC_B5, 
	    IDC_STATIC_B6, IDC_STATIC_B7, IDC_STATIC_B8, IDC_STATIC_B9, IDC_STATIC_B10,
	    IDC_STATIC_B11,  0};     
	int Radio3items[]= {IDC_FILENAME, IDC_OPEN, 0};
	
	int i;
	for (i=0; Radio1items[i]!=0; i++) GetDlgItem(Radio1items[i])->EnableWindow(b1);
	for (i=0; Radio3items[i]!=0; i++) GetDlgItem(Radio3items[i])->EnableWindow(b3);
}
