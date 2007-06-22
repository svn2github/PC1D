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
#include "spectdlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpectrumDlg dialog


CSpectrumDlg::CSpectrumDlg()
	: CPropertyPage(CSpectrumDlg::IDD)
{
	//{{AFX_DATA_INIT(CSpectrumDlg)
	m_Temperature = 0;
	m_Number = 0;
	m_Maximum = 0;
	m_Minimum = 0;
	m_LambdaTR1 = 0;
	m_LambdaTR2 = 0;
	m_Radio = -1;
	m_LambdaSS = 0;
	//}}AFX_DATA_INIT
}

void CSpectrumDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpectrumDlg)
	DDX_Text(pDX, IDC_TEMPERATURE, m_Temperature);
	DDX_Text(pDX, IDC_NUMBER, m_Number);
	DDV_MinMaxInt(pDX, m_Number, 10, 50);
	DDX_Text(pDX, IDC_MAXIMUM, m_Maximum);
	DDX_Text(pDX, IDC_MINIMUM, m_Minimum);
	DDX_Text(pDX, IDC_ILLUMINATION_LAMBDATR1, m_LambdaTR1);
	DDX_Text(pDX, IDC_ILLUMINATION_LAMBDATR2, m_LambdaTR2);
	DDX_Radio(pDX, IDC_RADIO1, m_Radio);
	DDX_Text(pDX, IDC_ILLUMINATION_LAMBDASS, m_LambdaSS);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSpectrumDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CSpectrumDlg)
	ON_BN_CLICKED(IDC_OPEN, OnOpen)
	ON_BN_CLICKED(IDC_RADIO1, DoDisabling)
	ON_BN_CLICKED(IDC_RADIO2, DoDisabling)
	ON_BN_CLICKED(IDC_RADIO3, DoDisabling)
	ON_BN_CLICKED(IDC_ENABLE, DoDisabling)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSpectrumDlg message handlers

BOOL CSpectrumDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	if (!m_title.IsEmpty()) SetWindowText(m_title);	
	if (m_Filename.IsEmpty()) SetDlgItemText(IDC_FILENAME, "(No file selected)");
	else SetDlgItemText(IDC_FILENAME, m_Filename);
	DoDisabling();
	return TRUE;
}


void CSpectrumDlg::OnOpen()
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

void CSpectrumDlg::DoDisabling()
{
	BOOL b1=((CButton*)GetDlgItem(IDC_RADIO1))->GetState();
	BOOL b2=((CButton*)GetDlgItem(IDC_RADIO2))->GetState();
	BOOL b3=! ( b1 | b2);
	
//	BOOL bEnabled=((CButton*)GetDlgItem(IDC_ENABLE))->GetCheck();
BOOL bEnabled=TRUE;
	b1 &=bEnabled;
	b2 &=bEnabled;
	b3 &=bEnabled;
	GetDlgItem(IDC_RADIO1)->EnableWindow(bEnabled);
	GetDlgItem(IDC_RADIO2)->EnableWindow(bEnabled);
	GetDlgItem(IDC_RADIO3)->EnableWindow(bEnabled);

	int Radio1items[]= {
			IDC_ILLUMINATION_LAMBDASS, IDC_ILLUMINATION_LAMBDATR1,IDC_ILLUMINATION_LAMBDATR2,
	 		IDC_STATIC1,IDC_STATIC2,IDC_STATIC3,IDC_STATIC4,IDC_STATIC5,IDC_STATIC6, 0};
	int Radio2items[]= {
			IDC_TEMPERATURE, IDC_MINIMUM, IDC_MAXIMUM ,IDC_NUMBER,
			IDC_STATIC_B1,IDC_STATIC_B2,IDC_STATIC_B3,IDC_STATIC_B4,IDC_STATIC_B5,
			IDC_STATIC_B6,IDC_STATIC_B7,IDC_STATIC_B8, 0};
	int Radio3items[]= {IDC_FILENAME, IDC_OPEN, 0};
	int i;
	for (i=0; Radio1items[i]!=0; i++) GetDlgItem(Radio1items[i])->EnableWindow(b1);	
	for (i=0; Radio2items[i]!=0; i++) GetDlgItem(Radio2items[i])->EnableWindow(b2);	
	for (i=0; Radio3items[i]!=0; i++) GetDlgItem(Radio3items[i])->EnableWindow(b3);
}
