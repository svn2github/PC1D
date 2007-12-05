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
#include "descdlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDescDlg dialog


CDescDlg::CDescDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDescDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDescDlg)
	m_Description = "";
	//}}AFX_DATA_INIT
}

void CDescDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDescDlg)
	DDX_Text(pDX, IDC_EDIT, m_Description);
	DDV_MaxChars(pDX, m_Description, 500);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDescDlg, CDialog)
	//{{AFX_MSG_MAP(CDescDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDescDlg message handlers
