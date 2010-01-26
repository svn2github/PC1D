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
///////////////////////////////////////////////////////////////
//	CPersistentFrame class header file (derived from David J. Kruglinski, "Inside Visual C++")

class CPersistentFrame : public CFrameWnd
{	// remembers where it was on the desktop
	DECLARE_DYNAMIC(CPersistentFrame)
private:
	CString m_profileHeading, m_profileRect, m_profileIcon;
	CString m_profileMax, m_profileTool, m_profileStatus;
	BOOL m_bFirstTime;
protected:	// create from serialization only
	CPersistentFrame();
	~CPersistentFrame();
	virtual void ActivateFrame(int nCmdShow = -1);
	//{{AFX_MSG(CPersistentFrame)
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

