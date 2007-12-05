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
#include "fxmobdlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFxMobDlg dialog


CFxMobDlg::CFxMobDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFxMobDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFxMobDlg)
	m_Elec = 0;
	m_Hole = 0;
	//}}AFX_DATA_INIT
}

void CFxMobDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFxMobDlg)
	DDX_Text(pDX, IDC_ELECTRONS, m_Elec);
	DDV_MinMaxDouble(pDX, m_Elec, 1.e-006, 100000.);
	DDX_Text(pDX, IDC_HOLES, m_Hole);
	DDV_MinMaxDouble(pDX, m_Hole, 1.e-006, 100000.);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFxMobDlg, CDialog)
	//{{AFX_MSG_MAP(CFxMobDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFxMobDlg message handlers
