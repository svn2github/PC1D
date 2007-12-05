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
#include "pc1ddoc.h"
#include "plotload.h"
#include "graphdlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGraphDlg dialog


CGraphDlg::CGraphDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGraphDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGraphDlg)
	m_Abscissa = "";
	m_Curve1 = "";
	m_Curve2 = "";
	m_Curve3 = "";
	m_Curve4 = "";
	m_Title = "";
	m_Temporal = -1;
	m_bYLog = FALSE;
	m_bXLog = FALSE;
	m_YLabel = _T("");
	//}}AFX_DATA_INIT
}

void CGraphDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGraphDlg)
	DDX_Control(pDX, IDC_CURVE4, m_Curve4Box);
	DDX_Control(pDX, IDC_CURVE3, m_Curve3Box);
	DDX_Control(pDX, IDC_CURVE2, m_Curve2Box);
	DDX_Control(pDX, IDC_CURVE1, m_Curve1Box);
	DDX_Control(pDX, IDC_ABSCISSA, m_AbscissaBox);
	DDX_CBString(pDX, IDC_ABSCISSA, m_Abscissa);
	DDV_MaxChars(pDX, m_Abscissa, 30);
	DDX_CBString(pDX, IDC_CURVE1, m_Curve1);
	DDV_MaxChars(pDX, m_Curve1, 30);
	DDX_CBString(pDX, IDC_CURVE2, m_Curve2);
	DDV_MaxChars(pDX, m_Curve2, 30);
	DDX_CBString(pDX, IDC_CURVE3, m_Curve3);
	DDV_MaxChars(pDX, m_Curve3, 30);
	DDX_CBString(pDX, IDC_CURVE4, m_Curve4);
	DDV_MaxChars(pDX, m_Curve4, 30);
	DDX_Text(pDX, IDC_GRAPHTITLE, m_Title);
	DDV_MaxChars(pDX, m_Title, 30);
	DDX_Radio(pDX, IDC_SPATIAL, m_Temporal);
	DDX_Check(pDX, IDC_YSCALE, m_bYLog);
	DDX_Check(pDX, IDC_XSCALE, m_bXLog);
	DDX_Text(pDX, IDC_YLABEL, m_YLabel);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGraphDlg, CDialog)
	//{{AFX_MSG_MAP(CGraphDlg)
	ON_BN_CLICKED(IDC_SPATIAL, OnSpatial)
	ON_BN_CLICKED(IDC_TEMPORAL, OnTemporal)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg message handlers

void CGraphDlg::SetLabelLists()
{
	GraphableQuantity gr=CData::GetFirstQuantity();
	CString lab;
	while (gr) {
		if (CData::GetType(gr)==m_Temporal) {
			lab=CData::GetLabel(gr);
			m_AbscissaBox.AddString(lab);
			m_Curve1Box.AddString(lab);
			m_Curve2Box.AddString(lab);
			m_Curve3Box.AddString(lab);
			m_Curve4Box.AddString(lab);
		}
		if (CData::GetType(gr)==EXPERIMENTAL_ARRAY) {
			// it's not added to abscissa, but IS added to curves
			// notice it's included in both space & time arrays
			lab=CData::GetLabel(gr);
			m_Curve1Box.AddString(lab);
			m_Curve2Box.AddString(lab);
			m_Curve3Box.AddString(lab);
			m_Curve4Box.AddString(lab);
		}
		gr=CData::GetNextQuantity(gr);
	}            
	m_AbscissaBox.AddString(NONE);
	m_Curve1Box.AddString(NONE);
	m_Curve2Box.AddString(NONE);
	m_Curve3Box.AddString(NONE);
	m_Curve4Box.AddString(NONE);            
	if (m_Abscissa.IsEmpty()) m_Abscissa=NONE;
	if (m_Curve1.IsEmpty()) m_Curve1 = NONE;
	if (m_Curve2.IsEmpty()) m_Curve2 = NONE;
	if (m_Curve3.IsEmpty()) m_Curve3 = NONE;
	if (m_Curve4.IsEmpty()) m_Curve4 = NONE;
	m_AbscissaBox.SelectString(-1,m_Abscissa);
	m_Curve1Box.SelectString(-1,m_Curve1);
	m_Curve2Box.SelectString(-1,m_Curve2);
	m_Curve3Box.SelectString(-1,m_Curve3);
	m_Curve4Box.SelectString(-1,m_Curve4);
}


BOOL CGraphDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
//	m_XscaleBox.AddString("Linear");
//	m_XscaleBox.AddString("Log");
//	m_XscaleBox.SelectString(-1, m_Xscale);
//	m_YscaleBox.AddString("Linear");
//	m_YscaleBox.AddString("Log");
//	m_YscaleBox.SelectString(-1, m_Yscale);		
	SetLabelLists();
	return TRUE;
}

void CGraphDlg::OnSpatial()
{
	if (!m_Temporal) return;
	while (m_AbscissaBox.DeleteString(0)>0);
	while (m_Curve1Box.DeleteString(0)>0);
	while (m_Curve2Box.DeleteString(0)>0);
	while (m_Curve3Box.DeleteString(0)>0);
	while (m_Curve4Box.DeleteString(0)>0);
	m_Temporal=FALSE;
	SetLabelLists();
	m_Abscissa = "Distance from Front";
	m_AbscissaBox.SelectString(-1,m_Abscissa);
	m_Curve1.Empty(); 	m_Curve1Box.SelectString(-1,NONE);
	m_Curve2.Empty(); 	m_Curve2Box.SelectString(-1,NONE); 
	m_Curve3.Empty(); 	m_Curve3Box.SelectString(-1,NONE); 
	m_Curve4.Empty(); 	m_Curve4Box.SelectString(-1,NONE);
	GetDlgItem(IDC_GRAPHTITLE)->SetWindowText("");
}

void CGraphDlg::OnTemporal()
{
	if (m_Temporal) return;
	while (m_AbscissaBox.DeleteString(0)>0);
	while (m_Curve1Box.DeleteString(0)>0);
	while (m_Curve2Box.DeleteString(0)>0);
	while (m_Curve3Box.DeleteString(0)>0);
	while (m_Curve4Box.DeleteString(0)>0);
	m_Temporal=TRUE;
	SetLabelLists();
	m_Abscissa = "Elapsed Time";
	m_AbscissaBox.SelectString(-1,m_Abscissa);
	m_Curve1.Empty(); 	m_Curve1Box.SelectString(-1,NONE);
	m_Curve2.Empty(); 	m_Curve2Box.SelectString(-1,NONE); 
	m_Curve3.Empty(); 	m_Curve3Box.SelectString(-1,NONE); 
	m_Curve4.Empty(); 	m_Curve4Box.SelectString(-1,NONE);
	GetDlgItem(IDC_GRAPHTITLE)->SetWindowText("");
}
