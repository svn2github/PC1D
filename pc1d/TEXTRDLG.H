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
// CTextrDlg dialog

class CTextrDlg : public CDialog
{
// Construction
public:
	CTextrDlg(CWnd* pParent = NULL);	// standard constructor
// Dialog Data
	//{{AFX_DATA(CTextrDlg)
	enum { IDD = IDD_TEXTURE };
	BOOL	m_CheckFrontTexture;
	BOOL	m_CheckRearTexture;
	double	m_FrontDepth;
	double	m_FrontAngle;
	double	m_RearAngle;
	double	m_RearDepth;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	BOOL OnInitDialog();
    void DoDisabling();
	// Generated message map functions
	//{{AFX_MSG(CTextrDlg)
	afx_msg void OnCheckfronttexture();
	afx_msg void OnCheckreartexture();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
