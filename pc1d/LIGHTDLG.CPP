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
#include "lightdlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLightDlg dialog


CLightDlg::CLightDlg()
	: CPropertyPage(CLightDlg::IDD)
{
	//{{AFX_DATA_INIT(CLightDlg)
	m_IntensitySS = 0;
	m_IntensityTR1 = 0;
	m_IntensityTR2 = 0;
	m_Back = -1;
	m_Radio = -1;
	m_Enable = FALSE;
	//}}AFX_DATA_INIT
}

void CLightDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLightDlg)
	DDX_Text(pDX, IDC_ILLUMINATION_INTENSITYSS, m_IntensitySS);
	DDV_MinMaxDouble(pDX, m_IntensitySS, 0., 10000000.);
	DDX_Text(pDX, IDC_ILLUMINATION_INTENSITYTR1, m_IntensityTR1);
	DDV_MinMaxDouble(pDX, m_IntensityTR1, 0., 10000000.);
	DDX_Text(pDX, IDC_ILLUMINATION_INTENSITYTR2, m_IntensityTR2);
	DDV_MinMaxDouble(pDX, m_IntensityTR2, 0., 10000000.);
	DDX_Radio(pDX, IDC_FRONT, m_Back);
	DDX_Radio(pDX, IDC_RADIO1, m_Radio);
	DDX_Check(pDX, IDC_ENABLE, m_Enable);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLightDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CLightDlg)
	ON_BN_CLICKED(IDC_OPEN, OnOpen)
	ON_BN_CLICKED(IDC_RADIO1, DoDisabling)
	ON_BN_CLICKED(IDC_RADIO3, DoDisabling)
	ON_BN_CLICKED(IDC_ENABLE, DoDisabling)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CLightDlg message handlers

BOOL CLightDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	if (!m_title.IsEmpty()) SetWindowText(m_title);
	if (m_Filename.IsEmpty()) SetDlgItemText(IDC_FILENAME, "(No file selected)");
	else SetDlgItemText(IDC_FILENAME, m_Filename);
	DoDisabling();
	return TRUE;
}

void CLightDlg::OnOpen()
{
	BOOL FileOpen = TRUE;
	CString Filter = "Light Intensity (*."+m_Ext+")|*."+m_Ext+"|All Files (*.*)|*.*||";
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

void CLightDlg::DoDisabling()
{
	BOOL b1=((CButton*)GetDlgItem(IDC_RADIO1))->GetState();
	BOOL b3=!b1;
	
	BOOL bEnabled=((CButton*)GetDlgItem(IDC_ENABLE))->GetCheck();
	b1 &=bEnabled;
	b3 &=bEnabled;
	
	GetDlgItem(IDC_RADIO1)->EnableWindow(bEnabled);
	GetDlgItem(IDC_RADIO3)->EnableWindow(bEnabled);

	int GlobalItems[]={IDC_RADIO1, IDC_RADIO3, IDC_FRONT, IDC_BACK, IDC_STATIC7, 0};	          
	int Radio1items[]={	IDC_ILLUMINATION_INTENSITYSS,
		IDC_ILLUMINATION_INTENSITYTR1, IDC_ILLUMINATION_INTENSITYTR2,
	    IDC_STATIC1,IDC_STATIC2,IDC_STATIC3, 
	    IDC_STATIC4, IDC_STATIC5,IDC_STATIC6, 0};     
	int Radio3items[]= {IDC_FILENAME, IDC_OPEN, 0};
	
	int i;
	for (i=0; GlobalItems[i]!=0; i++) GetDlgItem(GlobalItems[i])->EnableWindow(bEnabled);
	for (i=0; Radio1items[i]!=0; i++) GetDlgItem(Radio1items[i])->EnableWindow(b1);
	for (i=0; Radio3items[i]!=0; i++) GetDlgItem(Radio3items[i])->EnableWindow(b3);
}