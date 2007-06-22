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
#include "PhysicalConstants.h"
#include "ProgramLimits.h"
#include "device.h"
#include "excite.h"
#include "solve.h"
#include "batch.h"

enum CState{NO_STATE, EQ_STATE, SS_STATE, TR_STATE};


//////////////////////////////////////////////////////////////////
// CStatus
// this is heavily used by the multitasking routine DoNextPartOfCalculation()
// If it was implemented as a loop without multitasking, this would be the local variables for
// the OnComputeRun() function.

class CStatus : public CObject
{
	DECLARE_SERIAL(CStatus)
// Attributes
public:
	CState	state;			// State of the last completed solution.
	int		time_step;		// Time step of last completed solution.
	double	time;			// Real time for current time step.
	int		iteration;		// Number of iterations completed on this solution.
	double	progress;		// Progress to completion, from 0 to 1.
	BOOL	in_progress;	// Is computation currently in progress?
	CString	message;		// Posted in message area of status bar
	BOOL 	m_bStopRequested; 			 // TRUE if user has requested calculation to stop
	BOOL	m_bSingleStep;				 // TRUE if we're in single-step mode
	BOOL	m_bConverged;				 // TRUE if last iteration converged. (for NO_STATE, 
						// means no calculations have been done at all). Used by multitasking.
	BOOL	m_bEquilRenode;		// TRUE if renode has happened so far (used in equilibrium calculation, bcos
						// equil needs to modify the var

	bool m_bSilent; // TRUE = don't draw any graphics on status bar
// Operations
public:
	CStatus();
	void Initialize();
	void Serialize(CArchive& ar);
// Implementation
protected:
	long 	m_accumulatedtickcount;      // total number of clock ticks spent on this timestep
	long	m_tickcountwhenStopRequested;// accumtickcountwhen stop was pressed.
	long	m_starttickcount;
	// We use the windows tick count to check whether we've hit the time limit. I'm
	// worried that future PCs might be so fast that the loop takes less than 50 ms.
	// This would mean they wouldn't hit the time limit (tick count resolution is 55ms).
	// So I make sure tick count always goes up even in this case.
	int m_IterationWhenStopRequested;
public:
	void ResetTicker(void) { m_accumulatedtickcount=0; };
	void StartTicker(void) { m_starttickcount=(long)::GetTickCount(); };
	void StopTicker(void)  { long tempticks=(long)::GetTickCount()-m_starttickcount; 
							tempticks>0 ? m_accumulatedtickcount+=tempticks 
										: m_accumulatedtickcount++;
						  };
	void OnStopRequested(void);
	void OnSingleStepRequested(void) { m_bSingleStep=TRUE; m_bStopRequested=FALSE;
								in_progress=TRUE; };
	BOOL TickerHasExceededLimit(int numberofseconds);
};

/// The problem to be solved. This is the object which is actually saved on disk.
class CProblem
{
public:
	CProblem();
	~CProblem();
private:
	WORD m_FileVersion;	// version number it's saved on disk as
	CStatus		m_Status;
private:
	CDevice*	pD;
	CExcite*	pE;
	CSolve*		pS;
	CBatch*		pBatch;
public:
	CDevice*	GetDevice() { return pD; };
	CExcite*	GetExcite() { return pE;};
	CBatch*		GetBatch() { return pBatch; };
	CSolve*		GetSolve() { return pS; };
	CNode*		GetNode() { return  pS->GetNodeData(); };
	CRegionData *GetRegionData() { return pS->GetRegionData(); };


	CString		m_Description;
	double		m_Time[MAX_TIME_STEPS+2];	// real elapsed time array
	double		m_Aux[MAX_TIME_STEPS+2];	// array containing collected Aux Data values
	CString		m_AuxUnits;

public:
	CStatus *GetStatus() { return &m_Status; };
private:
	// reference values
	double	T;		// K, device temperature
	double	Vt;		// V, current thermal voltage
	double	Eg;		// eV, current un-narrowed energy gap at x=0
	double	nir;	// cm-3, current reference ni at x=0
	double	Aref;	// cm2, current reference area
	double	Vn0, Vp0;	// V, band-edge potentials at x=0 with no BGN
public:
	double GetT() const{ return T; };
	double GetVt() const { return Vt; };
	double GetEg() const { return Eg; };
	double GetNir() const { return nir; };
	double GetAref() const { return Aref; };
private:
	double GetVn0() const { return Vn0; };
	double GetVp0() const { return Vp0;}
public:
	// options set from INI
	BOOL	m_bDeviceUpdate;	// reloads node values after any change in EQ configuration
public:
	void SetFileVersion(int PC1DVersion);
	int  GetFileVersion();
	void Initialize();
	void Serialize(CArchive& ar);   // overridden for document i/o
public:
	BOOL IsProblemCompletelySolved(void);
	BOOL IsProblemAtBeginning(void);
	void StartNewCalculation();
	void DoRun(bool bSilent=false); // if bSilent, don't draw any graphics.
	void DoComputeSingleStep();
	void DoComputeStartAgain();
	void DoComputeContinue();
private:
	void Initialize_Locals(int region_to_use);
	void InitializeTimeValues();
private:
	void SetReferenceValues(int region_num);
	void LoadBlackbodySpectrum(CLight* pL);
	int SetNodePositions(CNode* pN, int first, int region_num);		///< returns last node in region
	int GetLastNode(CNode* pN, int first);			///< returns last node in region
	void SetNodeDoping(CNode* pN, int first, int last, int region_num);
	void SetNodeParameters(CNode* pN, int first, int last, int region_num);
	double Reflectance(double lambda, CLight* pL);
	void LoadOpticParms(BOOL m_Back, double lambda,
				double& cosTheta1, double& cosTheta2, double& ThetaDepth, 
				double absorb[], double index[], double w[],  double nfreecarrcoeff[], double pfreecarrcoeff[]);
	void CalculateCarrierConcentrationsForFCA(double *effectiven, double *effectivep);
	void GenerationWithParasitics(double lambda, CLight *pL, double Gen[], double &escape, double *effectiven, double *effectivep);
	void SourcePhotogeneration(int time_step, CLight* pL);
	void SetPhotogeneration(int time_step);
	void SetNeutralEQ(CNode* pN);
	void SetSurfacePotentials(CNode* pN);
	void AdjustNodeValues(CNode* pN, double Tratio);	///< correct for T change
	void SetRegionParameters(int reg, int first, int last);
	void SetExcitation(int time_step, BOOL initialize);
	void SetLumpedParameters();
	void SetCircuitNodes();
	void SetTimeValues(int time_step);
	void LoadNodesAndRegions();
public:
	BOOL DoNextPartOfCalculation(CDocument *pDoc);
	BOOL IsCalculationInProgress(void) { return m_Status.in_progress; };
	void OnDeviceChanged();
	void OnMajorChange(); ///< changes are so big that we need to abandon the last simulation.
};
