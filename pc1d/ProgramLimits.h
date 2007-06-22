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
#ifndef PROGRAMLIMITS_H
#define PROGRAMLIMITS_H


const int PC1DVERSION = 53;			// Current PC1D version


// Global Constants
const int MAX_ELEMENTS = 500;		// Must be evenly divisible by MAX_REGIONS
const int MAX_TIME_STEPS = 200;		// Limited by size of CExcite
const int MAX_WAVELENGTHS = 200;	// 
const int MAX_REGIONS = 5;			// limited by menus in CDevice
const int MAX_LUMPED = 4;			// limited by dialog in CDevice
const int MAX_SOURCES = 3;			// limited by menus in CExcite
const int FIXED_NODE_ARRAY_SIZE = MAX_SOURCES+2*MAX_REGIONS+2*MAX_LUMPED;
const int MAX_LAYERS = 3;			// limited by dialog in CDevice

#endif