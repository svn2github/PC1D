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
#include "batchdlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBatchDlg dialog


CBatchDlg::CBatchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBatchDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBatchDlg)
	m_Enable = FALSE;
	//}}AFX_DATA_INIT
}

void CBatchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBatchDlg)
	DDX_Check(pDX, IDC_ENABLE, m_Enable);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBatchDlg, CDialog)
	//{{AFX_MSG_MAP(CBatchDlg)
	ON_BN_CLICKED(IDC_ENABLE, DoDisabling)
	ON_BN_CLICKED(IDC_OPEN, OnOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBatchDlg message handlers

BOOL CBatchDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	if (m_Filename.IsEmpty()) SetDlgItemText(IDC_FILENAME, "(No file selected)");
	else SetDlgItemText(IDC_FILENAME, m_Filename);
	DoDisabling();
	return TRUE;
}

void CBatchDlg::DoDisabling()
{
	BOOL bEnabled=((CButton*)GetDlgItem(IDC_ENABLE))->GetCheck();
	int Radio1items[]= {IDC_FILENAME, IDC_OPEN, IDC_STATIC1, 0};
	int i;
	for (i=0; Radio1items[i]!=0; i++) GetDlgItem(Radio1items[i])->EnableWindow(bEnabled);
}

void CBatchDlg::OnOpen()
{
	BOOL FileOpen = TRUE;
	CString Filter = "Tab delimited text (*.txt)|*.txt|All Files (*.*)|*.*||";
	CFileDialog dlg(FileOpen, "txt", m_Filename, OFN_OVERWRITEPROMPT, Filter);
	dlg.m_ofn.lpstrTitle = "Select file for batch run";
	dlg.m_ofn.lpstrInitialDir = m_Path;
	if (dlg.DoModal()==IDOK)
	{
		m_Filename = dlg.GetPathName();
		m_Filename.MakeLower();
		SetDlgItemText(IDC_FILENAME, m_Filename);
	}                         
}
