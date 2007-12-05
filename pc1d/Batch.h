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
// BATCH.H - header file for the CBatch class
// See batch.cpp for info on adding more batch parameters

const int MAXBATCHFIELDS=30; // max # of varying parms (only the first 12 will print out)

const int MAXNUMBEROFPERMUTES=4;	// interface-limited

class CProblem;

class CBatchLine {
public:
	BOOL m_bConverged;		// did this line converge?
	double field[MAXBATCHFIELDS];
};

class CBatch : public CObject
{
	DECLARE_SERIAL(CBatch)
// enums
public:
	enum BatchParameter {
		// Device Parameters
		NoBatchParameter=-1,
		FirstBatchParameter=0,
		Area=FirstBatchParameter, 
		FrontTextureAngle, FrontTextureDepth,
		RearTextureAngle, RearTextureDepth,
		FrontBarrier, FrontCharge,
		RearBarrier, RearCharge,
		EmitterInternRes, BaseInternRes, CollInternRes,
		EmitterX, BaseX, CollectorX,
		Shunt1Value, Shunt2Value, Shunt3Value, Shunt4Value,
		Shunt1Anode, Shunt1Cathode, Shunt2Anode, Shunt2Cathode,
		Shunt3Anode, Shunt3Cathode, Shunt4Anode, Shunt4Cathode,
		FrontFixedReflectance, RearFixedReflectance,
		FrontBroadbandReflectance, RearBroadbandReflectance,
		FrOuterThickness, FrMiddleThickness, FrInnerThickness,
		FrOuterIndex, FrMiddleIndex, FrInnerIndex,
		RrOuterThickness, RrMiddleThickness, RrInnerThickness,
		RrOuterIndex, RrMiddleIndex, RrInnerIndex,
		FrontInternReflFirst, FrontInternReflSubsequent,
		RearInternReflFirst, RearInternReflSubsequent,
		FrontInternRefl, RearInternRefl,

		// Region parameters
		FirstRegionParameter,		
		Thickness=FirstRegionParameter,
		BackgroundDoping,
		FrontDopingPeak1,FrontDopingDepth1,FrontDopingPos1,
		FrontDopingPeak2,FrontDopingDepth2,FrontDopingPos2,
		RearDopingPeak1,RearDopingDepth1,RearDopingPos1,
		RearDopingPeak2,RearDopingDepth2,RearDopingPos2,
		BulkTaun, BulkTaup, BulkTau, BulkEt,
		FrontSn, FrontSp, FrontEt,
		RearSn, RearSp,	RearEt,
		FrontS, RearS, 
		// Material parameters
		Bandgap, BandgapDirect1, BandgapDirect2,
		BandgapIndirect1, BandgapIndirect2, 
		Ni200, Ni300, Ni400,
		LastRegionParameter=Ni400, // Be sure to update this!!!
		                    
		// Excitation parameters           
		Temperature, 
		BaseResSS, BaseResTR, 
		BaseVoltageSS, BaseVoltageTR1, BaseVoltageTR2,
		CollResSS, CollResTR,  
		CollVoltageSS, CollVoltageTR1, CollVoltageTR2,
		PriIntensitySS, PriIntensityTR1, PriIntensityTR2,
		PriMonoSS, PriMonoTR1, PriMonoTR2,
		PriBlackTemp,		
		SecIntensitySS, SecIntensityTR1, SecIntensityTR2,
		SecMonoSS, SecMonoTR1, SecMonoTR2,
		SecBlackTemp,		
		
		ElementSizeFactor, ErrorLimit, Clamp,
// add extra numeric inputs here		

		FirstResultsParameter, BaseVoc=FirstResultsParameter, // Dont change this!!
		BaseIsc, BasePmax,
		CollVoc, CollIsc, CollPmax,
		Vb, Ib, Vc, Ic,
		V1, I1, V2, I2, V3, I3, V4, I4,
		RmsExptDiff, NumElements,
// add extra non-spatial results here		
		FirstSpatialResult,
		Na=FirstSpatialResult, Rbulk, Rho, Ec, Cond, Ccum, Ccum_, Gcum, Rcum,
		Perm, Ld, Nd, Eg, Nie, E, In, Jn, N, MuN,
		PhiN, Vn, Psi, Rho_, Cond_, N_, Psi_, P_, G,
	 	Ip, Jp, P, MuP, PhiP, Vp, Tau, Res, It, Jt, Evac, Ev,
	 	Nratio, Pratio, PNratio, PNnorm, Ndrift, Ndiff, Pdrift, Pdiff,
// add extra spatial results here
		LastResultsParameter=Pdiff	// be sure to update this!!!
  	};
public:
	CBatch();
	virtual void Serialize(CArchive& ar);

// Attributes
public:
	WORD m_FileVersion;	// what version is it saved as on disk?

	BOOL m_Enable;		// Are we in batch mode? TRUE=yes
	BOOL m_bExternal;	// TRUE=using external batch file, FALSE=Quickbatch
	CString m_Filename;
public: 
	int GetNumberOfFields(void) { return m_numfields; };
	int GetNumberOfLines(void) { return m_numlines; };
    CString GetFieldName(int fieldnum); // get name of field n
    double GetValue(int fieldnum);
	double GetValue(int linenum, int fieldnum);
	BOOL IsValueValid(int linenum, int fieldnum);
	int GetNumberOfFailedSimulations(void);

// Operations
public:
	void SetFileVersion(int PC1DVersion);
	int GetFileVersion(void);

	void Initialize();
	void OnStartBatch(); // Call to start a new batch
	void OnSimBegin(CProblem *pProb);
	void OnSimFinished(CProblem *pProb); // Call when one line has been finished
	void OnSimFailed(CProblem *pProb); // Call when line failed - want to go to next line
	BOOL IsBatchFinished(); // TRUE if no more work to do
	BOOL IsAtStartOfNewSim();
	BOOL LoadFromString(CString str);	// Like load from file, except it's from string
	BOOL CanPaste();
	void PasteBatchFile();
	BOOL DoBatchDlg();
	BOOL DoQuickBatchDlg();

	void TerminateBatch(); 
    BOOL Read();    // read values from file
	BOOL Write();	// write values to file
	void CopyEntireBatchToClipboard(CWnd *wnd);

// Implementation
private:
//	static char *m_BatchParamTextList[];
	static char *m_BatchParamDescriptList[];
	static char *m_BatchResultDescriptList[];
	double m_fieldxcoord[MAXBATCHFIELDS];
    int m_numfields;
    int m_numlines;

	CArray<CBatchLine, CBatchLine &> m_ParamLine;
    double m_savedparamvalue[MAXBATCHFIELDS];
	BatchParameter m_fieldparam[MAXBATCHFIELDS];
	int m_regionnum[MAXBATCHFIELDS];
	int m_simnum;
	BOOL m_bStartOfSim; // TRUE if no work has been done so far on current line
    CString m_OriginalWindowTitle;

	// QuickBatch
	BatchParameter m_Quickfieldparam[MAXBATCHFIELDS];
	int m_Quickregionnum[MAXBATCHFIELDS];
	double m_Quickfieldxcoord[MAXBATCHFIELDS];
	double m_QuickFrom[MAXBATCHFIELDS], m_QuickTo[MAXBATCHFIELDS];
	BOOL m_bQuickIsLog[MAXBATCHFIELDS];
	BOOL m_bQuickIsSame[MAXBATCHFIELDS];	// FALSE=do all permutations
	int m_nQuickSteps[MAXNUMBEROFPERMUTES];
	int m_nTotalQuickSims;	// total number of simulations for quick batch
public:
	void GenerateQuickBatch();
public:
	int GetPreviousSim(void) { return m_simnum-1; }; // returns # of most recently completed sim, -1=none.
public:
	static BOOL IsResultsParameter(BatchParameter p);
    static BOOL IsSpatialParameter(BatchParameter p);
    static BOOL IsRegionParameter(BatchParameter p);
	static BatchParameter Str2BatchParam(CString paramname);
	static CString BatchParam2Str(BatchParameter bp);
	static CString GetUnitsStr(BatchParameter bp);
	static CString GetDescriptionStr(BatchParameter bp);
private:
	void ParseParameterName(CString fieldstr, int n);
    BatchParameter GetFieldType(int fieldnum);
    void SetValue(int fieldnum, double value);
    int GetRegionNumForField(int fieldnum);
    void SaveOriginalParams(CProblem *pProb);
    void RestoreOriginalParams(CProblem *pProb);
    void InvalidateBatchResults(void);
	static void SetParameterValue(CProblem *pProb, BatchParameter p, double value, int region_num);
	static double GetParameterValue(CProblem *pProb, BatchParameter p, int region_num);
	static double GetResultValue(CProblem *pProb, CBatch::BatchParameter p);
	static double GetSpatialResultValue(CProblem *pProb, CBatch::BatchParameter p, double xposition);
	static int FindStringInArray(CString str, char *strlist[]);
	void ShowProgress();
};  

// Define a postfix increment operator for Suit.
inline CBatch::BatchParameter operator++( CBatch::BatchParameter &rs, int )
{
    return rs = (CBatch::BatchParameter)(rs + 1);
}
  
