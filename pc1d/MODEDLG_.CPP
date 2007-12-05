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
#include "modedlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModeDlg dialog


CModeDlg::CModeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModeDlg)
	m_FirstStep = 0;
	m_TranNum = 0;
	m_TranStep = 0;
	m_Mode = -1;
	//}}AFX_DATA_INIT
}

void CModeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModeDlg)
	DDX_Text(pDX, IDC_FIRSTSTEP, m_FirstStep);
	DDX_Text(pDX, IDC_TRAN_NUM, m_TranNum);
	DDV_MinMaxInt(pDX, m_TranNum, 1, 200);
	DDX_Text(pDX, IDC_TRAN_STEP, m_TranStep);
	DDX_Radio(pDX, IDC_RADIOEQUILIBRIUM, m_Mode);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CModeDlg, CDialog)
	//{{AFX_MSG_MAP(CModeDlg)
	ON_BN_CLICKED(IDC_RADIOEQUILIBRIUM, DoDisabling)
	ON_BN_CLICKED(IDC_RADIOSTEADYSTATE, DoDisabling)
	ON_BN_CLICKED(IDC_RADIOTRANSIENT, DoDisabling)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CModeDlg message handlers

BOOL CModeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	DoDisabling();
	return TRUE;
}

void CModeDlg::DoDisabling()
{
	BOOL b=((CButton*)GetDlgItem(IDC_RADIOTRANSIENT))->GetState();
	GetDlgItem(IDC_TRAN_NUM)->EnableWindow(b);
	GetDlgItem(IDC_TRAN_STEP)->EnableWindow(b);
	GetDlgItem(IDC_FIRSTSTEP)->EnableWindow(b);
	GetDlgItem(IDC_STATIC1)->EnableWindow(b);
	GetDlgItem(IDC_STATIC2)->EnableWindow(b);
	GetDlgItem(IDC_STATIC3)->EnableWindow(b);
	GetDlgItem(IDC_STATIC4)->EnableWindow(b);
	GetDlgItem(IDC_STATIC5)->EnableWindow(b);
}
