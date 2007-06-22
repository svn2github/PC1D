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

// Hints passed to optimize redraws
const long HINT_EVERYTHINGCHANGED = 1;// make no assumptions - redo everything
const long HINT_PARAMSCHANGED = 2;	  // params have changed (but node data not modified)
const long HINT_GRAPHDATACHANGED = 3; // the node data has changed so graph must be redrawn 
const long HINT_ITERATION = 4;		// we're at the end of an iteration. User wants to see graph converging 
									// (same as HINT_GRAPHDATACHANGED, except node values don't make physical sense).
									// This hint is ONLY passed for iterations which didn't converge.
const long HINT_DIFFERENTGRAPH = 5; // a different graph has been selected, passes a CGraph
const long HINT_SWITCHTO4GRAPHS = 6; // we're about to change to 4graphs view.
const long HINT_DIFFERENTREGION = 7; // user changed the current region. Parmview should be updated.
const long HINT_SIMFINISHED = 8;     // we finished a complete simulation. This is the only time that batch
									 // parameters need to be recalculated during a run.
const long HINT_NEWGRAPHS = 9;		 // The definitions of active graph and 4graphs have been changed
									 // e.g. due to a .EXC file being loaded.
