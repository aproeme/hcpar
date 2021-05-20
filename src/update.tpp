#ifndef HC_UPDATE_H
#define HC_UPDATE_H

#include <hydrology.tpp>

// Overall update routine - this is what LibGeoDecomp calls each time
// step for each cell
//
// The update cycle for each physical timestep is separated into
// nanoStep subcycles as required by the need for synchronisation to
// ensure all cells can access updated Cell member values (e.g. grid
// variables such as elevation and water_depth) belonging to their
// neighbours.
//
// In general, when adding or modifying operations *within* an
// existing nanoStep, heed the guidance from LibGeoDecomp developers
// (taken from http://www.libgeodecomp.org/faq.html):
//
//   "When writing the update function of your cell, it's tempting to
//   read its members directly.  But LibGeoDecomp always keeps two
//   (logical) grids to prevent concurrent updates from getting in
//   each others way: you'll need to read from one grid and write to
//   the other. That means that you can only safely read from the new
//   grid (where "this" is currently pointing) if you have already
//   written those members in the current iterations. For similar
//   reasons to the point above, we do not copy old values to the new
//   grid by default: it's slow and in fact most applications can do
//   without. Solution: You should rather read the members of the last
//   time step's cells, which can be accessed through the neighborhood
//   object, and only read those members from "this" that have already
//   been written by the current invocation of update(). The 3D Jacobi
//   code in our repo demonstrates that a default copy from the last
//   time step is in deed not required, as it will overwrite its only
//   data member in any case. If everything else fails:
//   *this = hood[Coord()]; might help."
//
// After each nanoStep the neighborhood object is synchronised so that
// at the start of the next nanoStep it stores the state of the
// neighbouring cells at the end of the previous nanoStep. The first
// time the value of a Cell member variable (e.g. a grid variable such
// as elevation or water_depth) is needed within a nanoStep, one
// should therefore read its value using the neighborhood object
// (e.g. here.water_depth). If it is modified cumulatively in more
// than one place during a nanoStep, care should be taken that all
// modifications after the first one read the new value
// (e.g. water_depth) rather than the neighborhood object. 



template<typename COORD_MAP>
void Cell::update(const COORD_MAP& neighborhood, unsigned nanoStep)
{
    // Full update cycle for timestep completes once all nanosteps
    // complete. Each nanostep involves synchronisation, which impacts
    // performance, so only create a new nanostep if all cells
    // absolutely need to be able to access the updated grid values of
    // their neighbours.
    if(nanoStep == 0)
    {
	catchmentWaterInputs(neighborhood);	
    }
    
    // New nanostep because we want to route the flow resulting from
    // rainfall-updated water depths of this and neighbouring cells
    // computed in nanoStep 0, so need to synchronise across all cells
    // first.
    if(nanoStep == 1) 
    {
	std::cout << "nanostep == 1" << std::endl;
	flowRoute(neighborhood);
    }
    
    // New nanostep because we want to update the water depths based
    // on updated currents q in/out of neighbour cells computed in
    // previous nanostep
    if(nanoStep == 2)
    {
	depthUpdate(neighborhood);
    }
    if(nanoStep == 3)
    {
	// Water outputs from edges/catchment outlet 
	//water_flux_out(neighborhood);
    }
}


#endif
