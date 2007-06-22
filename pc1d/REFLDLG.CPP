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
#include "refldlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReflectDlg dialog


CReflectDlg::CReflectDlg(UINT tabnameID)
	: CPropertyPage(CReflectDlg::IDD, tabnameID)
{
	//{{AFX_DATA_INIT(CReflectDlg)
	m_Radio = -1;
	m_Broadband = 0;
	m_Fixed = 0;
	m_InnerIndex = 0;
	m_InnerThick = 0;
	m_MiddleIndex = 0;
	m_MiddleThick = 0;
	m_OuterIndex = 0;
	m_OuterThick = 0;
	//}}AFX_DATA_INIT
}

void CReflectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CReflectDlg)
	DDX_Radio(pDX, IDC_RADIO1, m_Radio);
	DDX_Text(pDX, IDC_BROADBAND, m_Broadband);
	DDX_Text(pDX, IDC_FIXED, m_Fixed);
	DDX_Text(pDX, IDC_INNERINDEX, m_InnerIndex);
	DDX_Text(pDX, IDC_INNERTHICK, m_InnerThick);
	DDX_Text(pDX, IDC_MIDDLEINDEX, m_MiddleIndex);
	DDX_Text(pDX, IDC_MIDDLETHICK, m_MiddleThick);
	DDX_Text(pDX, IDC_OUTERINDEX, m_OuterIndex);
	DDX_Text(pDX, IDC_OUTERTHICK, m_OuterThick);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CReflectDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CReflectDlg)
	ON_BN_CLICKED(IDC_RADIO1, DoDisabling)
	ON_BN_CLICKED(IDC_RADIO2, DoDisabling)
	ON_BN_CLICKED(IDC_RADIO3, DoDisabling)
	ON_BN_CLICKED(IDC_OPEN, OnOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CReflectDlg message handlers

BOOL CReflectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	if (!m_title.IsEmpty()) SetWindowText(m_title);	
	if (m_Filename.IsEmpty()) SetDlgItemText(IDC_FILENAME, "(No file selected)");
	else SetDlgItemText(IDC_FILENAME, m_Filename);
	DoDisabling();
	return TRUE;
}

void CReflectDlg::DoDisabling()
{
	BOOL b1=((CButton*)GetDlgItem(IDC_RADIO1))->GetState();
	BOOL b2=((CButton*)GetDlgItem(IDC_RADIO2))->GetState();
	BOOL b3=! ( b1 | b2); 

	int Radio1items[]= {IDC_FIXED, IDC_STATIC1, 0};
	int Radio2items[]= {IDC_BROADBAND, IDC_OUTERTHICK,IDC_OUTERINDEX, 
		IDC_MIDDLETHICK, IDC_MIDDLEINDEX, IDC_INNERTHICK, IDC_INNERINDEX,
		IDC_STATIC_B1,IDC_STATIC_B2,IDC_STATIC_B3,IDC_STATIC_B4,
		IDC_STATIC_B5,IDC_STATIC_B6,IDC_STATIC_B7,IDC_STATIC_B8, 0};
	int Radio3items[]= {IDC_FILENAME, IDC_OPEN, 0};
	int i;
	for (i=0; Radio1items[i]!=0; i++) GetDlgItem(Radio1items[i])->EnableWindow(b1);	
	for (i=0; Radio2items[i]!=0; i++) GetDlgItem(Radio2items[i])->EnableWindow(b2);	
	for (i=0; Radio3items[i]!=0; i++) GetDlgItem(Radio3items[i])->EnableWindow(b3);	
}

void CReflectDlg::OnOpen()
{
	BOOL FileOpen = TRUE;
	CString Filter = "Reflectance (*."+m_Ext+")|*."+m_Ext+"|All Files (*.*)|*.*||";
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
