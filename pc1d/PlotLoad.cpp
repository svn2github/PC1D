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
// plotload.cpp - CData and CGraph classes for PC1D                                   
// Provides an interface between document node data and view classes
// Includes:* calculations to convert node data to graphable quantities;
//			* definitions of various graphs in terms of graphable quantities;
//			* calls to Auxiliary Dialog & UserGraph Dialog
//			* SetAuxData() function, to be called after each timestep during calculations
//------------------------------------------------------------------------------------
// To add extra quantities:
// (a) Add to the list of GraphableQuantities in plotload.h
// (b) Create a function to load the quantities into an array in plotload.cpp
// (c) Declare the function in plotload.h
// (d) Add this function to the case statement in LoadQuantity in plotload.cpp
// To add extra graphs:
// (a) Add to list of graphs in plotload.h
// (b) Add to case statement in SetPredefinedGraph() in plotload.cpp


// Note on types of array: SPATIAL_ARRAY: function of x
//							TIME_ARRAY: any function of time.
//		EXPERIMENTAL_ARRAY: this can be either space or time, but can't be used
//				for auxiliary graphs, or for x-axis.

#include "stdafx.h"
#include "problem.h"
#include "plotload.h"
#include "mathstat.h"	// CMath
#include "physics.h"	// CPhysics
#include "graphdlg.h"
#include "auxdlg.h"
#include "ascfile.h"
#include "useful.h"
#include "path.h"
#include "PC1D.h"


#define MAX_EXPT_DATA_POINTS 500



////////////////////////////////////////////////////////////////////////////
// CData

CData::CData()
{
	units.Empty(); label.Empty(); scale = LINEAR; type = SPACE_ARRAY;
}

void CData::Set(CString sunits, CString slabel, CString abbrev, CScaleType e1, CArrayType e2)
{
	units = sunits; label = slabel; scale = e1; type = e2; abbreviation=abbrev;
}

CData & CData::operator =(CData &da)
{
	units=da.units;
	label=da.label;
	scale=da.scale;
	type=da.type;
	abbreviation=da.abbreviation;
	return *this;
}

// same as Load Quantity, except it's a static function
void CData::LoadQuantityIntoArray(CProblem *pProb, int n, double z[], GraphableQuantity whichquantity)
{
	CData Dat;
	Dat.LoadQuantity(pProb, n, z, whichquantity);
}

CArrayType CData::GetType(GraphableQuantity g)
{
	CData Dat;
	Dat.LoadQuantity(NULL, 0, NULL, g);
	return Dat.type;
}

CScaleType CData::GetScale(GraphableQuantity g)
{
	CData Dat;
	Dat.LoadQuantity(NULL, 0, NULL, g);
	return Dat.scale;
}   

CString CData::GetUnits(GraphableQuantity g)
{
	CData Dat;
	Dat.LoadQuantity(NULL, 0, NULL, g);
	return Dat.units;
}

CString CData::GetLabel(GraphableQuantity g)
{
	CData Dat;
	Dat.LoadQuantity(NULL, 0, NULL, g);
	return Dat.label;
}

/// These next two functions allow you to save the data
/// in text form, which is less implementation-specific.
CString CData::GraphableQuantity2Text(GraphableQuantity g)
{
	CData Dat;
	Dat.LoadQuantity(NULL, 0, NULL, g);
	return Dat.abbreviation;
}

GraphableQuantity CData::Text2GraphableQuantity(CString name)
{
	GraphableQuantity g=GetFirstQuantity();
	CData Dat;
	do {
		Dat.LoadQuantity(NULL, 0, NULL, g);
		if (Dat.abbreviation==name) return g;
	} while ( 0!=(g=GetNextQuantity(g)) );
	return DataNothing;
}

/// x[] is only used for experimental data                        
void CData::LoadQuantity(CProblem* pProb, int n, double z[], GraphableQuantity whichquantity, double x[])
{
	switch (whichquantity)
	{
	case DataAux: 	LoadAux(pProb, n, z); 	break;
	case DataX:     LoadX(pProb, n, z);		break;
	case DataPsi:   LoadPsi(pProb, n, z); 	break;
	case DataNd:    LoadNd(pProb, n, z); 	break;
	case DataNa:    LoadNa(pProb, n, z); 	break;
	case DataN:     LoadN(pProb, n, z); 		break;
	case DataP:     LoadP(pProb, n, z); 		break;
	case DataE:     LoadE(pProb, n, z); 		break;
	case DataRho:   LoadRho(pProb, n, z); 	break;
	case DataJn:    LoadJn(pProb, n, z); 	break;
	case DataJp:    LoadJp(pProb, n, z); 	break;
	case DataJt:	LoadJt(pProb, n, z);		break;
	case DataGcum:  LoadGcum(pProb, n, z); 	break;
	case DataEg:    LoadEg(pProb, n, z); 	break;
	case DataNie:   LoadNie(pProb, n, z); 	break;
	case DataTau:   LoadTau(pProb, n, z); 	break;
	case DataRbulk: LoadRbulk(pProb, n, z); 	break;
	case DataRcum:  LoadRcum(pProb, n, z); 	break;
	case DataG:     LoadG(pProb, n, z); 		break;
	case DataVn:    LoadVn(pProb, n, z); 	break;
	case DataVp:    LoadVp(pProb, n, z); 	break;
	case DataMuN:   LoadMuN(pProb, n, z); 	break;
	case DataMuP:   LoadMuP(pProb, n, z); 	break;
	case DataN_:    LoadN_(pProb, n, z); 	break;
	case DataP_:    LoadP_(pProb, n, z); 	break;
	case DataPsi_:  LoadPsi_(pProb, n, z); 	break;
	case DataRho_:  LoadRho_(pProb, n, z); 	break;
	case DataPhiN:  LoadPhiN(pProb, n, z); 	break;
	case DataPhiP:  LoadPhiP(pProb, n, z); 	break;
	case DataEc:    LoadEc(pProb, n, z); 	break;
	case DataEv:    LoadEv(pProb, n, z); 	break;
	case DataEvac:  LoadEvac(pProb, n, z); 	break;
	case DataRes:   LoadRes(pProb, n, z); 	break;
	case DataCond:  LoadCond(pProb, n, z); 	break;
	case DataCcum:  LoadCcum(pProb, n, z); 	break;
	case DataCond_: LoadCond_(pProb, n, z); 	break;
	case DataCcum_: LoadCcum_(pProb, n, z); 	break;
	case DataLd:    LoadLd(pProb, n, z); 	break;
	case DataIn:    LoadIn(pProb, n, z); 	break;
	case DataIp:    LoadIp(pProb, n, z); 	break;
	case DataIt:	LoadIt(pProb, n, z);		break;
	case DataArea:  LoadArea(pProb, n, z); 	break;
	case DataPerm:	LoadPerm(pProb, n, z);	break;
	case DataT:     LoadT(pProb, n, z); 		break;
	case DataVb:    LoadVb(pProb, n, z); 	break;
	case DataIb:    LoadIb(pProb, n, z); 	break;
	case DataVc:    LoadVc(pProb, n, z); 	break;
	case DataIc:    LoadIc(pProb, n, z); 	break;
	case DataPb:    LoadPb(pProb, n, z); 	break;
	case DataPc:    LoadPc(pProb, n, z); 	break;
	case DataWp:    LoadWp(pProb, n, z); 	break;
	case DataEQEp:  LoadEQEp(pProb, n, z); 	break;
	case DataIQEp:  LoadIQEp(pProb, n, z); 	break;
	case DataRflp:  LoadRflp(pProb, n, z); 	break;
	case DataEscp:  LoadEscp(pProb, n, z); 	break;
	case DataInvIQEp:LoadInvIQEp(pProb, n, z);break;
	case DataAbsLp:	LoadAbsLp(pProb, n, z);	break;
	case DataIndxRp:LoadIndxRp(pProb, n, z);	break;
	case DataVint1:	LoadVint1(pProb, n, z);	break;
	case DataIint1:	LoadIint1(pProb, n, z);	break;
	case DataVint2:	LoadVint2(pProb, n, z);	break;
	case DataIint2:	LoadIint2(pProb, n, z);	break;
	case DataVint3:	LoadVint3(pProb, n, z);	break;
	case DataIint3:	LoadIint3(pProb, n, z);	break;
	case DataVint4:	LoadVint4(pProb, n, z);	break;
	case DataIint4:	LoadIint4(pProb, n, z);	break;
	case DataError:	LoadError(pProb, n, z);	break;
	case DataNratio:LoadNratio(pProb, n, z);	break;
	case DataPratio:LoadPratio(pProb, n, z);	break;
	case DataPNratio:LoadPNratio(pProb, n, z);	break;
	case DataPNnorm:LoadPNnorm(pProb, n, z);	break;
	case DataNdrift:LoadNdrift(pProb, n, z);	break;
	case DataNdiff:	LoadNdiff(pProb, n, z);	break;
	case DataPdrift:LoadPdrift(pProb, n, z);	break;
	case DataPdiff:	LoadPdiff(pProb, n, z);	break;
	case DataExperimental:	LoadExperimental(pProb, n, z, x); break;
	case DataBiasedIQEp:	LoadBiasedIQEp(pProb, n, z); break;
	case DataTotalPriReflectance:	LoadTotalPriReflectance(pProb, n, z); break;
// add extra quantities in here

		break;

	default:	//	LoadZero(pProb, n, z);	
			Set("","", "", LINEAR, SPACE_ARRAY);
	}
}

GraphableQuantity CData::m_AuxDataQuantity=DataN;
double	CData::m_AuxPosition=0;	// cm, distance from front for Aux Data
int CData::m_szExptData=0;
double CData::m_ExptX[MAX_EXPT_DATA_POINTS];
double CData::m_ExptY[MAX_EXPT_DATA_POINTS];
CString CData::m_ExptFilename(""); 

// returns FALSE if user cancelled or other problem    
BOOL CData::AskUserToDefineAuxiliary(CWnd* pParent)
{
	CAuxDlg dlg(pParent); 
	if (m_AuxDataQuantity!=0) {
		dlg.m_Data=GetLabel(m_AuxDataQuantity);
	}
	dlg.m_Position = m_AuxPosition*1e4;    	// cm to um
	if (dlg.DoModal()==IDOK && !dlg.m_Data.IsEmpty())
	{
		m_AuxPosition = dlg.m_Position*1e-4;	// um to cm
		m_AuxDataQuantity=DataPsi;
		GraphableQuantity pos=GetFirstQuantity();
		CString lab;
		while (pos){
			lab=GetLabel(pos);
	 		if (lab==dlg.m_Data) m_AuxDataQuantity=pos;
	 		pos=GetNextQuantity(pos);
		};
		return TRUE;
	} else return FALSE;
}
				
///////////////////////////////////////////////////////////////////////////
// CGraph

IMPLEMENT_SERIAL(CGraph, CObject, 0)

void CGraph::SetQuantities(GraphableQuantity pXin, GraphableQuantity pY0, GraphableQuantity pY1,
			GraphableQuantity pY2, GraphableQuantity pY3)
{                                                        
	m_xquantity=pXin; m_yquantity[0]=pY0; m_yquantity[1]=pY1;
	m_yquantity[2]=pY2;	m_yquantity[3]=pY3;
}

void CGraph::Set(CString titleIn, CString xLabelIn, CString yLabelIn,
				 CScaleType xScaleIn, CScaleType yScaleIn)
{                                                        
	title = titleIn; xLabel = xLabelIn; yLabel = yLabelIn; 
	xScale = xScaleIn; yScale = yScaleIn;
}


CGraph::CGraph()
{   
	m_xquantity=DataNothing;	m_yquantity[0]=DataNothing; m_yquantity[1]=DataNothing;
	m_yquantity[2]=DataNothing;	m_yquantity[3]=DataNothing;   
	title.Empty(); xLabel.Empty(); yLabel.Empty(); 
	xScale=LINEAR; yScale=LINEAR;
}

BOOL operator ==(CGraph &first, CGraph &second)
{
	int k;
	if (first.m_xquantity!=second.m_xquantity) return FALSE;
	for (k=0; k<4; k++) {
		if (first.m_yquantity[k]!=second.m_yquantity[k]) return FALSE;
	}
	return TRUE;
}

CGraph & CGraph::operator =(CGraph &gr)
{
	int k;
	m_xquantity=gr.m_xquantity;
	for (k=0; k<4; k++)
		m_yquantity[k]=gr.m_yquantity[k];
	title=gr.title;
	xLabel=gr.xLabel;
	yLabel=gr.yLabel;
	xScale=gr.xScale;
	yScale=gr.yScale;
	m_numpoints=gr.m_numpoints;
	LoadQuantities(NULL, 0, NULL, NULL, NULL, NULL, NULL);
	return *this;
}


int CGraph::NumberOfValidCurves(void)
{         
	int numcurves;
	int k;    
	numcurves=0;
	for (k=0; k<4; k++) { if (m_yquantity[k]!=DataNothing) numcurves++; }
	return numcurves;
}

void CGraph::SetPredefinedGraph(enum GraphType whichgraph)
{
	switch(whichgraph) {
	case CarrierDensities :
		SetQuantities(DataX, DataN, DataP);
		Set("Carrier Densities", "", "", LINEAR, LOG);
		break;				
	case ChargeDensity: 
		SetQuantities(DataX, DataRho);
		Set("", "", "", LINEAR, LINEAR);
		break;
	case CurrentDensity:
	  	SetQuantities(DataX, DataJn, DataJp, DataJt);
	  	Set("Current Density", "", "", LINEAR, LINEAR);
	  	break;
	case DopingDensities:
		SetQuantities(DataX, DataNd, DataNa);
		Set("Doping Densities", "", "", LINEAR, LOG);
		break;
	case ElectricField:
		SetQuantities(DataX, DataE);
		Set("", "", "", LINEAR, LINEAR);
		break;
	case ElectrostaticPotential:
	 	SetQuantities(DataX, DataPsi);
	 	Set("", "", "", LINEAR, LINEAR);
	 	break;
	case Photogeneration:
		SetQuantities(DataX, DataGcum, DataRcum);
		Set("Generation & Recombination", "", "", LINEAR, LINEAR);
		break;
	case CarrierMobilities:
		SetQuantities(DataX, DataMuN, DataMuP);
		Set("Carrier Mobilities", "", "", LINEAR, LINEAR);
		break;
	case CarrierVelocities:
		SetQuantities(DataX, DataVn, DataVp);
		Set("Carrier Velocities", "", "", LINEAR, LINEAR);
		break;
	case EnergyBands:
		SetQuantities(DataX, DataEc, DataEv, DataPhiN, DataPhiP);
		Set("Energy Bands", "", "", LINEAR, LINEAR);
		break;
	case Auxiliary:	
		SetQuantities(DataT, DataAux);
		Set("", "", "", LINEAR, LINEAR); 
		break;
	case BaseCurrent:    	
		SetQuantities(DataT, DataIb);
		Set("Base Current", "", "", LINEAR, LINEAR);
		break;
	case BaseIV:
		SetQuantities(DataVb, DataIb);
		Set("Base Current vs. Voltage", "", "", LINEAR, LINEAR);
		break;
	case BaseIVPower:
		SetQuantities(DataVb, DataIb, DataPb);
		Set("Base Current & Power", "", "#A or #W", LINEAR, LINEAR);
		break;
	case BaseVoltage:
		SetQuantities(DataT, DataVb);
		Set("Base Voltage", "", "", LINEAR, LINEAR);
		break;
	case CollectorCurrent:
		SetQuantities(DataT, DataIc);
		Set("Collector Current", "", "", LINEAR, LINEAR);
		break;
	case CollectorIV:
		SetQuantities(DataVc, DataIc);
		Set("Collector Current vs. Voltage", "", "", LINEAR, LINEAR);
		break;
	case CollectorIVPower:
	 	SetQuantities(DataVc, DataIc, DataPc);
	 	Set("Collector Current & Power", "", "#A or #W", LINEAR, LINEAR);
	 	break;
	case CollectorVoltage:
		SetQuantities(DataT, DataVc);
		Set("Collector Voltage", "", "", LINEAR, LINEAR);
		break;
	case InternalElementV:
		SetQuantities(DataT, DataVint1, DataVint2, DataVint3, DataVint4);
		Set("Shunt Element Voltages", "", "", LINEAR, LINEAR);
		break;
	case InternalElementI:
		SetQuantities(DataT, DataIint1, DataIint2, DataIint3, DataIint4);
		Set("Shunt Element Currents", "", "", LINEAR, LINEAR);
		break;
	case QuantumEff:		
		SetQuantities(DataWp, DataIQEp, DataEQEp, DataTotalPriReflectance);
//		SetQuantities(DataWp, DataIQEp, DataEQEp, DataRflp, DataEscp);
		Set("Quantum Efficiency", "", "RFL, EQE, & IQE (%)", LINEAR, LINEAR);
		break;
	case DiffusionLength:
		SetQuantities(DataX, DataLd);
		Set("Diffusion Length", "", "", LINEAR, LINEAR);
        break;
	}
	LoadQuantities(NULL, 0, NULL, NULL, NULL, NULL, NULL);
}  

void CGraph::LoadQuantities(CProblem* pProb, int n, 
		double x[], double y0[], double y1[], double y2[], double y3[])
{
	pX.LoadQuantity(pProb, n, x, m_xquantity);
	pY[0].LoadQuantity(pProb, n, y0, m_yquantity[0], x);
	if (y1!=NULL) pY[1].LoadQuantity(pProb, n, y1, m_yquantity[1], x);
	if (y2!=NULL) pY[2].LoadQuantity(pProb, n, y2, m_yquantity[2], x);
	if (y3!=NULL) pY[3].LoadQuantity(pProb, n, y3, m_yquantity[3], x);
}
 
BOOL CGraph::AskUserToRedefineGraph(CWnd* pParent)
{
	CGraphDlg dlg(pParent); 
	dlg.m_Temporal=CData::GetType(m_xquantity);
	dlg.m_bXLog = (xScale==LOG);
	dlg.m_bYLog = (yScale==LOG);
	dlg.m_Abscissa=CData::GetLabel(m_xquantity);
	dlg.m_Curve1 = CData::GetLabel(m_yquantity[0]);
	dlg.m_Curve2 = CData::GetLabel(m_yquantity[1]);
	dlg.m_Curve3 = CData::GetLabel(m_yquantity[2]);
	dlg.m_Curve4 = CData::GetLabel(m_yquantity[3]);
	dlg.m_Title  = title;
	dlg.m_YLabel = yLabel;

	if (dlg.DoModal()==IDOK)
	{
		GraphableQuantity pos=CData::GetFirstQuantity();
		CString lab;
		m_xquantity=DataNothing;
		m_yquantity[0]=DataNothing; 
		m_yquantity[1]=DataNothing; 
		m_yquantity[2]=DataNothing; 
		m_yquantity[3]=DataNothing; 
		while (pos){
			lab=CData::GetLabel(pos);	
	 		if (lab==dlg.m_Abscissa) m_xquantity=pos;
	 		if (lab==dlg.m_Curve1)   m_yquantity[0]=pos;
	 		if (lab==dlg.m_Curve2)   m_yquantity[1]=pos;
	 		if (lab==dlg.m_Curve3)   m_yquantity[2]=pos;
	 		if (lab==dlg.m_Curve4)   m_yquantity[3]=pos; 
	 		pos=CData::GetNextQuantity(pos);
		};
		// now compress the array - move all empty slots to the end.
		int k;
		int kfilled=0; //points to first empty slot
		for (k=0; k<4; k++) {
			if (m_yquantity[k]!=DataNothing) { 
				if (k!=kfilled) {
					m_yquantity[kfilled]=m_yquantity[k];
					m_yquantity[k]=DataNothing;
				}
				kfilled++;
			}
		}
		xScale = dlg.m_bXLog ? LOG : LINEAR;
		yScale = dlg.m_bYLog ? LOG : LINEAR;
		title = dlg.m_Title;
		yLabel = dlg.m_YLabel;
		if (m_xquantity==DataNothing || m_yquantity[0]==DataNothing) 
			return FALSE; // all graphs are null!
		else return TRUE;
	} else return FALSE;
}

// Used for save & restore each member of a graph
// This code can cope with an unlimited number of Aux and Experimental data,
// even though existing PC1D can only have 1 of each.
void CData::SerializeGraphableQuantity(CArchive& ar, GraphableQuantity &g)
{
	if (ar.IsStoring())
	{
		ar << (WORD)g;
		if (g==DataAux) {
			ar << m_AuxPosition;
			ASSERT(m_AuxDataQuantity!=DataAux); // this would be infinite loop!
			SerializeGraphableQuantity(ar, m_AuxDataQuantity);
		}
		if (g==DataExperimental) {
			ar << (WORD)0;	// version number
			ar << m_ExptFilename;
		}
	} else { // loading...
		ar >> (WORD &)g;
		if (g==DataAux) {
			ar >> m_AuxPosition;
			ASSERT(m_AuxDataQuantity!=DataAux); // this would be infinite loop!
			SerializeGraphableQuantity(ar, m_AuxDataQuantity);
		}		
		if (g==DataExperimental) {
			WORD exptdatatype;
			ar >> (WORD &)exptdatatype; // version number
			ar >> m_ExptFilename;
			m_szExptData=MAX_EXPT_DATA_POINTS;
			if (!CAscFile::Read(m_ExptFilename, m_szExptData, m_ExptX, m_ExptY)) {
				AfxMessageBox("Error reading Experimental Data file "+m_ExptFilename);
				m_szExptData=0;
			}
		}
	}
}

/// These functions are used for saving & restoring the graph definition
/// For future expansion, graphs aren't limited to a particular number of y values.
void CGraph::Serialize(CArchive& ar)
{
	WORD ver_num;
	int i;
	if (ar.IsStoring())
	{
		ver_num = 1;
		ar << (WORD)ver_num;
		CData::SerializeGraphableQuantity(ar, m_xquantity);
		ar << (WORD)4;	// number of y quantities. (for future expansion)
		for (i=0; i<4; i++) {
			CData::SerializeGraphableQuantity(ar, m_yquantity[i]);
		}
		ar << title << xLabel << yLabel;
		ar << (WORD)xScale << (WORD)yScale;
	}
	else
	{
		ar >> (WORD &)ver_num;
		if (ver_num != 1)
		{
			AfxThrowArchiveException(CArchiveException::badIndex);
			return;
		}
		CData::SerializeGraphableQuantity(ar, m_xquantity);
		WORD numy;
		ar >> (WORD &)numy;
		for (i=0; i<numy; i++) {
			CData::SerializeGraphableQuantity(ar, m_yquantity[i]);
		}
		ar >> title >> xLabel >> yLabel;
		ar >> (WORD &)xScale >> (WORD &)yScale;
	}
}

/*          
					###################
                    #### Equations ####
					###################

             psi(k+1)-psi(k)   x(k)-x(k-1)       psi(k)-psi(k-1)   x(k+1)-x(k)
 E(k)= Vt * -------------- * ------------ + Vt* --------------- * ------------
               x(k+1)-x(k)     x(k+1)-x(k-1)       x(k)-x(k-1)     x(k+1)-x(k-1)

  electrostatic potential PSI(k) = Vt*psi(k)
  donor doping density    Nd(k)  = Nd(k)*nir
  acceptor doping density Na(k)  = Na(k)*nir

                                 psi(k)+Vn(k)-phi_n(k)
  electron density n(k) = nir * e

                                 -psi(k)+Vp(k)+phi_p(k)
  hole density     p(k) = nir * e

  rho(k) = Q * { p(k) - n(k) + Nd(k) - Na(k) }

  electron current density Jn= q * nir * In(k) / Area(k) 
  hole current density 	   Jp = q * nir * Ip(k) / Area(k)
  total current density    Jt = q * nir * { In(k) + Ip(k) } / Area(k)
  Vn(k) = -In(k) / Area(k) * n(k)
  
  electron velocity Vn(k)= - I_n(k) / { Area(k)* { psi(k)+Vn(k)-phi_n(k)}}

                   { Vn(k)+Vp(k) }/2
  Nie(k) = nir * e

         nir     { G(k-1)-G(k)    x(k)-x(k+1)     G(k)-G(k-1)   x(k+1)-x(k)   }
 G(k)= ------  * { ----------- * ------------- +  ----------- * ------------- }
       Area(k)   { x(k+1)-x(k)   x(k+1)-x(k-1)    x(k)-x(k-1)   x(k+1)-x(k-1) }

*/


//////////////////////////////////////////////////////////////////////////////
// CPc1dView data loading functions

// **** All Load functions must accept n=0, because this call is used
// **** with n=0 to initialize the label strings for each object

void CData::LoadZero(CProblem*, int n, double z[])
{
	Set("Zero", "Nothing", "Zero", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	for (int k=0; k<n; k++) z[k]=0;		// default implementation
}

void CData::SetAuxDataValue(CProblem *pProb)
{
	int elementsrequired;
	ASSERT(m_AuxDataQuantity!=DataAux);			// avoid infinite recursion!
	ASSERT(m_AuxDataQuantity!=DataNothing);
	elementsrequired=pProb->GetNode()->GetNodeCount();
	double *z=new double [elementsrequired+1];
	LoadQuantityIntoArray(pProb,pProb->GetNode()->GetNodeCount(), z, m_AuxDataQuantity); // Spatial basis
	int k=0;
	while (k<pProb->GetNode()->GetLastNode()
			&& m_AuxPosition>pProb->GetNode()->GetX(k)) k++;		// find location
	pProb->m_Aux[pProb->GetStatus()->time_step+1] = z[k];			// time array
	delete [] z;
}

void CData::OnGraphDefineExperimental(void)
{
	BOOL FileOpen=TRUE;
	CString Ext="txt";
	CString Filter = "Text files (*.txt)|*.txt|All Files (*.*)|*.*||";
	CFileDialog dlg(FileOpen, Ext, m_ExptFilename, OFN_OVERWRITEPROMPT, Filter);
	dlg.m_ofn.lpstrTitle = "Open Experimental Data File";
	dlg.m_ofn.lpstrInitialDir = ((CPc1dApp *)AfxGetApp())->m_Path.dev;
	if (dlg.DoModal()==IDOK)
	{
		m_ExptFilename = dlg.GetPathName();
//		m_ExptFilename.MakeLower();
		m_szExptData=MAX_EXPT_DATA_POINTS;
		if (!CAscFile::Read(m_ExptFilename, m_szExptData, m_ExptX, m_ExptY)) {
			AfxMessageBox("Error reading Experimental Data file "+m_ExptFilename);
			m_szExptData=0;
		}
	}
}

void CData::LoadAux(CProblem* pProb, int n, double z[])
{
	int k;

	if (pProb!=NULL) {
		CString cstr;
		char str[17];
		CData Dat;        
		k=0;
		while (k<pProb->GetNode()->GetLastNode()
				&& m_AuxPosition>pProb->GetNode()->GetX(k)) k++;		// find location
		safesprintf(str, 16, "%0.2f µm", pProb->GetNode()->GetX(k)*1e4);

		if (m_AuxDataQuantity!=DataNothing || m_AuxDataQuantity!=DataAux) { // avoid infinite recursion!
			Dat.LoadQuantity(NULL,0, NULL, m_AuxDataQuantity); // Spatial basis
		} else Dat.LoadZero(NULL, 0, NULL);
		cstr = Dat.units + " at " + str;

		pProb->m_AuxUnits = cstr;
		Set(pProb->m_AuxUnits, Dat.label, Dat.abbreviation, LINEAR, TIME_ARRAY);
	} else
		Set("", "Auxiliary Data", "Aux", LINEAR, TIME_ARRAY);
	if (n==0) return;
	
	if (m_AuxDataQuantity==DataNothing || m_AuxDataQuantity==DataAux) { // avoid infinite recursion!
			for (k=0; k<n; k++)	z[k] = 0;
			return;
	}
	for (k=0; k<n; k++)
		z[k] = pProb->m_Aux[k];
}

void CData::LoadPsi(CProblem* pProb, int n, double z[])
{
	Set("volts", "Electrostatic Potential", "Psi", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double Vt = pProb->GetVt();
	for (int k=0; k<n; k++)
		z[k] = Vt*pN->GetPsi(k);
}

void CData::LoadX(CProblem* pProb, int n, double z[])
{
	Set("#m", "Distance from Front", "x", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	for (int k=0; k<n; k++)
		z[k] = pN->GetX(k)*1e-2;
}

void CData::LoadNd(CProblem* pProb, int n, double z[])
{
	Set("cm-3", "Donor Doping Density", "Nd", LOG, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double nir = pProb->GetNir();
	for (int k=0; k<n; k++)
		z[k] = pN->GetNd(k)*nir;
}

void CData::LoadNa(CProblem* pProb, int n, double z[])
{
	Set("cm-3", "Acceptor Doping Density", "Na", LOG, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double nir = pProb->GetNir();
	for (int k=0; k<n; k++)
		z[k] = pN->GetNa(k)*nir;
}

void CData::LoadN(CProblem* pProb, int n, double z[])
{
	Set("cm-3", "Electron Density","n", LOG, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double nir = pProb->GetNir();
	for (int k=0; k<n; k++)
		z[k] = nir*pN->GetN_nir(k);
}

void CData::LoadP(CProblem* pProb, int n, double z[])
{
	Set("cm-3", "Hole Density", "p", LOG, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double nir = pProb->GetNir();
	for (int k=0; k<n; k++)
		z[k] = nir*pN->GetP_nir(k);
}

// calculate E using a weighted average of the E field at either side of
// the element.
void CData::LoadE(CProblem* pProb, int n, double z[])
{
	Set("#V/cm", "Electric Field", "E", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double Vt = pProb->GetVt();
	for (int k=0; k<n; k++) z[k]=Vt * pN->GetE_Vt(k);
}

void CData::LoadRho(CProblem* pProb, int n, double z[])
{
	Set("#C/cm3", "Charge Density", "Rho", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double nir = pProb->GetNir();
	for (int k=0; k<n; k++)
			z[k] = Q*nir*(   (pN->GetP_nir(k)-pN->GetNa(k)) 
							      + (pN->GetNd(k)- pN->GetN_nir(k))   );
}

void CData::LoadJn(CProblem* pProb, int n, double z[])
{
	Set("#A/cm2", "Electron Current Density", "Jn", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double nir = pProb->GetNir();
	for (int k=0; k<n; k++)
		z[k] = Q*nir*pN->GetI_n(k)/pN->GetArea(k);
}

void CData::LoadJp(CProblem* pProb, int n, double z[])
{
	Set("#A/cm2", "Hole Current Density", "Jp", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double nir = pProb->GetNir();
	for (int k=0; k<n; k++)
		z[k] = Q*nir*pN->GetI_p(k)/pN->GetArea(k);
}
		
void CData::LoadJt(CProblem* pProb, int n, double z[])
{
	Set("#A/cm2", "Total Current Density", "J total", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double nir = pProb->GetNir();
	for (int k=0; k<n; k++)
		z[k] = Q*nir*(pN->GetI_n(k)+pN->GetI_p(k))/pN->GetArea(k);
}

void CData::LoadGcum(CProblem* pProb, int n, double z[])
{
	Set("s-1", "Cumulative Photogeneration", "Cum. Photogen.", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double nir = pProb->GetNir();
	for (int k=0; k<n; k++)
		z[k] = nir*pProb->GetAref()*pN->GetGcum(k);
}

void CData::LoadEg(CProblem* pProb, int n, double z[])
{
	Set("eV", "Energy Gap (electrical)", "Eg", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double Vt = pProb->GetVt();
	for (int k=0; k<n; k++)
		z[k] = pProb->GetEg() - Vt*(pN->GetVn(k) + pN->GetVp(k));
}

void CData::LoadNie(CProblem* pProb, int n, double z[])
{
	Set("cm-3", "Intrinsic Conc. (effective)", "nie", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double nir = pProb->GetNir();
	for (int k=0; k<n; k++)
		z[k] = nir*pN->GetNie_nir(k);
}


void CData::LoadRbulk(CProblem* pProb, int n, double z[])
{
	Set("cm-3/s", "Bulk Recombination Rate", "Bulk Recom.", LOG, SPACE_ARRAY);
	if (n==0) return;
	double nir = pProb->GetNir();
	for (int k=0; k<n; k++)
	{
		z[k]=nir * pProb->GetSolve()->GetBulkRecombination_nir(k);
	}
}

void CData::LoadRcum(CProblem* pProb, int n, double z[])
{
	Set("s-1", "Cumulative Recombination", "Cum.Recom.", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	CRegionData *pRegData= pProb->GetRegionData();
	double nir = pProb->GetNir();
	double n_, p_, nie, RecC, RecL;
	int reg;
	RecL = 0;
	for (int k=0; k<n; k++)
	{
		n_ =  nir * pN->GetN_nir(k);
		p_ =  nir * pN->GetP_nir(k);
		nie = nir * pN->GetNie_nir(k);

		reg = pN->GetRegion(k);
		z[k] = 0;
		CSurfaceRecomData ss;

		if (pN->GetX(k)==pRegData->GetRegionFront(reg)) {
			ss = pRegData->GetFrontRecom(reg);
			z[k] += (pProb->GetAref()*pN->GetArea(k)
				*CPhysics::Surf_Recomb(n_, p_, nie, ss.m_Sn, ss.m_Sp, ss.m_Et, ss.m_Jo));
		}
		if (pN->GetX(k)==pRegData->GetRegionRear(reg)) {
			ss = pRegData->GetRearRecom(reg);
			z[k] += (pProb->GetAref()*pN->GetArea(k)
				*CPhysics::Surf_Recomb(n_, p_, nie, ss.m_Sn, ss.m_Sp, ss.m_Et, ss.m_Jo));
		}

		RecC = nir * pProb->GetSolve()->GetBulkRecombination_nir(k);

		if (k>0)
			z[k] += pProb->GetAref()*pN->GetArea(k)*(RecC+RecL)/2
					* pN->Get_dxR(k-1); // (pN->x[k]-pN->x[k-1]);
		RecL = RecC;
	}
	for (k=1; k<n; k++) z[k] += z[k-1];
}

void CData::LoadTau(CProblem* pProb, int n, double z[])
{
	Set("#s", "Minority Carrier Lifetime", "Tau", LOG, SPACE_ARRAY);
	if (n==0) return;
	for (int k=0; k<n; k++)
	{
		z[k] = pProb->GetSolve()->GetTau(k);
		if (z[k]<0) z[k]=0;
	}
}

void CData::LoadLd(CProblem* pProb, int n, double z[])
{
	Set("#m", "Diffusion Length", "Ld", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	for (int k=0; k<n; k++)
	{
		if (pN->IsNGreaterThanP(k)) {
			z[k] = pProb->GetSolve()->GetTau(k) * pN->GetMup_includinghighfieldeffect(k) / pN->GetArea(k);
		} else {
			// if n==p, tau will be zero, so z[k]=0
			z[k] = pProb->GetSolve()->GetTau(k) * pN->GetMun_includinghighfieldeffect(k) / pN->GetArea(k);
		}
		if (z[k]<=0) z[k] = 0;
		else z[k] = (1e-2*sqrt(z[k]));
	}
}

void CData::LoadG(CProblem* pProb, int n, double z[])
{
	Set("cm-3/s", "Generation Rate","Gen.Rate", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double nir = pProb->GetNir();
	for (int k=0; k<n; k++) 
		z[k] = pN->GetGenerationRate(k) * nir/pN->GetArea(k);
}

void CData::LoadVn(CProblem* pProb, int n, double z[])
{
	Set("cm/s", "Electron Velocity", "Vn", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double n_;
	for (int k=0; k<n; k++)
	{
		n_ = pN->GetN_nir(k);
		z[k] = - (pN->GetI_n(k) / (pN->GetArea(k) * n_));
	}
}

void CData::LoadVp(CProblem* pProb, int n, double z[])
{
	Set("cm/s", "Hole Velocity", "Vp", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double p_;
	for (int k=0; k<n; k++)
	{
		p_ = pN->GetP_nir(k);
		z[k] = pN->GetI_p(k) / (pN->GetArea(k) * p_);
	}
}

void CData::LoadMuN(CProblem* pProb, int n, double z[])
{
	Set("cm2/Vs", "Electron Mobility", "Mu n", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double Vt = pProb->GetVt();

	for (int k=0; k<n; k++)
	{
		z[k]= pN->GetMun_times_Vt(k)/Vt;
	}
}

void CData::LoadMuP(CProblem* pProb, int n, double z[])
{
	Set("cm2/Vs", "Hole Mobility", "Mu p", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double Vt = pProb->GetVt();
	for (int k=0; k<n; k++)
	{
		z[k]= pN->GetMup_times_Vt(k)/Vt;
//		z[k] = CMath::exp(pN->Up[k]-pN->Vp[k])/pN->Area[k]/pProb->Vt;
	}
}

void CData::LoadN_(CProblem* pProb, int n, double z[])
{
	Set("cm-3", "Excess Electron Density", "Excess n", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double nir = pProb->GetNir();
	for (int k=0; k<n; k++)
	{
		z[k] = nir* (pN->GetN_nir(k) - pN->GetNeq_nir(k));
//		double n_, neq;
//		n_ = nir* pN->GetN_nir(k);
//		neq = nir* pN->GetNeq_nir(k);
//		z[k] = (n_ - neq);
	}
}

void CData::LoadP_(CProblem* pProb, int n, double z[])
{
	Set("cm-3", "Excess Hole Density", "Excess p", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double nir = pProb->GetNir();
	for (int k=0; k<n; k++)
	{
//		double p_, peq;
//		p_ = pProb->nir*CMath::exp(-pN->Psi[k]+pN->Vp[k]+pN->Phi_p[k]);
//		peq = pProb->nir*CMath::exp(-pN->Psi_eq[k]+pN->Vp[k]);
		z[k] = nir * (pN->GetP_nir(k) - pN->GetPeq_nir(k));
	}
}

void CData::LoadPsi_(CProblem* pProb, int n, double z[])
{
	Set("#V", "Excess Electrostatic Potential", "Excess Psi", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double Vt = pProb->GetVt();
	for (int k=0; k<n; k++)
	{
		z[k] = Vt*(pN->GetPsi(k)-pN->GetPsi_eq(k));
	}
}

void CData::LoadRho_(CProblem* pProb, int n, double z[])
{
	Set("#C/cm3", "Excess Charge Density", "Excess rho", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double nir = pProb->GetNir();
	double n_, p_, neq, peq;
	for (int k=0; k<n; k++)
	{
		n_ = pN->GetN_nir(k);
		neq = pN->GetNeq_nir(k);
		p_  = pN->GetP_nir(k);
		peq = pN->GetPeq_nir(k);
		z[k] = Q*nir*(p_ - n_ - peq + neq);
	}
}

void CData::LoadPhiN(CProblem* pProb, int n, double z[])
{
	Set("eV", "Electron Quasi-Fermi Energy", "Phi N", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double Vt = pProb->GetVt();
	for (int k=0; k<n; k++)
		z[k] = -Vt*pN->GetPhi_n(k);
}

void CData::LoadPhiP(CProblem* pProb, int n, double z[])
{
	Set("eV", "Hole Quasi-Fermi Energy", "Phi P", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double Vt = pProb->GetVt();
	for (int k=0; k<n; k++)
		z[k] = -Vt*pN->GetPhi_p(k);
}

void CData::LoadEc(CProblem* pProb, int n, double z[])
{
	Set("eV", "Conduction Band Edge", "Ec", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	CRegionData *pReg = pProb->GetRegionData();
	double Vt = pProb->GetVt();
	int reg;
	for (int k=0; k<n; k++)
	{
		reg = pN->GetRegion(k);
		z[k] = Vt*(CMath::log(pReg->GetNc_nir(reg))
						- pN->GetPsi(k) - pN->GetVn(k));
	}
}

void CData::LoadEv(CProblem* pProb, int n, double z[])
{
	Set("eV", "Valence Band Edge", "Ev", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	CRegionData *pReg = pProb->GetRegionData();
	double Vt = pProb->GetVt();
	int reg;
	for (int k=0; k<n; k++)
	{
		reg = pN->GetRegion(k);
		z[k] = -Vt*(CMath::log(pReg->GetNv_nir(reg))
						+ pN->GetPsi(k) - pN->GetVp(k));
	}
}

void CData::LoadEvac(CProblem* pProb, int n, double z[])
{
	Set("eV", "Vacuum Energy", "E vac", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	CRegionData *pReg = pProb->GetRegionData();
	double Vt = pProb->GetVt();
	int reg;
	for (int k=0; k<n; k++)
	{
		reg = pN->GetRegion(k);
		z[k] = Vt*(CMath::log(pReg->GetNc_nir(reg))
						- pN->GetPsi(k) - pN->GetVn(k))
					+ Vt*pReg->GetAff_Vt(reg);
	}
}

void CData::LoadRes(CProblem* pProb, int n, double z[])
{
	Set("ohm*cm", "Resistivity", "Res", LOG, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double nir = pProb->GetNir();
	double Vt = pProb->GetVt();
	for (int k=0; k<n; k++)
	{
		double cond = Q * nir * 
			( pN->GetMun_times_N(k) + pN->GetMup_times_P(k))
			/ (Vt * pN->GetArea(k));
		z[k]=  1/cond;
/**
		z[k] = 1/(Q * pProb->nir *
					( CMath::exp(pN->Psi[k]+pN->Vn[k]-pN->Phi_n[k])			// n
					* CMath::exp(pN->Un[k]-pN->Vn[k])/pN->Area[k]/pProb->Vt		// mu_n
		            + CMath::exp(-pN->Psi[k]+pN->Vp[k]+pN->Phi_p[k])		// p
					* CMath::exp(pN->Up[k]-pN->Vp[k])/pN->Area[k]/pProb->Vt ) );	// mu_p
**/
	}
}

void CData::LoadCond(CProblem* pProb, int n, double z[])
{
	Set("S/cm", "Conductivity", "Cond", LOG, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double nir = pProb->GetNir();
	double Vt = pProb->GetVt();

	for (int k=0; k<n; k++)
	{
//		z[k] = Q * pProb->nir/pProb->Vt *
//					( pN->GetN_nir(k) * pN->GetMun_times_Vt(k)
//					+ pN->GetP_nir(k) * pN->GetMup_times_Vt(k));
		z[k] = Q * nir*
				( pN->GetMun_times_N(k) + pN->GetMup_times_P(k))
					/(Vt*pN->GetArea(k)) ;
	}
}

void CData::LoadCcum(CProblem* pProb, int n, double z[])
{
	Set("S", "Cumulative Conductivity", "Cum. Cond.", LOG, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double nir = pProb->GetNir();
	double Vt = pProb->GetVt();
	for (int k=0; k<n; k++)
	{
//		z[k] = Q * pProb->nir / pProb->Vt *
//					( pN->GetN_nir(k) * pN->GetMun_times_Vt(k)
//					+ pN->GetP_nir(k) * pN->GetMup_times_Vt(k));
		z[k] = Q * nir *
					(pN->GetMun_times_N(k) + pN->GetMup_times_P(k))
					/( pN->GetArea(k) * Vt);
	}
	for (k=n-1; k>0; k--)	// replace z[k] with average z[k] for left element
		if (z[k]!=z[k-1]) z[k] = ((z[k]-z[k-1])/CMath::log(z[k]/z[k-1]));
	z[0] = 0;	// perform cumulation
//	for (k=1; k<n; k++) z[k] = z[k-1] + (z[k]*(pN->x[k]-pN->x[k-1]));
	for (k=1; k<n; k++) z[k] = z[k-1] + z[k]*pN->Get_dxR(k-1);
}

void CData::LoadCond_(CProblem* pProb, int n, double z[])
{
	Set("#S/cm", "Excess Conductivity", "Excess Cond.", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double nir = pProb->GetNir();
	double Vt = pProb->GetVt();
	for (int k=0; k<n; k++)
	{
		z[k] = Q * nir/Vt 
				* (	pN->GetMun_times_N(k) + pN->GetMup_times_P(k)
				  - pN->GetMun_times_Neq(k) - pN->GetMup_times_Peq(k))
				/pN->GetArea(k);
/******
		z[k] = Q * pProb->nir * (
					( pN->GetN_nir(k) // n
					* CMath::exp(pN->Un[k]-pN->Vn[k])/pN->Area[k]/pProb->Vt		// mu_n
					+ pN->GetP_nir(k) // p
					* CMath::exp(pN->Up[k]-pN->Vp[k])/pN->Area[k]/pProb->Vt ) 		// mu_p
				  - ( pN->GetNeq_nir(k) // neq
					* CMath::exp(pN->Un[k]-pN->Vn[k])/pN->Area[k]/pProb->Vt		// mu_n
					+ pN->GetPeq_nir(k) // peq
					* CMath::exp(pN->Up[k]-pN->Vp[k])/pN->Area[k]/pProb->Vt ) );	// mu_p
*****/
	}
}

void CData::LoadCcum_(CProblem* pProb, int n, double z[])
{
	Set("#S", "Cumulative Excess Conductivity", "Cum. Excess Cond.", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double nir = pProb->GetNir();
	double Vt = pProb->GetVt();
	for (int k=0; k<n; k++)
	{
		z[k] = Q * nir/Vt 
				* (	pN->GetMun_times_N(k) + pN->GetMup_times_P(k)
				  - pN->GetMun_times_Neq(k) - pN->GetMup_times_Peq(k))
				/pN->GetArea(k);
/*****
		z[k] = Q * pProb->nir * (
					( pN->GetN_nir(k) // n
					* CMath::exp(pN->Un[k]-pN->Vn[k])/pN->Area[k]/pProb->Vt		// mu_n
					+ pN->GetP_nir(k) // p
					* CMath::exp(pN->Up[k]-pN->Vp[k])/pN->Area[k]/pProb->Vt ) 		// mu_p
				  - ( pN->GetNeq_nir(k) // neq
					* CMath::exp(pN->Un[k]-pN->Vn[k])/pN->Area[k]/pProb->Vt		// mu_n
					+ pN->GetPeq_nir(k) // peq
					* CMath::exp(pN->Up[k]-pN->Vp[k])/pN->Area[k]/pProb->Vt ) );	// mu_p
******/
	}
	for (k=n-1; k>0; k--)	// replace z[k] with average z[k] for left element
		if (z[k]!=z[k-1]) z[k] = ((z[k]-z[k-1])/CMath::log(z[k]/z[k-1]));
	z[0] = 0;	// perform cumulation
//	for (k=1; k<n; k++) z[k] = z[k-1] + (z[k]*(pN->x[k]-pN->x[k-1]));
	for (k=1; k<n; k++) z[k] = z[k-1] + z[k]*pN->Get_dxR(k-1);
}

void CData::LoadIn(CProblem* pProb, int n, double z[])
{
	Set("#A", "Electron Current", "In", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double nir = pProb->GetNir();
	double Aref = pProb->GetAref();

	for (int k=0; k<n; k++)
		z[k] = Q*nir*Aref*pN->GetI_n(k);
}

void CData::LoadIp(CProblem* pProb, int n, double z[])
{
	Set("#A", "Hole Current", "Ip", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double nir = pProb->GetNir();
	double Aref = pProb->GetAref();
	for (int k=0; k<n; k++)
		z[k] = Q*nir*Aref*pN->GetI_p(k);
}

void CData::LoadIt(CProblem* pProb, int n, double z[])
{
	Set("#A", "Total Current", "I tot", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double nir = pProb->GetNir();
	double Aref = pProb->GetAref();
	for (int k=0; k<n; k++)
		z[k] = Q*nir*Aref*(pN->GetI_n(k)+pN->GetI_p(k));
}

void CData::LoadArea(CProblem* pProb, int n, double z[])
{
	Set("cm2", "Cross-Sectional Area", "Area", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CString str;
	double mult;
	switch (pProb->GetDevice()->m_Aunit)
	{
		case CM2: {str = "cm2"; mult = 1; break;}
		case MM2: {str = "mm2"; mult = 1e2; break;}
		case UM2: {str = "µm2"; mult = 1e8; break;}
	}
	Set(str, "Cross-Sectional Area", "Area", LINEAR, SPACE_ARRAY);
	CNode *pN = pProb->GetNode();
	double Aref = pProb->GetAref();
	for (int k=0; k<n; k++)
		z[k] = Aref*pN->GetArea(k)*mult;
}

void CData::LoadPerm(CProblem* pProb, int n, double z[])
{
	Set("", "Dielectric Constant", "Perm.", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double nir = pProb->GetNir();
	double Vt = pProb->GetVt();
	for (int k=0; k<n; k++)
		z[k] = (Q*nir*pN->GetEps(k)/(Vt*EPS0));
}

void CData::LoadT(CProblem* pProb, int n, double z[])
{
	Set("#s", "Elapsed Time", "t", LINEAR, TIME_ARRAY);
	if (n==0) return;
	for (int k=0; k<n; k++)
		z[k] = pProb->m_Time[k];
}

void CData::LoadVb(CProblem* pProb, int n, double z[])
{
	Set("#V", "Base Voltage", "Vb", LINEAR, TIME_ARRAY);
	if (n==0) return;
	CExcite *pE=pProb->GetExcite();
	for (int k=0; k<n; k++)
		z[k] = pE->m_Base.m_Volts[k];
}

void CData::LoadIb(CProblem* pProb, int n, double z[])
{
	Set("#A", "Base Current", "Ib", LINEAR, TIME_ARRAY);
	if (n==0) return;
	CExcite *pE=pProb->GetExcite();
	for (int k=0; k<n; k++)
		z[k] = pE->m_Base.m_Amps[k];
}

void CData::LoadVc(CProblem* pProb, int n, double z[])
{
	Set("#V", "Collector Voltage", "Vc", LINEAR, TIME_ARRAY);
	if (n==0) return;
	CExcite *pE=pProb->GetExcite();
	for (int k=0; k<n; k++)
		z[k] = pE->m_Coll.m_Volts[k];
}

void CData::LoadIc(CProblem* pProb, int n, double z[])
{
	Set("#A", "Collector Current", "Ic", LINEAR, TIME_ARRAY);
	if (n==0) return;
	CExcite *pE=pProb->GetExcite();
	for (int k=0; k<n; k++)
		z[k] = pE->m_Coll.m_Amps[k];
}

void CData::LoadPb(CProblem* pProb, int n, double z[])
{
	Set("#W", "Base Power", "Pb", LINEAR, TIME_ARRAY);
	if (n==0) return;
	CExcite *pE=pProb->GetExcite();
	for (int k=0; k<n; k++)
		z[k] = pE->m_Base.m_Volts[k]*pE->m_Base.m_Amps[k];
}

void CData::LoadPc(CProblem* pProb, int n, double z[])
{
	Set("#W", "Collector Power", "Pc", LINEAR, TIME_ARRAY);
	if (n==0) return;
	CExcite *pE=pProb->GetExcite();
	for (int k=0; k<n; k++)
		z[k] = pE->m_Coll.m_Volts[k]*pE->m_Coll.m_Amps[k];
}

void CData::LoadWp(CProblem* pProb, int n, double z[])
{
	Set("nm", "Primary Source Wavelength", "P/Lambda", LINEAR, TIME_ARRAY);
	if (n==0) return;
	CExcite *pE=pProb->GetExcite();
	for (int k=0; k<n; k++)
		z[k] = pE->m_LightPri.m_Lambda[k];
}

void CData::LoadEQEp(CProblem* pProb, int n, double z[])
{
	Set("%", "External Quantum Efficiency", "EQE", LINEAR, TIME_ARRAY);
	if (n==0) return;
	double Aref = pProb->GetAref();
	CExcite *pE=pProb->GetExcite();
	for (int k=0; k<n; k++)
		if (pE->m_LightPri.m_Intensity[k]>0 &&
			pE->m_LightPri.m_Lambda[k]>0)
			z[k] = 100*HC/pE->m_LightPri.m_Lambda[k] *
					fabs(pE->m_Base.m_Amps[k])/
					(Aref*pE->m_LightPri.m_Intensity[k]);
}

void CData::LoadIQEp(CProblem* pProb, int n, double z[])
{
	Set("%", "Internal Quantum Efficiency", "IQE", LINEAR, TIME_ARRAY);
	if (n==0) return;
	double Aref = pProb->GetAref();
	CExcite *pE=pProb->GetExcite();
	for (int k=0; k<n; k++)
		if (pE->m_LightPri.m_Intensity[k]>0 &&
			pE->m_LightPri.m_Lambda[k]>0)
			z[k] = 100*HC/pE->m_LightPri.m_Lambda[k] *
					fabs(pE->m_Base.m_Amps[k])/
					(pE->m_LightPri.m_Intensity[k]*Aref
						*( (1-pE->m_LightPri.m_Reflectance[k])
							* (1-pE->m_LightPri.m_Escape[k]) ) );	
							// m_Escape is fraction of light entering device that escapes,
							// not fraction of light incident on device.
}

// subtract steady-state value
void CData::LoadBiasedIQEp(CProblem* pProb, int n, double z[])
{
	Set("%", "IQE adjusted for light bias", "Biased IQE", LINEAR, TIME_ARRAY);
	if (n==0) return;
	z[0]=0;
	CExcite *pE=pProb->GetExcite();
	double Aref = pProb->GetAref();
	for (int k=1; k<n; k++)
		if (pE->m_LightPri.m_Intensity[k]>0 &&
			pE->m_LightPri.m_Lambda[k]>0)
			z[k] = 100*HC/pE->m_LightPri.m_Lambda[k] *
					fabs(pE->m_Base.m_Amps[k]-pE->m_Base.m_Amps[0])/
					(pE->m_LightPri.m_Intensity[k]*Aref
						*( (1-pE->m_LightPri.m_Reflectance[k])
							* (1-pE->m_LightPri.m_Escape[k]) ) );	
							// m_Escape is fraction of light entering device that escapes,
							// not fraction of light incident on device.
	else z[k]=0;

	if (n>1) z[0]=z[1];
}


void CData::LoadRflp(CProblem* pProb, int n, double z[])
{
	Set("%", "Pri-Surface Reflectance", "P/Refl", LINEAR, TIME_ARRAY);
	if (n==0) return;
	CExcite *pE=pProb->GetExcite();
	for (int k=0; k<n; k++)
		z[k] = 100*pE->m_LightPri.m_Reflectance[k];
}

// escape + reflectance
void CData::LoadTotalPriReflectance(CProblem* pProb, int n, double z[])
{
	Set("%", "Pri-Surface Total Reflectance", "P/TotalRefl", LINEAR, TIME_ARRAY);
	if (n==0) return;
	CExcite *pE=pProb->GetExcite();
	for (int k=0; k<n; k++)
		z[k] = 100*pE->m_LightPri.m_Reflectance[k] +
			100*(1-pE->m_LightPri.m_Reflectance[k])*pE->m_LightPri.m_Escape[k];
}

void CData::LoadEscp(CProblem* pProb, int n, double z[])
{
	Set("%", "Pri-Surface Escape", "P/Esc", LINEAR, TIME_ARRAY);
	if (n==0) return;
	CExcite *pE=pProb->GetExcite();
	for (int k=0; k<n; k++)
		z[k] = 100*(1-pE->m_LightPri.m_Reflectance[k])*pE->m_LightPri.m_Escape[k];
	// m_Escape is fraction of light entering device that escapes,
	// not fraction of light incident on device.
}

void CData::LoadInvIQEp(CProblem* pProb, int n, double z[])
{
	Set("", "Inverse IQE", "Inv IQE", LINEAR, TIME_ARRAY);
	if (n==0) return;
	double Aref = pProb->GetAref();
	CExcite *pE=pProb->GetExcite();
	for (int k=0; k<n; k++)
		if (pE->m_LightPri.m_Intensity[k]>0 &&
			pE->m_LightPri.m_Lambda[k]>0 &&
			pE->m_Base.m_Amps[k]!=0)
			z[k] =  1/
					(HC/pE->m_LightPri.m_Lambda[k] *
					fabs(pE->m_Base.m_Amps[k])/
					(pE->m_LightPri.m_Intensity[k]*Aref
						*( (1-pE->m_LightPri.m_Reflectance[k])
							* (1-pE->m_LightPri.m_Escape[k]) ) ) );	
}

void CData::LoadAbsLp(CProblem* pProb, int n, double z[])
{
	Set("#m", "Pri-Surface Absorption Length", "P/Abs Length", LINEAR, TIME_ARRAY);
	if (n==0) return;
	CMaterial* pM;
	CExcite *pE=pProb->GetExcite();
	int reg;
	if (pE->m_LightPri.m_Back) reg=pProb->GetDevice()->m_NumRegions-1; else reg=0;
	pM=pProb->GetDevice()->GetMaterialForRegion(reg);
	for (int k=0; k<n; k++) {
		double lambda = pE->m_LightPri.m_Lambda[k];
		z[k]=0;
		if (lambda>0) {
			double absorb = pM->GetAbsorptionAtWavelength(lambda, pProb->GetT());
			if (absorb>0) z[k] = 1e-2/absorb;
		}
	}
}

void CData::LoadIndxRp(CProblem* pProb, int n, double z[])
{
	Set("", "Pri-Surface Refractive Index", "P/Index", LINEAR, TIME_ARRAY);
	if (n==0) return;
	CExcite *pE=pProb->GetExcite();
	CMaterial* pM;
	int reg;
	if (pE->m_LightPri.m_Back) reg=pProb->GetDevice()->m_NumRegions-1; else reg=0;
	pM=pProb->GetDevice()->GetMaterialForRegion(reg);
	for (int k=0; k<n; k++) {
		double lambda = pE->m_LightPri.m_Lambda[k];
		if (lambda>0) {
			z[k] = pM->GetRefractiveIndexAtWavelength(lambda);
		} else z[k]=0;
	}
}

/***
void CData::LoadIndxRp(CProblem* pProb, int n, double z[])
{
	Set("", "Pri-Surface Refractive Index", "P/Index", LINEAR, TIME_ARRAY);
	if (n==0) return;
	CExcite *pE=pProb->GetExcite();
	CMaterial* pM;
	double index = 0;
	int reg;
//	int SaveRegion = pProb->pD->m_CurrentRegion;
	if (pE->m_LightPri.m_Back) reg=pProb->GetDevice()->m_NumRegions-1; else reg=0;
//	pProb->pD->SelectRegion(reg); pM=pProb->pD->GetCurrentMaterial();
	pM=pProb->GetDevice()->GetMaterialForRegion(reg);
	for (int k=0; k<n; k++) if (pE->m_LightPri.m_Lambda[k]>0)
	{
		double lambda = pE->m_LightPri.m_Lambda[k];
		z[k] = pM->GetRefractiveIndexAtWavelength(lambda);
		if (pM->m_IndexExternal)
   			index = CMath::LinearInterp(pE->m_LightPri.m_Lambda[k], pM->m_nIndex, 
    							pM->m_IdxLambda, pM->m_Index);
		else
			index = pM->m_FixedIndex;
		z[k] = index;
	}
// 	pProb->pD->SelectRegion(SaveRegion);
}
***/

void CData::LoadVint1(CProblem* pProb, int n, double z[])
{
	Set("#V", "Shunt #1 Voltage", "V1", LINEAR, TIME_ARRAY);
	if (n==0) return;
	CDevice *pD = pProb->GetDevice();
	for (int k=0; k<n; k++)
		z[k] = pD->GetLumpedElement(0)->m_Volts[k];
}

void CData::LoadIint1(CProblem* pProb, int n, double z[])
{
	Set("#A", "Shunt #1 Current", "I1", LINEAR, TIME_ARRAY);
	if (n==0) return;
	CDevice *pD = pProb->GetDevice();
	for (int k=0; k<n; k++)
		z[k] = pD->GetLumpedElement(0)->m_Amps[k];
}

void CData::LoadVint2(CProblem* pProb, int n, double z[])
{
	Set("#V", "Shunt #2 Voltage", "V2", LINEAR, TIME_ARRAY);
	if (n==0) return;
	CDevice *pD = pProb->GetDevice();
	for (int k=0; k<n; k++)
		z[k] = pD->GetLumpedElement(1)->m_Volts[k];
}

void CData::LoadIint2(CProblem* pProb, int n, double z[])
{
	Set("#A", "Shunt #2 Current", "I2", LINEAR, TIME_ARRAY);
	if (n==0) return;
	CDevice *pD = pProb->GetDevice();
	for (int k=0; k<n; k++)
		z[k] = pProb->GetDevice()->GetLumpedElement(1)->m_Amps[k];
}

void CData::LoadVint3(CProblem* pProb, int n, double z[])
{
	Set("#V", "Shunt #3 Voltage", "V3", LINEAR, TIME_ARRAY);
	if (n==0) return;
	CDevice *pD = pProb->GetDevice();
	for (int k=0; k<n; k++)
		z[k] = pD->GetLumpedElement(2)->m_Volts[k];
}

void CData::LoadIint3(CProblem* pProb, int n, double z[])
{
	Set("#A", "Shunt #3 Current", "I3", LINEAR, TIME_ARRAY);
	if (n==0) return;
	CDevice *pD = pProb->GetDevice();
	for (int k=0; k<n; k++)
		z[k] = pD->GetLumpedElement(2)->m_Amps[k];
}

void CData::LoadVint4(CProblem* pProb, int n, double z[])
{
	Set("#V", "Shunt #4 Voltage", "V4", LINEAR, TIME_ARRAY);
	if (n==0) return;
	CDevice *pD = pProb->GetDevice();
	for (int k=0; k<n; k++)
		z[k] = pD->GetLumpedElement(3)->m_Volts[k];
}

void CData::LoadIint4(CProblem* pProb, int n, double z[])
{
	Set("#A", "Shunt #4 Current", "I4", LINEAR, TIME_ARRAY);
	if (n==0) return;
	CDevice *pD = pProb->GetDevice();
	for (int k=0; k<n; k++)
		z[k] = pD->GetLumpedElement(3)->m_Amps[k];
}

void CData::LoadError(CProblem* pProb, int n, double z[])
{
	Set("", "Convergence Error", "Error", LOG, SPACE_ARRAY);
	if (n==0) return;
	CSolve *pS = pProb->GetSolve();
	for (int k=0; k<n; k++){
		z[k]=pS->GetLastErrorAtNode(k);
	}
}

void CData::LoadNratio(CProblem* pProb, int n, double z[])
{
	Set("", "Excess Electron Density Ratio", "n'/eq", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode* pN = pProb->GetNode();
	for (int k=0; k<n; k++)
	{
		double neq = pN->GetNeq_nir(k);
		z[k]= (pN->GetN_nir(k)-neq)/neq;
//		z[k] = CMath::exp(pN->Psi_eq[k]+pN->Vn[k]);	// neq
//		z[k] = (CMath::exp(pN->Psi[k]+pN->Vn[k]-pN->Phi_n[k]) - z[k])/z[k];
	}
}

void CData::LoadPratio(CProblem* pProb, int n, double z[])
{
	Set("", "Excess Hole Density Ratio", "p'/eq", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode* pN = pProb->GetNode();
	for (int k=0; k<n; k++)
	{
		double peq = pN->GetPeq_nir(k);
		z[k] = (pN->GetP_nir(k) - peq) / peq;
//		z[k] = CMath::exp(-pN->Psi_eq[k]+pN->Vp[k]);	// peq
//		z[k] = (CMath::exp(-pN->Psi[k]+pN->Vp[k]+pN->Phi_p[k]) - z[k])/z[k];
	}
}

void CData::LoadPNratio(CProblem* pProb, int n, double z[])
{
	Set("", "Excess pn Product Ratio", "pn'/eq", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode* pN = pProb->GetNode();
	for (int k=0; k<n; k++)
	{
//		z[k] = CMath::exp(pN->Phi_p[k]-pN->Phi_n[k]) - 1;
		z[k] = pN->GetPNratio(k);
	}
}

void CData::LoadPNnorm(CProblem* pProb, int n, double z[])
{
	Set("", "Excess Normalized pn Product", "pn*", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode* pN = pProb->GetNode();
	double zmax = 0;
	for (int k=0; k<n; k++)
	{
//		z[k] = CMath::exp(pN->Phi_p[k]-pN->Phi_n[k]) - 1;
		z[k] = pN->GetPNratio(k);
		if (fabs(z[k])>zmax) zmax = fabs(z[k]);
	}
	if (zmax>0) for (int k=0; k<n; k++) z[k] /= zmax;
}

void CData::LoadNdrift(CProblem* pProb, int n, double z[])
{
	Set("#A/cm2", "Electron Drift Current Density", "Jn-drift", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double nir = pProb->GetNir();
	// Get E and multiply by q*mu*n to get Jdrift
	for (int k=0; k<n; k++)
	{
		z[k] = pN->GetE_Vt(k) * Q * nir * pN->GetN_nir(k) *
			pN->GetMun_includinghighfieldeffect(k) / pN->GetArea(k);
	} 
}

void CData::LoadPdrift(CProblem* pProb, int n, double z[])
{
	Set("#A/cm2", "Hole Drift Current Density", "Jp-drift", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double nir = pProb->GetNir();
	// Jdrift = q*E*mu*p
	for (int k=0; k<n; k++)
	{
		z[k] = pN->GetE_Vt(k) * Q * nir * pN->GetP_nir(k) *
			pN->GetMup_includinghighfieldeffect(k) / pN->GetArea(k);
	} 
}

void CData::LoadNdiff(CProblem* pProb, int n, double z[])
{
	Set("#A/cm2", "Electron Diff. Current Density", "Jn-diff", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double nir = pProb->GetNir();
	LoadNdrift(pProb, n, z);
	Set("A/cm2", "Electron Diff. Current Density", "Jn-diff", LINEAR, SPACE_ARRAY);
	for (int k=0; k<n; k++)
		z[k] = Q*nir*pN->GetI_n(k)/pN->GetArea(k) - z[k];
}

void CData::LoadPdiff(CProblem* pProb, int n, double z[])
{
	Set("#A/cm2", "Hole Diff. Current Density", "Jp-diff", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	double nir = pProb->GetNir();
	LoadPdrift(pProb, n, z);
	Set("A/cm2", "Hole Diff. Current Density", "Jp-diff", LINEAR, SPACE_ARRAY);
	//  Jdiff = Jtot - Jdrift
	//   = q* (Ip - E*mu*Area*p)/Area
	for (int k=0; k<n; k++)
//		z[k] = Q*pProb->nir*pN->I_p[k]/pN->Area[k] - z[k];
		z[k] = Q*nir*pN->GetI_p(k)/pN->GetArea(k) - z[k];
}

void CData::LoadExperimental(CProblem* , int n, double z[], double x[])
{
	Set("", "Experimental Data", "Expt", LINEAR, EXPERIMENTAL_ARRAY);
	if (n==0) return;
	int k;
	if (m_szExptData==0||x==NULL) {	for (k=0; k<n; k++) z[k]=0; return;	}
	for (k=0; k<n; k++)
		z[k]=CMath::LinearInterp(x[k], 
						m_szExptData, m_ExptX, m_ExptY);
}


#if 0
/****
void CData::LoadRbulk(CProblem* pProb, int n, double z[])
{
	Set("cm-3/s", "Bulk Recombination Rate", "Bulk Recom.", LOG, SPACE_ARRAY);
	if (n==0) return;
	CNode* pN = &pProb->pS->node;
	double n_, p_, nie, nir2;
	int reg;
	for (int k=0; k<n; k++)
	{
//		z[k]=pProb->nir*pProb->pS->GetBulkRecombination_nir(k);
		n_ = pProb->nir * pN->GetN_nir(k);
		p_ = pProb->nir * pN->GetP_nir(k);
		nie= pProb->nir * pN->GetNie_nir(k);
		nir2 = pow(pProb->nir, -2);
		reg = pN->GetRegion(k);
		if (pProb->pS->m_HurkxEnable[reg]) {
			z[k] = CPhysics::Bulk_Recomb_Hurkx(n_, p_, nie,
				pN->GetTaun(k), pN->GetTaup(k), pProb->pS->m_Et[reg],
				pProb->nir*pN->GetNd(k), pProb->nir*pN->GetNa(k), 
				nir2*pProb->pS->m_Cn[reg],
				nir2*pProb->pS->m_Cp[reg], 
				nir2*pProb->pS->m_Cnp[reg],
				pProb->pS->m_B[reg]/pProb->nir,
				pN->GetE_Vt(k), pProb->pS->m_HurkxPrefactor[reg], pProb->pS->m_HurkxFgamma_Vt[reg]); 
		} else {
			z[k] = CPhysics::Bulk_Recomb(n_, p_, nie,
				pN->GetTaun(k), pN->GetTaup(k), pProb->pS->m_Et[reg],
				pProb->nir*pN->GetNd(k), pProb->nir*pN->GetNa(k), 
				nir2*pProb->pS->m_Cn[reg],
				nir2*pProb->pS->m_Cp[reg], 
				nir2*pProb->pS->m_Cnp[reg],
				pProb->pS->m_B[reg]/pProb->nir);
		}
	}
}

void CData::LoadRcum(CProblem* pProb, int n, double z[])
{
	Set("s-1", "Cumulative Recombination", "Cum.Recom.", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode* pN = &pProb->pS->node;
	double n_, p_, nie, RecC, RecL;
	double nir = pProb->nir;
	int reg;
	RecL = 0;
	for (int k=0; k<n; k++)
	{
		n_ =  nir * pN->GetN_nir(k);
		p_ =  nir * pN->GetP_nir(k);
		nie = nir * pN->GetNie_nir(k);

//		n_ = pProb->nir*CMath::exp(pN->Psi[k]+pN->Vn[k]-pN->Phi_n[k]);
//		p_ = pProb->nir*CMath::exp(-pN->Psi[k]+pN->Vp[k]+pN->Phi_p[k]);
//		nie = pProb->nir*CMath::exp((pN->Vn[k]+pN->Vp[k])/2);
		reg = pN->GetRegion(k);
		z[k] = 0;
		if (pN->GetX(k)==pProb->pS->m_xRegionFront[reg])
			z[k] += (pProb->Aref*pN->GetArea(k)
				*CPhysics::Surf_Recomb(n_, p_, nie, 
				pProb->pS->m_SnFront[reg], pProb->pS->m_SpFront[reg],
				pProb->pS->m_EtFront[reg], pProb->pS->m_JoFront[reg]));
		if (pN->GetX(k)==pProb->pS->m_xRegionRear[reg])
			z[k] += (pProb->Aref*pN->GetArea(k)
				*CPhysics::Surf_Recomb(n_, p_, nie, 
				pProb->pS->m_SnRear[reg], pProb->pS->m_SpRear[reg],
				pProb->pS->m_EtRear[reg], pProb->pS->m_JoRear[reg]));
		if (pProb->pS->m_HurkxEnable[reg]) {
		RecC = CPhysics::Bulk_Recomb_Hurkx(n_, p_, nie,
				pN->GetTaun(k), pN->GetTaup(k), pProb->pS->m_Et[reg],
				pProb->nir*pN->GetNd(k), pProb->nir*pN->GetNa(k), 
				pow(pProb->nir,-2)*pProb->pS->m_Cn[reg],
				pow(pProb->nir,-2)*pProb->pS->m_Cp[reg], 
				pow(pProb->nir,-2)*pProb->pS->m_Cnp[reg],
				pProb->pS->m_B[reg]/pProb->nir,
				pN->GetE_Vt(k), pProb->pS->m_HurkxPrefactor[reg], pProb->pS->m_HurkxFgamma_Vt[reg]); 
		} else {
			RecC = CPhysics::Bulk_Recomb(n_, p_, nie,
				pN->GetTaun(k), pN->GetTaup(k), pProb->pS->m_Et[reg],
				pProb->nir*pN->GetNd(k), pProb->nir*pN->GetNa(k), 
				pow(pProb->nir,-2)*pProb->pS->m_Cn[reg],
				pow(pProb->nir,-2)*pProb->pS->m_Cp[reg], 
				pow(pProb->nir,-2)*pProb->pS->m_Cnp[reg],
				pow(pProb->nir,-1)*pProb->pS->m_B[reg]);
		}
		if (k>0)
			z[k] += pProb->Aref*pN->GetArea(k)*(RecC+RecL)/2
					* pN->Get_dxR(k-1); // (pN->x[k]-pN->x[k-1]);
		RecL = RecC;
	}
	for (k=1; k<n; k++) z[k] += z[k-1];
}

***/
/***

  void CData::LoadTau(CProblem* pProb, int n, double z[])
{
	Set("#s", "Minority Carrier Lifetime", "Tau", LOG, SPACE_ARRAY);
	if (n==0) return;
	CNode* pN = &pProb->pS->node;
	double n_, p_, neq, peq, nir2, nie;
	int reg;
	for (int k=0; k<n; k++)
	{
		n_ = pProb->nir * pN->GetN_nir(k);
		p_ = pProb->nir * pN->GetP_nir(k);
		neq = pProb->nir * pN->GetNeq_nir(k);
		peq = pProb->nir * pN->GetPeq_nir(k);
//		neq = pProb->nir*CMath::exp(pN->Psi_eq[k]+pN->Vn[k]);
//		peq = pProb->nir*CMath::exp(-pN->Psi_eq[k]+pN->Vp[k]);
		nie = sqrt(neq*peq);
		nir2 = pow(pProb->nir,-2);
		reg = pN->GetRegion(k);


		if (n_>p_) {
			if (p_==peq) p_ = 1.01*peq;
		}else {
			if (n_==neq) n_ = 1.01*neq;
		}

		double bulkrec;
		if (pProb->pS->m_HurkxEnable[reg]) {
			bulkrec = CPhysics::Bulk_Recomb_Hurkx(n_,p_,nie,
				pN->GetTaun(k), pN->GetTaup(k), pProb->pS->m_Et[reg],
				pProb->nir*pN->GetNd(k), pProb->nir*pN->GetNa(k), 
				nir2*pProb->pS->m_Cn[reg],
				nir2*pProb->pS->m_Cp[reg], 
				nir2*pProb->pS->m_Cnp[reg],
				1/pProb->nir*pProb->pS->m_B[reg],
				pN->GetE_Vt(k), pProb->pS->m_HurkxPrefactor[reg], pProb->pS->m_HurkxFgamma_Vt[reg]); 
		} else {
			bulkrec = CPhysics::Bulk_Recomb(n_,p_,nie,
				pN->GetTaun(k), pN->GetTaup(k), pProb->pS->m_Et[reg],
				pProb->nir*pN->GetNd(k), pProb->nir*pN->GetNa(k), 
				nir2*pProb->pS->m_Cn[reg],
				nir2*pProb->pS->m_Cp[reg], 
				nir2*pProb->pS->m_Cnp[reg],
				1/pProb->nir*pProb->pS->m_B[reg]);
		}

		if (n_>p_) {
			z[k]=(p_-peq)/bulkrec;
		} else if (p_>n_) {
			z[k]=(n_-neq)/bulkrec;
		} else z[k]=0;
		if (z[k]<0) z[k] = 0;
	}
}

void CData::LoadLd(CProblem* pProb, int n, double z[])
{
	Set("#m", "Diffusion Length", "Ld", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode* pN = &pProb->pS->node;
	double n_, p_, neq, peq, nie, nir2, tmp1, mu_times_n, mu_times_p;
	int reg;
	for (int k=0; k<n; k++)
	{
		n_ = pProb->nir* pN->GetN_nir(k);
		p_ = pProb->nir* pN->GetP_nir(k);
		neq = pProb->nir* pN->GetNeq_nir(k);
		peq = pProb->nir* pN->GetPeq_nir(k);
//		n_ = pProb->nir*CMath::exp(pN->Psi[k]-pN->Phi_n[k]+pN->Vn[k]);
//		p_ = pProb->nir*CMath::exp(-pN->Psi[k]+pN->Phi_p[k]+pN->Vp[k]);
//		neq = pProb->nir*CMath::exp(pN->Psi_eq[k]+pN->Vn[k]);
//		peq = pProb->nir*CMath::exp(-pN->Psi_eq[k]+pN->Vp[k]);
		nie = sqrt(neq*peq);
		nir2 = pow(pProb->nir,-2);
		reg = pN->GetRegion(k);
		if (n_>p_)
		{
			if (p_==peq) p_ = 1.01*peq;
//			z[k] = CMath::exp(pN->Up[k]-pN->Vp[k])/pN->Area[k];	// low-field Dp
			z[k] = pN->GetMup(k)/pN->GetArea(k); // low-field Dp
			if (pN->GetEsat_p(k)>0) {	// correct for high-field effect
				mu_times_p = pN->GetMu_times_P(k);  // CMath::exp(-pN->Psi[k]+pN->Up[k]+pN->Phi_p[k]);
				tmp1 = (pN->GetI_p(k)/mu_times_p) / pN->GetEsat_p(k);
				z[k] /= sqrt(1 + tmp1*tmp1);
			}
			if (pProb->pS->m_HurkxEnable[reg]) {
				z[k] *= (p_-peq)
				/CPhysics::Bulk_Recomb_Hurkx(n_,p_,nie,
					pN->GetTaun(k), pN->GetTaup(k), pProb->pS->m_Et[reg],
					pProb->nir*pN->GetNd(k), pProb->nir*pN->GetNa(k), 
					nir2*pProb->pS->m_Cn[reg],
					nir2*pProb->pS->m_Cp[reg], 
					nir2*pProb->pS->m_Cnp[reg],
					1/pProb->nir*pProb->pS->m_B[reg],
					pN->GetE_Vt(k), pProb->pS->m_HurkxPrefactor[reg], pProb->pS->m_HurkxFgamma_Vt[reg]); 
			} else {
				z[k] *= (p_-peq)
				/CPhysics::Bulk_Recomb(n_,p_,nie,
					pN->GetTaun(k), pN->GetTaup(k), pProb->pS->m_Et[reg],
					pProb->nir*pN->GetNd(k), pProb->nir*pN->GetNa(k), 
					nir2*pProb->pS->m_Cn[reg],
					nir2*pProb->pS->m_Cp[reg], 
					nir2*pProb->pS->m_Cnp[reg],
					1/pProb->nir*pProb->pS->m_B[reg]);
			}
		}
		else if (p_>n_)
		{
			if (n_==neq) n_ = 1.01*neq;
//			z[k] = CMath::exp(pN->Un[k]-pN->Vn[k])/pN->Area[k];	// low-field Dn
			z[k] = pN->GetMun(k) / pN->GetArea(k); // low-field Dn
			if (pN->GetEsat_n(k)>0) {	// correct for high-field effect
				mu_times_n = pN->GetMu_times_N(k); // CMath::exp(pN->Psi[k]+pN->Un[k]-pN->Phi_n[k]);
				tmp1 = (pN->GetI_n(k)/mu_times_n) / pN->GetEsat_n(k);
				z[k] /= sqrt(1 + tmp1*tmp1);
			}
			if (pProb->pS->m_HurkxEnable[reg]) {
				z[k] *= (n_-neq)/CPhysics::Bulk_Recomb_Hurkx(n_,p_,nie,
					pN->GetTaun(k), pN->GetTaup(k), pProb->pS->m_Et[reg],
					pProb->nir*pN->GetNd(k), pProb->nir*pN->GetNa(k), 
					nir2*pProb->pS->m_Cn[reg],
					nir2*pProb->pS->m_Cp[reg], 
					nir2*pProb->pS->m_Cnp[reg],
					1/pProb->nir*pProb->pS->m_B[reg],
					pN->GetE_Vt(k), pProb->pS->m_HurkxPrefactor[reg], pProb->pS->m_HurkxFgamma_Vt[reg]); 
			} else {
				z[k] *= (n_-neq)/CPhysics::Bulk_Recomb(n_,p_,nie,
					pN->GetTaun(k), pN->GetTaup(k), pProb->pS->m_Et[reg],
					pProb->nir*pN->GetNd(k), pProb->nir*pN->GetNa(k), 
					nir2*pProb->pS->m_Cn[reg],
					nir2*pProb->pS->m_Cp[reg], 
					nir2*pProb->pS->m_Cnp[reg],
					1/pProb->nir*pProb->pS->m_B[reg]);
			}
		} else z[k] = 0;
		if (z[k]<0) z[k] = 0;
	}
	for (k=0; k<n; k++) if (z[k]>0) z[k] = (1e-2*sqrt(z[k])); else z[k] = 0;
}

void CData::LoadG(CProblem* pProb, int n, double z[])
{
	Set("cm-3/s", "Generation Rate","Gen.Rate", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode* pN = &pProb->pS->node;
	for (int k=0; k<n; k++) 
		z[k] = pN->GetGenerationRate(k) * pProb->nir/pN->GetArea(k);
#if FALSE
	z[0] = ((pN->G[1]-pN->G[0])/(pN->x[1]-pN->x[0]));
	for (int k=1; k<n-1; k++)
		z[k] = 
			  ((pN->G[k+1]-pN->G[k])/(pN->x[k+1]-pN->x[k])
				*(pN->x[k]-pN->x[k-1])/(pN->x[k+1]-pN->x[k-1])
			 + (pN->G[k]-pN->G[k-1])/(pN->x[k]-pN->x[k-1])
				*(pN->x[k+1]-pN->x[k])/(pN->x[k+1]-pN->x[k-1]));
	z[n-1] = ((pN->G[n-1]-pN->G[n-2])/(pN->x[n-1]-pN->x[n-2]));
	for (k=0; k<n; k++) z[k] *= pProb->nir/pN->Area[k];
#endif
}
****/
#endif

#if 0
/********
void CData::LoadNdrift(CProblem* pProb, int n, double z[])
{
	Set("#A/cm2", "Electron Drift Current Density", "Jn-drift", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode* pN = &pProb->pS->node;
	double mu, tmp1, mu_times_n;
	LoadE(pProb, n, z);
	Set("#A/cm2", "Electron Drift Current Density", "Jn-drift", LINEAR, SPACE_ARRAY);
	// Now, multiply by q*mu*n to get Jdrift
	for (int k=0; k<n; k++)
	{
//		mu = CMath::exp(pN->Un[k]-pN->Vn[k])/pN->Area[k]/pProb->Vt;	// low-field mobility
		mu = pN->GetMun_times_Vt(k)/pProb->Vt;	// low-field mobility
		if (pN->GetEsat_n(k)>0) {	// correct for high-field effect
//			mu_times_n = CMath::exp(pN->Psi[k]+pN->Un[k]-pN->Phi_n[k]);
			mu_times_n = pN->GetMu_times_N(k);
			tmp1 = (pN->GetI_n(k)/mu_times_n) / pN->GetEsat_n(k);
			mu /= sqrt(1 + tmp1*tmp1);
			}
//		z[k] *= Q*mu*pProb->nir*CMath::exp(pN->Psi[k]+pN->Vn[k]-pN->Phi_n[k]);
		z[k] *= Q*mu*pProb->nir*pN->GetN_nir(k);
	} 
}

void CData::LoadPdrift(CProblem* pProb, int n, double z[])
{
	Set("#A/cm2", "Hole Drift Current Density", "Jp-drift", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode* pN = &pProb->pS->node;
	double mu, tmp1, mu_times_p;
	LoadE(pProb, n, z);
	Set("A/cm2", "Hole Drift Current Density", "Jp-drift", LINEAR, SPACE_ARRAY);
	// Now, multiply by q*mu*p to get Jdrift
	for (int k=0; k<n; k++)
	{
//		mu = CMath::exp(pN->Up[k]-pN->Vp[k])/pN->Area[k]/pProb->Vt;	// low-field mobility
		mu = pN->GetMup_times_Vt(k)/pProb->Vt;	// low-field mobility
		if (pN->GetEsat_p(k)>0) {	// correct for high-field effect
//			mu_times_p = CMath::exp(-pN->Psi[k]+pN->Up[k]+pN->Phi_p[k]);
			mu_times_p = pN->GetMu_times_P(k);
			tmp1 = (pN->GetI_p(k)/mu_times_p) / pN->GetEsat_p(k);
			mu /= sqrt(1 + tmp1*tmp1);
			}
//		z[k] *= Q*mu*pProb->nir*CMath::exp(-pN->Psi[k]+pN->Vp[k]+pN->Phi_p[k]);
		z[k] *= Q*mu*pProb->nir*pN->GetP_nir(k);
	} 
}

void CData::LoadAbsLp(CProblem* pProb, int n, double z[])
{
	Set("#m", "Pri-Surface Absorption Length", "P/Abs Length", LINEAR, TIME_ARRAY);
	if (n==0) return;
	CMaterial* pM;
	CExcite *pE=pProb->GetExcite();
	double absorb = 0;
	int reg;
//	int SaveRegion = pProb->pD->m_CurrentRegion;
	if (pE->m_LightPri.m_Back) reg=pProb->GetDevice()->m_NumRegions-1; else reg=0;
//	pProb->pD->SelectRegion(reg); pM=pProb->pD->GetCurrentMaterial();
	pM=pProb->GetDevice()->GetMaterialForRegion(reg);
	for (int k=0; k<n; k++) if (pE->m_LightPri.m_Lambda[k]>0)
	{
		double lambda = pE->m_LightPri.m_Lambda[k];
		if (pM->m_AbsExternal)
			absorb = CMath::LogInterp(lambda, pM->m_nAbsorb, 
						pM->m_AbsLambda, pM->m_Absorption);
		else
		    absorb =  
	    	CPhysics::Absorption(lambda, pM->m_AbsEd1, pM->m_AbsEd2, 
	    		pM->m_AbsEi1, pM->m_AbsEi2, pM->m_AbsEp1, pM->m_AbsEp2, 
	    		pM->m_AbsAd1, pM->m_AbsAd2, pM->m_AbsA11, pM->m_AbsA12,
	    		pM->m_AbsA21, pM->m_AbsA22, pM->m_AbsTcoeff, pM->m_AbsToffset, pProb->GetT());
		if (absorb>0) z[k] = 1e-2/absorb;
	}
 //	pProb->pD->SelectRegion(SaveRegion);
}

*****/
/****
void CData::LoadRcum(CProblem* pProb, int n, double z[])
{
	Set("s-1", "Cumulative Recombination", "Cum.Recom.", LINEAR, SPACE_ARRAY);
	if (n==0) return;
	CNode *pN = pProb->GetNode();
	CRegionData *pRegData= pProb->GetRegionData();
	double nir = pProb->GetNir();
	double n_, p_, nie, RecC, RecL;
	int reg;
	RecL = 0;
	for (int k=0; k<n; k++)
	{
		n_ =  nir * pN->GetN_nir(k);
		p_ =  nir * pN->GetP_nir(k);
		nie = nir * pN->GetNie_nir(k);

		reg = pN->GetRegion(k);
		z[k] = 0;
		if (pN->GetX(k)==pProb->GetSolve()->m_xRegionFront[reg]) 
			z[k] += (pProb->GetAref()*pN->GetArea(k)
				*CPhysics::Surf_Recomb(n_, p_, nie, 
				pProb->GetSolve()->m_SnFront[reg], pProb->GetSolve()->m_SpFront[reg],
				pProb->GetSolve()->m_EtFront[reg], pProb->GetSolve()->m_JoFront[reg]));
		if (pN->GetX(k)==pProb->GetSolve()->m_xRegionRear[reg])
			z[k] += (pProb->GetAref()*pN->GetArea(k)
				*CPhysics::Surf_Recomb(n_, p_, nie, 
				pProb->GetSolve()->m_SnRear[reg], pProb->GetSolve()->m_SpRear[reg],
				pProb->GetSolve()->m_EtRear[reg], pProb->GetSolve()->m_JoRear[reg]));

		RecC = nir * pProb->GetSolve()->GetBulkRecombination_nir(k);

		if (k>0)
			z[k] += pProb->GetAref()*pN->GetArea(k)*(RecC+RecL)/2
					* pN->Get_dxR(k-1); // (pN->x[k]-pN->x[k-1]);
		RecL = RecC;
	}
	for (k=1; k<n; k++) z[k] += z[k-1];
}
***/

#endif