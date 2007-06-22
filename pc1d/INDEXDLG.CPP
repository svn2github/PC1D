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
#include "indexdlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIndexDlg dialog


CIndexDlg::CIndexDlg()
	: CPropertyPage(CIndexDlg::IDD)
{
	//{{AFX_DATA_INIT(CIndexDlg)
	m_Index = 0;
	m_Radio = -1;
	//}}AFX_DATA_INIT
}

void CIndexDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIndexDlg)
	DDX_Text(pDX, IDC_INDEX, m_Index);
	DDV_MinMaxDouble(pDX, m_Index, 1., 20.);
	DDX_Radio(pDX, IDC_RADIO1, m_Radio);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CIndexDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CIndexDlg)
	ON_BN_CLICKED(IDC_OPEN, OnOpen)
	ON_BN_CLICKED(IDC_RADIO1, DoDisabling)
	ON_BN_CLICKED(IDC_RADIO2, DoDisabling)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CIndexDlg message handlers

BOOL CIndexDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	if (m_Filename.IsEmpty()) SetDlgItemText(IDC_FILENAME, "(No file selected)");
	else SetDlgItemText(IDC_FILENAME, m_Filename);
	DoDisabling();
	return TRUE;
}


void CIndexDlg::OnOpen()
{
	BOOL FileOpen = TRUE;
	CString Filter = "Refraction Index (*."+m_Ext+")|*."+m_Ext+"|All Files (*.*)|*.*||";
	CFileDialog dlg(FileOpen, m_Ext, m_Filename, OFN_OVERWRITEPROMPT, Filter);
	dlg.m_ofn.lpstrTitle = "Open Index of Refraction File";
	dlg.m_ofn.lpstrInitialDir = m_Path;
	if (dlg.DoModal()==IDOK)
	{
		m_Filename = dlg.GetPathName();
		m_Filename.MakeLower();
		SetDlgItemText(IDC_FILENAME, m_Filename);
	}                         
}

void CIndexDlg::DoDisabling()
{
	BOOL b=((CButton*)GetDlgItem(IDC_RADIO1))->GetState();
	GetDlgItem(IDC_FILENAME)->EnableWindow(!b);
	GetDlgItem(IDC_OPEN)->EnableWindow(!b);
	GetDlgItem(IDC_INDEX)->EnableWindow(b);
	GetDlgItem(IDC_STATIC1)->EnableWindow(b);
}
