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
// DeviceDiagram.cpp   Device Diagram    

// routines for drawing a picture of the device in parameter view.

#include "stdafx.h"
#include "math.h"
#include "mathstat.h" 	// CMath
#include "pc1ddoc.h"
#include "DeviceDiagram.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

const int CELL_WID = 140;
const int CELL_REGION_DEPTH = 50;
const int GAP_BETWEEN_REGIONS = 10;
const int CELL_TEXTURE_WID = 5;
const int CELL_TEXTURE_DEPTH = 5;
	

CDeviceDiagram::CDeviceDiagram()
{
	m_xorigin=m_yorigin=0;
}
	

void CDeviceDiagram::DrawTexture(CDC *pDC, int x, int y, BOOL bFront)
{
	int y0, y1;
	int i;
	if (bFront) { y0=y+CELL_TEXTURE_DEPTH; y1=y; }
		else { y1=y+CELL_TEXTURE_DEPTH; y0=y; }
	pDC->MoveTo(x,y0);
	for (i=1; i<CELL_WID/CELL_TEXTURE_WID; i++, i++) {
		pDC->LineTo(x+CELL_TEXTURE_WID*i,y1);
		pDC->LineTo(x+CELL_TEXTURE_WID*(i+1), y0);
	}
}

void CDeviceDiagram::DrawCharge(CDC *pDC, int x, int y, BOOL bPositive)
{
	int i;
	for (i=0; i<CELL_WID/5; i+=2) {
		pDC->MoveTo(x+5*i, y+2);
		pDC->LineTo(x+5*i+5, y+2);
		if (bPositive) {
			pDC->MoveTo(x+5*i+2, y);
			pDC->LineTo(x+5*i+2, y+5);
		}
	}
}

void CDeviceDiagram::DrawDiffusion(CDC *pDC, CDiffusion &diff, int reg, BOOL bFront)
{
//	int diffusiony;
	int k=reg;
	CBrush DopingBrush;
	int y=m_regionystart[k];
	if (!diff.IsEnabled()) return; // don't draw if not enabled
	
	double diffstart, diffend; // first & last y coord of diffusion
	switch (diff.m_Profile) {
		case UNIFORM_PROFILE: diffstart=diff.m_Xpeak; diffend=diff.m_Xpeak+diff.m_Depth; break;
		case ERFC_PROFILE:	diffstart=0; diffend=diff.m_Xpeak+2*diff.m_Depth; break;
		case EXPONENTIAL_PROFILE:
		case GAUSSIAN_PROFILE:
				diffstart=diff.m_Xpeak-2*diff.m_Depth; diffend=diff.m_Xpeak+2*diff.m_Depth; break;
	}
	double tmp;
	if (diffstart<0) diffstart=0;
	if (diffend>m_w[k]) diffend=m_w[k];
	if (!bFront) { tmp = m_w[k]-diffstart; diffstart = m_w[k]-diffend; diffend=tmp;}
		
	int firsty, lasty;
	firsty = DistFromFront2Client(m_w_accum[k]+diffstart);
	lasty = DistFromFront2Client(m_w_accum[k]+diffend);
	ASSERT(firsty<=lasty);
	if (lasty>y+m_hgt[k]) lasty=y+m_hgt[k];
	if (firsty<0) firsty=0;
	if (firsty+5>lasty) { // make it at least 5 pixels wide
		if (firsty+5<=y+m_hgt[k]) lasty=firsty+5; else firsty=lasty-5;
	}

	
/**********	
	if (bFront) {
diffusiony = DistFromFront2Client(m_w_accum[k]+diff.m_Depth);
//		diffusiony = DistFromFront2Client(m_w_accum[k]+m_w[k]/10);
		if (y+5>diffusiony) diffusiony=y+5;
	} else {
		diffusiony = DistFromFront2Client(m_w_accum[k]+m_w[k]-diff.m_Depth);
//		diffusiony = DistFromFront2Client(m_w_accum[k]+m_w[k]-m_w[k]/10);

		if (y+m_hgt[k]-5<diffusiony) diffusiony=y+m_hgt[k]-5;
	}
************/		
	COLORREF colr=CalculateDopingColour(diff.m_Type, diff.m_Npeak);
	DopingBrush.CreateSolidBrush(colr);
	pDC->SelectObject(&DopingBrush);
	pDC->Rectangle(cellleft+1, firsty+1, cellright, lasty);
//	if (bFront) pDC->Rectangle(cellleft+1, y+1, cellright, diffusiony);
//		else 	pDC->Rectangle(cellleft+1, diffusiony, cellright, y+m_hgt[k]);
	pDC->SelectStockObject(WHITE_BRUSH);
	DopingBrush.DeleteObject();
}

void CDeviceDiagram::DrawContact(CDC *pDC, double ContactPosition, double resistance, CString name)
{
	int x,y;
	int contactypos=DistFromFront2Client(ContactPosition);
	pDC->MoveTo(m_xContacts+15, contactypos);
	pDC->LineTo(m_xContacts+15+6,contactypos);		
	x=m_xContacts+15+6; y=contactypos;
	if (resistance>1e-6) {
			pDC->MoveTo(x, y); x+=3;
			pDC->LineTo(x, y+3); x+=6;
			pDC->LineTo(x, y-3); x+=6;
			pDC->LineTo(x, y+3); x+=6;
			pDC->LineTo(x, y-3); x+=3;
			pDC->LineTo(x, y);
	}
//	pDC->MoveTo(m_xShunts, contactypos);
	pDC->LineTo(cellleft, contactypos);
	pDC->TextOut(m_xContacts, contactypos-10, name);
}


// Given a point 'distfromfront' nm from front, calculate the y coordinate in pixels,
// relative to the front of the cell.
int CDeviceDiagram::DistFromFront2Client(double distfromfront)
{
	int y=m_regionystart[0];
	int reg=0;
	double w_=0;	// start of region
	while(distfromfront>=w_+m_w[reg]) { 
		w_+=m_w[reg]; 
		reg++;
		if (reg>m_numregions-1) return m_regionystart[reg-1]+m_hgt[reg-1]-1;
		y=m_regionystart[reg];
	}
	// point is in somewhere in region reg.
	y+=(int)( m_hgt[reg]*(distfromfront-w_)/m_w[reg] );	
	return y;
}


COLORREF CDeviceDiagram::CalculateDopingColour(CMatType doptype, double dopdensity)
{
	double	logdoping=CMath::log(dopdensity);		
	if (doptype== P_TYPE) return RGB(424-logdoping*8, 424-logdoping*8, 255);
	else return RGB(255, 424-logdoping*8, 424-logdoping*8);
}
 

// calculate dimensions for all the internal variables
void CDeviceDiagram::CalculateCoordinates(CDevice *pD, int xorigin, int yorigin)
{
//	int SaveRegion = pD->m_CurrentRegion;
	int k;
	      
	m_w_tot=0;
	m_numregions=pD->m_NumRegions;
	for (k=0; k<m_numregions; k++) {
//		pD->SelectRegion(k);
//		m_w[k]=pD->GetCurrentRegion()->m_Thickness;
		m_w[k]=pD->GetRegion(k)->m_Thickness;
		m_w_accum[k]=m_w_tot;
		m_w_tot+=m_w[k];
	}
 //	pD->SelectRegion(SaveRegion);

	for (k=0; k<m_numregions; k++) {
		if (m_w[k]*250 < 20*m_w_tot) m_hgt[k]=20; // minimum hgt 20 pixels
			else m_hgt[k]=(int) (m_w[k]*250/m_w_tot); // total hgt 250 pixels
	}
	
	int numlumped=0;
	int i;
	for (i=0; i<MAX_LUMPED; i++) if (pD->GetLumpedElement(i)->m_Enable)  numlumped++;
	
	// Calculate coordinates of all the parts of the diagram...

	m_xorigin=xorigin; m_yorigin=yorigin;
	
	int x;
	x=xorigin;	

	m_diagramleft=x;
	x+=10;
	m_xContacts=x;
	x+=2*10+24;		// 24 is length of resistor
	m_xShunts=x;
	x+=(numlumped+1)*10;
	cellleft=x;
	x+=CELL_WID;
	cellright=x;
	x+=10;	
	m_diagramright=x;

	int y;	
	y=yorigin;
	m_diagramtop=y;
	y+=30;	// title of device
	y+=10;
	m_yFrontCharge=y;
	y+=5;
	y+=10;
	m_yFrontTexture=y;
	y+=CELL_TEXTURE_DEPTH;
	for (k=0; k<m_numregions; k++) {
		m_regionystart[k]=y;
		y+=m_hgt[k]+GAP_BETWEEN_REGIONS;
	}
	y-=GAP_BETWEEN_REGIONS;
	m_yRearTexture=y;
	y+=CELL_TEXTURE_DEPTH;
	y+=10;
	m_yRearCharge=y;
	y+=5;
	y+=10;
	m_diagrambottom=y;
}

void CDeviceDiagram::SendCommandToRegion(CDevice *pD, WORD cmd, int region_num)
{
	int SaveRegion = pD->GetCurrentRegionNumber();
	pD->SetCurrentRegionNumber(region_num);
	cmd=ID_DEVICE_THICKNESS;                    
	AfxGetApp()->m_pMainWnd->SendMessage(WM_COMMAND, cmd, 0);
	pD->SetCurrentRegionNumber(SaveRegion);
}


void CDeviceDiagram::OnDoubleClick(CDevice *pD, int x, int y)
{
	WORD cmd=0;
	int k;
//	int SaveRegion;
	
	if (x>=cellright) {
		if (y>=m_yRearTexture) cmd=0;
		else if (y>=m_regionystart[0]) {
			// it's in one of the regions - need to work out which one
			for (k=m_numregions-1; k>=0; k--) 
				if (y>=m_regionystart[k] && y < m_regionystart[k]+m_hgt[k]) {
					// click in region....
					SendCommandToRegion(pD, ID_DEVICE_THICKNESS, k);
//					SaveRegion = pD->GetCurrentRegionNumber();
//					pD->SetCurrentRegionNumber(k);
//					cmd=ID_DEVICE_THICKNESS;                    
//					AfxGetApp()->m_pMainWnd->SendMessage(WM_COMMAND, cmd, 0);
//					pD->SelectRegion(SaveRegion);
				}
				cmd=0;	// stop from sending it again.			
		}
	} else if (x>=cellleft) {
		// it's somewhere in the main area of the cell
		if (y>=m_yRearCharge) cmd=ID_DEVICE_SURFACES_REAR;
		else if (y>=m_yRearTexture) cmd=ID_DEVICE_TEXTURE;
		else if (y>=m_regionystart[0]) {
			// it's in one of the regions - need to work out which one
			for (k=m_numregions-1; k>=0; k--) 
				if (y>=m_regionystart[k] && y < m_regionystart[k]+m_hgt[k]) {
					// click in region....
//					SaveRegion = pD->m_CurrentRegion;
//					pD->SelectRegion(k);
					cmd=ID_DEVICE_DOPING_BACKGROUND;

					int ylimit=m_regionystart[k]+m_hgt[k]/5;
//					if (pD->pR->m_FrontDiff1.m_Enable) {
//						if (ylimit<m_yFrontDiffusion[k]) ylimit=m_yFrontDiffusion[k];
//					}					
					if (y<=ylimit) cmd=ID_DEVICE_DOPING_DIFFUSIONS_FIRSTFRONT;
					
					ylimit=m_regionystart[k]+m_hgt[k]*4/5;
//					if (pD->pR->m_RearDiff1.m_Enable) {
//						if (ylimit>m_yRearDiffusion[k]) ylimit=m_yRearDiffusion[k];
//					}                    
                    if (y>=ylimit)  cmd=ID_DEVICE_DOPING_DIFFUSIONS_FIRSTREAR;

                    SendCommandToRegion(pD, cmd, k);
//					AfxGetApp()->m_pMainWnd->SendMessage(WM_COMMAND, cmd, 0);
//					pD->SelectRegion(SaveRegion);
				}
				cmd=0;	// stop from sending it again.			
		}else if (y>=m_yFrontTexture-5) cmd=ID_DEVICE_TEXTURE;	// -5 is arbitrary, just to give a bit more area to click on 
		else if (y>=m_yFrontCharge-5) cmd=ID_DEVICE_SURFACES_FRONT;
				
	} else if (x>=m_xShunts) cmd=ID_DEVICE_CIRCUIT_INTERNALELEMENTS;
	else if (x>=m_xContacts) cmd=ID_DEVICE_CIRCUIT_CONTACTS;
	
	if (cmd!=0)
		AfxGetApp()->m_pMainWnd->SendMessage(WM_COMMAND, cmd, 0);
}

CSize CDeviceDiagram::GetSize(CDevice * /*pD*/)
{
//	CalculateCoordinates(pD, m_xorigin, m_yorigin);
	return CSize(m_diagramright-m_diagramleft, m_diagrambottom-m_diagramtop);
}

CRect CDeviceDiagram::GetBoundingBox(void)
{
	return CRect(m_diagramleft, m_diagramtop, m_diagramright, m_diagrambottom);
}


void CDeviceDiagram::Update(CDevice *pD, int xorigin, int yorigin)
{
	  CalculateCoordinates(pD, xorigin, yorigin);
}

void CDeviceDiagram::DrawIfVisible(CDevice *pD, CDC *pDC)
{
	if ( pDC->RectVisible(GetBoundingBox()) )
		Draw(pD, pDC);
}
	
// draw a picture of the cell at xorigin, yorigin. 
void CDeviceDiagram::Draw(CDevice*pD, CDC *pDC)
{
//	int SaveRegion = pD->m_CurrentRegion;
	
	CFont Font;
	CFont *pOldFont;
	int lh; 
//	if (pDC!=NULL && pDC->IsPrinting()) lh = pDC->GetDeviceCaps(VERTRES)/100;
//	else lh=22;
	lh=22;
	Font.CreateFont(lh,0,0,0,FW_NORMAL,0,0,0,0,OUT_TT_PRECIS,
		CLIP_TT_ALWAYS, PROOF_QUALITY, VARIABLE_PITCH | TMPF_TRUETYPE | FF_ROMAN, 
		"Times New Roman");	
	pOldFont = pDC->SelectObject(&Font);

	CPen BlackPen1;
	BlackPen1.CreatePen(PS_SOLID, 1, RGB(0,0,0));
	pDC->SelectObject(&BlackPen1);
	
	
	pDC->Rectangle(m_diagramleft, m_diagramtop, m_diagramright, m_diagrambottom);
	CRect titlerect;
	titlerect.left=m_diagramleft;
	titlerect.top=m_diagramtop+5;
	titlerect.right=m_diagramright;
	titlerect.bottom=m_diagramtop+5+20;
	pDC->DrawText("Device Schematic", -1, titlerect, DT_CENTER |DT_TOP);
	
	int i,k;	
	
	if (pD->m_FrontSurface==BARRIER_SURFACE && pD->m_FrontBarrier!=0) DrawCharge(pDC, cellleft, m_yFrontCharge, pD->m_FrontBarrier>0);
	if (pD->m_FrontSurface==CHARGED_SURFACE && pD->m_FrontCharge!=0) DrawCharge(pDC, cellleft, m_yFrontCharge, pD->m_FrontCharge>0);

    pDC->SelectStockObject(BLACK_PEN);
	
	if (pD->m_FrontTexture) DrawTexture(pDC, cellleft, m_yFrontTexture, TRUE);
	
	CBrush DopingBrush;	
	COLORREF colr;
	
	for (k=0; k<m_numregions; k++) {
		CRegion *pR;
		pR=pD->GetRegion(k);
			// Draw the background			
		colr=CalculateDopingColour(pR->m_BkgndType, pR->m_BkgndDop);
		DopingBrush.CreateSolidBrush(colr);
		pDC->SelectObject(&DopingBrush);				
        pDC->Rectangle(cellleft, m_regionystart[k], cellright, m_regionystart[k]+m_hgt[k]);
		pDC->SelectStockObject(WHITE_BRUSH);
		DopingBrush.DeleteObject();
		
			// Draw the diffusions		
		
		CDiffusion *diffs[4];
		BOOL isFront[4];
		diffs[0]=&(pR->m_FrontDiff1);  isFront[0]=TRUE;
		diffs[1]=&(pR->m_FrontDiff2);  isFront[1]=TRUE;
		diffs[2]=&(pR->m_RearDiff1);  isFront[2]=FALSE;
		diffs[3]=&(pR->m_RearDiff2);  isFront[3]=FALSE;
		// now sort the diffusions in order of peak depth. Use simple bubble sort
		// diffs[0]=lowest peak
		CDiffusion *tempdiff;
		BOOL btemp;
		BOOL bSwapHappened;
		do {
			bSwapHappened=FALSE;
			for (i=0; i<3; i++) 
			  if (diffs[i]->m_Xpeak>diffs[i+1]->m_Xpeak) {
				tempdiff=diffs[i]; diffs[i]=diffs[i+1]; diffs[i+1]=tempdiff; 
				btemp=isFront[i]; isFront[i]=isFront[i+1]; isFront[i+1]=btemp; 
				bSwapHappened=TRUE;
			  }
		} while (bSwapHappened);
		pDC->SelectStockObject(NULL_PEN);
		for (i=0; i<4; i++)
			DrawDiffusion(pDC, *diffs[i], k, isFront[i]);
		// Draw external diffusions if required. We don't know what sort of doping they are,
		// so do them as a uniform grey colour, 10% of region width.
		if (pR->m_FrontExternal) {
			pDC->SelectStockObject(LTGRAY_BRUSH);
			pDC->Rectangle(cellleft+1, m_regionystart[k]+1, cellright, m_regionystart[k]+m_hgt[k]/10);
		}
		if (pR->m_RearExternal) {
			pDC->SelectStockObject(LTGRAY_BRUSH);
			pDC->Rectangle(cellleft+1, m_regionystart[k]+m_hgt[k]-m_hgt[k]/10, cellright, m_regionystart[k]+m_hgt[k]);
		}

//	if (bFront) pDC->Rectangle(cellleft+1, y+1, cellright, diffusiony);
//		else 	pDC->Rectangle(cellleft+1, diffusiony, cellright, y+m_hgt[k]);
			

/*****		
		m_yFrontDiffusion[k]=DrawDiffusion(pDC, pD->pR->m_FrontDiff1, k, TRUE);
							 DrawDiffusion(pDC, pD->pR->m_FrontDiff2, k, TRUE);
		m_yRearDiffusion[k]= DrawDiffusion(pDC, pD->pR->m_RearDiff1, k, FALSE);
							 DrawDiffusion(pDC, pD->pR->m_RearDiff2, k, FALSE);		
*****/							 
		pDC->SelectStockObject(BLACK_PEN);
	}
	if (pD->m_RearTexture) 	DrawTexture(pDC, cellleft, m_yRearTexture, FALSE);

	pDC->SelectObject(&BlackPen1);

	if (pD->m_RearSurface==BARRIER_SURFACE && pD->m_RearBarrier!=0) DrawCharge(pDC, cellleft, m_yRearCharge, pD->m_RearBarrier>0);
	if (pD->m_RearSurface==CHARGED_SURFACE && pD->m_RearCharge!=0) DrawCharge(pDC, cellleft, m_yRearCharge, pD->m_RearCharge>0);


	pDC->SelectStockObject(WHITE_BRUSH);
// 	pD->SelectRegion(SaveRegion);

	int numlumped=0;
	for (i=0; i<MAX_LUMPED; i++) if (pD->GetLumpedElement(i)->m_Enable)  numlumped++;

//	int basepos, emitterpos, collectorpos;

	if (pD->m_EnableEmitter) {
//		emitterpos=DistFromFront2Client(pD->m_EmitterX);
		DrawContact(pDC, pD->m_EmitterX, pD->m_EmitterR, "E");
	}
	if (pD->m_EnableBase) {
//		basepos=DistFromFront2Client(pD->m_BaseX);
		DrawContact(pDC, pD->m_BaseX, pD->m_BaseR, "B");
	}
	if (pD->m_EnableCollector) {
//		collectorpos=DistFromFront2Client(pD->m_CollectorX);
		DrawContact(pDC, pD->m_CollectorX, pD->m_CollectorR, "C");
	}

	int y;	
	pDC->SelectStockObject(BLACK_BRUSH);
	int anodepos, cathodepos;
	int tempx;	
	k=0;
	for (i=0; i<MAX_LUMPED; i++) {
		CLumped *pL = pD->GetLumpedElement(i);
		if (pL->m_Enable) {
			anodepos=DistFromFront2Client(pL->m_Xa);
			cathodepos=DistFromFront2Client(pL->m_Xc);
			int halfway=(anodepos+cathodepos)/2;
			int gapsize=0;
			
			switch(pL->m_Type) {
			case CONDUCTOR:
				if (pL->m_Value<=10) { // resistor if >= 0.1 ohms
					gapsize=12;
				} else gapsize=0;
				break;
			case DIODE:
				gapsize=5;
				break;
			case CAPACITOR:
				gapsize=3;
				break;
			}
			
			if (abs(anodepos-cathodepos)<=2*gapsize) gapsize=0;
			
			tempx=cellleft-10*(numlumped-k);
			pDC->MoveTo(cellleft, anodepos);
			pDC->LineTo(tempx, anodepos);
			if (anodepos<cathodepos) {
				pDC->LineTo(tempx, halfway-gapsize);
				pDC->MoveTo(tempx, halfway+gapsize);
			} else {
				pDC->LineTo(tempx, halfway+gapsize-1);
				pDC->MoveTo(tempx, halfway-gapsize);
			}
			pDC->LineTo(tempx, cathodepos);
			pDC->LineTo(cellleft, cathodepos);
			
			// Draw the element symbol (unless there's no space to fit it)
			if (gapsize!=0) {
				switch(pL->m_Type) {
				case CONDUCTOR: // resistor... gapsize=12
					y=halfway-12;
					pDC->MoveTo(tempx, y); y+=3;
					pDC->LineTo(tempx+3, y); y+=6;
					pDC->LineTo(tempx-3, y); y+=6;
					pDC->LineTo(tempx+3, y); y+=6;
					pDC->LineTo(tempx-3, y); y+=3;
					pDC->LineTo(tempx, y);
					break;
				case DIODE:
					pDC->MoveTo(tempx-3, halfway-5);
					pDC->LineTo(tempx+4, halfway-5);
					pDC->MoveTo(tempx-3, halfway+5);
					pDC->LineTo(tempx+4, halfway+5);
					if (anodepos>cathodepos) {
						pDC->MoveTo(tempx-3, halfway+gapsize);
						pDC->LineTo(tempx, halfway-gapsize);
						pDC->LineTo(tempx+3, halfway+gapsize);
					} else {
						pDC->MoveTo(tempx-3, halfway-gapsize);
						pDC->LineTo(tempx, halfway+gapsize);
						pDC->LineTo(tempx+3, halfway-gapsize);
					}
					break;
				case CAPACITOR:
					pDC->MoveTo(tempx-3, halfway-3);
					pDC->LineTo(tempx+4, halfway-3);
					pDC->MoveTo(tempx-3, halfway+3);
					pDC->LineTo(tempx+4, halfway+3);
					break;
				}
			}
			
			pDC->Ellipse(tempx-2, anodepos-2, tempx+3, anodepos+3);
			pDC->Ellipse(tempx-2, cathodepos-2, tempx+3, cathodepos+3);
			k++;
		}
	}
	
	pDC->SelectObject(pOldFont);
	Font.DeleteObject();
	pDC->SelectStockObject(BLACK_PEN);
	BlackPen1.DeleteObject();
}
