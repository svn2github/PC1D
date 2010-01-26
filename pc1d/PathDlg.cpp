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
#include "pathdlg.h"
#include "path.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPathDlg dialog


CPathDlg::CPathDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPathDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPathDlg)
	m_abs = "";
	m_dev = "";
	m_dop = "";
	m_exc = "";
	m_gen = "";
	m_inr = "";
	m_lgt = "";
	m_mat = "";
	m_prm = "";
	m_ref = "";
	m_spc = "";
	m_vlt = "";
	m_bat = "";
	//}}AFX_DATA_INIT
}

void CPathDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPathDlg)
	DDX_Text(pDX, IDC_ABS, m_abs);
	DDV_MaxChars(pDX, m_abs, 63);
	DDX_Text(pDX, IDC_DEV, m_dev);
	DDV_MaxChars(pDX, m_dev, 63);
	DDX_Text(pDX, IDC_DOP, m_dop);
	DDV_MaxChars(pDX, m_dop, 63);
	DDX_Text(pDX, IDC_EXC, m_exc);
	DDV_MaxChars(pDX, m_exc, 63);
	DDX_Text(pDX, IDC_GEN, m_gen);
	DDV_MaxChars(pDX, m_gen, 63);
	DDX_Text(pDX, IDC_INR, m_inr);
	DDV_MaxChars(pDX, m_inr, 63);
	DDX_Text(pDX, IDC_LGT, m_lgt);
	DDV_MaxChars(pDX, m_lgt, 63);
	DDX_Text(pDX, IDC_MAT, m_mat);
	DDV_MaxChars(pDX, m_mat, 63);
	DDX_Text(pDX, IDC_PRM, m_prm);
	DDV_MaxChars(pDX, m_prm, 63);
	DDX_Text(pDX, IDC_REF, m_ref);
	DDV_MaxChars(pDX, m_ref, 63);
	DDX_Text(pDX, IDC_SPC, m_spc);
	DDV_MaxChars(pDX, m_spc, 63);
	DDX_Text(pDX, IDC_VLT, m_vlt);
	DDV_MaxChars(pDX, m_vlt, 63);
	DDX_Text(pDX, IDC_BATCH, m_bat);
	DDV_MaxChars(pDX, m_bat, 63);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPathDlg, CDialog)
	//{{AFX_MSG_MAP(CPathDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPathDlg message handlers
