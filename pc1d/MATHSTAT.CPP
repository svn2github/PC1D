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
#include "math.h"
#include "mathstat.h"

/////////////////////////////////////////////////////////////////////////////
// mathstat functions

Complex Complex::operator+(Complex x)
{
	return Complex(r + x.r, i + x.i);
}

Complex Complex::operator-(Complex x)
{
	return Complex(r - x.r, i - x.i);
}

Complex Complex::operator*(Complex x)
{
	return Complex(r*x.r - i*x.i, r*x.i + i*x.r);
}

Complex Complex::operator/(Complex x)
{
	return Complex(
			(r*x.r + i*x.i)/(pow(x.r,2)+pow(x.i,2)),
			(i*x.r - r*x.i)/(pow(x.r,2)+pow(x.i,2)) );
}

double Complex::mag2(Complex x)
{
	return pow(x.r,2)+pow(x.i,2);
}

Complex Complex::expj(double x)
{
	return Complex(cos(x), -sin(x));
}

Complex Complex::exp(Complex x)
{
	return Complex(CMath::exp(x.r)*cos(x.i), -CMath::exp(x.r)*sin(x.i));
}

#ifdef OLD_LINEAR_INTERP

double CMath::LinearInterp(double w, int arraysize, const double x[], const double y[])
//	Returns an end value if w is outside the range of x. 
//	Assumes that x and y arrays are equally dimensioned to arraysize
{
	int k=0;
	while (x[k]<w && k<arraysize-1) k++;
	while (x[k]>w && k>0) k--;
	if (k==0 && w<x[k]) return y[k];
	if (k==arraysize-1 && w>x[k]) return y[k];
	if (x[k+1]==x[k]) return y[k];
	double f = (w-x[k])/(x[k+1]-x[k]);
	return y[k]+f*(y[k+1]-y[k]);
}
#else

// Calculates y(w) by linear interpolation of y[] as a function of x[]
//	Returns an end value if w is outside the range of x. 
//	Assumes that x and y arrays are equally dimensioned to arraysize
// Uses a binary chop to find the index to use. --- FAST!
// Works as long as X is monotonic (either increasing or decreasing)
double CMath::LinearInterp(double w, int arraysize, const double x[], const double y[])
{
	if (x[0]<x[arraysize-1]) {
		// x is increasing...
		// x[0]<=x[1]<=..<=x[arraysize-1]
		if (w<=x[0]) return y[0];
		if (w>=x[arraysize-1]) return y[arraysize-1];

		int k=0; int endk=arraysize-1;
		int newk;
		while (endk > k+1) {		// Loop invariant: x[k]<=w<x[endk]
			newk=(k+endk)/2;  // Note: if endk==k+1, newk=k, so no further progress is made.
			if ( x[newk]<=w ) k=newk; else endk=newk;
		}
		if (x[k+1]==x[k]) return y[k];
		double f = (w-x[k])/(x[k+1]-x[k]);
		return y[k]+f*(y[k+1]-y[k]);
	} else {
		// x is decreasing...
		if (w>=x[0]) return y[0];
		if (w<=x[arraysize-1]) return y[arraysize-1];

		int k=0; int endk=arraysize-1;
		int newk;
		while (endk > k+1) {		// Loop invariant: x[k] >= w > x[endk]
			newk=(k+endk)/2;  // Note: if endk==k+1, newk=k, so no further progress is made.
			if ( x[newk]>=w ) k=newk; else endk=newk;
		}
		if (x[k+1]==x[k]) return y[k];
		double f = (w-x[k])/(x[k+1]-x[k]);
		return y[k]+f*(y[k+1]-y[k]);
	}
}
#endif

double CMath::LogInterp(double w, int arraysize, const double x[], const double y[])
//	Returns an end value if w is outside the range of x.
//	Assumes that x and y arrays are equally dimensioned to arraysize
{
	int k=0;
	while (x[k]<w && k<arraysize-1) k++;
	while (x[k]>w && k>0) k--;
	if (k==0 && w<x[k]) return y[k];
	if (k==arraysize-1 && w>x[k]) return y[k];
	if (x[k+1]==x[k]) return y[k];
	double f = (w-x[k])/(x[k+1]-x[k]);
	if (y[k]<1e-100 || y[k+1]<1e-100) return y[k] + f*(y[k+1]-y[k]);
	else return y[k] * exp(f*log(y[k+1]/y[k]));
}

void CMath::LeastSquaresFit(int start, int stop, const double x[], const double y[],
						double& a, double& b, double& error)
//	Computes y=ax+b, and rms error, for indices between start & stop, inclusive
{
	double sumx=0, sumx2=0, sumy=0, sumy2=0, sumxy=0;
	int n, k;
	
	a = b = error = 0;
	n = stop-start+1;
	if (n==0) {b=y[start]; return;}
	for (k=start; k<=stop; k++)
	{
		sumxy += x[k]*y[k]; sumx2 += x[k]*x[k]; sumx += x[k]; 
		sumy += y[k]; sumy2 += y[k]*y[k];
	}
	a = (n*sumxy - sumx*sumy)/(n*sumx2 - sumx*sumx);
	b = (sumx2*sumy - sumxy*sumx)/(n*sumx2-sumx*sumx);
	for (k=start; k<=stop; k++)
		error += pow((y[k] - a*x[k] - b),2);
	if (error>0) error = sqrt(error/n);
}

// Calculate the rms error of the difference between two arrays 'first' and 'second'
// e.g. if the two arrays are identical, result will be 0.
double CMath::ArrayRmsDifference(int arraysize, const double *first, const double *second)
{
	if (arraysize==0) return 0;
	double sumsq=0;
	int i;
	for (i=0; i<arraysize; i++) {
		sumsq+=pow(first[i]-second[i], 2);
	}
	return sqrt(sumsq/arraysize);
}


double CMath::round(double x, int k)
{
	if (x==0) return 0;
	BOOL negative = (x<0);
	x = fabs(x);
	double intthislog = floor(log10(x));
	x = x / pow(10.0, intthislog);
	x = x * pow(10.0, k-1);
	if (x-(double)(int)x>0.5) x = (double)(int)x + 1;
	else x = (double)(int)x;
	x = x / pow(10.0,k-1);
	x = x * pow(10.0, intthislog);
	if (negative) return -x; else return x;
}


double CMath::exp(double x)
{
	if (x>230) return 1e100;
	if (x<-230) return 1e-100;
	return ::exp(x);
}

double CMath::log(double x)
{
	if (x==0) return -1e100;
	return ::log(fabs(x));
}

double CMath::erfc(double x)
{
	double t = 1/(1 + fabs(x)/2);
	double y = t * exp(-x*x - 1.26551223 + t*(1.00002368 + t*(0.37409196 +
        t*(0.09678418 + t*(-0.18628806 + t*(0.27886807 + t*(-1.13520398 +
        t*(1.48851587 + t*(-0.82215223 + t*0.17087277)))))))));
	if (x<=0) return 1; else return y;	// Pure Erfc is 2-y for x<0
}

double CMath::ParabolicInterp(double x0, const double x[3], const double y[3], double c[3])
{
	double A[3][3];
	double Det =  x[1]*x[2]*x[2] - x[2]*x[1]*x[1] 
				+ x[2]*x[0]*x[0] - x[0]*x[2]*x[2]
				+ x[0]*x[1]*x[1] - x[1]*x[0]*x[0];
	if (Det==0) return 0;
	double rDet=1.0/Det;
	A[0][0] = rDet*(x[1]*x[2]*x[2] - x[2]*x[1]*x[1]);
	A[1][0] = rDet*(x[1]*x[1] - x[2]*x[2]);
	A[2][0] = rDet*(x[2] - x[1]);
	A[0][1] = rDet*(x[2]*x[0]*x[0] - x[0]*x[2]*x[2]);
	A[1][1] = rDet*(x[2]*x[2] - x[0]*x[0]);
	A[2][1] = rDet*(x[0] - x[2]);
	A[0][2] = rDet*(x[0]*x[1]*x[1] - x[1]*x[0]*x[0]);
	A[1][2] = rDet*(x[0]*x[0] - x[1]*x[1]);
	A[2][2] = rDet*(x[1] - x[0]);
	c[0] = c[1] = c[2] = 0;
	for (int row=0; row<=2; row++) for (int col=0; col<=2; col++) 
		c[row] += A[row][col]*y[col];
	return c[0] + c[1]*x0 + c[2]*x0*x0;
}

void CMath::CopyArray(int len, double p1[], double p2[])
{
	for (int k=0; k<len; k++) p2[k] = p1[k];
}

void CMath::NormalizeArray(int n, double x[])
{
	double sum=0; int k;
	for (k=0; k<n; k++) sum += x[k];
	double reciprocalsum=1/sum;			// mult is MUCH faster than division! (5x faster on 486; more on Pentium).
	for (k=0; k<n; k++) x[k] *= reciprocalsum;
}

double CMath::Min(int n, double x[])
{
	double m = x[0];
	for (int k=1; k<n; k++) if (x[k]<m) m=x[k];
	return m;
}

double CMath::Max(int n, double x[])
{
	double m = x[0];
	for (int k=1; k<n; k++) if (x[k]>m) m=x[k];
	return m;
}

// Given an array x[] of length n, calculates the minimum and maximum value
// in the array.
void CMath::ArrayMinMax(int n, double x[], double &minval, double &maxval)
{
	minval=maxval = x[0];
	for (int k=1; k<n; k++) {
		if (x[k]<minval) minval=x[k];
		if (x[k]>maxval) maxval=x[k];
	}
}


// Same as Max, except takes abs value of each element, rather than the element itself.
double CMath::MaxAbs(int n, double x[])
{
	double m=fabs(x[0]);
	for (int k=1; k<n; k++) if (fabs(x[k])>m) m=fabs(x[k]);
	return m;
}

// returns index of x which is left side of interval containing x0.
// returns i such that x[i] <= x < x[i+1] OR x[i] >= x > x[i+1]
// assumes x[-1]=-infinity, x[arraysize]=+infinity
// returns a value from -1,0,..arraysize-1
int CMath::FindBoundingInterval(double x0, int arraysize, double x[])
{
	double endx=x[arraysize-1];
	double startx=x[0];
	int n;
	if (startx<=endx) {
		// x is increasing
		if (startx > x0) return -1;
		if (endx < x0) return arraysize-1;
		for (n=0; n<arraysize; n++) {
	        if (x[n]<= x0 && x0 < x[n+1]) return n;
		}
	} else {
		// x is decreasing
		if (startx < x0) return -1;
		if (endx > x0) return arraysize-1;
		for (n=0; n<arraysize; n++) {
	        if (x[n]>= x0 && x0 > x[n+1]) return n;
		}
	}
	return 0; // this should never happen!!
}

double CMath::InterpMin(int n, const double y[])
{	// returns parabolic minimum of y versus index, 0 if none found
	double xx[3], yy[3], c[3], x0;
	if (n<3) return 0;
	double ymin = y[0]; int kmin = 0;
	for (int k=1; k<n; k++) if (y[k]<ymin) {kmin = k; ymin = y[k];}
	if (kmin==0) kmin=1; else if (kmin==n-1) kmin=n-2;
	xx[0]=kmin-1; xx[1]=kmin; xx[2]=kmin+1;
	yy[0]=y[kmin-1]; yy[1]=y[kmin]; yy[2]=y[kmin+1];
	double y1 = ParabolicInterp(xx[1], xx, yy, c);
	if (c[2]!=0) x0 = -c[1]/(2*c[2]); else x0 = kmin;
	if (x0<0 || x0>n-1) return 0;	// don't extrapolate
	return c[0] + c[1]*x0 + c[2]*x0*x0; 
}	

double CMath::InterpMin(int n, const double x[], const double y[])
{	// returns parabolic minimum of y versus x, 0 if none found
	double xx[3], yy[3], c[3], x0;
	if (n<3) return 0;
	double ymin = y[0]; int kmin = 0;
	for (int k=1; k<n; k++) if (y[k]<ymin) {kmin = k; ymin = y[k];}
	if (kmin==0) kmin=1; else if (kmin==n-1) kmin=n-2;
	xx[0]=x[kmin-1]; xx[1]=x[kmin]; xx[2]=x[kmin+1];
	yy[0]=y[kmin-1]; yy[1]=y[kmin]; yy[2]=y[kmin+1];
	double y1 = ParabolicInterp(xx[1], xx, yy, c);
	if (c[2]!=0) x0 = -c[1]/(2*c[2]); else x0 = x[kmin];
	if (x0<x[0] || x0>x[n-1]) return 0;	// don't extrapolate
	return c[0] + c[1]*x0 + c[2]*x0*x0; 
}	

double CMath::InterpMax(int n, const double y[])
{	// returns parabolic maximum of y versus index, 0 if none found
	double xx[3], yy[3], c[3], x0;
	if (n<3) return 0;
	double ymax = y[0]; int kmax = 0;
	for (int k=1; k<n; k++) if (y[k]>ymax) {kmax = k; ymax = y[k];}
	if (kmax==0) kmax=1; else if (kmax==n-1) kmax=n-2;
	xx[0]=kmax-1; xx[1]=kmax; xx[2]=kmax+1;
	yy[0]=y[kmax-1]; yy[1]=y[kmax]; yy[2]=y[kmax+1];
	double y1 = ParabolicInterp(xx[1], xx, yy, c);
	if (c[2]!=0) x0 = -c[0]/(2*c[2]); else x0 = kmax;
	if (x0<0 || x0>n-1) return 0;	// don't extrapolate
	return c[0] + c[1]*x0 + c[2]*x0*x0; 
}	

double CMath::InterpMax(int n, const double x[], const double y[])
{	// returns parabolic maximum of y versus x, 0 if none found
	double xx[3], yy[3], c[3], x0;
	if (n<3) return 0;
	double ymax = y[0]; int kmax = 0;
	for (int k=1; k<n; k++) if (y[k]>ymax) {kmax = k; ymax = y[k];}
	if (kmax==0) kmax=1; else if (kmax==n-1) kmax=n-2;
	xx[0]=x[kmax-1]; xx[1]=x[kmax]; xx[2]=x[kmax+1];
	yy[0]=y[kmax-1]; yy[1]=y[kmax]; yy[2]=y[kmax+1];
	double y1 = ParabolicInterp(xx[1], xx, yy, c);
	if (c[2]!=0) x0 = -c[0]/(2*c[2]); else x0 = x[kmax];
	if (x0<x[0] || x0>x[n-1]) return 0;	// don't extrapolate
	return c[0] + c[1]*x0 + c[2]*x0*x0; 
}	

double CMath::Intcpt(int arraysize, const double x[], const double y[])
{
//	if (0<x[0] || 0>x[arraysize-1]) return 0;	// no intercept found
//	else return LinearInterp(0, arraysize, x, y);

	// It must cross the x-axis somewhere
	if (x[0]*x[arraysize-1]>0) 
		return 0; // Doesn't cross axis, so can't find intercept
	else return LinearInterp(0, arraysize, x, y);
}
