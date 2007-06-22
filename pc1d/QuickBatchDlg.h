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

/////////////////////////////////////////////////////////////////////////////
// CQuickBatchDlg dialog

class CQuickBatchDlg : public CDialog
{
// Construction
public:
	CQuickBatchDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CQuickBatchDlg)
	enum { IDD = IDD_QUICKBATCH };
	double	m_From1;
	double	m_From2;
	double	m_From3;
	double	m_From4;
	BOOL	m_Log1;
	BOOL	m_Log2;
	BOOL	m_Log3;
	BOOL	m_Log4;
	int		m_Steps1;
	int		m_Steps2;
	int		m_Steps3;
	int		m_Steps4;
	double	m_To1;
	double	m_To2;
	double	m_To3;
	double	m_To4;
	int		m_Reg1;
	int		m_Reg2;
	int		m_Reg3;
	int		m_Reg4;
	BOOL	m_Same1;
	BOOL	m_Same2;
	BOOL	m_Same3;
	int		m_Radio;
	//}}AFX_DATA
	CComboBox	m_ParmBox[4];
	CComboBox	m_ResultBox[4];
	CString	m_Result[4];
	CString	m_Parm[4];

	CString m_Filename;
	CString m_Path;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQuickBatchDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL OnInitDialog();

	// Generated message map functions
	//{{AFX_MSG(CQuickBatchDlg)
	afx_msg void DoDisabling();
	afx_msg void OnSelChange();
	afx_msg void OnOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
