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
// pc1ddoc.cpp : implementation of the CPc1dDoc class
//
//	A huge number of functions, but they're mostly very simple.
// Includes: handlers for Device, Excitation and Options menus (mostly trivial)
//			 handlers for Compute menu
//			 handlers for Graph menu (all trivial)
//		Most complex routine is DoNextPartOfCalculation(), which calls the solve 
// routines. It's complex because it needs to allow multitasking.
// Most critical variable is m_Status.in_progress. If TRUE,  multitasking happens.
//  if FALSE, multitasking will stop.

#include "stdafx.h"
#include "pc1ddoc.h"
#include "descdlg.h"
#include "plotload.h"
#include "useful.h"
#include <afxcmn.h>		// for toolbars
#include "path.h"
#include "pc1d.h"
#include "hints.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPc1dDoc

IMPLEMENT_SERIAL(CPc1dDoc, CDocument, 0)	// used to be dyncreate

BEGIN_MESSAGE_MAP(CPc1dDoc, CDocument)
	//{{AFX_MSG_MAP(CPc1dDoc)
	ON_COMMAND(ID_EXC_MODE, OnExcMode)
	ON_COMMAND(ID_COMPUTE_RUN, OnComputeRun)
	ON_COMMAND(ID_COMPUTE_NUMERICAL, OnComputeNumerical)
	ON_COMMAND(ID_COMPUTE_STOP, OnComputeStop)
	ON_COMMAND(ID_COMPUTE_SINGLESTEP, OnComputeSinglestep)
	ON_COMMAND(ID_COMPUTE_STARTAGAIN, OnComputeStartagain)
	ON_COMMAND(ID_COMPUTE_CONTINUE, OnComputeContinue)
	ON_UPDATE_COMMAND_UI(ID_COMPUTE_STOP, OnUpdateComputeStop)
	ON_UPDATE_COMMAND_UI(ID_COMPUTE_RUN, OnUpdateComputeRun)
	ON_UPDATE_COMMAND_UI(ID_COMPUTE_SINGLESTEP, OnUpdateComputeSinglestep)
	ON_UPDATE_COMMAND_UI(ID_COMPUTE_STARTAGAIN, OnUpdateComputeStartagain)
	ON_UPDATE_COMMAND_UI(ID_COMPUTE_CONTINUE, OnUpdateComputeContinue)
	ON_COMMAND(ID_DEVICE_AREA, OnDeviceArea)
	ON_COMMAND(ID_DEVICE_THICKNESS, OnDeviceThickness)
	ON_COMMAND(ID_EXCITATION_NEW, OnExcitationNew)
	ON_COMMAND(ID_EXCITATION_OPEN, OnExcitationOpen)
	ON_COMMAND(ID_EXCITATION_SAVEAS, OnExcitationSaveas)
	ON_COMMAND(ID_EXC_BASE_SOURCE, OnExcBaseSource)
	ON_COMMAND(ID_EXC_COLL_SOURCE, OnExcCollSource)
	ON_COMMAND(ID_EXC_PHOTO_EXTERNAL, OnExcPhotoExternal)
	ON_UPDATE_COMMAND_UI(ID_EXC_PHOTO_EXTERNAL, OnUpdateExcPhotoExternal)
	ON_COMMAND(ID_EXC_PRI_SPECTRUM, OnExcPriSpectrum)
	ON_COMMAND(ID_EXC_SEC_SPECTRUM, OnExcSecSpectrum)
	ON_COMMAND(ID_EXC_PRI_INTENSITY, OnExcPriIntensity)
	ON_COMMAND(ID_EXC_SEC_INTENSITY, OnExcSecIntensity)
	ON_COMMAND(ID_REGION_NEXT, OnRegionNext)
	ON_COMMAND(ID_EXC_TEMPERATURE, OnExcTemperature)
	ON_COMMAND(ID_DEVICE_NEW, OnDeviceNew)
	ON_COMMAND(ID_DEVICE_OPEN, OnDeviceOpen)
	ON_COMMAND(ID_DEVICE_SAVEAS, OnDeviceSaveas)
	ON_COMMAND(ID_DEVICE_MATERIAL_MOBILITIES_VARIABLE, OnDeviceMaterialMobilitiesVariable)
	ON_UPDATE_COMMAND_UI(ID_DEVICE_MATERIAL_MOBILITIES_VARIABLE, OnUpdateDeviceMaterialMobilitiesVariable)
	ON_COMMAND(ID_DEVICE_MATERIAL_MOBILITIES_FIXED, OnDeviceMaterialMobilitiesFixed)
	ON_UPDATE_COMMAND_UI(ID_DEVICE_MATERIAL_MOBILITIES_FIXED, OnUpdateDeviceMaterialMobilitiesFixed)
	ON_COMMAND(ID_MATERIAL_NEW, OnMaterialNew)
	ON_COMMAND(ID_MATERIAL_OPEN, OnMaterialOpen)
	ON_COMMAND(ID_MATERIAL_SAVEAS, OnMaterialSaveas)
	ON_COMMAND(ID_DEVICE_MATERIAL_PERMITTIVITY, OnDeviceMaterialPermittivity)
	ON_COMMAND(ID_DEVICE_MATERIAL_BANDSTRUCTURE, OnDeviceMaterialBandstructure)
	ON_COMMAND(ID_DEVICE_MATERIAL_BGNARROWING_MODEL, OnDeviceMaterialBgnarrowingModel)
	ON_COMMAND(ID_MATERIAL_PARASITICS, OnMaterialParasiticAbsorption)
	ON_COMMAND(ID_MATERIAL_INTRINSIC, OnMaterialIntrinsicAbsorption)
	ON_COMMAND(ID_DEVICE_MATERIAL_REFRACTIVEINDEX, OnDeviceRefractiveindex)
	ON_COMMAND(ID_DEVICE_DOPING_BACKGROUND, OnDeviceDopingBackground)
	ON_COMMAND(ID_DEVICE_DOPING_DIFFUSIONS_FIRSTFRONT, OnDeviceDopingDiffusionsFirstfront)
	ON_COMMAND(ID_DEVICE_DOPING_DIFFUSIONS_FIRSTREAR, OnDeviceDopingDiffusionsFirstrear)
	ON_COMMAND(ID_DEVICE_DOPING_DIFFUSIONS_SECONDFRONT, OnDeviceDopingDiffusionsSecondfront)
	ON_COMMAND(ID_DEVICE_DOPING_DIFFUSIONS_SECONDREAR, OnDeviceDopingDiffusionsSecondrear)
	ON_UPDATE_COMMAND_UI(ID_DEVICE_DOPING_DIFFUSIONS_FIRSTFRONT, OnUpdateDeviceDopingDiffusionsFirstfront)
	ON_UPDATE_COMMAND_UI(ID_DEVICE_DOPING_DIFFUSIONS_FIRSTREAR, OnUpdateDeviceDopingDiffusionsFirstrear)
	ON_UPDATE_COMMAND_UI(ID_DEVICE_DOPING_DIFFUSIONS_SECONDFRONT, OnUpdateDeviceDopingDiffusionsSecondfront)
	ON_UPDATE_COMMAND_UI(ID_DEVICE_DOPING_DIFFUSIONS_SECONDREAR, OnUpdateDeviceDopingDiffusionsSecondrear)
	ON_COMMAND(ID_DEVICE_RECOMBINATION_BULK, OnDeviceRecombinationBulk)
	ON_COMMAND(ID_DEVICE_RECOMBINATION_FRONTSURFACE, OnDeviceRecombinationFrontsurface)
	ON_COMMAND(ID_DEVICE_RECOMBINATION_REARSURFACE, OnDeviceRecombinationRearsurface)
	ON_UPDATE_COMMAND_UI(ID_DEVICE_RECOMBINATION_BULK, OnUpdateDeviceRecombinationBulk)
	ON_UPDATE_COMMAND_UI(ID_DEVICE_RECOMBINATION_FRONTSURFACE, OnUpdateDeviceRecombinationFrontsurface)
	ON_UPDATE_COMMAND_UI(ID_DEVICE_RECOMBINATION_REARSURFACE, OnUpdateDeviceRecombinationRearsurface) 
	ON_COMMAND(ID_REFLECTANCE_FRONT, OnReflectanceFront)
	ON_COMMAND(ID_REFLECTANCE_REAR, OnReflectanceRear)
	ON_COMMAND(ID_DEVICE_REFLECTANCE_INTERNAL, OnDeviceReflectanceInternal)
	ON_COMMAND(ID_DEVICE_TEXTURE, OnDeviceTexture)
	ON_COMMAND(ID_DEVICE_SURFACES_FRONT, OnDeviceSurfacesFront)
	ON_COMMAND(ID_DEVICE_SURFACES_REAR, OnDeviceSurfacesRear)
	ON_COMMAND(ID_DEVICE_CIRCUIT_CONTACTS, OnDeviceCircuitContacts)
	ON_COMMAND(ID_DEVICE_CIRCUIT_INTERNALELEMENTS, OnDeviceCircuitInternalelements)
	ON_UPDATE_COMMAND_UI(ID_DEVICE_CIRCUIT_INTERNALELEMENTS, OnUpdateDeviceCircuitInternalelements)
	ON_COMMAND(ID_DEVICE_DOPING_FRONT_EXTERNAL, OnDeviceDopingFrontExternal)
	ON_UPDATE_COMMAND_UI(ID_DEVICE_DOPING_FRONT_EXTERNAL, OnUpdateDeviceDopingFrontExternal)
	ON_COMMAND(ID_DEVICE_DOPING_REAR_EXTERNAL, OnDeviceDopingRearExternal)
	ON_UPDATE_COMMAND_UI(ID_DEVICE_DOPING_REAR_EXTERNAL, OnUpdateDeviceDopingRearExternal)
	ON_COMMAND(ID_OPTIONS_STORENODEDATA, OnOptionsStorenodedata)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_STORENODEDATA, OnUpdateOptionsStorenodedata)
	ON_COMMAND(ID_OPTIONS_DEVICEUPDATE, OnOptionsDeviceupdate)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_DEVICEUPDATE, OnUpdateOptionsDeviceupdate)
	ON_COMMAND(ID_FILE_DESCRIPTION, OnFileDescription)
	ON_COMMAND(ID_GRAPH_SPATIAL_CARRIERDENSITIES, OnGraphSpatialCarrierdensities)
	ON_COMMAND(ID_GRAPH_SPATIAL_CHARGEDENSITY, OnGraphSpatialChargedensity)
	ON_COMMAND(ID_GRAPH_SPATIAL_CURRENTDENSITY, OnGraphSpatialCurrentdensity)
	ON_COMMAND(ID_GRAPH_SPATIAL_DOPINGDENSITIES, OnGraphSpatialDopingdensities)
	ON_COMMAND(ID_GRAPH_SPATIAL_ELECTRICFIELD, OnGraphSpatialElectricfield)
	ON_COMMAND(ID_GRAPH_SPATIAL_ELECTROSTATICPOTENTIAL, OnGraphSpatialElectrostaticpotential)
	ON_COMMAND(ID_GRAPH_SPATIAL_PHOTOGENERATION, OnGraphSpatialPhotogeneration)
	ON_COMMAND(ID_GRAPH_SPATIAL_CARRIERMOBILITIES, OnGraphSpatialCarriermobilities)
	ON_COMMAND(ID_GRAPH_SPATIAL_CARRIERVELOCITIES, OnGraphSpatialCarriervelocities)
	ON_COMMAND(ID_GRAPH_SPATIAL_ENERGYBANDS, OnGraphSpatialEnergybands)
	ON_COMMAND(ID_GRAPH_DEFINED, OnGraphDefined)
	ON_COMMAND(ID_GRAPH_AUX, OnGraphAux)
	ON_COMMAND(ID_GRAPH_TEMPORAL_AUXILIARY, OnGraphTemporalAuxiliary)
	ON_COMMAND(ID_GRAPH_TEMPORAL_BASECURRENT, OnGraphTemporalBasecurrent)
	ON_COMMAND(ID_GRAPH_TEMPORAL_BASEIV, OnGraphTemporalBaseiv)
	ON_COMMAND(ID_GRAPH_TEMPORAL_BASEIVPOWER, OnGraphTemporalBaseivpower)
	ON_COMMAND(ID_GRAPH_TEMPORAL_BASEVOLTAGE, OnGraphTemporalBasevoltage)
	ON_COMMAND(ID_GRAPH_TEMPORAL_COLLECTORCURRENT, OnGraphTemporalCollectorcurrent)
	ON_COMMAND(ID_GRAPH_TEMPORAL_COLLECTORIV, OnGraphTemporalCollectoriv)
	ON_COMMAND(ID_GRAPH_TEMPORAL_COLLECTORIVPOWER, OnGraphTemporalCollectorivpower)
	ON_COMMAND(ID_GRAPH_TEMPORAL_COLLECTORVOLTAGE, OnGraphTemporalCollectorvoltage)
	ON_COMMAND(ID_GRAPH_TEMPORAL_QUANTUMEFF, OnGraphTemporalQuantumeff)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_TEMPORAL_QUANTUMEFF, OnUpdateGraphTemporalQuantumeff)
	ON_COMMAND(ID_GRAPH_TEMPORAL_SHUNTELEMENTCURRENT, OnGraphTemporalShuntelementcurrent)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_TEMPORAL_SHUNTELEMENTCURRENT, OnUpdateGraphTemporalShuntelementcurrent)
	ON_COMMAND(ID_GRAPH_TEMPORAL_SHUNTELEMENTVOLTAGE, OnGraphTemporalShuntelementvoltage)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_TEMPORAL_SHUNTELEMENTVOLTAGE, OnUpdateGraphTemporalShuntelementvoltage)
	ON_COMMAND(ID_DEVICE_MATERIAL_RECOMBINATION, OnDeviceMaterialRecombination)
	ON_COMMAND(ID_GRAPH_SPATIAL_DIFFUSIONLENGTH, OnGraphSpatialDiffusionlength)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_TEMPORAL_BASECURRENT, OnUpdateGraphTemporalBasecurrent)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_TEMPORAL_BASEIV, OnUpdateGraphTemporalBaseiv)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_TEMPORAL_BASEIVPOWER, OnUpdateGraphTemporalBaseivpower)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_TEMPORAL_BASEVOLTAGE, OnUpdateGraphTemporalBasevoltage)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_TEMPORAL_COLLECTORCURRENT, OnUpdateGraphTemporalCollectorcurrent)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_TEMPORAL_COLLECTORIV, OnUpdateGraphTemporalCollectoriv)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_TEMPORAL_COLLECTORIVPOWER, OnUpdateGraphTemporalCollectorivpower)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_TEMPORAL_COLLECTORVOLTAGE, OnUpdateGraphTemporalCollectorvoltage)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_EXC_PHOTO_PRI, OnExcPhotoPri)
	ON_COMMAND(ID_EXC_PHOTO_SEC, OnExcPhotoSec)
	ON_COMMAND(ID_DEVICE_SURFACES, OnDeviceSurfaces)
	ON_COMMAND(ID_DEVICE_RECOMB, OnDeviceRecomb)
	ON_COMMAND(ID_DEVICE_REFLECTANCE, OnDeviceReflectance)
	ON_COMMAND(ID_MATERIAL_OPTICAL, OnMaterialOptical)
	ON_COMMAND(ID_GRAPH_EXPERIMENTAL, OnGraphExperimental)
	ON_COMMAND(ID_COMPUTE_QUICKBATCH, OnComputeQuickbatch)
	ON_COMMAND(ID_HACK_TERESAEVA, OnHackTeresaeva)
	ON_UPDATE_COMMAND_UI(ID_COMPUTE_QUICKBATCH, OnUpdateComputeQuickbatch)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_STATE, OnUpdateState)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_MODE, OnUpdateMode)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_TIME, OnUpdateTime)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_PROGRESS, OnUpdateProgress)
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(ID_DEVICE_INSERT_1, ID_DEVICE_INSERT_5, OnDeviceInsertRegion)
	ON_UPDATE_COMMAND_UI_RANGE(ID_DEVICE_INSERT_1, ID_DEVICE_INSERT_5, OnUpdateDeviceInsertRegion)
	ON_COMMAND_RANGE(ID_DEVICE_SELECT_1, ID_DEVICE_SELECT_5, OnDeviceSelectRegion)
	ON_UPDATE_COMMAND_UI_RANGE(ID_DEVICE_SELECT_1, ID_DEVICE_SELECT_5, OnUpdateDeviceSelectRegion)
	ON_COMMAND_RANGE(ID_DEVICE_REMOVE_1, ID_DEVICE_REMOVE_5, OnDeviceRemoveRegion)
	ON_UPDATE_COMMAND_UI_RANGE(ID_DEVICE_REMOVE_1, ID_DEVICE_REMOVE_5, OnUpdateDeviceRemoveRegion)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPc1dDoc construction/destruction

CPc1dDoc::CPc1dDoc() : m_INIheading("Options"), m_INIupdate("Update")
{
	CUseful::InitializeFPU();	// so we can detect numeric problems easily when debugging

//	pD = new CDevice;
//	pE = new CExcite;
//	pS = &m_FASTsolv;
//	pBatch = new CBatch;
	GetProblem()->m_bDeviceUpdate = (AfxGetApp()->GetProfileString(m_INIheading, m_INIupdate, "TRUE")=="TRUE");
	int fontheight=20;
	m_GraphFont.CreateFont(fontheight,0,0,0,FW_NORMAL,0,0,0,0,OUT_TT_PRECIS,
		CLIP_TT_ALWAYS, PROOF_QUALITY, VARIABLE_PITCH|TMPF_TRUETYPE|FF_SWISS, 
		"Arial");	
	m_GraphFont90.CreateFont(fontheight,0,900,900,FW_NORMAL,0,0,0,0,OUT_TT_PRECIS,
		CLIP_TT_ALWAYS, PROOF_QUALITY, VARIABLE_PITCH|TMPF_TRUETYPE|FF_SWISS, 
		"Arial");
}


BOOL CPc1dDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	GetProblem()->Initialize();
/***
	m_Description.Empty();
	ASSERT_VALID(pE);
	pE->Initialize();
	ASSERT_VALID(pD);
	pD->Initialize();
	ASSERT_VALID(pS);
	pS->Initialize();
	m_Status.Initialize();
	pBatch->Initialize();
	Initialize_Locals(0);

	SetFileVersion(PC1DVERSION);

	LoadNodesAndRegions();
***/
	UpdateRegionButton();
	UpdateAllViews(NULL, HINT_EVERYTHINGCHANGED);
	// (SDI documents will reuse this document)
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CPc1dDoc serialization

void CPc1dDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring()) 
	{
		GetProblem()->Serialize(ar);
	}	
	else
	{
		GetProblem()->Serialize(ar);
		UpdateRegionButton();
		UpdateAllViews(NULL, HINT_EVERYTHINGCHANGED);
		UpdateAllViews(NULL, HINT_NEWGRAPHS);	
	}
}

void CPc1dDoc::OnFileSave()
{
		GetProblem()->SetFileVersion(GetProblem()->GetFileVersion());
		CDocument::OnFileSave();
		UpdateAllViews(NULL, HINT_EVERYTHINGCHANGED); // do this because filename may have changed.
}

void CPc1dDoc::OnFileSaveAs()
{
	CString Filename=GetPathName();
	CString Ext = ".prm";
	CString Filter = "Parameter File (*.prm)|*.prm|PC1D 4.5 Parameters (*.prm)|*.prm|All Files (*.*)|*.*||";
	CFileDialog dlg(FALSE, Ext, Filename, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, Filter);
	dlg.m_ofn.lpstrTitle = "Save As";
//	dlg.m_ofn.lpstrInitialDir = ((CPc1dApp *)AfxGetApp())->m_Path.prm;	
	if (GetProblem()->GetFileVersion()<50) dlg.m_ofn.nFilterIndex=2;
						else dlg.m_ofn.nFilterIndex=1;
	if (dlg.DoModal()==IDOK)
	{
		CFile f;
		if (!f.Open(dlg.GetPathName(), CFile::modeCreate | CFile::modeWrite))
		{
			AfxMessageBox("Warning: Unable to create parameter file");
		}
		else 
		{
			if (dlg.m_ofn.nFilterIndex==2) {
				GetProblem()->SetFileVersion(45);
			} else {
				GetProblem()->SetFileVersion(PC1DVERSION);
			}
			CArchive ar(&f, CArchive::store); 
			Serialize(ar);
			Filename = dlg.GetPathName();
//			m_Filename.MakeLower();
			SetPathName(Filename);
			SetModifiedFlag(FALSE);
			UpdateAllViews(NULL, HINT_EVERYTHINGCHANGED); // do this because filename may have changed.
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPc1dDoc diagnostics

#ifdef _DEBUG
void CPc1dDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPc1dDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPc1dDoc status bar updates

void CPc1dDoc::OnUpdateState(CCmdUI* pCmdUI)
{
	CState currstate=GetProblem()->GetStatus()->state;
	pCmdUI->Enable(currstate!=NO_STATE);
	switch (currstate)
	{
	case EQ_STATE: pCmdUI->SetText("Equil"); break;
	case SS_STATE: pCmdUI->SetText("Steady"); break;
	case TR_STATE: pCmdUI->SetText("Trans"); break;
	}
}

void CPc1dDoc::OnUpdateMode(CCmdUI* pCmdUI)
{
	CString iterstr;
	if ( !GetProblem()->IsProblemAtBeginning()&&!GetProblem()->IsProblemCompletelySolved() ) {
		iterstr.Format("%4d", GetProblem()->GetStatus()->iteration);
	} else iterstr.Empty();
	pCmdUI->SetText(iterstr);
}

void CPc1dDoc::OnUpdateTime(CCmdUI* pCmdUI)
{
	CStatus *pStatus=GetProblem()->GetStatus();
	CString timestepstr;
	timestepstr.Empty();
	if (pStatus->state==TR_STATE) {
		timestepstr.Format("%3d", pStatus->time_step);
	}
	pCmdUI->SetText(timestepstr);
}

void CPc1dDoc::OnUpdateProgress(CCmdUI* pCmdUI)
{
	CStatus *pStatus=GetProblem()->GetStatus();
	pCmdUI->Enable(TRUE);
	CString progress_str("||||||||||||||||||||||||||||||||||||||||");
	char str[16];
	if (!GetProblem()->IsProblemAtBeginning()&&!GetProblem()->IsProblemCompletelySolved())
	{
		if (pStatus->message.IsEmpty())
		{
			if (pStatus->progress>1) pStatus->progress = 1; 
			if (pStatus->progress<1/40.0) pStatus->progress = 1/40.0;
			progress_str = progress_str.Left((int)(40*pStatus->progress));
		}
		else progress_str = pStatus->message;
	}
	else
	{
		ASSERT_VALID(GetProblem()->GetSolve());
		safesprintf(str, 15, "%3d elements", GetProblem()->GetNode()->NumElements());
		progress_str = str;
	}
	pCmdUI->SetText(progress_str);
}

static int fast_update_tick_count=0;
// MUCH faster than using OnIdle()
// But even so, this function is >60% of the time for an equilibrium iteration!
// Speedup: don't update the status bar more than 10 times per second.
// This makes the program ~20% faster.
void CPc1dDoc::FastUpdateStatusBar(void)
{
	CStatus *pStatus = GetProblem()->GetStatus();
	if (pStatus->m_bSilent) return; // don't draw ANYTHING if silent.

	CWnd *parent=AfxGetMainWnd();
	CStatusBar *statusbar;
	if (!parent) return; // ignore if main frame not made yet
	statusbar=(CStatusBar *)(parent->GetDescendantWindow(AFX_IDW_STATUS_BAR, TRUE));
	if (!statusbar) return;	// ignore if status bar not made yet

	CString statestr, iterstr, timestepstr;

	switch (pStatus->state)
	{
		case EQ_STATE: statestr="Equil"; break;
		case SS_STATE: statestr="Steady"; break;
		case TR_STATE: statestr="Trans"; break;
		default: statestr.Empty(); break;
	}

	if ( !GetProblem()->IsProblemAtBeginning()&&!GetProblem()->IsProblemCompletelySolved() ) {
		iterstr.Format("%4d", pStatus->iteration);
	} else iterstr.Empty();

	timestepstr.Empty();
	if (pStatus->state==TR_STATE) {
		timestepstr.Format("%3d", pStatus->time_step);
	}


	CString progress_str("||||||||||||||||||||||||||||||||||||||||");
	char str[16];
	if (!GetProblem()->IsProblemAtBeginning()&&!GetProblem()->IsProblemCompletelySolved())
	{
		if (pStatus->message.IsEmpty())
		{
			if (pStatus->progress>1) pStatus->progress = 1; 
			if (pStatus->progress<1/40.0) pStatus->progress = 1/40.0;
			progress_str = progress_str.Left((int)(40*pStatus->progress));
		}
		else progress_str = pStatus->message;
	}
	else
	{
		ASSERT_VALID(GetProblem()->GetSolve());
		safesprintf(str, 15, "%3d elements", GetProblem()->GetNode()->NumElements());
		progress_str = str;
	}

	if (!pStatus->m_bSingleStep && !GetProblem()->GetSolve()->m_bGraphsAfterEveryIteration && !pStatus->m_bConverged && ::GetTickCount()-fast_update_tick_count<100) {
		return; // don't update it again if we drew it in the last 10th of a second,
		// unless it's just converged, or if we're single-stepping, or displaying after every iteration
	}
	fast_update_tick_count=::GetTickCount();
	statusbar->SetPaneText(1, iterstr);
	statusbar->SetPaneText(2, progress_str);
	statusbar->SetPaneText(3, statestr);
	statusbar->SetPaneText(4, timestepstr);
}


/////////////////////////////////////////////////////////////////////////////
// CPc1dDoc commands

void CPc1dDoc::DeviceModified()
{
	SetModifiedFlag();
	GetProblem()->OnDeviceChanged();
	if (GetProblem()->m_bDeviceUpdate)
	{
		UpdateAllViews(NULL, HINT_EVERYTHINGCHANGED);
	}
	else UpdateAllViews(NULL, HINT_PARAMSCHANGED);
/***
	GetProblem()->pD->SetModifiedFlag();
	SetModifiedFlag();
	GetProblem()->pS->m_bUsePrevious = FALSE;
	GetProblem()->m_Status.time_step = -1;
	GetProblem()->m_Status.state = NO_STATE;
	GetProblem()->m_Time[0] = 0; 
	GetProblem()->m_Aux[0] = 0;
	if (GetProblem()->m_bDeviceUpdate)
	{
		GetProblem()->LoadNodesAndRegions();
		UpdateAllViews(NULL, HINT_EVERYTHINGCHANGED);
	}
	else UpdateAllViews(NULL, HINT_PARAMSCHANGED);
***/
}		
                  
void CPc1dDoc::OnComputeRun()
{
	GetProblem()->DoRun();
}

void CPc1dDoc::OnComputeContinue()
{
	GetProblem()->DoComputeContinue();
/***
	ASSERT_VALID(pS);
	ASSERT_VALID(pE);
	ASSERT_VALID(pD);
	if ( !GetProblem()->pBatch->m_Enable && GetProblem()->IsProblemCompletelySolved() ) {
		// MOD 27/6/96: If the excitation has been changed, we want to be able to do the last
		//   timestep again - esp for steady state, to allow convergence techniques.
		switch(GetProblem()->m_Status.state) {
		case EQ_STATE: GetProblem()->m_Status.state=NO_STATE; break;
		case SS_STATE: GetProblem()->m_Status.state=EQ_STATE; break;
		case TR_STATE: GetProblem()->m_Status.state=SS_STATE; break;
		}
  	}
	if (GetProblem()->m_Status.state==TR_STATE) {
		GetProblem()->SetExcitation(GetProblem()->m_Status.time_step+1, TRUE);
	} else if (GetProblem()->m_Status.state==SS_STATE) {
		GetProblem()->SetExcitation(-1, TRUE);
	}
  	if (GetProblem()->pBatch->m_Enable && GetProblem()->pBatch->IsBatchFinished()) {
		CDonStatusBar::WriteStatusBarMessage("Batch is already completely solved.");
  		return; // can't continue beyond end of problem
  	}
	GetProblem()->m_Status.in_progress = TRUE; // calculation will be done in background by DoNextPartOfCalculation()
	GetProblem()->m_Status.m_bStopRequested=FALSE;
	GetProblem()->m_Status.m_bSingleStep=FALSE;
***/
}

void CPc1dDoc::OnComputeStop()
{
	// put in a request to stop calculation after next timestep
	GetProblem()->GetStatus()->OnStopRequested();
	CDonStatusBar::WriteStatusBarMessage("Preparing to stop...");
}

void CPc1dDoc::OnComputeSinglestep()
{
	GetProblem()->DoComputeSingleStep();
}

void CPc1dDoc::OnComputeStartagain()
{
	GetProblem()->DoComputeStartAgain();
	UpdateAllViews(NULL, HINT_GRAPHDATACHANGED);
}

void CPc1dDoc::OnUpdateComputeRun(CCmdUI* pCmdUI)
{	pCmdUI->Enable(!IsCalculationInProgress());	}  

void CPc1dDoc::OnUpdateComputeStop(CCmdUI* pCmdUI)
{	pCmdUI->Enable(IsCalculationInProgress()); }

void CPc1dDoc::OnUpdateComputeContinue(CCmdUI* pCmdUI)
{	pCmdUI->Enable(!IsCalculationInProgress() && !GetProblem()->IsProblemAtBeginning());	}  

void CPc1dDoc::OnUpdateComputeSinglestep(CCmdUI* pCmdUI)
{	pCmdUI->Enable(!IsCalculationInProgress() && !GetBatch()->m_Enable);	}  

void CPc1dDoc::OnUpdateComputeStartagain(CCmdUI* pCmdUI)
{	pCmdUI->Enable(!GetProblem()->IsProblemAtBeginning());	}  

void CPc1dDoc::OnUpdateComputeQuickbatch(CCmdUI* pCmdUI) 
{	pCmdUI->Enable(!IsCalculationInProgress());
	pCmdUI->SetCheck(GetBatch()->m_Enable);
}  
 
BOOL CPc1dDoc::DoNextPartOfCalculation(void)
{
	BOOL retval = GetProblem()->DoNextPartOfCalculation(this);
	FastUpdateStatusBar();
	return retval;
}

void CPc1dDoc::OnComputeQuickbatch() 
{
	if (GetBatch()->DoQuickBatchDlg()) {
		SetModifiedFlag();
		UpdateAllViews(NULL, HINT_EVERYTHINGCHANGED);
	}
}

void CPc1dDoc::OnComputeNumerical()
{if (GetSolve()->SetNumericParameters()) SetModifiedFlag();}

//======================================================================================
//		Options menu handlers
//======================================================================================

void CPc1dDoc::OnOptionsStorenodedata()
{GetSolve()->m_bStoreNodes = !GetSolve()->m_bStoreNodes; GetSolve()->StoreOptions();}

void CPc1dDoc::OnUpdateOptionsStorenodedata(CCmdUI* pCmdUI)
{pCmdUI->SetCheck(GetSolve()->m_bStoreNodes);}

void CPc1dDoc::OnOptionsDeviceupdate()
{
	GetProblem()->m_bDeviceUpdate = !GetProblem()->m_bDeviceUpdate;
	CString str;
	if (GetProblem()->m_bDeviceUpdate) str = "TRUE"; else str = "FALSE";
	AfxGetApp()->WriteProfileString(m_INIheading,m_INIupdate,str);	
}

void CPc1dDoc::OnUpdateOptionsDeviceupdate(CCmdUI* pCmdUI)
{pCmdUI->SetCheck(GetProblem()->m_bDeviceUpdate);}

void CPc1dDoc::OnFileDescription()
{
	CDescDlg dlg;
	dlg.m_Description = GetProblem()->m_Description;
	if (dlg.DoModal()==IDOK) 
	{
		GetProblem()->m_Description = dlg.m_Description;
		SetModifiedFlag();
		UpdateAllViews(NULL, HINT_PARAMSCHANGED);
	}
}

//======================================================================================
//		Excitation menu handlers
//======================================================================================

void CPc1dDoc::ExcModified()
{
	GetExcitation()->SetModifiedFlag();
	SetModifiedFlag();
	UpdateAllViews(NULL, HINT_PARAMSCHANGED);
}

void CPc1dDoc::OnExcMode()
{ if (GetExcitation()->OnMode()) ExcModified(); }

void CPc1dDoc::OnExcTemperature()
{
	if (GetExcitation()->SetDeviceTemperature()) 
	{
		ExcModified();
		GetProblem()->OnMajorChange();
		if (GetProblem()->m_bDeviceUpdate) {
			UpdateAllViews(NULL, HINT_EVERYTHINGCHANGED);
		} else UpdateAllViews(NULL, HINT_PARAMSCHANGED);
	}
}

void CPc1dDoc::OnExcitationOpen()
{
	if (GetExcitation()->OnExcitationOpen()) {
		UpdateAllViews(NULL, HINT_NEWGRAPHS);	
		UpdateAllViews(NULL, HINT_PARAMSCHANGED);
	}
}

void CPc1dDoc::OnExcitationNew()
{GetExcitation()->Initialize(); UpdateAllViews(NULL, HINT_PARAMSCHANGED);}

void CPc1dDoc::OnExcitationSaveas()
{GetExcitation()->OnExcitationSaveas(); UpdateAllViews(NULL, HINT_PARAMSCHANGED);}

void CPc1dDoc::OnExcBaseSource()
{	if (GetExcitation()->DoCircuitDlg()) ExcModified();	}

void CPc1dDoc::OnExcPhotoExternal()
{
	if (GetExcitation()->SetFilename(((CPc1dApp *)AfxGetApp())->m_Path.gen))
	{
		CExcite *pE=GetExcitation();
		pE->m_LightExternal = TRUE; 
		pE->m_LightPri.m_On = FALSE; pE->m_LightSec.m_On = FALSE;
		ExcModified();
	}
}

void CPc1dDoc::OnUpdateExcPhotoExternal(CCmdUI* pCmdUI)
{pCmdUI->SetCheck(GetExcitation()->m_LightExternal);}


void CPc1dDoc::OnExcPhotoPri()
{
	if (GetExcitation()->m_LightPri.DoLightPropertySheet("Primary",0) ) {
		GetExcitation()->m_LightExternal = FALSE;
		ExcModified();
	}
}

void CPc1dDoc::OnExcPhotoSec()
{
	if (GetExcitation()->m_LightSec.DoLightPropertySheet("Secondary",0) ) {
		GetExcitation()->m_LightExternal = FALSE;
		ExcModified();
	}
}

// These next ones are only used for diagram double-click shortcuts.
void CPc1dDoc::OnExcCollSource()
{	if (GetExcitation()->DoCircuitDlg(1)) ExcModified();	}

void CPc1dDoc::OnExcPriSpectrum()
{
	if (GetExcitation()->m_LightPri.DoLightPropertySheet("Primary",1) ) {
		GetExcitation()->m_LightExternal = FALSE;
		ExcModified();
	}
}

void CPc1dDoc::OnExcSecSpectrum()
{
	if (GetExcitation()->m_LightSec.DoLightPropertySheet("Secondary",1) ) {
		GetExcitation()->m_LightExternal = FALSE;
		ExcModified();
	}
}

void CPc1dDoc::OnExcPriIntensity()
{
	if (GetExcitation()->m_LightPri.DoLightPropertySheet("Primary",0) ) {
		GetExcitation()->m_LightExternal = FALSE;
		ExcModified();
	}
}

void CPc1dDoc::OnExcSecIntensity()
{
	if (GetExcitation()->m_LightSec.DoLightPropertySheet("Secondary",0) ) {
		GetExcitation()->m_LightExternal = FALSE;
		ExcModified();
	}
}

//======================================================================================
//		Device menu handlers
//======================================================================================

// --- Multiple regions ----
void CPc1dDoc::OnDeviceSelectRegion(UINT nID)
{
	int regnum=nID-ID_DEVICE_SELECT_1;
	GetDevice()->SetCurrentRegionNumber(regnum); UpdateRegionButton(); UpdateAllViews(NULL, HINT_DIFFERENTREGION);
}

void CPc1dDoc::OnDeviceInsertRegion(UINT nID)
{
	int regnum=nID-ID_DEVICE_INSERT_1;
	GetDevice()->InsertRegion(regnum); DeviceModified(); UpdateRegionButton();
}

void CPc1dDoc::OnDeviceRemoveRegion(UINT nID)
{
	int regnum=nID-ID_DEVICE_REMOVE_1;
	GetDevice()->RemoveRegion(regnum); DeviceModified(); UpdateRegionButton();

}

void CPc1dDoc::OnUpdateDeviceSelectRegion(CCmdUI* pCmdUI)
{
	int regnum=(pCmdUI->m_nID)-ID_DEVICE_SELECT_1;
	pCmdUI->Enable(GetDevice()->m_NumRegions>regnum);
	pCmdUI->SetCheck(GetDevice()->GetCurrentRegionNumber()==regnum);
}

void CPc1dDoc::OnUpdateDeviceInsertRegion(CCmdUI* pCmdUI)
{
	int regnum=(pCmdUI->m_nID)-ID_DEVICE_INSERT_1;
	pCmdUI->Enable(GetDevice()->m_NumRegions>regnum-1 && GetDevice()->m_NumRegions<MAX_REGIONS);
	pCmdUI->SetCheck(GetDevice()->GetCurrentRegionNumber()==regnum);
}


void CPc1dDoc::OnUpdateDeviceRemoveRegion(CCmdUI* pCmdUI)
{
	int regnum=(pCmdUI->m_nID)-ID_DEVICE_REMOVE_1;
	pCmdUI->Enable(GetDevice()->m_NumRegions>regnum && GetDevice()->m_NumRegions>1);
	pCmdUI->SetCheck(GetDevice()->GetCurrentRegionNumber()==regnum);
}

// --- Global device stuff ----

void CPc1dDoc::OnDeviceArea()
{if (GetDevice()->DoAreaDlg()) DeviceModified();}

void CPc1dDoc::OnDeviceThickness()
{if (GetDevice()->GetCurrentRegion()->SetRegionThickness()) DeviceModified();}

void CPc1dDoc::OnDeviceOpen()
{if (GetDevice()->OnDeviceOpen()) {DeviceModified(); GetDevice()->m_bModified=FALSE;}}

void CPc1dDoc::OnDeviceNew()
{GetDevice()->Initialize(); DeviceModified(); GetDevice()->m_bModified=FALSE;}

void CPc1dDoc::OnDeviceSaveas()
{GetDevice()->OnDeviceSaveas(); UpdateAllViews(NULL, HINT_PARAMSCHANGED);}

// --- Materials ---

void CPc1dDoc::OnDeviceMaterialMobilitiesVariable()
{if (GetCurrentMaterial()->SetMobilityModel()) 
	{GetCurrentMaterial()->SetModifiedFlag(); DeviceModified();}}

void CPc1dDoc::OnUpdateDeviceMaterialMobilitiesVariable(CCmdUI* pCmdUI)
{pCmdUI->SetCheck(!GetCurrentMaterial()->m_FixedMobility);}

void CPc1dDoc::OnDeviceMaterialMobilitiesFixed()
{if (GetCurrentMaterial()->SetMobilityFixed()) 
	{GetCurrentMaterial()->SetModifiedFlag(); DeviceModified();}}

void CPc1dDoc::OnUpdateDeviceMaterialMobilitiesFixed(CCmdUI* pCmdUI)
{pCmdUI->SetCheck(GetCurrentMaterial()->m_FixedMobility);}

void CPc1dDoc::OnMaterialNew()
{GetCurrentMaterial()->Initialize(); DeviceModified();}

void CPc1dDoc::OnMaterialOpen()
{if (GetCurrentMaterial()->OnMaterialOpen()) DeviceModified();}

void CPc1dDoc::OnMaterialSaveas()
{GetCurrentMaterial()->OnMaterialSaveas(); UpdateAllViews(NULL, HINT_PARAMSCHANGED);}

void CPc1dDoc::OnMaterialOptical()
{if (GetCurrentMaterial()->DoOpticalDlg())
	{	GetCurrentMaterial()->SetModifiedFlag(); DeviceModified(); }}


void CPc1dDoc::OnDeviceMaterialPermittivity()
{if (GetCurrentMaterial()->SetPermittivity()) 
	{GetCurrentMaterial()->SetModifiedFlag(); DeviceModified();}}

void CPc1dDoc::OnDeviceMaterialBandstructure()
{if (GetCurrentMaterial()->SetBandStructure())
	{GetCurrentMaterial()->SetModifiedFlag(); DeviceModified();}}
	
void CPc1dDoc::OnDeviceMaterialBgnarrowingModel()
{if (GetCurrentMaterial()->SetBandgapNarrowing())
	{GetCurrentMaterial()->SetModifiedFlag(); DeviceModified();}}

void CPc1dDoc::OnDeviceMaterialRecombination()
{if (GetCurrentMaterial()->DoRecombinationDlg())
	{GetCurrentMaterial()->SetModifiedFlag(); DeviceModified();}}

void CPc1dDoc::OnMaterialIntrinsicAbsorption()
{if (GetCurrentMaterial()->DoOpticalDlg())
	{		GetCurrentMaterial()->SetModifiedFlag(); DeviceModified(); }}

void CPc1dDoc::OnMaterialParasiticAbsorption()
{if (GetCurrentMaterial()->DoOpticalDlg(1)) 	
	{GetCurrentMaterial()->SetModifiedFlag(); DeviceModified();}}
	
void CPc1dDoc::OnDeviceRefractiveindex()
{if (GetCurrentMaterial()->DoOpticalDlg(2))
	{	GetCurrentMaterial()->SetModifiedFlag(); DeviceModified(); }}


void CPc1dDoc::OnDeviceDopingBackground()
{if (GetCurrentRegion()->SetBackgroundDoping()) DeviceModified();}

void CPc1dDoc::OnDeviceDopingFrontExternal()
{
	if (GetCurrentRegion()->SetFrontDopingFile(((CPc1dApp *)AfxGetApp())->m_Path.dop))
	{
		GetCurrentRegion()->m_FrontExternal = TRUE;
		DeviceModified();
	}
}

void CPc1dDoc::OnUpdateDeviceDopingFrontExternal(CCmdUI* pCmdUI)
{pCmdUI->SetCheck(GetCurrentRegion()->m_FrontExternal);}

void CPc1dDoc::OnDeviceDopingRearExternal()
{
	if (GetCurrentRegion()->SetRearDopingFile(((CPc1dApp *)AfxGetApp())->m_Path.dop))
	{
		GetCurrentRegion()->m_RearExternal = TRUE;
		DeviceModified();
	}
}

void CPc1dDoc::OnUpdateDeviceDopingRearExternal(CCmdUI* pCmdUI)
{pCmdUI->SetCheck(GetDevice()->GetCurrentRegion()->m_RearExternal);}

void CPc1dDoc::OnDeviceDopingDiffusionsFirstfront()
{
	if (GetCurrentRegion()->m_FrontDiff1.SetDiffusion("First Front Diffusion"))
	{
		GetCurrentRegion()->m_FrontExternal = FALSE;
		DeviceModified(); 
	}
}

void CPc1dDoc::OnDeviceDopingDiffusionsFirstrear()
{
	if (GetCurrentRegion()->m_RearDiff1.SetDiffusion("First Rear Diffusion"))
	{
		GetCurrentRegion()->m_RearExternal = FALSE;
		DeviceModified(); 
	}
}

void CPc1dDoc::OnDeviceDopingDiffusionsSecondfront()
{
	if (GetCurrentRegion()->m_FrontDiff2.SetDiffusion("Second Front Diffusion"))
	{
		GetCurrentRegion()->m_FrontExternal = FALSE;
		DeviceModified(); 
	}
}

void CPc1dDoc::OnDeviceDopingDiffusionsSecondrear()
{
	if (GetCurrentRegion()->m_RearDiff2.SetDiffusion("Second Rear Diffusion"))
	{
		GetCurrentRegion()->m_RearExternal = FALSE;
		DeviceModified(); 
	}
}

void CPc1dDoc::OnUpdateDeviceDopingDiffusionsFirstfront(CCmdUI* pCmdUI)
{pCmdUI->SetCheck(GetCurrentRegion()->m_FrontDiff1.IsEnabled() && !GetCurrentRegion()->m_FrontExternal);}

void CPc1dDoc::OnUpdateDeviceDopingDiffusionsFirstrear(CCmdUI* pCmdUI)
{pCmdUI->SetCheck(GetCurrentRegion()->m_RearDiff1.IsEnabled() && !GetCurrentRegion()->m_RearExternal);}

void CPc1dDoc::OnUpdateDeviceDopingDiffusionsSecondfront(CCmdUI* pCmdUI)
{pCmdUI->SetCheck(GetCurrentRegion()->m_FrontDiff2.IsEnabled() && !GetCurrentRegion()->m_FrontExternal);}

void CPc1dDoc::OnUpdateDeviceDopingDiffusionsSecondrear(CCmdUI* pCmdUI)
{pCmdUI->SetCheck(GetCurrentRegion()->m_RearDiff2.IsEnabled() && !GetCurrentRegion()->m_RearExternal);}


void CPc1dDoc::OnDeviceRecomb()
{ if (GetCurrentRegion()->DoRecombinationDlg(GetDevice()->GetCurrentRegionNumber(), 0)) DeviceModified();}

void CPc1dDoc::OnDeviceRecombinationBulk()
{ if (GetCurrentRegion()->DoRecombinationDlg(GetDevice()->GetCurrentRegionNumber(), 0)) DeviceModified();}

void CPc1dDoc::OnDeviceRecombinationFrontsurface()
{ if (GetCurrentRegion()->DoRecombinationDlg(GetDevice()->GetCurrentRegionNumber(), 1)) DeviceModified();}

void CPc1dDoc::OnDeviceRecombinationRearsurface()
{ if (GetCurrentRegion()->DoRecombinationDlg(GetDevice()->GetCurrentRegionNumber(), 2)) DeviceModified();}

void CPc1dDoc::OnUpdateDeviceRecombinationBulk(CCmdUI* pCmdUI)
{pCmdUI->SetCheck(GetCurrentRegion()->m_TauN!=1e-3 || GetCurrentRegion()->m_TauP!=1e-3);}

void CPc1dDoc::OnUpdateDeviceRecombinationFrontsurface(CCmdUI* pCmdUI)
{pCmdUI->SetCheck(GetCurrentRegion()->m_FrontSn!=0 || GetCurrentRegion()->m_FrontSp!=0);}

void CPc1dDoc::OnUpdateDeviceRecombinationRearsurface(CCmdUI* pCmdUI)
{pCmdUI->SetCheck(GetCurrentRegion()->m_RearSn!=0 || GetCurrentRegion()->m_RearSp!=0);}

void CPc1dDoc::OnDeviceReflectance()
{
	if (GetDevice()->DoReflectanceDlg()) DeviceModified();
}


void CPc1dDoc::OnReflectanceFront()
{	if (GetDevice()->DoReflectanceDlg(0)) DeviceModified(); }

void CPc1dDoc::OnReflectanceRear()
{	if (GetDevice()->DoReflectanceDlg(1)) DeviceModified(); }

void CPc1dDoc::OnDeviceReflectanceInternal()
{	if (GetDevice()->DoReflectanceDlg(2)) DeviceModified(); }


void CPc1dDoc::OnDeviceTexture()
{
	if (GetDevice()->DoTextureDlg()) DeviceModified();
}

void CPc1dDoc::OnDeviceSurfaces()
{	if (GetDevice()->DoSurfaceDlg(0)) DeviceModified();	}



void CPc1dDoc::OnDeviceSurfacesFront()
{	if (GetDevice()->DoSurfaceDlg(0)) DeviceModified(); }

void CPc1dDoc::OnDeviceSurfacesRear()
{	if (GetDevice()->DoSurfaceDlg(1)) DeviceModified();  }


void CPc1dDoc::OnDeviceCircuitContacts()
{	if (GetDevice()->DoContactsDlg()) DeviceModified(); }

void CPc1dDoc::OnDeviceCircuitInternalelements()
{if (GetDevice()->DoInternalElementsDlg()) DeviceModified(); }
// SetInternalElements sets m_EnableInternal if any internal elements are enabled

void CPc1dDoc::OnUpdateDeviceCircuitInternalelements(CCmdUI* pCmdUI)
{pCmdUI->SetCheck(GetDevice()->m_EnableInternal);}
                                     
                                     
//======================================================================================
// 			Graph selection handlers - used for switching to an interactive graph
//(These could be in the actgraph view, but then they wouldn't work from other views)
//======================================================================================

// Switch to interactive graph view, and change to a different predefined graph
// (we change the graph first, because it won't require 2 redraws (?)
void CPc1dDoc::SwitchToInteractivePredefinedGraph(enum GraphType whichgraph)
{
	static CGraph gr;
	gr.SetPredefinedGraph(whichgraph);
	SwitchToInteractiveGraph(NULL, &gr);	
}

// this passes a CGraph rather than just the enumerated type.
//pView=the view which made the call.
void CPc1dDoc::SwitchToInteractiveGraph(CView *pView, CGraph *whichgraph)
{
	UpdateAllViews(pView, HINT_DIFFERENTGRAPH, whichgraph);
	((CPc1dApp *)AfxGetApp())->OnViewInteractivegraph();
}


void CPc1dDoc::SwitchToFourGraphsView(void)
{   
	((CPc1dApp *)AfxGetApp())->OnViewFourgraphs();
}
//---------------

void CPc1dDoc::OnGraphDefined()
{
	((CPc1dApp *)AfxGetApp())->OnViewInteractivegraph();
	AfxGetApp()->m_pMainWnd->SendMessage(WM_COMMAND, ID_GRAPH_REDEFINE, 0);
}

void CPc1dDoc::OnGraphAux()
{
	if (CData::AskUserToDefineAuxiliary()){	// display the AuxData dialog.
		// The existing aux data will now be invalid, we need to set it to zero
		// to indicate it don't make sense no more.
		int i;
		for (i=0; i<MAX_TIME_STEPS+2; i++) GetProblem()->m_Aux[i]=0;	
		UpdateAllViews(NULL, HINT_NEWGRAPHS, NULL);
	}
}

void CPc1dDoc::OnGraphExperimental()
{
	CData::OnGraphDefineExperimental();	// display ExpData dialog
}

void CPc1dDoc::OnGraphSpatialCarrierdensities()
{	SwitchToInteractivePredefinedGraph(CarrierDensities); }

void CPc1dDoc::OnGraphSpatialChargedensity()
{	SwitchToInteractivePredefinedGraph(ChargeDensity); }

void CPc1dDoc::OnGraphSpatialCurrentdensity()
{	SwitchToInteractivePredefinedGraph(CurrentDensity); }

void CPc1dDoc::OnGraphSpatialDopingdensities()
{	SwitchToInteractivePredefinedGraph(DopingDensities); }

void CPc1dDoc::OnGraphSpatialElectricfield()
{	SwitchToInteractivePredefinedGraph(ElectricField); }

void CPc1dDoc::OnGraphSpatialElectrostaticpotential()
{	SwitchToInteractivePredefinedGraph(ElectrostaticPotential); }

void CPc1dDoc::OnGraphSpatialPhotogeneration()
{  	SwitchToInteractivePredefinedGraph(Photogeneration); }

void CPc1dDoc::OnGraphSpatialCarriermobilities()
{	SwitchToInteractivePredefinedGraph(CarrierMobilities); }

void CPc1dDoc::OnGraphSpatialCarriervelocities()
{	SwitchToInteractivePredefinedGraph(CarrierVelocities); }

void CPc1dDoc::OnGraphSpatialEnergybands()
{	SwitchToInteractivePredefinedGraph(EnergyBands);}

void CPc1dDoc::OnGraphSpatialDiffusionlength()
{   SwitchToInteractivePredefinedGraph(DiffusionLength);}

void CPc1dDoc::OnGraphTemporalAuxiliary()
{	SwitchToInteractivePredefinedGraph(Auxiliary);}

void CPc1dDoc::OnGraphTemporalBasecurrent()
{	SwitchToInteractivePredefinedGraph(BaseCurrent); }

void CPc1dDoc::OnGraphTemporalBaseiv()
{	SwitchToInteractivePredefinedGraph(BaseIV); }

void CPc1dDoc::OnGraphTemporalBaseivpower()
{	SwitchToInteractivePredefinedGraph(BaseIVPower); }

void CPc1dDoc::OnGraphTemporalBasevoltage()
{	SwitchToInteractivePredefinedGraph(BaseVoltage); }

void CPc1dDoc::OnGraphTemporalCollectorcurrent()
{	SwitchToInteractivePredefinedGraph(CollectorCurrent);}

void CPc1dDoc::OnGraphTemporalCollectoriv()
{	SwitchToInteractivePredefinedGraph(CollectorIV); }

void CPc1dDoc::OnGraphTemporalCollectorivpower()
{	SwitchToInteractivePredefinedGraph(CollectorIVPower);}

void CPc1dDoc::OnGraphTemporalCollectorvoltage()
{  	SwitchToInteractivePredefinedGraph(CollectorVoltage); }

void CPc1dDoc::OnGraphTemporalQuantumeff()
{	SwitchToInteractivePredefinedGraph(QuantumEff); }

void CPc1dDoc::OnUpdateGraphTemporalQuantumeff(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetExcitation()->m_LightPri.m_Lambda[0]>0 &&
 				GetExcitation()->m_LightPri.m_Intensity[0]>0);
}

void CPc1dDoc::OnGraphTemporalShuntelementcurrent()
{   SwitchToInteractivePredefinedGraph(InternalElementI); }

void CPc1dDoc::OnUpdateGraphTemporalShuntelementcurrent(CCmdUI* pCmdUI)
{   pCmdUI->Enable(GetDevice()->m_EnableInternal); }

void CPc1dDoc::OnGraphTemporalShuntelementvoltage()
{	SwitchToInteractivePredefinedGraph(InternalElementV); }

void CPc1dDoc::OnUpdateGraphTemporalShuntelementvoltage(CCmdUI* pCmdUI)
{   pCmdUI->Enable(GetDevice()->m_EnableInternal); }

void CPc1dDoc::OnUpdateGraphTemporalBasecurrent(CCmdUI* pCmdUI)
{	pCmdUI->Enable(GetDevice()->m_EnableBase); }

void CPc1dDoc::OnUpdateGraphTemporalBaseiv(CCmdUI* pCmdUI)
{	pCmdUI->Enable(GetDevice()->m_EnableBase); }

void CPc1dDoc::OnUpdateGraphTemporalBaseivpower(CCmdUI* pCmdUI)
{	pCmdUI->Enable(GetDevice()->m_EnableBase); }

void CPc1dDoc::OnUpdateGraphTemporalBasevoltage(CCmdUI* pCmdUI)
{	pCmdUI->Enable(GetDevice()->m_EnableBase); }

void CPc1dDoc::OnUpdateGraphTemporalCollectorcurrent(CCmdUI* pCmdUI)
{	pCmdUI->Enable(GetDevice()->m_EnableCollector); }

void CPc1dDoc::OnUpdateGraphTemporalCollectoriv(CCmdUI* pCmdUI)
{	pCmdUI->Enable(GetDevice()->m_EnableCollector); }

void CPc1dDoc::OnUpdateGraphTemporalCollectorivpower(CCmdUI* pCmdUI)
{	pCmdUI->Enable(GetDevice()->m_EnableCollector); }

void CPc1dDoc::OnUpdateGraphTemporalCollectorvoltage(CCmdUI* pCmdUI)
{	pCmdUI->Enable(GetDevice()->m_EnableCollector); }

void CPc1dDoc::UpdateRegionButton(void)
{
	CWnd *parent=AfxGetMainWnd();
	if (!parent) return;
	CToolBar *toolbar;
//	toolbar=(CToolBar *)(parent->GetControlBar(AFX_IDW_TOOLBAR));	
	toolbar=(CToolBar *)(parent->GetDescendantWindow(AFX_IDW_TOOLBAR, TRUE));
	if (!toolbar) return;

	CToolBarCtrl &bar=toolbar->GetToolBarCtrl();
	int regnext=bar.CommandToIndex(ID_REGION_NEXT);
	TBBUTTON regButton1;
	int regindex1=bar.CommandToIndex(ID_DUD_BUTTON1);
	bar.GetButton(regindex1, &regButton1);
	int reg1=regButton1.iBitmap;
	toolbar->SetButtonInfo(regnext, ID_REGION_NEXT, TBBS_BUTTON, reg1+GetDevice()->GetCurrentRegionNumber());
}

void CPc1dDoc::OnRegionNext()
{
	int curregion=GetDevice()->GetCurrentRegionNumber();
	curregion++;
	if (curregion> GetDevice()->m_NumRegions-1) curregion=0;
	GetDevice()->SetCurrentRegionNumber(curregion);
	UpdateRegionButton();
	UpdateAllViews(NULL, HINT_DIFFERENTREGION);
}


void CPc1dDoc::OnHackTeresaeva() 
{
	// Hack for Teresa Eva's thesis
	if (GetBatch()->m_Enable && GetBatch()->m_bExternal) {
		GetBatch()->Read();
	}
	OnComputeRun();
	AfxGetApp()->OnIdle(0);
	while (IsCalculationInProgress()) {
		AfxGetApp()->OnIdle(0);				
		AfxGetMainWnd()->UpdateWindow();
	}
	SwitchToInteractivePredefinedGraph(QuantumEff);
	AfxGetMainWnd()->UpdateWindow();
	AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_EDIT_COPY);
}
