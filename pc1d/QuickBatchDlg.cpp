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
// QuickBatchDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "QuickBatchDlg.h"
#include "Batch.h"	// for parameter names
#include "useful.h" // for status bar

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQuickBatchDlg dialog


CQuickBatchDlg::CQuickBatchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQuickBatchDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CQuickBatchDlg)
	m_From1 = 0;
	m_From2 = 0;
	m_From3 = 0;
	m_From4 = 0;
	m_Log1 = FALSE;
	m_Log2 = FALSE;
	m_Log3 = FALSE;
	m_Log4 = FALSE;
	m_Steps1 = 0;
	m_Steps2 = 0;
	m_Steps3 = 0;
	m_Steps4 = 0;
	m_To1 = 0;
	m_To2 = 0;
	m_To3 = 0;
	m_To4 = 0;
	m_Reg1 = 0;
	m_Reg2 = 0;
	m_Reg3 = 0;
	m_Reg4 = 0;
	m_Same1 = FALSE;
	m_Same2 = FALSE;
	m_Same3 = FALSE;
	m_Radio = -1;
	//}}AFX_DATA_INIT
	int k;
	for (k=0; k<4; k++) m_Result[k]= _T("");
	for (k=0; k<4; k++) m_Parm[k] = _T("");
}

void CQuickBatchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQuickBatchDlg)
	DDX_ScientificDouble(pDX, IDC_FROM1, m_From1);
	DDX_ScientificDouble(pDX, IDC_FROM2, m_From2);
	DDX_ScientificDouble(pDX, IDC_FROM3, m_From3);
	DDX_ScientificDouble(pDX, IDC_FROM4, m_From4);
	DDX_Check(pDX, IDC_LOG1, m_Log1);
	DDX_Check(pDX, IDC_LOG2, m_Log2);
	DDX_Check(pDX, IDC_LOG3, m_Log3);
	DDX_Check(pDX, IDC_LOG4, m_Log4);
	DDX_Text(pDX, IDC_STEPS1, m_Steps1);
	DDV_MinMaxInt(pDX, m_Steps1, 1, 100);
	DDX_Text(pDX, IDC_STEPS2, m_Steps2);
	DDV_MinMaxInt(pDX, m_Steps2, 1, 100);
	DDX_Text(pDX, IDC_STEPS3, m_Steps3);
	DDV_MinMaxInt(pDX, m_Steps3, 1, 100);
	DDX_Text(pDX, IDC_STEPS4, m_Steps4);
	DDV_MinMaxInt(pDX, m_Steps4, 1, 100);
	DDX_ScientificDouble(pDX, IDC_TO1, m_To1);
	DDX_ScientificDouble(pDX, IDC_TO2, m_To2);
	DDX_ScientificDouble(pDX, IDC_TO3, m_To3);
	DDX_ScientificDouble(pDX, IDC_TO4, m_To4);
	DDX_Text(pDX, IDC_REG1, m_Reg1);
	DDV_MinMaxInt(pDX, m_Reg1, 1, 5);
	DDX_Text(pDX, IDC_REG2, m_Reg2);
	DDV_MinMaxInt(pDX, m_Reg2, 1, 5);
	DDX_Text(pDX, IDC_REG3, m_Reg3);
	DDV_MinMaxInt(pDX, m_Reg3, 1, 5);
	DDX_Text(pDX, IDC_REG4, m_Reg4);
	DDV_MinMaxInt(pDX, m_Reg4, 1, 5);
	DDX_Check(pDX, IDC_SAME1, m_Same1);
	DDX_Check(pDX, IDC_SAME2, m_Same2);
	DDX_Check(pDX, IDC_SAME3, m_Same3);
	DDX_Radio(pDX, IDC_RADIO1, m_Radio);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_PARM4, m_ParmBox[3]);
	DDX_Control(pDX, IDC_PARM3, m_ParmBox[2]);
	DDX_Control(pDX, IDC_PARM2, m_ParmBox[1]);
	DDX_Control(pDX, IDC_PARM1, m_ParmBox[0]);
	DDX_Control(pDX, IDC_OUTPUT4, m_ResultBox[3]);
	DDX_Control(pDX, IDC_OUTPUT3, m_ResultBox[2]);
	DDX_Control(pDX, IDC_OUTPUT2, m_ResultBox[1]);
	DDX_Control(pDX, IDC_OUTPUT1, m_ResultBox[0]);

	DDX_CBString(pDX, IDC_PARM1, m_Parm[0]);
	DDX_CBString(pDX, IDC_PARM2, m_Parm[1]);
	DDX_CBString(pDX, IDC_PARM3, m_Parm[2]);
	DDX_CBString(pDX, IDC_PARM4, m_Parm[3]);

	DDX_CBString(pDX, IDC_OUTPUT1, m_Result[0]);
	DDX_CBString(pDX, IDC_OUTPUT2, m_Result[1]);
	DDX_CBString(pDX, IDC_OUTPUT3, m_Result[2]);
	DDX_CBString(pDX, IDC_OUTPUT4, m_Result[3]);
}


BEGIN_MESSAGE_MAP(CQuickBatchDlg, CDialog)
	//{{AFX_MSG_MAP(CQuickBatchDlg)
	ON_BN_CLICKED(IDC_SAME1, DoDisabling)
	ON_CBN_SELCHANGE(IDC_PARM1, OnSelChange)
	ON_BN_CLICKED(IDC_SAME2, DoDisabling)
	ON_BN_CLICKED(IDC_SAME3, DoDisabling)
	ON_CBN_SELCHANGE(IDC_PARM2, OnSelChange)
	ON_CBN_SELCHANGE(IDC_PARM3, OnSelChange)
	ON_CBN_SELCHANGE(IDC_PARM4, OnSelChange)
	ON_CBN_SELCHANGE(IDC_OUTPUT1, OnSelChange)
	ON_CBN_SELCHANGE(IDC_OUTPUT2, OnSelChange)
	ON_CBN_SELCHANGE(IDC_OUTPUT3, OnSelChange)
	ON_CBN_SELCHANGE(IDC_OUTPUT4, OnSelChange)
	ON_BN_CLICKED(IDC_OPEN, OnOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQuickBatchDlg message handlers

const char *NOPARAMETER="(None)";

BOOL CQuickBatchDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	int k;

	if (m_Filename.IsEmpty()) SetDlgItemText(IDC_FILENAME, "(No file selected)");
	else SetDlgItemText(IDC_FILENAME, m_Filename);

	CString str;
	for (k=0; k<4; k++) m_ParmBox[k].AddString(NOPARAMETER);
	CBatch::BatchParameter i;
	for (i=CBatch::FirstBatchParameter; i< CBatch::FirstResultsParameter; i++) {
		str=CBatch::BatchParam2Str(i);
		for (k=0; k<4; k++) m_ParmBox[k].AddString(str);
	}
	for (k=0; k<4; k++) {
		if (m_Parm[k].IsEmpty()) m_Parm[k] = NOPARAMETER;
		m_ParmBox[k].SelectString(-1, m_Parm[k]);
	}

	for (k=0; k<4; k++)	m_ResultBox[k].AddString(NOPARAMETER);
	for (i=CBatch::FirstResultsParameter; i < CBatch::FirstSpatialResult; i++) {
		str=CBatch::BatchParam2Str(i);
		for (k=0; k<4; k++) m_ResultBox[k].AddString(str);
	}
	for (k=0; k<4; k++) {
		if (m_Result[k].IsEmpty()) m_Result[k] = NOPARAMETER;
		m_ResultBox[k].SelectString(-1, m_Result[k]);
	}
	DoDisabling();
	OnSelChange();
	return TRUE;
}

void CQuickBatchDlg::DoDisabling()
{
	int JoinItems[]={IDC_SAME1, IDC_SAME2, IDC_SAME3};
	int StepItems[]={IDC_STEPS2, IDC_STEPS3, IDC_STEPS4};

	int i;
	for (i=0; i<3; i++) {
		BOOL bJoin=((CButton*)GetDlgItem(JoinItems[i]))->GetCheck();
		GetDlgItem(StepItems[i])->EnableWindow(bJoin);
	}
}

void CQuickBatchDlg::OnSelChange() 
{
	int RegItems[]={IDC_REG1, IDC_REG2, IDC_REG3, IDC_REG4};
	int UnitsItems[]={IDC_UNITS1, IDC_UNITS2, IDC_UNITS3, IDC_UNITS4};

	CString str;
	BOOL bReg;
	CBatch::BatchParameter bp;
	int i;
	CString outstr="";
	CWnd *focus=GetFocus();
	for (i=0; i<4; i++) {		
		m_ParmBox[i].GetLBText(m_ParmBox[i].GetCurSel(), str);
		bp=CBatch::Str2BatchParam(str);
		if (focus->m_hWnd==m_ParmBox[i].m_hWnd) outstr=CBatch::GetDescriptionStr(bp);
		bReg=CBatch::IsRegionParameter(bp);
		GetDlgItem(RegItems[i])->EnableWindow(bReg);
		SetDlgItemText(UnitsItems[i], CBatch::GetUnitsStr(bp));
  	}
	for (i=0; i<4; i++) {		
		m_ResultBox[i].GetLBText(m_ResultBox[i].GetCurSel(), str);
		bp=CBatch::Str2BatchParam(str);
		if (focus->m_hWnd==m_ResultBox[i].m_hWnd) outstr=CBatch::GetDescriptionStr(bp);
  	}
	SetDlgItemText(IDC_DESCRIPTION, outstr);
//	WriteStatusBarMessage(outstr);
}

void CQuickBatchDlg::OnOpen() 
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
