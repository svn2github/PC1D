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
#include "cntctdlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCntctDlg dialog


CCntctDlg::CCntctDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCntctDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCntctDlg)
	m_EmitterPosition = 0;
	m_BasePosition = 0;
	m_CollectorPosition = 0;
	m_BaseEnable = FALSE;
	m_CollectorEnable = FALSE;
	m_EmitterEnable = FALSE;
	m_BaseResistance = 0;
	m_CollectorResistance = 0;
	m_EmitterResistance = 0;
	//}}AFX_DATA_INIT
}

void CCntctDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCntctDlg)
	DDX_Text(pDX, IDC_POSITION, m_EmitterPosition);
	DDX_Text(pDX, IDC_POSITION2, m_BasePosition);
	DDX_Text(pDX, IDC_POSITION3, m_CollectorPosition);
	DDX_Check(pDX, IDC_BASE, m_BaseEnable);
	DDX_Check(pDX, IDC_COLLECTOR, m_CollectorEnable);
	DDX_Check(pDX, IDC_EMITTER, m_EmitterEnable);
	DDX_ScientificDouble(pDX, IDC_RESISTANCE2, m_BaseResistance);
	DDX_ScientificDouble(pDX, IDC_RESISTANCE3, m_CollectorResistance);
	DDX_ScientificDouble(pDX, IDC_RESISTANCE, m_EmitterResistance);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCntctDlg, CDialog)
	//{{AFX_MSG_MAP(CCntctDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCntctDlg message handlers

BOOL CCntctDlg::OnInitDialog()
{
	if (!m_title.IsEmpty()) SetWindowText(m_title);
	CDialog::OnInitDialog();
	return TRUE;
}
