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
#ifndef _INC_MATH
#include <math.h>
#endif
////////////////////////////////////////////////////////////////////////
// Complex

class Complex
{
public:
	double r;
	double i;
// Operations
public:
	Complex::Complex() : r(0), i(0) {}
	Complex::Complex(double r0, double i0) : r(r0), i(i0) {}
	Complex operator+(Complex);
	Complex operator-(Complex);
	Complex operator*(Complex);
	Complex operator/(Complex);
	static double mag2(Complex x);
	static Complex expj(double x);
	static Complex exp(Complex x);
};

#define FIND_ZERO( f, x ) \
	double f##_y, f##_dfdx, f##_dx; \
	if ( x != 0) f##_dx = x; else f##_dx=1; \
	for (int f##_i=0; f##_i<10; f##_i++) \
	{ 	f##_y = f(x); \
		if (f##_y!=0) \
		{	f##_dfdx = (f(x + 0.01*f##_dx)-f##_y)/(0.01*f##_dx); \
			if (f##_dfdx!=0) {f##_dx = -f##_y/f##_dfdx; x += f##_dx;} \
			else break; } else break;}

/////////////////////////////////////////////////////////////////////////////
// CMath

struct CMath
{
// Implementation
static double LinearInterp(double w, int arraysize,	const double x[], const double y[]);
static double LogInterp(double w, int arraysize, const double x[], const double y[]);
static void   LeastSquaresFit(int start, int stop, const double x[], const double y[],
								double& a, double& b, double& error);
static double round(double x, int k);
static double exp(double x);	// Like regular exp, but with over/under range protection
//inline static double exp(double x);	// Like regular exp, but with over/under range protection
static double log(double x);	// Like regular log, but with over/under range protection
static double erfc(double x);	// Set equal to 1.0 for x<=0
static double ParabolicInterp(double x0, const double x[3], const double y[3], double c[3]);
static void   CopyArray(int len, double srcarray[], double destarray[]);	// copies from p1 to p2
static void   NormalizeArray(int n, double x[]);
static double Min(int n, double x[]);
static double Max(int n, double x[]); 
static void   ArrayMinMax(int n, double x[], double &minval, double &maxval);
static double ArrayRmsDifference(int arraysize, const double *first, const double *second);
static double MaxAbs(int n, double x[]);
static int	  FindBoundingInterval(double x0, int arraysize, double x[]);
static double InterpMin(int n, const double y[]);
static double InterpMin(int n, const double x[], const double y[]);
static double InterpMax(int n, const double y[]);
static double InterpMax(int n, const double x[], const double y[]);
static double Intcpt(int arraysize, const double x[], const double y[]);
};

/**
inline double CMath::exp(double x)
{
	if (x>230) return 1e100;
	if (x<-230) return 1e-100;
	return ::exp(x);
}
**/