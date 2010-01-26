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
#include "pc1d.h"
#include "splshwnd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSplashWnd

CSplashWnd::CSplashWnd()
{
}

CSplashWnd::~CSplashWnd()
{
}

BEGIN_MESSAGE_MAP(CSplashWnd, CWnd)
	//{{AFX_MSG_MAP(CSplashWnd)
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_KILLFOCUS()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSplashWnd message handlers

// This is to avoid a rather stupid Microsoft limitation: CWnd::Create() cannot make
// popup windows [ in direct defiance of the manual]. The reason is that Create() ORs
// the window style with WS_CHILD, for reasons known only to Microsoft.
// A window cannot be both WS_POPUP and WS_CHILD.
// This function is lifted straight from CWnd::Create() but doesn'r OR with WS_CHILD.
BOOL CSplashWnd::DisplaySplashScreen( CWnd* pParentWnd, UINT nID,
	CCreateContext* pContext /* = NULL */)
{
	CRect rect;
	pParentWnd->GetClientRect(rect);
	rect.top=(rect.bottom-256)/2; if (rect.top<=0) rect.top=0;
	rect.left=(rect.right-512)/2; if (rect.left<=0) rect.left=0;
	rect.bottom=256+rect.top;
	rect.right=512+rect.left;
	pParentWnd->ClientToScreen(rect);

	BOOL b= CreateEx(0, AfxRegisterWndClass(0), NULL,
		WS_CHILD | WS_POPUP | WS_VISIBLE,
		rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
		pParentWnd->GetSafeHwnd(), (HMENU)nID, (LPSTR)pContext);
	SetTimer(1, 3000, NULL); // To be destroyed after 3 seconds = 3000 milliseconds
	return b;
}

void CSplashWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	DestroyWindow();
}

void CSplashWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	DestroyWindow();
}

void CSplashWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	DestroyWindow();
}


void CSplashWnd::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);
	DestroyWindow();	
}

void CSplashWnd::OnTimer(UINT nIDEvent)
{
	DestroyWindow();
	CWnd::OnTimer(nIDEvent);
}


const COLORREF DKGRAY= RGB(127,127,127);
const COLORREF LTGRAY= RGB(192,192,192);

void CSplashWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect myrect;
	
	COLORREF shadowcolour=DKGRAY;
	COLORREF facecolour=LTGRAY;
	
	CBrush facebrush(facecolour);
	CBrush shadowbrush(shadowcolour);
	
	dc.SelectStockObject(WHITE_BRUSH);
	GetClientRect(myrect);
	dc.Rectangle(myrect);
	int hgt=myrect.bottom-1;
	int wid=myrect.right-1;
	
	dc.SelectObject(&facebrush);
	dc.SelectStockObject(NULL_PEN);	
	myrect.InflateRect(-8,-8);
	dc.Rectangle(myrect);

	dc.SelectObject(&shadowbrush);
	int outlinepoly[]= { 0,hgt, 8,hgt-8, wid-8,hgt-8, wid-8,8, wid,0, wid,hgt };
	dc.Polygon((LPPOINT)outlinepoly, 6);

	
	CBitmap PC1DBitmap;
	CBitmap CentreBitmap;
	CBitmap *pOldBitmap;
	PC1DBitmap.LoadBitmap(IDB_PC1DLOGO);
	CDC myDC;
	myDC.CreateCompatibleDC(NULL);
	pOldBitmap=myDC.SelectObject(&PC1DBitmap);
	dc.StretchBlt(32, 32, 128, 128, &myDC, 0,0, 64,64, SRCCOPY);

	CentreBitmap.LoadBitmap(IDB_CENTRELOGO);
	myDC.SelectObject(&CentreBitmap);
	
	dc.BitBlt(32,32+128+20, 64, 48, &myDC, 0,0, SRCCOPY);
	
	myDC.SelectObject(pOldBitmap);
	PC1DBitmap.DeleteObject();
	
	CFont Font;	
	Font.CreateFont(40,0,0,0,FW_NORMAL,0,0,0,0,OUT_TT_PRECIS,
		CLIP_TT_ALWAYS, PROOF_QUALITY, VARIABLE_PITCH|TMPF_TRUETYPE|FF_SWISS, 
		"Arial");
	dc.SelectObject(&Font);
	
	CString titlestring="PC1D 5.0 Beta";
	CString ownerstring1="Photovoltaics Special Research Centre";
	CString ownerstring2="University of New South Wales";
	CString authorstring1="Paul A. Basore";
	CString authorstring2="Donald A. Clugston";
	dc.SetBkColor(LTGRAY);
	
	dc.TextOut(32+128+32, 32, titlestring, titlestring.GetLength());	
		
	dc.SelectStockObject(SYSTEM_FONT);
	Font.DeleteObject();
	Font.CreateFont(24,0,0,0,FW_NORMAL,0,0,0,0,OUT_TT_PRECIS,
		CLIP_TT_ALWAYS, PROOF_QUALITY, VARIABLE_PITCH|TMPF_TRUETYPE|FF_SWISS, 
		"Arial");
	dc.SelectObject(&Font);
	
	dc.TextOut(32+128+32-8 + 100 , 32+40+8, authorstring1, authorstring1.GetLength()); 
	dc.TextOut(32+128+32-8 + 100 , 32+40+8+24+4, authorstring2, authorstring2.GetLength());

	dc.TextOut(32+32+32+8, 32+128+20, ownerstring1, ownerstring1.GetLength());
	dc.TextOut(32+32+32+8, 32+128+20+24+4, ownerstring2, ownerstring2.GetLength());
		
	dc.SelectStockObject(SYSTEM_FONT);
	dc.SelectStockObject(WHITE_BRUSH);
}

void CSplashWnd::OnDestroy()
{
	CWnd::OnDestroy();
	KillTimer(1);	
}
