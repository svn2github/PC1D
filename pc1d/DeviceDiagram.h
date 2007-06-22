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
// DeviceDiagram.h   Device Diagram    

#define DEVICEDIAGRAM_H 1

// a picture of a solar cell
class CDeviceDiagram : public CObject
{
// Attributes	
private:
	int m_numregions;
	int m_hgt[MAX_REGIONS];
	double m_w[MAX_REGIONS];
	double m_w_tot;
	double m_w_accum[MAX_REGIONS]; // distance from start of region to front of cell
	
	int m_xorigin, m_yorigin;
	int cellleft, cellright; 	// x coords of left & right sides of cell
	int	m_xContacts, m_xShunts;

	int m_diagramtop, m_diagrambottom;
	int m_diagramleft, m_diagramright;
	int m_yFrontCharge, m_yRearCharge;
	int m_yFrontTexture, m_yRearTexture;
	int m_regionystart[MAX_REGIONS];
	int m_yFrontDiffusion[MAX_REGIONS];
	int m_yRearDiffusion[MAX_REGIONS];
	
public:
	CDeviceDiagram();
	void DrawIfVisible(CDevice *pD, CDC *pDC);
	void Update(CDevice *pD, int xorigin, int yorigin);
	CSize GetSize(CDevice *pD);
	void OnDoubleClick(CDevice *pD, int x, int y);		
	CRect GetBoundingBox(void);
// Implementation
protected:
	void Draw(CDevice *pD, CDC *pDC);
	int DistFromFront2Client(double distfromfront);
	COLORREF CalculateDopingColour(CMatType doptype, double dopdensity);
	void CalculateCoordinates(CDevice *pD, int xorigin, int yorigin);
	
	void DrawTexture(CDC *pDC, int x, int y, BOOL bFront);
	void DrawCharge(CDC *pDC, int x, int y, BOOL bPositive);
	void DrawDiffusion(CDC *pDC, CDiffusion &diff, int reg, BOOL bFront);
	void DrawContact(CDC *pDC, double contactpos, double resistance, CString name);
	void SendCommandToRegion(CDevice *pD, WORD cmd, int region_num);
};
