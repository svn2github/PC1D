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

#include "Problem.h"

class CGraph;  // forward declaration

/////////////////////////////////////////////////////////////////////////////
// CPc1dDoc

class CPc1dDoc : public CDocument
{
protected: // create from serialization only
	CPc1dDoc();
	DECLARE_SERIAL(CPc1dDoc)	// was dyncreate

// Attributes
private:
	const CString m_INIheading, m_INIupdate;
public:
	CFont m_GraphFont, m_GraphFont90;	///< fonts for graphs
private:
	CProblem m_problem; ///< The problem we're solving
public:
	CProblem *GetProblem() { return &m_problem; };
protected:
	CDevice *GetDevice() { return GetProblem()->GetDevice(); };
	CExcite *GetExcitation() { return GetProblem()->GetExcite(); };
	CSolve *GetSolve() { return GetProblem()->GetSolve(); };
	CBatch *GetBatch() { return GetProblem()->GetBatch(); };
	CStatus *GetStatus() { return GetProblem()->GetStatus(); };
	CMaterial *GetCurrentMaterial() { return GetDevice()->GetCurrentMaterial(); };
	CRegion *GetCurrentRegion() { return GetDevice()->GetCurrentRegion(); };

	// Operations
public:
	BOOL DoNextPartOfCalculation(void);
	BOOL IsCalculationInProgress(void) { return GetProblem()->IsCalculationInProgress(); };
	void SwitchToInteractivePredefinedGraph(enum GraphType whichgraph);
	void SwitchToInteractiveGraph(CView *pView, CGraph *whichgraph);
	void SwitchToFourGraphsView(void);
	void UpdateRegionButton(void);

// Implementation
public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual BOOL OnNewDocument();
public:
	void FastUpdateStatusBar(void);
protected:
	void ExcModified();
	void DeviceModified();
// Generated message map functions
protected:
	// Status Bar updates not handled by Class Wizard
	afx_msg void OnUpdateState(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMode(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTime(CCmdUI* pCmdUI);
	afx_msg void OnUpdateProgress(CCmdUI* pCmdUI);

	//{{AFX_MSG(CPc1dDoc)
	afx_msg void OnExcMode();
	afx_msg void OnComputeRun();
	afx_msg void OnComputeNumerical();
	afx_msg void OnComputeStop();
	afx_msg void OnComputeSinglestep();
	afx_msg void OnComputeStartagain();
	afx_msg void OnComputeContinue();
	afx_msg void OnUpdateComputeStop(CCmdUI* pCmdUI);
	afx_msg void OnUpdateComputeRun(CCmdUI* pCmdUI);
	afx_msg void OnUpdateComputeSinglestep(CCmdUI* pCmdUI);
	afx_msg void OnUpdateComputeStartagain(CCmdUI* pCmdUI);
	afx_msg void OnUpdateComputeContinue(CCmdUI* pCmdUI);
	afx_msg void OnDeviceArea();
	afx_msg void OnDeviceThickness();
	afx_msg void OnExcitationNew();
	afx_msg void OnExcitationOpen();
	afx_msg void OnExcitationSaveas();
	afx_msg void OnExcBaseSource();
	afx_msg void OnExcCollSource();
	afx_msg void OnExcPhotoExternal();
	afx_msg void OnUpdateExcPhotoExternal(CCmdUI* pCmdUI);
	afx_msg void OnExcPriSpectrum();
	afx_msg void OnExcSecSpectrum();
	afx_msg void OnExcPriIntensity();
	afx_msg void OnExcSecIntensity();
	afx_msg void OnRegionNext();
	afx_msg void OnExcTemperature();
	afx_msg void OnDeviceNew();
	afx_msg void OnDeviceOpen();
	afx_msg void OnDeviceSaveas();
	afx_msg void OnDeviceMaterialMobilitiesVariable();
	afx_msg void OnUpdateDeviceMaterialMobilitiesVariable(CCmdUI* pCmdUI);
	afx_msg void OnDeviceMaterialMobilitiesFixed();
	afx_msg void OnUpdateDeviceMaterialMobilitiesFixed(CCmdUI* pCmdUI);
	afx_msg void OnMaterialNew();
	afx_msg void OnMaterialOpen();
	afx_msg void OnMaterialSaveas();
	afx_msg void OnDeviceMaterialPermittivity();
	afx_msg void OnDeviceMaterialBandstructure();
	afx_msg void OnDeviceMaterialBgnarrowingModel();
	afx_msg void OnMaterialParasiticAbsorption();
	afx_msg void OnMaterialIntrinsicAbsorption();
	afx_msg void OnDeviceRefractiveindex();
	afx_msg void OnDeviceDopingBackground();
	afx_msg void OnDeviceDopingDiffusionsFirstfront();
	afx_msg void OnDeviceDopingDiffusionsFirstrear();
	afx_msg void OnDeviceDopingDiffusionsSecondfront();
	afx_msg void OnDeviceDopingDiffusionsSecondrear();
	afx_msg void OnUpdateDeviceDopingDiffusionsFirstfront(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDeviceDopingDiffusionsFirstrear(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDeviceDopingDiffusionsSecondfront(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDeviceDopingDiffusionsSecondrear(CCmdUI* pCmdUI);
	afx_msg void OnDeviceRecombinationBulk();
	afx_msg void OnDeviceRecombinationFrontsurface();
	afx_msg void OnDeviceRecombinationRearsurface();
	afx_msg void OnUpdateDeviceRecombinationBulk(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDeviceRecombinationFrontsurface(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDeviceRecombinationRearsurface(CCmdUI* pCmdUI);
	afx_msg void OnReflectanceFront();
	afx_msg void OnReflectanceRear();
	afx_msg void OnDeviceReflectanceInternal();
	afx_msg void OnDeviceTexture();
	afx_msg void OnDeviceSurfacesFront();
	afx_msg void OnDeviceSurfacesRear();
	afx_msg void OnDeviceCircuitContacts();
	afx_msg void OnDeviceCircuitInternalelements();
	afx_msg void OnUpdateDeviceCircuitInternalelements(CCmdUI* pCmdUI);
	afx_msg void OnDeviceDopingFrontExternal();
	afx_msg void OnUpdateDeviceDopingFrontExternal(CCmdUI* pCmdUI);
	afx_msg void OnDeviceDopingRearExternal();
	afx_msg void OnUpdateDeviceDopingRearExternal(CCmdUI* pCmdUI);
	afx_msg void OnOptionsStorenodedata();
	afx_msg void OnUpdateOptionsStorenodedata(CCmdUI* pCmdUI);
	afx_msg void OnOptionsDeviceupdate();
	afx_msg void OnUpdateOptionsDeviceupdate(CCmdUI* pCmdUI);
	afx_msg void OnFileDescription();
	afx_msg void OnGraphSpatialCarrierdensities();
	afx_msg void OnGraphSpatialChargedensity();
	afx_msg void OnGraphSpatialCurrentdensity();
	afx_msg void OnGraphSpatialDopingdensities();
	afx_msg void OnGraphSpatialElectricfield();
	afx_msg void OnGraphSpatialElectrostaticpotential();
	afx_msg void OnGraphSpatialPhotogeneration();
	afx_msg void OnGraphSpatialCarriermobilities();
	afx_msg void OnGraphSpatialCarriervelocities();
	afx_msg void OnGraphSpatialEnergybands();
	afx_msg void OnGraphDefined();
	afx_msg void OnGraphAux();
	afx_msg void OnGraphTemporalAuxiliary();
	afx_msg void OnGraphTemporalBasecurrent();
	afx_msg void OnGraphTemporalBaseiv();
	afx_msg void OnGraphTemporalBaseivpower();
	afx_msg void OnGraphTemporalBasevoltage();
	afx_msg void OnGraphTemporalCollectorcurrent();
	afx_msg void OnGraphTemporalCollectoriv();
	afx_msg void OnGraphTemporalCollectorivpower();
	afx_msg void OnGraphTemporalCollectorvoltage();
	afx_msg void OnGraphTemporalQuantumeff();
	afx_msg void OnUpdateGraphTemporalQuantumeff(CCmdUI* pCmdUI);
	afx_msg void OnGraphTemporalShuntelementcurrent();
	afx_msg void OnUpdateGraphTemporalShuntelementcurrent(CCmdUI* pCmdUI);
	afx_msg void OnGraphTemporalShuntelementvoltage();
	afx_msg void OnUpdateGraphTemporalShuntelementvoltage(CCmdUI* pCmdUI);
	afx_msg void OnDeviceMaterialRecombination();
	afx_msg void OnGraphSpatialDiffusionlength();
	afx_msg void OnUpdateGraphTemporalBasecurrent(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGraphTemporalBaseiv(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGraphTemporalBaseivpower(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGraphTemporalBasevoltage(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGraphTemporalCollectorcurrent(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGraphTemporalCollectoriv(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGraphTemporalCollectorivpower(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGraphTemporalCollectorvoltage(CCmdUI* pCmdUI);
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg void OnExcPhotoPri();
	afx_msg void OnExcPhotoSec();
	afx_msg void OnDeviceSurfaces();
	afx_msg void OnDeviceRecomb();
	afx_msg void OnDeviceReflectance();
	afx_msg void OnMaterialOptical();
	afx_msg void OnGraphExperimental();
	afx_msg void OnComputeQuickbatch();
	afx_msg void OnHackTeresaeva();
	afx_msg void OnUpdateComputeQuickbatch(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg void OnDeviceSelectRegion(UINT nID);
	afx_msg void OnUpdateDeviceSelectRegion(CCmdUI* pCmdUI);
	afx_msg void OnDeviceInsertRegion(UINT nID);
	afx_msg void OnUpdateDeviceInsertRegion(CCmdUI* pCmdUI);
	afx_msg void OnDeviceRemoveRegion(UINT nID);
	afx_msg void OnUpdateDeviceRemoveRegion(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

