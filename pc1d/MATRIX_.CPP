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
// ------ MATRIX.CPP ------- Band Matrix linear equation solver.
//
// Decompose and solve a band matrix of bandwidth 11.
// Makes very efficient use of FPU stack to minimize bus accesses.
// This code was originally written for 8086/8087 by Greg Thorson in 1984.
// Converted to 32 bit and optimized for Pentium by Don Clugston in 1996.

// For an explanation of Pentium floating optimizations, see 
// 'Optimizing Matrix Math on the Pentium', H.W. Stockman - Dr Dobb's Journal, May 1994, p. 52-66
// 'Optimizing Pentium Code', Mike Schmit - DDJ Jan 1994, ,  p40-49.
// 'How to optimize for the Pentium Processors' - Agner Fog, 1997. www.agner.org


#include "stdafx.h"
#include "matrix.h"
#include "ProgramLimits.h"
#include <math.h>

// C routine is much slower but has been retained for testing purposes.
// Win 3.1 Timing results: Decom: C=162.8, asm=25.2, pentium=18.4 (Pentium,195 elements).
//						   Solve: C= 44.9, asm=    , pentium= 8.9
// With 32 bit compiler, C code is MUCH better than VC++ 1.5
// Win 95 timing: Decom+solve: 
//						Decom: C=235 asm=121		(K cycles, 161 elements)
//						Solve: C=120 asm=60

#define ASSEMBLER_SOLVE 1
#define ASSEMBLER_DECOM 1
//#undef ASSEMBLER_SOLVE
//#undef ASSEMBLER_DECOM


void Band_11_LU_Decompose(matrix_row * mat, int numelements);
void Band_11_Forward_Solve(matrix_row * mat, double *vect, int numelements);
void Band_11_Backward_Solve(double * matrixend, double *vectend, int numelements);


/////////////////////////////////////////////////////////////////////
// CMatrix

double *allocatedm;

CMatrix::CMatrix() 
{
	allocatedm = new double[(bw_1+1)*(MAX_ELEMENTS+1)*3];
//		m = new row[(MAX_ELEMENTS+1)*3];
//		v = new double[8+(MAX_ELEMENTS+1)*3];
		
		allocatedv = new double[32+(MAX_ELEMENTS+1)*3];

		// It's important to ensure that the matrix & vector are aligned for fastest memory access.
		// Fastest is when they're on an 16-byte boundary.
		
		// set v = vector of doubles (total 3 * (MAX_ELEMENTS+1) ), 
		// aligned to an 16-byte boundary. (ie v&0xF==0)
		// must save 'allocatedv' so it can be deleted.
		char * tmpv;
		tmpv=(char *)allocatedv; 
		while (((long)tmpv) & 0x1F ) tmpv++; 
		v=(double *)tmpv;
		tmpv=(char *)allocatedm; 
		while (((long)tmpv) & 0x1F ) tmpv++; 
		m=(row *)tmpv;
}

CMatrix::~CMatrix()
{
		delete [] allocatedm;
		delete [] allocatedv;
}


// --------------------------------------------------------
//				lu_decom  -- LU decompose a matrix
// --------------------------------------------------------
// Factors the matrix into its lower and upper triangles

#ifdef ASSEMBLER_DECOM
void CMatrix::LU_decompose()
{
	int n_1 = m_matrixsize-1;
	// do the main band
	Band_11_LU_Decompose(m, n_1 - 4);

	// now tidy up the corner...
	double T;
	int p, j, k;

	for (k=n_1-4; k<n_1; k++) {
		T = -1 / m[k][diag];
		for (p=1; p<=n_1-k; p++) m[k+p][diag-p] *= T;
		for (j=1; j<=n_1-k; j++)
		{
			T = m[k][diag+j];
			for (p=1; p<=n_1-k; p++)	m[k+p][diag+j-p] += T*m[k+p][diag-p];
		}

	}
}
#endif

// --------------------------------------------------------
//				lu_solve
// --------------------------------------------------------


#ifdef ASSEMBLER_SOLVE
void CMatrix::LU_solve()	// Solves the two triangular linear systems using 
							//  a front solve and then a back solve.
{
	int n_1 = m_matrixsize-1;
	int j, k, p;
	double T;

	Band_11_Forward_Solve(m, v, n_1-4);

	for (k=n_1-4; k<n_1; k++) {
		T=v[k];
		for (p=1; p<=n_1-k; p++) v[k+p]+=T*m[k+p][diag-p];
	}

// *************Back Solve*************

	double *matend= 11*n_1 + (double *)m;
	double *vectend= n_1 + (double *)v;

	Band_11_Backward_Solve(matend, vectend,  n_1-4);

	for (j=4; j>=0; j--) {
		v[j] /= m[j][diag];
		T = -v[j];
		for (p=j-1; p>=0; p--) v[p] += T*m[p][diag+j-p];
	}
}
#endif


// --------------------------------------------------------
//				Band_11_LU_Decompose
// --------------------------------------------------------
//
// Decompose a matrix of bandwidth 11.
// A band matrix has two parts: the band section (b) and the leftover corner (c)
//
//	  b-b-b				   b parts are shaped like  d---
//	  b	b-b-b			where d is the diagonal     |  
//	  b b b-b-b										|  d----
//		b b	b-b-b									   |
//		  b	b b-b-b									   |
//			b b	c-c
//            b c c
//
// This asm code does the 'b' parts. The 'c' parts need to be tidied up after 
// calling this function. There's no point writing them in asm, they don't need
// to be fast.

#pragma warning(disable: 4725) // FDIV is inaccurate on some Pentiums.
void Band_11_LU_Decompose(matrix_row * mat, int numelements)
{
	_asm {
											//   FOR k := 1 TO (n_1-4) DO
		mov    ebx, numelements
		mov    esi,mat

	  ///////////////////////
	  // Start of main loop...
	  // Each iteration takes 199 cycles on a Pentium
	  // + cache loads: ~50 cycles/iteration

decom_outer_loop:

		fld1
		fchs
		FDIV QWORD PTR [esi+40]             //     T := -1 / m[k-1,diag]
// fdiv takes a *long* time to complete (36 cycles). We can use this time to pre-load
// the CPU cache with the values we'll need.
	  mov eax, [esi+120]	// pre-load to hide cache stalls inside the FDIV latency
	  mov ecx, [esi+200]
	  mov eax, [esi+280]
	  mov ecx, [esi+360]
	  mov eax, [esi+440]
	  mov ecx, [esi+120+32]
	  mov eax, [esi+200+32]
	  mov ecx, [esi+280+32]
	  mov eax, [esi+360+32]
	  mov ecx, [esi+440+32]
	  mov eax, [esi+40+8*11]	// get ready for fdiv next time round!
		// STEP 2: do [d-1]*=T, [d-2]*=T, [d-3]*=T, [d-4]*=T, [d-5]*=T
		// where [d-1] = m[k,diag-1]  [d-3]=m[k+2,diag-3], ...

		FLD  QWORD PTR    [esi+120]         //     m[k,diag-1]   := T * m[k,diag-1]
		fmul st, st(1)
		FLD  QWORD PTR    [esi+200]         //     m[k+1,diag-2] := T * m[k+1,diag-2]
		fmul st, st(2)
		FLD  QWORD PTR    [esi+280]         //     m[k+2,diag-3] := T * m[k+2,diag-3]
		fmul st, st(3)
			fxch st(2)	// d1 d2 d3 T
			FSTP  QWORD PTR  [esi+120]      //     m[k,diag-1]   := T * m[k,diag-1]
		FLD  QWORD PTR    [esi+360]         //     m[k+3,diag-4] := T * m[k+3,diag-4]
		fmul st, st(3)
		fxch st(3)		// T d2 d3 d4
		fmul QWORD PTR    [esi+440]         //     m[k+4,diag-5] := T * m[k+4,diag-5]
		fxch st(2) // d3 d2 d5 d4
			FSTP  QWORD PTR  [esi+280]         //     m[k+2,diag-3] := T * m[k+2,diag-3]
			FSTP  QWORD PTR  [esi+200]         //     m[k+1,diag-2] := T * m[k+1,diag-2]
			FSTP  QWORD PTR  [esi+440]         //     m[k+4,diag-5] := T * m[k+4,diag-5]
			FSTP  QWORD PTR  [esi+360]         //     m[k+3,diag-4] := T * m[k+3,diag-4]
	
	// reduce loop overhead by counting UP to 0.
	mov edx, -5

decom_inner_loop:

	fld  QWORD PTR [esi+120]			// [d-1]			  
	fmul QWORD PTR [esi+edx*8+40 +8*6]	// *T				T = m[k-1,diag+j]
	fld  QWORD PTR [esi+edx*8+40 +8*6]	// T*[d-2]
	fmul QWORD PTR [esi+200]
	fld  QWORD PTR [esi+edx*8+40 +8*6]	// T*[d-3]
	fmul QWORD PTR [esi+280]
		fxch st(2)	// d1 d2 d3
		FADD  QWORD PTR [esi+edx*8+120 +8*6]    //       m[k,diag+j-1]   += T*m[k,diag-1]
		fxch st(1)	// d2 D1 d3
		FADD  QWORD PTR [esi+edx*8+200 +8*6]    //       m[k+1,diag+j-2] += T*m[k+1,diag-2]
    FLD   QWORD PTR [esi+edx*8+40 +8*6]	// T*[d-5]
	fmul QWORD PTR [esi+440]  // d5 D2 D1 d3
			fxch st(2) // D1 D2 d5 d3
			FSTP  QWORD PTR [esi+edx*8+120 +8*6]
    FLD   QWORD PTR [esi+edx*8+40 +8*6]	// T*[d-4]
 	fmul QWORD PTR [esi+360]   // d4 D2 d5 d3
		fxch st(3)	// d3 D2 d5 d4
		FADD  QWORD PTR [esi+edx*8+280 +8*6]   //       m[k+2,diag+j-3] += T*m[k+2,diag-3]
		fxch st(2) // d5 D2 D3 d4
		FADD  QWORD PTR [esi+edx*8+440 +8*6]   //       m[k+4,diag+j-5] += T*m[k+4,diag-5]
		fxch st(3)	// d4 D2 D3 D5
		FADD  QWORD PTR [esi+edx*8+360 +8*6]   //       m[k+3,diag+j-4] += T*m[k+3,diag-4]
			fxch st(2)	// D3 D2 D4 D5
			FSTP  QWORD PTR [esi+edx*8+280 +8*6]	// save D3
			FSTP  QWORD PTR [esi+edx*8+200 +8*6]	// save D2
			FSTP  QWORD PTR [esi+edx*8+360 +8*6]	// save D4
			FSTP  QWORD PTR [esi+edx*8+440 +8*6]	// save D5
	inc edx
	jne decom_inner_loop

	  add esi, 8*11
      dec    ebx
      jnz    decom_outer_loop
	}
}

// --------------------------------------------------------
//				Band_11_Forward_Solve
// --------------------------------------------------------

void Band_11_Forward_Solve(matrix_row * mat, double *vect, int numelements)
{
	_asm {
										 ;   FOR k := 1 TO (n_1-4) DO
      mov    ecx, numelements
      mov    edi, vect			            
      mov    esi, mat            
      FLD  QWORD PTR    [edi]             ;   {v[0]}
      FLD  QWORD PTR    [edi+8]           ;   {v[1]}
      FLD  QWORD PTR    [edi+8*2]         ;   {v[2]}
      FLD  QWORD PTR    [edi+8*3]         ;   {v[3]}
      FLD  QWORD PTR    [edi+8*4]         ;   {v[4]}

	  ///////////////////////
	  // Start of forward loop...
	  // Each iteration should take 23 cycles on a Pentium
	  // + cache loads: ?? cycles/iteration

      // start of loop:  T v0 v1 v2 v3 
pfor_k:
      FLD  QWORD PTR    [edi+8*5]          ;     BEGIN
                                            ;     T := v[k-1]
								// T v0 v1 v2 v3 v4 
      FLD  QWORD PTR    [esi+88+4*8]            ;     v[k]   +=  T*m[k,diag-1]
      fmul   ST(0),ST(6)		// T v0 v1 v2 v3 v4  T*m0
	  FLD QWORD PTR [esi+2*88+3*8]        ;     v[k+1] += T*m[k+1,diag-2]
	  fmul ST(0), ST(7)		// T v0 v1 v2 v3 v4  T*m0  T*m1

  	  fxch ST(1)
 	  faddp ST(6), ST(0)		// T V0 v1 v2 v3 v4  T*m1
	  FLD  QWORD PTR    [esi+3*88+2*8]            ;     v[k+2] += T*m[k+2,diag-3]
	  fmul   ST(0),ST(7)  	// T V0 v1 v2 v3 v4 T*m1 T*m2
 	  fxch st(1)
	  faddp ST(5), ST(0)		// T V0 V1 v2 v3 v4 T*m2
	  FLD QWORD PTR [esi+4*88+8]      ;     v[k+3] += T*m[k+3,diag-4]
 	  fmul ST(0), ST(7)			// T V0 V1 v2 v3 v4 T*m2 T*m3 

	  fxch ST(1)
	  faddp ST(4), ST(0)	// T V0 V1 V2 v3 v4 T*m3

	  FLD  QWORD PTR    [esi+5*88]            ;     v[k+4] += T*m[k+4,diag-5]
	  fmul   ST(0),ST(7)      // T V0 V1 V2 v3 v4 T*m3 T*m4
      fxch st(1)					
	  faddp st(3), st(0)		// T V0 V1 V2 V3 v4 T*m4
		  fld ST(6)
		  FSTP QWORD PTR [edi]
		  ffree ST(6)			// V0 V1 V2 V3 v4 T*m4
		faddp ST(1), ST(0)      // V0 V1 V2 V3 V4

      add    esi,88                          ;
      add    edi,8                           ;
	  dec ecx
      jnz   pfor_k                          ;     END;

	  // End of forward loop
  	  ///////////////////////
									
	  // now must free all registers -- 5 are in use
								// V0 V1 V2 V3 V4
	  fstp QWORD PTR [edi+8*4]
	  fstp QWORD PTR [edi+8*3]
	  fstp QWORD PTR [edi+8*2]
	  fstp QWORD PTR [edi+8*1]
	  fstp QWORD PTR [edi]
	}
}

// --------------------------------------------------------
//				Band_11_Backward_Solve
// --------------------------------------------------------

void Band_11_Backward_Solve(double * matrixend, double *vectend, int numelements)
{
	_asm {
                                            ;   {*************Back Solve*************}
                                            ;   FOR j := n_1 DOWNTO 5 DO
	mov edi, vectend					  ; point to last element of vector
	mov esi, matrixend					  ; point to last row of matrix
	mov ecx, numelements

      FLD  QWORD PTR    [edi]              ;     {v[n_1]}
      FLD  QWORD PTR    [edi-8]            ;     {v[n_1-1]}
      FLD  QWORD PTR    [edi-2*8]          ;     {v[n_1-2]}
      FLD  QWORD PTR    [edi-3*8]          ;     {v[n_1-3]}
      FLD  QWORD PTR    [edi-4*8]          ;     {v[n_1-4]}

   	  /////////////////////////////
	  // Start of back solve loop...
	  // Each iteration should take 24+33=57 cycles on a Pentium
	  // + ?? cache misses
									// v0 v1 v2 v3 v4
pfor_j:
      FLD  QWORD PTR    [esi+40]             ;    v[j] /= m[j,diag];
      fdivp  ST(5),ST(0)                     ;    T := v[j];
			// As with decom, we use div latency to prime the cache
	  mov ebx, [esi-120]
	  mov eax, [esi-200]
	  mov ebx, [esi-280]
	  mov eax, [esi-360]
	  mov eax, [edi-40]
	  mov ebx, [esi-48]  // next fdiv
								// T v1 v2 v3 v4

      FLD    QWORD PTR    [esi-40]           ;    v[j-1] -= T*m[j-1,diag+1];
      fmul   ST(0),ST(5)		// T v1 v2 v3 v4  m1*T
      FLD    QWORD PTR    [esi-120]          ;    v[j-2] -= T*m[j-2,diag+2]
      fmul   ST(0),ST(6)
	  fxch st(1)
	  fsubp st(5), st(0)		// T V1 v2 v3 v4  m2*T
      FLD  QWORD PTR    [esi-200]         ;    v[j-3] := v[j-3] - T*m[j-3,diag+3]
      fmul   ST(0),ST(6)
	  fxch st(1)
	  fsubp st(4), st(0)		// T V1 V2 v3 v4  m3*T

      FLD  QWORD PTR    [esi-280]         ;    v[j-4] := v[j-4] - T*m[j-4,diag+4]
      fmul   ST(0),ST(6)
	  fxch st(1)
      fsubp  ST(3),ST(0)		 // T V1 V2 V3 v4 m4*T
      FLD  QWORD PTR    [esi-360]         ;    v[j-5] := v[j-5] - T*m[j-5,diag+5]
      fmul   ST(0),ST(6)
	  fxch st(1)
	  fsubp st(2), st(0)		 // T V1 V2 V3 V4   m5*T
	  	fld ST(5)
		FSTP QWORD PTR [EDI]
		ffree ST(5)				 // V1 V2 V3 V4  m5*T
	  fsubr QWORD PTR    [edi-40]         ;  v[j-5]
								// V1 V2 V3 V4 V5
	  sub    esi,88
      sub    edi,8
	  dec ecx
      jnz   pfor_j

	  // End of back solve loop
   	  /////////////////////////////
									
	  // now must free all registers -- 5 are in use
								// V0 V1 V2 V3 V4
	  fstp QWORD PTR [edi-8*4]
	  fstp QWORD PTR [edi-8*3]
	  fstp QWORD PTR [edi-8*2]
	  fstp QWORD PTR [edi-8*1]
	  fstp QWORD PTR [edi]
	}
}

// ------------------------------------------------------------
//		 C++ versions of solve + decom, for reference
// ------------------------------------------------------------

#ifndef ASSEMBLER_DECOM
void CMatrix::LU_decompose()	// Factors the matrix into its lower and upper triangles
{
	int n_1=m_matrixsize-1;
	// do the main band
	int j, k;
	double T;
	for (k=1; k<=n_1-4; k++)
	{
	    T = -1 / m[k-1][diag];
	    m[k][diag-1]   *= T;
	    m[k+1][diag-2] *= T;
	    m[k+2][diag-3] *= T;
	    m[k+3][diag-4] *= T;
	    m[k+4][diag-5] *= T;
	    for (j=1; j<=5; j++)
	    {
	        T = m[k-1][diag+j];
	        m[k][diag+j-1]   += T*m[k][diag-1];
	        m[k+1][diag+j-2] += T*m[k+1][diag-2];
	        m[k+2][diag+j-3] += T*m[k+2][diag-3];
	        m[k+3][diag+j-4] += T*m[k+3][diag-4];
	        m[k+4][diag+j-5] += T*m[k+4][diag-5];
		}
	}
	// now tidy up the corner...
	int p, q;
	for (q=4; q>=1; q--) {
		T = -1 / m[n_1-q][diag];
		for (p=1; p<=q; p++) {
			m[n_1+p-q][diag-p] *= T;
		}
		for (j=1; j<=q; j++)
		{
			T = m[n_1-q][diag+j];
			for (p=1; p<=q; p++) {
				m[n_1+p-q][diag+j-p] += T*m[n_1+p-q][diag-p];
			}
		}
	}
  
#endif

      
#ifndef ASSEMBLER_SOLVE      
void CMatrix::LU_solve()	// Solves the two triangular linear systems using 
							//  a front solve and then a back solve.
{
	int n_1=m_matrixsize-1;
	int j, k;
	double T;
	for (k=0; k<n_1-4; k++)
	{
	    T = v[k];
	    v[k+1] += T*m[k+1][diag-1];
	    v[k+2] += T*m[k+2][diag-2];
	    v[k+3] += T*m[k+3][diag-3];
	    v[k+4] += T*m[k+4][diag-4];
	    v[k+5] += T*m[k+5][diag-5];
	}
	int p;
	for (k=n_1-4; k<n_1; k++) {
		T=v[k];
		for (p=1; p<=n_1-k; p++) v[k+p]+=T*m[k+p][diag-p];

	}

// *************Back Solve*************
	for (j=n_1; j>=5; j--)
	{
    	v[j] /= m[j][diag];
	    T = -v[j];
	    v[j-1] += T*m[j-1][diag+1];
	    v[j-2] += T*m[j-2][diag+2];
	    v[j-3] += T*m[j-3][diag+3];
	    v[j-4] += T*m[j-4][diag+4];
	    v[j-5] += T*m[j-5][diag+5];
	}

	for (j=4; j>=0; j--) {
		v[j] /= m[j][diag];
		T = -v[j];
		for (p=j-1; p>=0; p--) v[p] += T*m[p][diag+j-p];

	}
}
#endif

/**********
// C was 6 cycles/operation
void C_Sum_Mul(double *a, double *b, int n, double mulval)
{
	double sum=a[0];
	int k;
	for (k=1; k<=n; k++) sum-=a[k]*b[k];
	a[0]=sum*mulval;
}

// 13 cycles for 6 operations
//newfrond=(double*)( ((int)newfrond+31)&0xFFFFffe0 );
// about 1% of calls have a and b wanting the same cache set-value
void Asm_Sum_Mul(double *a, double *b, int n, double mulval)
{
	_asm {
		mov edi, a
		mov esi, b
		mov eax, n
		cmp eax, 0
		jle nothingtodo
		xor ecx, ecx
		sub ecx, eax
		fld QWORD PTR [edi-8]
		lea edi, [edi+8*eax]
		lea esi, [esi+8*eax]

		test al,1
		jz SHORT sumeven
		fld QWORD PTR [esi+ecx*8]
		fmul QWORD PTR [edi+ecx*8]
		fsubp st(1), st
		inc ecx
sumeven:
		test al,2
		jz SHORT extraeven
		fld QWORD PTR [esi+ecx*8]
		fmul QWORD PTR [edi+ecx*8]
		fld QWORD PTR [esi+ecx*8+8]
		fmul QWORD PTR [edi+ecx*8+8]
		fxch st(1)
		fsubp st(2), st
		fsubp st(1), st
		add ecx,2
extraeven:
		test ecx, ecx
		jz done
		jmp SHORT sum2

sumloop:
		fsubp st(1), st
sum2:
		fld QWORD PTR [esi+ecx*8]
		fmul QWORD PTR [edi+ecx*8]
		fld QWORD PTR [esi+ecx*8+8]
		fmul QWORD PTR [edi+ecx*8+8]
		fxch st(1)
		fsubp st(2), st
		fld QWORD PTR [esi+ecx*8+8*2]
		fmul QWORD PTR [edi+ecx*8+8*2]
		fxch st(1)
		fsubp st(2), st
		fld QWORD PTR [esi+ecx*8+8*3]
		fmul QWORD PTR [edi+ecx*8+8*3]
		fxch st(1)
		fsubp st(2), st
		add ecx,4
		jnz sumloop
		fsubp st(1), st
done:
		mov edi, a
		fmul mulval
		fstp QWORD PTR [edi-8]
nothingtodo:
	}
}
************/

/******
                            
//	CMatrix::IterativelyImproveSolution()                            
// This idea comes from Numerical Recipes in C, second edition, pp 55-58.
// Section 2.5 Iterative Improvement of a Solution to Linear Equations
// Basic theory: Suppose x is solution of linear set 
//					A.x=b.				(1)
// Only know a slightly wrong solution x+dx, where dx is the unknown error. When mult by A, gives
//		 			A.(x+dx)=b+db		(2)
//	(2)-(1)			A.dx=db				(3)
// Subst (2) in (3) A.dx=A.(x+dx)-b		(4)     
// Solve (4) for dx, then x'=(x+dx)-dx is an improved solution.
// In this case, originalmatrix=A, originalvector=b,
//  mat = LU decomposition of A, v=x+dx,
// origmat.dx = origmat.v-b
// Let r = A.(x+dx)-b = originalmatrix.v - originalvector.
// To solve this, lu_decom(m, n_1);
//				  lu_solve(m, r, n_1);
// But matrix has already been decomposed, so only solve() is required.

void CMatrix::IterativelyImproveSolution(row __huge *originalmatrix, int n_1, double *originalvector)
{
	double *r = new double[ (MAX_ELEMENTS+1)*3];
	int i,j;
	long double sdp;
	
	for (i=0; i<=n_1; i++) {
		sdp = -originalvector[i];		// calculate the right hand side, accumulating the residual
		for (j=0; j<=bw_1; j++) 
			if (i-diag+j>=0 && i-diag+j<=n_1) 
				sdp+=originalmatrix[i][j]*v[i-diag+j];
		r[i]=sdp;
	}
	pentium_lu_solve(m,r,n_1);			// solve for the error term
	for (i=0; i<=n_1; i++) v[i]-=r[i];    // and subtract it from the old solution
	delete [] r;
}

CMatrix & CMatrix::operator =(CMatrix &mat)
{
	int i, j, n;
	n=(MAX_ELEMENTS+1)*3;
	for (i=0; i<n; i++) for (j=0; j<=bw_1+1; j++) m[i][j]=mat.m[i][j]; 
	for (i=0; i<n; i++) v[i] = mat.v[i];
	return *this;
}

****/

/*******************
void CMatrix::CorrectForSparseElements(int n_1)
// Updates the solve value to include
// sparse elements (elements which aren't on the band diagonal)
// Uses the Woodbury Formula
//            t  -1      -1     -1          t  -1    -1  t  -1
//   ( A + U.V  )    =  A  - [ A  .U.( 1 + V .A  .U )  .V .A   ]
//
//         -1             t  -1
// Put Z= A  .U, H = 1 + V .A  .U    and b is the problem we want to solve
//                t -1     -1         -1       -1
//    x'= (A + U.V )  b = A  b - [Z. H  .Vt] (A  .b)
//              -1
//   we know x=A  b from our LU decomposition. we want x'
//                -1
//  So x'= x- [Z.H  .Vt].x
//
// This routine comes from Numerical Recipes in C, 2nd Edition, p.75-77
// I use an additional trick: because r = M-1.v is solution of M.r=v,
// use LUdecom + solve to calculate H-1.Vt (it isn't necessary to
//  explicitly generate the inverse of H).
// V[j]=unit vector sparsecol[j]. (e.g. [0 0 0 0 0 1 0 0 0] for sparsecol=5)
{
    if (m_NumberOfSparseElements==0) return;

    CSquareMatrix H(m_NumberOfSparseElements);
    int i,j;

	CVector Z[MAX_SPARSE_ELEMENTS];
    for (i=0; i<m_NumberOfSparseElements; i++)
    	Z[i].SetDimension(n_1+1);

    for (i=0; i< m_NumberOfSparseElements; i++) {
        Z[i].MakeZero();
        Z[i][ m_sparserow[i]   ]=m_sparseval[i][0];
        Z[i][ m_sparserow[i]+1 ]=m_sparseval[i][1];
        Z[i][ m_sparserow[i]+2 ]=m_sparseval[i][2];

        pentium_lu_solve(m,&Z[i][0],n_1);               // Z= A-1.U

        for (j=0; j<m_NumberOfSparseElements; j++) {
            H[i][j]=Z[i][m_sparsecol[j]];				// H= Vt.A-1.U
        }
        H[i][i]++;                                     // H= (1+Vt.A-1.U)
    }
    // Now H = 1 + Vtranspose . Z

    CVector inbrack(m_NumberOfSparseElements);
    for (i=0; i< m_NumberOfSparseElements; i++) {
        inbrack[i]=v[m_sparsecol[i]];             // Vtranspose.x
    }

    H.LUDecompose();
    H.LUSolve(inbrack);                   // inbrack=H-1.(Vt.x)

	double sum;
    for (i=0; i<=n_1; i++) {
        sum=0;
        for (j=0; j<m_NumberOfSparseElements; j++) {
            sum+= Z[j][i]*inbrack[j];
        }
        v[i]-=sum;                          // x' = x - Z. H-1.Vt.x
    }
}

void CMatrix::Add3SparseElements(int row, int colm, double val0, double val1, double val2)
// put extra values at [row][col], [r+1][c], [r+2][c]
{
    m_sparserow[m_NumberOfSparseElements]=row;
    m_sparsecol[m_NumberOfSparseElements]=colm;
    m_sparseval[m_NumberOfSparseElements][0]=val0;
    m_sparseval[m_NumberOfSparseElements][1]=val1;
    m_sparseval[m_NumberOfSparseElements][2]=val2;
    m_NumberOfSparseElements++;
    ASSERT(m_NumberOfSparseElements<MAX_SPARSE_ELEMENTS);
}

//--------------------------------------------------------------------------
// CVector
//

CVector::CVector()
{
    n=1;
    data=new double[n];
    data[0]=0;
}


CVector::CVector(int numberofelements)
{
    n=numberofelements;
    data=new double[n];
    MakeZero();
}

CVector::~CVector()
{
    delete [] data;
}

void CVector::SetDimension(int numberofelements)
{
    if (n==numberofelements) return; // don't change size if not required.
    if (data) delete [] data;
    n=numberofelements;
    data=new double[n];
}

void CVector::MakeZero()
{
    int i;
    for (i=0; i<n; i++) 
    	data[i]=0;
}

//--------------------------------------------------------------------------
// CSquareMatrix
//

CSquareMatrix::CSquareMatrix(int matrixsize)
{
	n=matrixsize;
	for (int i=0; i<n; i++) m[i]=new double[n];
    indx=new int[n];
    for (i=0; i<n; i++) indx[i]=i;
}

CSquareMatrix::~CSquareMatrix()
{
    for (int i=0; i<n; i++) delete [] m[i];
    delete [] indx;
}

void CSquareMatrix::mult(CSquareMatrix & mat1, CVector &vect, CVector &resultvect)
{
    int n=mat1.n;
    ASSERT(vect.n==n);
    ASSERT(resultvect.n==n);

    int i,j;
    double sum;
    for (i=0; i<n; i++) {
        sum=0;
        for (j=0; j<n; j++)  sum+=mat1.m[i][j]*vect.data[j];
        resultvect.data[i]=sum;
    }
}

void CSquareMatrix::LUDecompose(void)
{
    ludecom(m, n, indx);
}

void CSquareMatrix::LUSolve(CVector &vect)
{
    ASSERT(n==vect.n);
    lusolve(m, n, indx, vect.data);
}

void CSquareMatrix::ludecom(double **a, int n, int *indx)
{
     int i, imax, j,k;
     double *scaling = new double[n];   // implicit scaling of each row
     double big, dum, sum, temp;

     for (i=0; i<n; i++) {
        big=0;
        for (j=0; j<n; j++) if ( (temp = fabs(a[i][j]) ) > big) big=temp;
        ASSERT (big!=0);    // All elements zero
        scaling[i]=1/big;
    }

                    // loop over columns of Crout's method
    for (j=0; j<n; j++) {
        for (i=0; i<j; i++) {
            sum=a[i][j];
            for (k=0; k<i; k++) sum-=a[i][k]*a[k][j];
            a[i][j]=sum;
        }
        big=0;                  // search for largest pivot element
        for (i=j; i<n; i++) {
            sum=a[i][j];
            for (k=0; k<j; k++)  sum-=a[i][k]*a[k][j];
            a[i][j]=sum;
            if ( (dum=scaling[i]*fabs(sum)) > big ) {
                // is figure of merit for pivot better than the rest so far?
                big=dum;
                imax=i;
            }
        }
        if (j!=imax) {     // Do we need to interchange rows?
            for (k=0; k<n; k++) {
                dum=a[imax][k];
                a[imax][k]=a[j][k];
                a[j][k]=dum;
            }
            scaling[imax]=scaling[j];
        }
        indx[j]=imax;
        if (a[j][j]==0) a[j][j] = 1e-20;    // cope with singular matrices
        if (j!=n-1) {
            dum=1/a[j][j];
            for (i=j+1; i<n; i++) a[i][j]*=dum;
        }
    }
    delete [] scaling;
}

void CSquareMatrix::lusolve(double **a, int n, int *indx, double b[])
{
    int i,ii=-1, ip, j;    // ii is index of first non-zero element of b.
    double sum;

    for (i=0; i<n; i++) {   // forward substitution
        ip=indx[i];
        sum=b[ip];
        b[ip]=b[i];             // unscramble the permutation as we go.
        if (ii>=0)
            for (j=ii; j<=i-1; j++) sum-=a[i][j]*b[j];
        else if (sum) ii=i;
        b[i]=sum;
    }
    for (i=n-1;i>=0;i--) {  // back substitution
		sum=b[i];
        for (j=i+1;j<n;j++) sum -= a[i][j]*b[j];
		b[i]=sum/a[i][i];
	}
}

*************/