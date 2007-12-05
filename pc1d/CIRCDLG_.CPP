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
#include "circdlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCircDlg dialog


CCircDlg::CCircDlg(UINT nIDCaption)
	: CPropertyPage(CCircDlg::IDD, nIDCaption)
{
	//{{AFX_DATA_INIT(CCircDlg)
	m_RSS = 0;
	m_RTR = 0;
	m_VSS = 0;
	m_VTR1 = 0;
	m_VTR2 = 0;
	m_OhmsSS = -1;
	m_OhmsTR = -1;
	m_Radio = -1;
	//}}AFX_DATA_INIT
}

void CCircDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCircDlg)
	DDX_Text(pDX, IDC_RSS, m_RSS);
	DDV_MinMaxDouble(pDX, m_RSS, 0., 1.e030);
	DDX_Text(pDX, IDC_RTR, m_RTR);
	DDV_MinMaxDouble(pDX, m_RTR, 0., 1.e030);
	DDX_Text(pDX, IDC_VSS, m_VSS);
	DDV_MinMaxDouble(pDX, m_VSS, -10000., 10000.);
	DDX_Text(pDX, IDC_VTR1, m_VTR1);
	DDV_MinMaxDouble(pDX, m_VTR1, -10000., 10000.);
	DDX_Text(pDX, IDC_VTR2, m_VTR2);
	DDV_MinMaxDouble(pDX, m_VTR2, -10000., 10000.);
	DDX_Radio(pDX, IDC_OHMCM2_SS, m_OhmsSS);
	DDX_Radio(pDX, IDC_OHMCM2_TR, m_OhmsTR);
	DDX_Radio(pDX, IDC_RADIO1, m_Radio);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCircDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CCircDlg)
	ON_BN_CLICKED(IDC_RADIO1, DoDisabling)
	ON_BN_CLICKED(IDC_OPEN, OnOpen)
	ON_BN_CLICKED(IDC_RADIO3, DoDisabling)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCircDlg message handlers

CCircDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	if (!m_title.IsEmpty()) SetWindowText(m_title);
	if (m_Filename.IsEmpty()) SetDlgItemText(IDC_FILENAME, "(No file selected)");
	else SetDlgItemText(IDC_FILENAME, m_Filename);
	DoDisabling();
	return TRUE;
}

void CCircDlg::DoDisabling()
{
	BOOL b1 = ((CButton*)GetDlgItem(IDC_RADIO1))->GetState();
	BOOL b2 = !b1;
	BOOL b1a, b1b;	
	if (b1){
		b1a=b1b=TRUE;
//		b1a=((CButton*)GetDlgItem(IDC_CONNECT_SS))->GetCheck();
//		b1b=((CButton*)GetDlgItem(IDC_CONNECT_TR))->GetCheck();
	}else { b1a=b1b=FALSE; };

	int Radio1items[]={IDC_STATIC4,IDC_STATIC_B6,0};
	int Radio1aitems[]= {IDC_STATIC1,IDC_STATIC2,IDC_STATIC3,  
			IDC_RSS,IDC_OHMCM2_SS,IDC_OHMS_SS,IDC_VSS, 0};
	int Radio1bitems[]= {IDC_STATIC_B1,IDC_STATIC_B2,IDC_STATIC_B3,IDC_STATIC_B4,IDC_STATIC_B5,
			IDC_RTR,IDC_OHMCM2_TR,IDC_OHMS_TR, IDC_VTR1,IDC_VTR2, 0};
	int Radio2items[]= {IDC_FILENAME, IDC_OPEN, 0};
	int i;
	for (i=0; Radio1items[i]!=0;  i++) GetDlgItem(Radio1items[i])->EnableWindow(b1);	
	for (i=0; Radio1aitems[i]!=0; i++) GetDlgItem(Radio1aitems[i])->EnableWindow(b1a);	
	for (i=0; Radio1bitems[i]!=0; i++) GetDlgItem(Radio1bitems[i])->EnableWindow(b1b);	
	for (i=0; Radio2items[i]!=0;  i++) GetDlgItem(Radio2items[i])->EnableWindow(b2);	
}

void CCircDlg::OnOpen()
{
	BOOL FileOpen = TRUE;
	CString Filter = "Voltage (*."+m_Ext+")|*."+m_Ext+"|All Files (*.*)|*.*||";
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
