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
// CGraphDlg dialog

#define NONE " (none)"

class CGraphDlg : public CDialog
{
// Construction
public:
	CGraphDlg(CWnd* pParent = NULL);
// Dialog Data
	//{{AFX_DATA(CGraphDlg)
	enum { IDD = IDD_USERGRAPH };
	CComboBox	m_Curve4Box;
	CComboBox	m_Curve3Box;
	CComboBox	m_Curve2Box;
	CComboBox	m_Curve1Box;
	CComboBox	m_AbscissaBox;
	CString	m_Abscissa;
	CString	m_Curve1;
	CString	m_Curve2;
	CString	m_Curve3;
	CString	m_Curve4;
	CString	m_Title;
	int		m_Temporal;
	BOOL	m_bYLog;
	BOOL	m_bXLog;
	CString	m_YLabel;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	BOOL OnInitDialog();
	void SetLabelLists();

	// Generated message map functions
	//{{AFX_MSG(CGraphDlg)
	afx_msg void OnSpatial();
	afx_msg void OnTemporal();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
