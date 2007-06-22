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
#ifndef PHYSICALCONSTANTS_H
#define PHYSICALCONSTANTS_H
// Global Constants
const double HC = 1239.8424;		// eV*nm
const double Q = 1.6021773E-19;		// C
const double VT300 = 0.025851483;	// V at 300 K
const double PI	= 3.14159265359;
const double EPS0 = 8.8541878E-14;	// F/cm
const double TKC = 273.15;			// kelvin to Celsius
const double SMALL_REAL = 1.0E-20;

// Global Enumerated Types
enum CMatType{N_TYPE, P_TYPE};
enum CTempUnit{KELVIN, CELSIUS};
enum CTexture{PLANAR_SURFACE, TEXTURED_SURFACE, LAMBERTIAN_SURFACE};
enum CMode{EQ_MODE, SS_MODE, TR_MODE};
enum CSurface{NEUTRAL_SURFACE, CHARGED_SURFACE, BARRIER_SURFACE};
enum CProfile{UNIFORM_PROFILE, EXPONENTIAL_PROFILE, GAUSSIAN_PROFILE, ERFC_PROFILE};
enum CAreaUnit{CM2, MM2, UM2};
enum CElemType{CONDUCTOR, DIODE, CAPACITOR};
enum CScaleType{LINEAR, LOG};
enum CSource{EMITTER, BASE, COLLECTOR};
enum CArrayType{SPACE_ARRAY, TIME_ARRAY, EXPERIMENTAL_ARRAY};

#endif