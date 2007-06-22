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
#include "hints.h"
#include "problem.h"
#include "PhysicalConstants.h"
#include "ProgramLimits.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////
// CStatus

IMPLEMENT_SERIAL(CStatus, CObject, 0)

CStatus::CStatus()
{
	Initialize();
}

void CStatus::Initialize()
{
	state = NO_STATE;
	time_step = 0;
	time = 0;
	iteration = 0;
	progress = 0;
	in_progress = FALSE;
	message.Empty();
	m_bConverged=TRUE;
}

void CStatus::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);
	if (ar.IsStoring())
		ar << (WORD)state << (WORD)time_step << time;
	else 
	{
		Initialize();
		ar >> (WORD&)state >> (WORD&)time_step >> time;
		time=0; // since time is a measurement of user's patience, it's better to start again
	}
}

void CStatus::OnStopRequested(void) 
{ 
	m_tickcountwhenStopRequested=m_accumulatedtickcount; 
	m_IterationWhenStopRequested=iteration;
	m_bStopRequested=TRUE; 
};
         
// returns TRUE if ticker is greater than numberofseconds OR if 1 sec has passed
// since stop was requested.
BOOL CStatus::TickerHasExceededLimit(int numberofseconds)
{
	if (m_bStopRequested) {
		// give it 1 second or 20 iterations to converge, whichever is shorter.
		if (iteration-m_IterationWhenStopRequested>20) return TRUE;
		return (m_accumulatedtickcount-m_tickcountwhenStopRequested)> 1000; // give it 1 sec to converge
	} else return (m_accumulatedtickcount>(long)numberofseconds*1000);
}

// Some IDIOT working on the Visual C++ compiler wrote a really bad 'new' operator.
// It doesn't align (at all)!, so there are horrendous misalignment penalties 
// (Every memory access takes 40% longer than it should).
// To get around this, make it a static variable. Messy, but at least it's aligned.

CSolve m_FASTsolv;

CProblem::CProblem()
{
	pD = new CDevice;
	pE = new CExcite;
//	pS = new CSolve; 
	pS = &m_FASTsolv;
	pBatch = new CBatch;
}


CProblem::~CProblem()
{
	delete pD;
	delete pE;
//	delete pS;
	delete pBatch;
}

void CProblem::Initialize_Locals(int region_to_use)
{
	ASSERT_VALID(pD);
	T = 300; Eg = pD->GetMaterialForRegion(region_to_use)->m_BandGap; Vt = VT300; 
	nir = pD->GetMaterialForRegion(region_to_use)->m_ni300; Aref = 1; Vn0=0; Vp0=0;
	int k;
	for (k=0; k<MAX_TIME_STEPS+2; k++)
		m_Time[k] = m_Aux[k] = 0;
	m_AuxUnits.Empty();
}

void CProblem::Initialize()
{
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
}

void CProblem::SetFileVersion(int PC1DVersion)
{
	pD->SetFileVersion(PC1DVersion);
	pE->SetFileVersion(PC1DVersion);
	pBatch->SetFileVersion(PC1DVersion);
	m_FileVersion=40;	// same for all PC1D versions so far
}

// the earliest version it could be saved as
// this will be latest of device, material and excitation files
int CProblem::GetFileVersion()
{
	int ver_num, tmp;
	ver_num=40;		// same for all PC1D versions so far
	tmp=pD->GetFileVersion(); if (tmp>ver_num) ver_num=tmp;
	tmp=pE->GetFileVersion(); if (tmp>ver_num) ver_num=tmp;
	tmp=pBatch->GetFileVersion(); if (tmp>ver_num) ver_num=tmp;
	return ver_num;
}

void CProblem::Serialize(CArchive& ar)
{
//	const this_ver = 40;
//	WORD ver_num = this_ver;
	if (ar.IsStoring()) 
	{
//		ar << WORD(ver_num);
ar << m_FileVersion;
		ar << m_Description;
		m_Status.Serialize(ar);
		pD->Serialize(ar);
		pE->Serialize(ar);
		pS->Serialize(ar);
		pBatch->Serialize(ar);
	}	
	else
	{
/***
		ar >> (WORD&) ver_num;
		if (ver_num != this_ver) 
		{
			AfxThrowArchiveException(CArchiveException::badIndex);
			return;
		}
***/
		ar >> (WORD&) m_FileVersion;
		if (m_FileVersion != 40) 
		{
			AfxThrowArchiveException(CArchiveException::badIndex);
			return;
		}

 		ar >> m_Description;
		m_Status.Serialize(ar);
		ASSERT_VALID(pD);
		pD->Serialize(ar);
		ASSERT_VALID(pE);
		pE->Serialize(ar);
		ASSERT_VALID(pS);
		pS->Serialize(ar);
		if (!pS->m_bStoreNodes) m_Status.Initialize();
		pBatch->Serialize(ar);		
		Initialize_Locals(0);
		if (!pS->m_bStoreNodes)	LoadNodesAndRegions();
	}
}

void CProblem::LoadNodesAndRegions()
{
	ASSERT_VALID(pD);
//	int SaveRegion = pD->m_CurrentRegion;
	int first_node = 0; int last_node = 0; int reg = 0;
	GetNode()->Initialize(); 
	for (reg=0; reg<pD->m_NumRegions; reg++)
	{
//		pD->SelectRegion(reg);
		if (reg==0) SetReferenceValues(reg);
		last_node = SetNodePositions(GetNode(), first_node, reg);
		SetNodeParameters(GetNode(), first_node, last_node, reg);
		SetRegionParameters(reg, first_node, last_node);
		first_node = last_node+1;
	}
	GetNode()->SetNumberOfNodes(last_node+1);
	SetNeutralEQ(GetNode());
	SetSurfacePotentials(GetNode());
	GetNode()->CopyPsi_eqIntoPsi();
//	pD->SelectRegion(SaveRegion);
}

/// Sets time values for current time_step except 
/// (a) m_Aux, which is set by the View
/// (b) photogen values, set by optical routines in Prepare.cpp
void CProblem::SetTimeValues(int time_step)
{   
	ASSERT_VALID(pS);
	ASSERT_VALID(pE);
	ASSERT_VALID(pD);

	if (time_step<0)
		m_Time[0] = 0;
	else if (time_step==0)
	{
		m_Time[0] = -pE->m_FirstStep;
		m_Time[1] = 0;
	}                                                           
	else
		m_Time[time_step+1] = m_Time[time_step] + pE->m_TranStep;
	pE->m_Base.m_Volts[time_step+1] = pS->m_SourceV[BASE]*Vt;
	pE->m_Base.m_Amps[time_step+1] = pS->m_SourceI[BASE]*Q*nir*Aref;
	pE->m_Coll.m_Volts[time_step+1] = pS->m_SourceV[COLLECTOR]*Vt;
	pE->m_Coll.m_Amps[time_step+1] = pS->m_SourceI[COLLECTOR]*Q*nir*Aref;
	for (int j=0; j<MAX_LUMPED; j++)
	{
		pD->GetLumpedElement(j)->m_Volts[time_step+1] = pS->m_LumpedV[j]*Vt;
		pD->GetLumpedElement(j)->m_Amps[time_step+1] = pS->m_LumpedI[j]*Q*nir*Aref;
	}
}

static BOOL initialize_photogeneration;		// initialize photogeneration first time only

void CProblem::DoRun(bool bSilent)
{
	m_Status.m_bSilent=bSilent;
	if (pBatch->m_Enable) { 
		pBatch->OnStartBatch();
		m_Status.in_progress = TRUE; // calculation will be done in background by DoNextPartOfCalculation()
		m_Status.m_bStopRequested=FALSE;
		m_Status.m_bSingleStep=FALSE;
	} else StartNewCalculation();
}

void CProblem::StartNewCalculation()
{
	ASSERT_VALID(pS);
	ASSERT_VALID(pE);
	ASSERT_VALID(pD);
	
	if (pBatch->m_Enable) pBatch->OnSimBegin(this);
	
	initialize_photogeneration = TRUE;		// initialize photogeneration first time only
	m_Status.state = NO_STATE; m_Status.time_step = -1; m_Status.time = 0;
	m_Status.m_bConverged=TRUE;	
	SetTimeValues(m_Status.time_step); 
	LoadNodesAndRegions();
	SetLumpedParameters();	// added by PAB 1/4/96
	SetCircuitNodes();	// fixes contact locations prior to any renoding
	m_Status.in_progress = TRUE; // calculation will be done in background by DoNextPartOfCalculation()
	m_Status.m_bStopRequested=FALSE;
	m_Status.m_bSingleStep=FALSE;
}

// returns TRUE if it's OK to stop now  (we've done everything user asked for)
BOOL CProblem::IsProblemCompletelySolved(void)
{
	switch(m_Status.state) {
	case NO_STATE:
		return FALSE;
	case EQ_STATE: 
		return (pE->m_Mode<=EQ_MODE);
	case SS_STATE:
		return (pE->m_Mode<=SS_MODE);
	case TR_STATE:
		return (m_Status.time_step >= pE->m_TranNum);
	default:
		return TRUE;
	}
}
 
// returns TRUE if no calculation has been done so far
BOOL CProblem::IsProblemAtBeginning(void)
{
	return (m_Status.state==NO_STATE && m_Status.m_bConverged);
}

/// ****************************************************************************
///		DoNextPartOfCalculation() 
/// ****************************************************************************
/// "Main loop" of PC1D: this is called by OnIdle() so it is a parallel process.
/// It's MUCH more complex than the old, non-multitasking version because it
/// must do only one iteration, and there are many types of iteration during a
/// calculation. 
/// A few less obvious parts of this code:
///  Suppose the user presses STOP halfway through a calculation. We don't want to
/// stop unless we've just finished a timestep. But, it might never converge for
/// the next timestep, so we make it timeout after a second.
/// The variable m_StopRequested in m_Status is TRUE whenever STOP is pressed.
/// We also use this variable when single-stepping.
/// If m_bGraphsAfterEveryIteration is TRUE, we can stop at any time.
BOOL CProblem::DoNextPartOfCalculation(CDocument *pDoc)
{
	ASSERT_VALID(pS);
	ASSERT_VALID(pE);
	ASSERT_VALID(pD);
	BOOL bFailed;	// TRUE if failed to converge (time ran out)
	
	if (pBatch->m_Enable && pBatch->IsAtStartOfNewSim() ) {
		StartNewCalculation();
	}

//static int tweentime, eqtime, sstime, trtime, phototime, updatetime;
//tweentime+=CUseful::CyclesSinceLastCall();

	
    bFailed=FALSE;

	int* pIter = &m_Status.iteration;
	double* pProg = &m_Status.progress;
	CString* pMess = &m_Status.message;
	switch(m_Status.state)
	{
	case NO_STATE:
		if (m_Status.m_bConverged) {
			// start of calculation- must initialize everything
//tweentime=0;
//eqtime=sstime=trtime=phototime=updatetime=0;
			*pIter=0;
			*pProg=0;
			pMess->Empty();
			m_Status.m_bEquilRenode=pS->m_bRenodeEQ; // this is just a temporary variable stored between DoNextPart..() calls
			m_Status.ResetTicker();
			m_Status.StartTicker();
			m_Status.m_bConverged=pS->DoNextEquilibriumIteration(*pIter,*pProg,*pMess,m_Status.m_bEquilRenode);
			m_Status.StopTicker();
		} else {
			m_Status.StartTicker();
			m_Status.m_bConverged=pS->DoNextEquilibriumIteration(*pIter,*pProg,*pMess,m_Status.m_bEquilRenode);
 			m_Status.StopTicker();
		}
//eqtime+=CUseful::CyclesSinceLastCall();		
		if (m_Status.m_bConverged) {
			m_Status.state = EQ_STATE;
			GetNode()->CopyPsi_eqIntoPsi();
	        CData::SetAuxDataValue(this);
			pDoc->UpdateAllViews(NULL, HINT_GRAPHDATACHANGED);
			if (m_Status.m_bStopRequested || m_Status.m_bSingleStep)
				m_Status.in_progress = FALSE;
		} else { // hasn't converged yet
			if (m_Status.TickerHasExceededLimit(pS->m_nMaxTime)) { // it failed to converge inside the time limit
				bFailed=TRUE;
 			}else { // OK -> this was a valid iteration
 				if (pS->m_bGraphsAfterEveryIteration) {				
					GetNode()->CopyPsi_eqIntoPsi();
					pDoc->UpdateAllViews(NULL, HINT_ITERATION);
					if (m_Status.m_bStopRequested || m_Status.m_bSingleStep)
						m_Status.in_progress = FALSE;
				}		 				
			}
		}
		break;
		
	case EQ_STATE:
		if (m_Status.m_bConverged) {
			SetExcitation(-1, initialize_photogeneration);	// SS time_step = -1
//phototime+=CUseful::CyclesSinceLastCall();
			m_Status.time = 0;
			initialize_photogeneration = FALSE;
			pS->m_rdt = 0;
			m_Status.ResetTicker();
			m_Status.StartTicker();
			if (!pS->m_bRenodeSS) pS->UpdateVariablesAfterRenode(); 
 			m_Status.m_bConverged=pS->PrepareForFirstSystemIteration(*pIter, *pProg, *pMess, pS->m_bRenodeSS, pDoc);
			m_Status.StopTicker();
 			if (!m_Status.m_bConverged){
 				bFailed=TRUE;
  				break;
 			}
		}
		m_Status.StartTicker();
 		m_Status.m_bConverged=pS->DoNextSystemIteration(*pIter, *pProg, *pMess, pS->m_bRenodeSS);
   		pMess->Empty();
		m_Status.StopTicker();
//sstime+=CUseful::CyclesSinceLastCall();
 		if (m_Status.m_bConverged) {
 			m_Status.state = SS_STATE;
			SetTimeValues(m_Status.time_step);	// time_step should == -1
	        CData::SetAuxDataValue(this);
			pDoc->UpdateAllViews(NULL, HINT_GRAPHDATACHANGED);
			if (m_Status.m_bStopRequested || m_Status.m_bSingleStep)
				m_Status.in_progress = FALSE;
		} else { // hasn't converged yet
			if (m_Status.TickerHasExceededLimit(pS->m_nMaxTime) || pS->HasHugeError()) { // it failed to converge inside the time limit
				bFailed=TRUE;
 			}else { // OK -> this was a valid iteration
 				if (pS->m_bGraphsAfterEveryIteration) {				
					pDoc->UpdateAllViews(NULL, HINT_ITERATION);
					if (m_Status.m_bStopRequested || m_Status.m_bSingleStep)
						m_Status.in_progress = FALSE;
				}		 				
			}
		}     
		break;

	case SS_STATE:
	case TR_STATE:
		if (m_Status.m_bConverged) {
			// we're starting a new timestep, so we need to do some initializing...
			if (m_Status.time_step<0)
			{
				m_Status.time = 0;
				if (pE->m_FirstStep>0) pS->m_rdt = 1/pE->m_FirstStep;
				else pS->m_rdt = 0;
			} else {
					m_Status.time += pE->m_TranStep;
					if (pE->m_TranStep>0) pS->m_rdt = 1/pE->m_TranStep;
					else pS->m_rdt = 0;
			}
			
			BOOL bWantRenode=m_Status.time>0 ? pS->m_bRenodeTR : FALSE ;

			SetExcitation(m_Status.time_step+1, initialize_photogeneration);
//phototime+=CUseful::CyclesSinceLastCall();
			initialize_photogeneration = FALSE;
			m_Status.ResetTicker();
			m_Status.StartTicker();
 	 		m_Status.m_bConverged=pS->PrepareForFirstSystemIteration(*pIter, *pProg, *pMess, bWantRenode, pDoc);
			m_Status.StopTicker();
	 	 	if (!m_Status.m_bConverged){
	 	 		bFailed=TRUE;
  				break;
	 	 	}
		}
		m_Status.StartTicker();
		m_Status.m_bConverged=pS->DoNextSystemIteration(*pIter, *pProg, *pMess, pS->m_bRenodeTR);
   		pMess->Empty();
		m_Status.StopTicker();
//trtime+=CUseful::CyclesSinceLastCall();
 		if (m_Status.m_bConverged) { 			
			m_Status.time_step++;
			SetTimeValues(m_Status.time_step);
			m_Status.state = TR_STATE;
			CData::SetAuxDataValue(this);
			pDoc->UpdateAllViews(NULL, HINT_GRAPHDATACHANGED);
			if (m_Status.m_bStopRequested || m_Status.m_bSingleStep)
				m_Status.in_progress = FALSE;
 		} else { // hasn't converged yet
			if (m_Status.TickerHasExceededLimit(pS->m_nMaxTime) || pS->HasHugeError()) {
 				bFailed=TRUE;
 			} else {
 				// OK -> this was a valid iteration
 				if (pS->m_bGraphsAfterEveryIteration) {
					pDoc->UpdateAllViews(NULL, HINT_ITERATION);
					if (m_Status.m_bStopRequested || m_Status.m_bSingleStep)
						m_Status.in_progress = FALSE;
				}
			}
		}
		break;
	} // end switch



	char tmpstr[80];
	
	if (bFailed) {		// it failed to converge inside the time limit
		CString statestr;
		switch(m_Status.state)
		{
		case NO_STATE:		statestr="Equilibrium";		break;
		case EQ_STATE:		statestr="Steady-State";	break;
		case SS_STATE:
		case TR_STATE:		statestr="Transient";		break;
		}
		
 		if (pBatch->m_Enable) {
 			if (m_Status.m_bStopRequested) {
 				m_Status.in_progress=FALSE;
	 			AfxMessageBox(statestr+" Convergence Failure", MB_OK, IDP_CONVERGENCE_FAILURE);
				pDoc->UpdateAllViews(NULL, HINT_GRAPHDATACHANGED);		
 			}else {
 				pBatch->OnSimFailed(this);
				pDoc->UpdateAllViews(NULL, HINT_GRAPHDATACHANGED);
				if (pBatch->IsBatchFinished()) {
						m_Status.in_progress = FALSE; // we're done
						if (pBatch->GetNumberOfFailedSimulations()!=0) {
							sprintf(tmpstr, "%d simulation(s) failed to converge", pBatch->GetNumberOfFailedSimulations());
							AfxMessageBox(tmpstr, MB_OK, IDP_CONVERGENCE_FAILURE);
						}
				}
			}
 		} else {
 			AfxMessageBox(statestr+" Convergence Failure", MB_OK, IDP_CONVERGENCE_FAILURE);
			if (!pS->m_bGraphsAfterEveryIteration) {
				m_Status.Initialize();
			} else {
				m_Status.in_progress = FALSE; // we're done
			}
			pDoc->UpdateAllViews(NULL, HINT_GRAPHDATACHANGED);
		}
	}
//updatetime+=CUseful::CyclesSinceLastCall();	
	if (IsProblemCompletelySolved()) {  
//CString timingstr;
//timingstr.Format("Tween=%d(K), Eq=%d, SS=%d, TR=%d, Excite=%d, Update=%d", tweentime/1000, eqtime/1000, sstime/1000, trtime/1000, phototime/1000, updatetime/1000);
//WriteStatusBarMessage(timingstr);
//CUseful::CyclesSinceLastCall();

		if (pBatch->m_Enable) {
			pBatch->OnSimFinished(this);
			pDoc->UpdateAllViews(NULL, HINT_GRAPHDATACHANGED);			
			pDoc->UpdateAllViews(NULL, HINT_SIMFINISHED);			
			if (pBatch->IsBatchFinished()) {
				m_Status.in_progress = FALSE; // we're done
				if (pBatch->GetNumberOfFailedSimulations()!=0) {
					sprintf(tmpstr, "%d simulation(s) failed to converge", pBatch->GetNumberOfFailedSimulations());
					AfxMessageBox(tmpstr, MB_OK, IDP_CONVERGENCE_FAILURE);
				}
			}
		} else {
			pDoc->UpdateAllViews(NULL, HINT_SIMFINISHED);			
			m_Status.in_progress = FALSE; // we're done
		}	
	}

	return IsCalculationInProgress();
}

#include "useful.h"

void CProblem::DoComputeSingleStep()
{
	if (m_Status.state==NO_STATE && m_Status.m_bConverged) {
		// at very start of calculation, where nothing has been done so far
		initialize_photogeneration = TRUE;		// initialize photogeneration first time only
		m_Status.state = NO_STATE; m_Status.time_step = -1; m_Status.time = 0;
		SetTimeValues(m_Status.time_step);
		LoadNodesAndRegions();
		SetLumpedParameters();	// Added by PAB 1/4/96
		SetCircuitNodes();	// fixes contact locations prior to any renoding
	}
	if (IsProblemCompletelySolved()) {
		CDonStatusBar::WriteStatusBarMessage("Problem is already completely solved.");
  		return; // can't single step beyond end of problem
  	}
	m_Status.OnSingleStepRequested();
}

void CProblem::DoComputeStartAgain()
{
	m_Status.Initialize();
	initialize_photogeneration=TRUE;
}

void CProblem::OnDeviceChanged()
{
	pD->SetModifiedFlag();
	OnMajorChange();
}

void CProblem::OnMajorChange()
{
	pS->m_bUsePrevious = FALSE;
	m_Status.time_step = -1;
	m_Status.state = NO_STATE;
	m_Time[0] = 0; 
	m_Aux[0] = 0;
	if (m_bDeviceUpdate) LoadNodesAndRegions();
}


void CProblem::DoComputeContinue()
{
	ASSERT_VALID(pS);
	ASSERT_VALID(pE);
	ASSERT_VALID(pD);
	if ( !pBatch->m_Enable && IsProblemCompletelySolved() ) {
		// MOD 27/6/96: If the excitation has been changed, we want to be able to do the last
		//   timestep again - esp for steady state, to allow convergence techniques.
		switch(m_Status.state) {
		case EQ_STATE: m_Status.state=NO_STATE; break;
		case SS_STATE: m_Status.state=EQ_STATE; break;
		case TR_STATE: m_Status.state=SS_STATE; break;
		}
  	}
	if (m_Status.state==TR_STATE) {
		SetExcitation(m_Status.time_step+1, TRUE);
	} else if (m_Status.state==SS_STATE) {
		SetExcitation(-1, TRUE);
	}
  	if (pBatch->m_Enable && pBatch->IsBatchFinished()) {
		CDonStatusBar::WriteStatusBarMessage("Batch is already completely solved.");
  		return; // can't continue beyond end of problem
  	}
	m_Status.in_progress = TRUE; // calculation will be done in background by DoNextPartOfCalculation()
	m_Status.m_bStopRequested=FALSE;
	m_Status.m_bSingleStep=FALSE;
}


