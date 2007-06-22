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
// CMatRecDoping dialog

class CMatRecDoping : public CPropertyPage
{
// Construction
public:
	CMatRecDoping();	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMatRecDoping)
	enum { IDD = IDD_MATRECOM_DOPING };
	double	m_BulkNalphaN;
	double	m_BulkNalphaP;
	double	m_SurfNalphaN;
	double	m_SurfNalphaP;
	double	m_BulkNrefN;
	double	m_BulkNrefP;
	double	m_SurfNrefN;
	double	m_SurfNrefP;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMatRecDoping)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CMatRecAuger dialog

class CMatRecAuger : public CPropertyPage
{
// Construction
public:
	CMatRecAuger();	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMatRecAuger)
	enum { IDD = IDD_MATRECOM_AUGER };
	double	m_BB;
	double	m_Cn;
	double	m_Cnp;
	double	m_Cp;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMatRecAuger)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CMatRecTemperature dialog

class CMatRecTemperature : public CPropertyPage
{
// Construction
public:
	CMatRecTemperature();	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMatRecTemperature)
	enum { IDD = IDD_MATRECOM_TEMP };
	double	m_BulkTalpha;
	double	m_SurfTalpha;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMatRecTemperature)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};





/////////////////////////////////////////////////////////////////////////////
// CMatRecHurkx dialog

class CMatRecHurkx : public CPropertyPage
{
// Construction
public:
	CMatRecHurkx();	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMatRecHurkx)
	enum { IDD = IDD_MATRECOM_HURX };
	double	m_Fgamma;
	double	m_prefactor;
	BOOL	m_Enable;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();

	// Generated message map functions
	//{{AFX_MSG(CMatRecHurkx)
	afx_msg void DoDisabling();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
