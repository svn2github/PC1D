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
////////////////////////////////////////////////////////////////
// parmview.cpp   Parameter View    

// The parameters are divided into 'CParamSection's. Only the appropriate one(s) are
// redrawn -- aids in making it flickerless. Also, each deals with mouse clicks in
// its own area (so they are sort of like subwindows). Device diagram works in the
// same way.

#include "stdafx.h"
#include "hints.h"
#include "math.h"
#include "mathstat.h" 	// CMath
#include "pc1ddoc.h"
#include "parmview.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define LEFTMARGINSIZE 16
#define DEVICEDIAGRAMXPOS 400

/////////////////////////////////////////////////////////////////////////////
// CParameterView

IMPLEMENT_DYNCREATE(CParameterView, CScrollView)

BEGIN_MESSAGE_MAP(CParameterView, CScrollView)
	//{{AFX_MSG_MAP(CParameterView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_SELECTPARMFONT, OnSelectParmFont)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParameterView construction/destruction

CParameterView::CParameterView()
{  
	m_sizeParmDoc=CSize(512,1024);
	CString ParmFontName;		// font used for parameter display
	int ParmFontHgt;			// height in twips
    ParmFontName= AfxGetApp()->GetProfileString("Parameter View", "Font Name", "Times New Roman");
    ParmFontHgt= AfxGetApp()->GetProfileInt("Parameter View", "Font Size", 14*20); // font size in twips
	CParamSection::SetFontForAllSections(ParmFontName, ParmFontHgt);

}

CParameterView::~CParameterView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CParameterView drawing

void CParameterView::OnUpdate(CView* /*pView*/, LPARAM lHint, CObject* /*pHint*/)
{
	CPoint pt;
	int desiredypos;
	int regheight;
	CRect rect;
	pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	switch (lHint){
		case 0:		// generic update - e.g. first call
		case HINT_EVERYTHINGCHANGED:
		case HINT_PARAMSCHANGED:
			RedoTextForAllSections();
			Invalidate(TRUE);	// Erase the background first	
			break;
		case HINT_DIFFERENTREGION:
			// make sure current region is visible in the view. 
			rect=m_regionsection[pDoc->GetProblem()->GetDevice()->GetCurrentRegionNumber()].GetBoundingBox();
			desiredypos=rect.top;
			regheight=rect.bottom-rect.top;
			pt=GetScrollPosition();
			GetClientRect(&rect);
			pt.x=0; 
			if (pt.y+rect.bottom < desiredypos+regheight) pt.y=desiredypos+regheight-rect.bottom;
			if (pt.y>desiredypos) pt.y=desiredypos;
			ScrollToPosition(pt);
			break;
		case HINT_GRAPHDATACHANGED: 
			// results might have changed. Everything else will be the same though.
			rect=m_resultssection.GetClientBBox(); // the area we need to blank out
			m_resultssection.Update(pDoc);
			m_resultssection.SetBBoxLeftRight(LEFTMARGINSIZE, LEFTMARGINSIZE+m_resultssection.GetWidth());
			rect|=m_resultssection.GetClientBBox();
			{
				CClientDC dc(this);		// must convert to logical coords
				OnPrepareDC(&dc);
				dc.LPtoDP(&rect);
			}
			InvalidateRect(rect);
			break;
		case HINT_SIMFINISHED:
			// results and batch might have changed.
			rect=m_resultssection.GetClientBBox(); // the area we need to blank out
			rect|=m_batchsection.GetClientBBox();
			m_resultssection.Update(pDoc);
			m_batchsection.UpdateCurrentSim(pDoc);
//			m_batchsection.Update(pDoc);
			m_resultssection.SetBBoxLeftRight(LEFTMARGINSIZE, LEFTMARGINSIZE+m_resultssection.GetWidth());
			{
				CClientDC dc(this);		
				OnPrepareDC(&dc);
			if (pDoc->GetProblem()->GetBatch()->m_Enable) {
				m_batchsection.SetBBoxLeftRight(LEFTMARGINSIZE, LEFTMARGINSIZE+16*(1+pDoc->GetProblem()->GetBatch()->GetNumberOfFields())*m_batchsection.GetFontWidth());
			}   
			rect|=m_resultssection.GetClientBBox();
			rect|=m_batchsection.GetClientBBox();			
				dc.LPtoDP(&rect); // must convert to logical coords
			}
			InvalidateRect(rect);
			break;
		default:		// else no action required
			break;
	}
}

void CParameterView::RedoTextForAllSections(void)
{
	int k;
	m_filesection.Update(pDoc);
	m_devicesection.Update(pDoc);
	m_excitationsection.Update(pDoc);
	for (k=0; k<pDoc->GetProblem()->GetDevice()->m_NumRegions; k++) {
		m_regionsection[k].SetRegionNumber(k);
		m_regionsection[k].Update(pDoc);
	}
	m_resultssection.Update(pDoc);
	m_batchsection.Update(pDoc);

	CalculateMetrics(NULL);
}

void CParameterView::CalculateMetrics(CDC *pDC)
{
	CalculateSectionCoordinates(pDC);
	CalculateScrollSizes();
}

void CParameterView::CalculateSectionCoordinates(CDC *pDC)
{
	int k;
	int top;
	int ysize;
	ysize=0;
	top=ysize;
	ysize+=m_filesection.GetHeight(pDC);
	m_filesection.SetBBoxTopBottom(top, ysize);
	m_devicediagram.Update(pDoc->GetProblem()->GetDevice(), DEVICEDIAGRAMXPOS, ysize);		// this positions the diagram
	top=ysize;
	ysize+=m_devicesection.GetHeight(pDC);
	m_devicesection.SetBBoxTopBottom(top, ysize);
	top=ysize;
	for (k=0; k<pDoc->GetProblem()->GetDevice()->m_NumRegions; k++) {
		ysize+=m_regionsection[k].GetHeight(pDC);	
		m_regionsection[k].SetBBoxTopBottom(top, ysize);
		top=ysize;
	}
	ysize+=m_excitationsection.GetHeight(pDC);
	m_excitationsection.SetBBoxTopBottom(top, ysize);
	top=ysize;
	ysize+=m_resultssection.GetHeight(pDC);
	m_resultssection.SetBBoxTopBottom(top, ysize);
	top=ysize;
	ysize+=m_batchsection.GetHeight(pDC);
	m_batchsection.SetBBoxTopBottom(top, ysize);

	int xsize;
	xsize=LEFTMARGINSIZE+m_filesection.GetWidth(pDC);
	m_filesection.SetBBoxLeftRight(LEFTMARGINSIZE, xsize);
	xsize=LEFTMARGINSIZE+m_devicesection.GetWidth(pDC);	
	m_devicesection.SetBBoxLeftRight(LEFTMARGINSIZE, xsize);
	for (k=0; k<pDoc->GetProblem()->GetDevice()->m_NumRegions; k++) {
		xsize=LEFTMARGINSIZE+m_regionsection[k].GetWidth(pDC);
		m_regionsection[k].SetBBoxLeftRight(LEFTMARGINSIZE, xsize);
	}
	xsize=LEFTMARGINSIZE+m_excitationsection.GetWidth(pDC);
	m_excitationsection.SetBBoxLeftRight(LEFTMARGINSIZE, xsize);
	xsize=LEFTMARGINSIZE+m_resultssection.GetWidth(pDC);
	m_resultssection.SetBBoxLeftRight(LEFTMARGINSIZE, xsize);
	xsize = LEFTMARGINSIZE+16*(1+pDoc->GetProblem()->GetBatch()->GetNumberOfFields())*m_batchsection.GetFontWidth();
	m_batchsection.SetBBoxLeftRight(LEFTMARGINSIZE, xsize);
}


int CParameterView::TotalDocLength(void)
{
	CRect rect=m_batchsection.GetBoundingBox();
	return rect.bottom;
}

//#pragma optimize("", off)
// This function used to fail when optimized for reasons which are unclear to me.
int CParameterView::TotalDocWidth(void)
{
	// rightmost point will either be batch or device diagram.
	int xsize;
	CRect rect=m_batchsection.GetBoundingBox();
	xsize=rect.right;
	rect=m_devicediagram.GetBoundingBox();
	if (rect.right>xsize) xsize=rect.right;
	return xsize;
}	
//#pragma optimize("", on)

void CParameterView::CalculateScrollSizes(void)
{
	CRect ClientRect;
	GetClientRect(&ClientRect);
	int hgt=CParamSection::GetFontHeight();
	m_sizeParmDoc.cy=TotalDocLength() + ClientRect.Height()%hgt;
	m_sizeParmDoc.cx=TotalDocWidth();
	CSize sizepage;
	CSize sizeline;
	sizeline.cx=hgt; sizeline.cy=hgt;
	sizepage.cy=hgt*(ClientRect.Height()/hgt-1);
	sizepage.cx=ClientRect.Width()/2;
	SetScrollSizes(MM_TEXT, m_sizeParmDoc, sizepage, sizeline);
}

void CParameterView::OnDraw(CDC* pDC)
{	
	int k;
	m_filesection.DrawIfVisible(pDC);	
	m_devicesection.DrawIfVisible(pDC);
	for (k=0; k<pDoc->GetProblem()->GetDevice()->m_NumRegions; k++) m_regionsection[k].DrawIfVisible(pDC);	
	m_excitationsection.DrawIfVisible(pDC);
	m_resultssection.DrawIfVisible(pDC);
	m_batchsection.DrawIfVisible(pDC);

	// diagrams are a special case, must be drawn seperately.
	if (!pDC->IsPrinting()) {
		m_devicediagram.DrawIfVisible(pDoc->GetProblem()->GetDevice(), pDC);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CParameterView printing

BOOL CParameterView::OnPreparePrinting(CPrintInfo* pInfo)
{
	return DoPreparePrinting(pInfo);
}

void CParameterView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	CParamSection::CalculateMetrics(pDC);
	CalculateSectionCoordinates(pDC);  // adjust for the new font size

	int mmhgt=pDC->GetDeviceCaps(VERTSIZE); 
	int pageheight=Lomets2LogicalDistance(mmhgt*10-50);
	int fntheight=CParamSection::GetFontHeight();
	pageheight-=(pageheight%fntheight);		// don't display fraction of a line
	int totdoclen=TotalDocLength();
	int lastpage=(abs(totdoclen)+pageheight)/pageheight;

	pInfo->SetMinPage(1);
	pInfo->SetMaxPage(lastpage);
}

void CParameterView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// cleanup after printing
	CParamSection::CalculateMetrics(NULL);
	CalculateSectionCoordinates(NULL);
}

void CParameterView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	CRect rect;
	rect = pInfo->m_rectDraw;
	int pagewidth = rect.right-rect.left;
	rect.bottom-=Lomets2LogicalDistance(50);
	int pageheight = rect.bottom-rect.top;
	int fntheight=CParamSection::GetFontHeight();
	rect.bottom-=(pageheight%fntheight);		// don't display fraction of a line
	pageheight = rect.bottom-rect.top;
	int pgnum=pInfo->m_nCurPage-1;
	int yorg=pageheight*pgnum;

	pDC->IntersectClipRect(rect);

	pDC->SetWindowOrg((int)(-0.05*pagewidth), yorg);
	OnDraw(pDC);
	
	rect=m_devicesection.GetBoundingBox();
	CPoint devdiagramorigin;
	devdiagramorigin.x=rect.right;
	devdiagramorigin.y=rect.top;

	CDeviceDiagram localdevicediagram;
	localdevicediagram.Update(pDoc->GetProblem()->GetDevice(), devdiagramorigin.x, devdiagramorigin.y);

	int spaceavailable=(int)(pagewidth*0.9)-devdiagramorigin.x;
	int spaceneeded=(m_devicediagram.GetSize(pDoc->GetProblem()->GetDevice())).cx;
	
	// we want to use about half the available width
	pDC->LPtoDP(&devdiagramorigin);

	pDC->SetMapMode(MM_ISOTROPIC);
	pDC->SetWindowExt(spaceneeded*2,spaceneeded*2);
	pDC->SetViewportExt(spaceavailable,spaceavailable);
	pDC->DPtoLP(&devdiagramorigin);
	devdiagramorigin.x+=spaceneeded;

	localdevicediagram.Update(pDoc->GetProblem()->GetDevice(), devdiagramorigin.x, devdiagramorigin.y);
	localdevicediagram.DrawIfVisible(pDoc->GetProblem()->GetDevice(), pDC);
	pDC->SetMapMode(MM_TEXT);
}

/////////////////////////////////////////////////////////////////////////////
// CParameterView diagnostics

#ifdef _DEBUG
void CParameterView::AssertValid() const
{
	CView::AssertValid();
}

void CParameterView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPc1dDoc* CParameterView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPc1dDoc)));
	return (CPc1dDoc*)m_pDocument;
}
#endif //_DEBUG

void CParameterView::OnEditCopy()
{
	pDoc->GetProblem()->GetBatch()->CopyEntireBatchToClipboard(this);
}

void CParameterView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{ 
	pCmdUI->SetText("&Copy Batch Data\tCtrl+C");
	pCmdUI->Enable(pDoc->GetProblem()->GetBatch()->m_Enable);	
}

void CParameterView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar) {
	 case VK_PRIOR: 	SendMessage(WM_VSCROLL, SB_PAGEUP);		break;
	 case VK_NEXT:		SendMessage(WM_VSCROLL, SB_PAGEDOWN);	break;
	 case VK_UP:		SendMessage(WM_VSCROLL, SB_LINEUP);		break;
	 case VK_DOWN:		SendMessage(WM_VSCROLL, SB_LINEDOWN);	break;
	 case VK_LEFT:		SendMessage(WM_HSCROLL, SB_LINEUP);		break;
	 case VK_RIGHT:		SendMessage(WM_HSCROLL, SB_LINEDOWN);	break;
	 case VK_HOME:
		if (GetKeyState(VK_CONTROL)& 0x1000) SendMessage(WM_VSCROLL, SB_TOP);
		else SendMessage(WM_HSCROLL, SB_TOP);
		break;
	 case VK_END:
		if (GetKeyState(VK_CONTROL)& 0x1000) SendMessage(WM_VSCROLL, SB_BOTTOM);
		else SendMessage(WM_HSCROLL, SB_BOTTOM);
		break;
	 default:	
		CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}

void CParameterView::OnLButtonDown(UINT /*nFlags*/, CPoint point)
{
	int k;
	CRect rect;
	CClientDC dc(this);		// must convert to logical coords
	OnPrepareDC(&dc);
	dc.DPtoLP(&point);	

	for (k=0; k<pDoc->GetProblem()->GetDevice()->m_NumRegions; k++) {
		rect=m_regionsection[k].GetBoundingBox();
		if (rect.PtInRect(point)) 
			m_regionsection[k].OnClick(pDoc, point.x, point.y-rect.top);
	}
}


void CParameterView::OnLButtonDblClk(UINT /*nFlags*/, CPoint point)
{
	int k;
	CClientDC dc(this);		// must convert to logical coords
	OnPrepareDC(&dc);
	dc.DPtoLP(&point);
	CRect rect;
	
	rect=m_devicediagram.GetBoundingBox();
	if (rect.PtInRect(point)) {
			m_devicediagram.OnDoubleClick(pDoc->GetProblem()->GetDevice(), point.x, point.y);
			return;
	}

	rect=m_filesection.GetBoundingBox();
	if (rect.PtInRect(point)) 
		m_filesection.OnDoubleClick(pDoc, point.x, point.y-rect.top);

	rect=m_devicesection.GetBoundingBox();
	if (rect.PtInRect(point)) 
		m_devicesection.OnDoubleClick(pDoc, point.x, point.y-rect.top);

	for (k=0; k<pDoc->GetProblem()->GetDevice()->m_NumRegions; k++) {
		rect=m_regionsection[k].GetBoundingBox();
		if (rect.PtInRect(point)) 
			m_regionsection[k].OnDoubleClick(pDoc, point.x, point.y-rect.top);
	}

	rect=m_excitationsection.GetBoundingBox();
	if (rect.PtInRect(point)) 
		m_excitationsection.OnDoubleClick(pDoc, point.x, point.y-rect.top);

	rect=m_resultssection.GetBoundingBox();
	if (rect.PtInRect(point)) 
		m_resultssection.OnDoubleClick(pDoc, point.x, point.y-rect.top);

	rect=m_batchsection.GetBoundingBox();
	if (rect.PtInRect(point)) 
		m_batchsection.OnDoubleClick(pDoc, point.x, point.y-rect.top);
}

void CParameterView::OnSelectParmFont() 
{
	if (CParamSection::OnSelectFont())
		GetDocument()->UpdateAllViews(NULL);
}
