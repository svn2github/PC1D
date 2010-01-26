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
#if !defined(PLOTLOAD_HEADER)
#define PLOTLOAD_HEADER 1                                                             
// aim: reduce all obtaining of graph data to a single call to CGraph:SetPredefinedGraph
// avoid references to graphable quanties except where absolutely necessary.
// See plotload.cpp for notes on how to add extra graphable quantities and/or graph types

class CProblem;

enum GraphableQuantity {
DataNothing,
DataAux=1, DataX, DataPsi, DataNd, DataNa, DataN, DataP, DataE, DataRho, DataJn, DataJp, DataJt,
DataGcum, DataEg, DataNie, DataTau, DataRbulk, DataRcum, DataG, DataVn, DataVp,
DataMuN, DataMuP, DataN_, DataP_, DataPsi_, DataRho_, DataPhiN, DataPhiP,
DataEc, DataEv, DataEvac, 
DataRes, DataCond, DataCcum, DataCond_, DataCcum_, DataLd, DataIn, DataIp, DataIt, DataArea,
DataPerm, DataT, DataVb, DataIb, DataVc, DataIc, DataPb, DataPc,
DataWp, DataEQEp, DataIQEp, DataRflp, DataEscp, DataInvIQEp, DataAbsLp, DataIndxRp,
DataVint1, DataIint1, DataVint2, DataIint2, DataVint3, DataIint3, DataVint4, DataIint4,
DataError, DataNratio, DataPratio, DataPNratio, DataPNnorm,
DataNdrift, DataNdiff, DataPdrift, DataPdiff,
DataExperimental, DataBiasedIQEp, DataTotalPriReflectance,
// add extra graphable quantities in here.
DataLastGraphableQuantity 
};

class CData : public CObject
{
	friend class CGraph;

private:
	static GraphableQuantity m_AuxDataQuantity;    // used by SetAuxDataValue()
	static double	m_AuxPosition;	// cm, distance from front for Aux Data
	static int m_szExptData;	// number of points in the ExptData arrays.
	static double m_ExptX[];
	static double m_ExptY[];
	static CString m_ExptFilename;
public:
	CString units, label;
	CString abbreviation;
	CScaleType scale;
	CArrayType type;

// Construction/destruction
public:
	CData();		// Null initialization of member variables
	CData & operator =(CData &da);

// Attributes
public:
	static CString GraphableQuantity2Text(GraphableQuantity g);
	static GraphableQuantity Text2GraphableQuantity(CString name);
	static GraphableQuantity GetFirstQuantity() { return (GraphableQuantity)1; };
	static GraphableQuantity GetNextQuantity(GraphableQuantity g) { return (g<DataLastGraphableQuantity-1)? (GraphableQuantity)(1+(int)g):(GraphableQuantity)0; };
	static void SerializeGraphableQuantity(CArchive& ar, GraphableQuantity &g);
	static CArrayType GetType(GraphableQuantity g);
	static CScaleType GetScale(GraphableQuantity g);
	static CString GetUnits(GraphableQuantity g);
	static CString GetLabel(GraphableQuantity g);

// Operations
public:
	static void LoadQuantityIntoArray(CProblem* pDoc, int n, double z[], GraphableQuantity whichquantity);
	static	BOOL AskUserToDefineAuxiliary(CWnd* pParent = CWnd::GetActiveWindow());
	static void OnGraphDefineExperimental(void);
	static void SetAuxDataValue(CProblem *pProb);

// Implementation
private:
	void Set(CString sunits, CString slabel, CString abbrev, CScaleType e1, CArrayType e2);
	void LoadQuantity(CProblem* pDoc, int n, double z[], GraphableQuantity whichquantity, double x[]=NULL); 
private:                                             
    void LoadZero(CProblem *pProb,int,double[]);                                         
	void LoadAux(CProblem *pProb,int,double[]);
	void LoadX(CProblem *pProb,int,double[]);
	void LoadPsi(CProblem *pProb,int,double[]);
	void LoadNd(CProblem *pProb,int,double[]);
	void LoadNa(CProblem *pProb,int,double[]); 
	void LoadN(CProblem *pProb,int,double[]);
	void LoadP(CProblem *pProb,int,double[]);
	void LoadE(CProblem *pProb,int,double[]);
	void LoadRho(CProblem *pProb,int,double[]);
	void LoadJn(CProblem *pProb,int,double[]);
	void LoadJp(CProblem *pProb,int,double[]);
	void LoadJt(CProblem *pProb,int,double[]);
    void LoadGcum(CProblem *pProb,int,double[]);
    void LoadEg(CProblem *pProb,int,double[]);
    void LoadNie(CProblem *pProb,int,double[]);
    void LoadTau(CProblem *pProb,int,double[]);
    void LoadRbulk(CProblem *pProb,int,double[]);
    void LoadRcum(CProblem *pProb,int,double[]);
    void LoadG(CProblem *pProb,int,double[]);
    void LoadVn(CProblem *pProb,int,double[]);
    void LoadVp(CProblem *pProb,int,double[]);
    void LoadMuN(CProblem *pProb,int,double[]);
    void LoadMuP(CProblem *pProb,int,double[]);
    void LoadN_(CProblem *pProb,int,double[]);
    void LoadP_(CProblem *pProb,int,double[]);
    void LoadPsi_(CProblem *pProb,int,double[]);
    void LoadRho_(CProblem *pProb,int,double[]);
    void LoadPhiN(CProblem *pProb,int,double[]);
    void LoadPhiP(CProblem *pProb,int,double[]);
    void LoadEc(CProblem *pProb,int,double[]);
    void LoadEv(CProblem *pProb,int,double[]);
    void LoadEvac(CProblem *pProb,int,double[]);
    void LoadRes(CProblem *pProb,int,double[]);
    void LoadCond(CProblem *pProb,int,double[]);
	void LoadCcum(CProblem *pProb,int,double[]);
	void LoadCond_(CProblem *pProb,int,double[]);
	void LoadCcum_(CProblem *pProb,int,double[]);
	void LoadLd(CProblem *pProb,int,double[]);
	void LoadIn(CProblem *pProb,int,double[]);
	void LoadIp(CProblem *pProb,int,double[]);
	void LoadIt(CProblem *pProb,int,double[]);
	void LoadArea(CProblem *pProb,int,double[]);
	void LoadPerm(CProblem *pProb,int,double[]);
	void LoadT(CProblem *pProb,int,double[]);
	void LoadVb(CProblem *pProb,int,double[]);
	void LoadIb(CProblem *pProb,int,double[]);
	void LoadVc(CProblem *pProb,int,double[]);
	void LoadIc(CProblem *pProb,int,double[]);
	void LoadPb(CProblem *pProb,int,double[]);
	void LoadPc(CProblem *pProb,int,double[]);
	void LoadWp(CProblem *pProb,int,double[]);
	void LoadEQEp(CProblem *pProb,int,double[]);
	void LoadIQEp(CProblem *pProb,int,double[]);
	void LoadRflp(CProblem *pProb,int,double[]);
	void LoadEscp(CProblem *pProb,int,double[]);
	void LoadInvIQEp(CProblem *pProb,int,double[]);
	void LoadAbsLp(CProblem *pProb,int,double[]);
	void LoadIndxRp(CProblem *pProb,int,double[]);
	void LoadVint1(CProblem *pProb,int,double[]);
	void LoadIint1(CProblem *pProb,int,double[]);
	void LoadVint2(CProblem *pProb,int,double[]);
	void LoadIint2(CProblem *pProb,int,double[]);
	void LoadVint3(CProblem *pProb,int,double[]);
	void LoadIint3(CProblem *pProb,int,double[]);
	void LoadVint4(CProblem *pProb,int,double[]);
	void LoadIint4(CProblem *pProb,int,double[]);
	void LoadError(CProblem *pProb,int,double[]);
	void LoadNratio(CProblem *pProb,int,double[]);
	void LoadPratio(CProblem *pProb,int,double[]);
	void LoadPNratio(CProblem *pProb,int,double[]);
	void LoadPNnorm(CProblem *pProb,int,double[]);
	void LoadNdrift(CProblem *pProb,int,double[]);
	void LoadNdiff(CProblem *pProb,int,double[]);
	void LoadPdrift(CProblem *pProb,int,double[]);
	void LoadPdiff(CProblem *pProb,int,double[]);
	void LoadExperimental(CProblem *pProb,int,double[], double x[]);
	void LoadBiasedIQEp(CProblem *pProb, int n, double z[]);
	void LoadTotalPriReflectance(CProblem *pProb, int n, double z[]);
};

enum GraphType {
CarrierDensities, ChargeDensity, CurrentDensity, DopingDensities,
ElectricField, ElectrostaticPotential,
Photogeneration, CarrierMobilities, CarrierVelocities, EnergyBands, 
Auxiliary, BaseCurrent, BaseIV, BaseIVPower, BaseVoltage, 
CollectorCurrent, CollectorIV, CollectorIVPower, CollectorVoltage, 
InternalElementI, InternalElementV, 
QuantumEff, DiffusionLength
// add extra graph types in here
};  

class CGraph : public CObject
{
public:
	CGraph();	// null-initializing constructor
	DECLARE_SERIAL(CGraph)

// Attributes
private:
	GraphableQuantity m_xquantity;
	GraphableQuantity m_yquantity[4];	
	int m_numpoints;	// number of points to be stored in the array
	CData pX, pY[4];
	CString title, xLabel, yLabel;
public:
	CScaleType xScale, yScale;
public:
	int NumberOfValidCurves(void);
	void GetXQuantity(CData &xquant) { xquant=pX; };
	void GetYQuantity(int curvenum, CData &yquant) { yquant=pY[curvenum]; };
	CString GetTitle() { return title; };
	CString GetXLabel() { return xLabel; };
	CString GetYLabel() { return yLabel; };
	int GetNumPoints() { return m_numpoints; };
	void SetNumPoints(int n) { m_numpoints=n; };

// Operations
public:
	void LoadQuantities(CProblem* pProb, int n, 
		double x[], double y0[], double y1[]=NULL, double y2[]=NULL, double y3[]=NULL);
	BOOL AskUserToRedefineGraph(CWnd* pParent = CWnd::GetActiveWindow());
	void SetPredefinedGraph(enum GraphType whichgraph);
	CGraph & operator =(CGraph &gr);
	friend BOOL operator ==(CGraph &first, CGraph &second);
	void Serialize(CArchive& ar);

	void SetQuantities(GraphableQuantity pXin, GraphableQuantity pY0, GraphableQuantity pY1=DataNothing,
			GraphableQuantity pY2=DataNothing, GraphableQuantity pY3=DataNothing);
// Implementation
private:      
	void Set(CString titleIn, CString xLabelIn, CString yLabelIn,
			 CScaleType xScaleIn, CScaleType yScaleIn);

};

#endif // PLOTLOAD_HEADER