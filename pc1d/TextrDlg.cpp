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
#include "textrdlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTextrDlg dialog


CTextrDlg::CTextrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTextrDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTextrDlg)
	m_CheckFrontTexture = FALSE;
	m_CheckRearTexture = FALSE;
	m_FrontDepth = 0;
	m_FrontAngle = 0;
	m_RearAngle = 0;
	m_RearDepth = 0;
	//}}AFX_DATA_INIT
}

void CTextrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextrDlg)
	DDX_Check(pDX, IDC_CHECKFRONTTEXTURE, m_CheckFrontTexture);
	DDX_Check(pDX, IDC_CHECKREARTEXTURE, m_CheckRearTexture);
	DDX_Text(pDX, IDC_FRONTDEPTH, m_FrontDepth);
	DDX_Text(pDX, IDC_FRONTANGLE, m_FrontAngle);
	DDX_Text(pDX, IDC_REARANGLE, m_RearAngle);
	DDX_Text(pDX, IDC_REARDEPTH, m_RearDepth);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTextrDlg, CDialog)
	//{{AFX_MSG_MAP(CTextrDlg)
	ON_BN_CLICKED(IDC_CHECKFRONTTEXTURE, OnCheckfronttexture)
	ON_BN_CLICKED(IDC_CHECKREARTEXTURE, OnCheckreartexture)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTextrDlg message handlers

BOOL CTextrDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	DoDisabling();
	return TRUE;
}

void CTextrDlg::DoDisabling()
{
	BOOL f= ((CButton *)GetDlgItem(IDC_CHECKFRONTTEXTURE))->GetCheck();
	BOOL r=((CButton *)GetDlgItem(IDC_CHECKREARTEXTURE))->GetCheck();
	GetDlgItem(IDC_FRONTANGLE)->EnableWindow(f);
	GetDlgItem(IDC_FRONTDEPTH)->EnableWindow(f);
	GetDlgItem(IDC_STATICF1)->EnableWindow(f);
	GetDlgItem(IDC_STATICF2)->EnableWindow(f);
	GetDlgItem(IDC_STATICF3)->EnableWindow(f);	
	GetDlgItem(IDC_STATICF4)->EnableWindow(f);
	GetDlgItem(IDC_REARANGLE)->EnableWindow(r);
	GetDlgItem(IDC_REARDEPTH)->EnableWindow(r);
	GetDlgItem(IDC_STATICR1)->EnableWindow(r);
	GetDlgItem(IDC_STATICR2)->EnableWindow(r);
	GetDlgItem(IDC_STATICR3)->EnableWindow(r);	
	GetDlgItem(IDC_STATICR4)->EnableWindow(r);
}
void CTextrDlg::OnCheckfronttexture()
{
	DoDisabling();	
}

void CTextrDlg::OnCheckreartexture()
{
	DoDisabling();
	
}
