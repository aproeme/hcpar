#include <cell.hpp>
#include <catchmentparameters.hpp>


void Cell::grid(LibGeoDecomp::GridBase<Cell, 2> *localGrid, LibGeoDecomp::Coord<2> globalDimensions, CatchmentParameters parameters)
{
    Cell::CellType celltype; 
    LibGeoDecomp::CoordBox<2> localBoundingBox = localGrid->boundingBox();
    
    // Set cell types (edge, corner, etc.) for all cells in local grid
    for (int x=0; x<globalDimensions.x(); x++)
    {
	for (int y=0; y<globalDimensions.y(); y++)
	{
	    if (y == 0)
	    {
		if (x == 0) celltype = Cell::CORNER_SW;
		else if (x == globalDimensions.x()-1) celltype = Cell::CORNER_SE;
		else celltype = Cell::EDGE_SOUTH;
	    }
	    else if (y == globalDimensions.y()-1)
	    {
		if (x == 0) celltype = Cell::CORNER_NW;
		else if (x == globalDimensions.x()-1) celltype = Cell::CORNER_NE;
		else celltype = Cell::EDGE_NORTH;
	    }
	    else  
	    {
		if (x == 0) celltype = Cell::EDGE_WEST;
		else if (x == globalDimensions.x()-1) celltype = Cell::EDGE_EAST;
		else celltype = Cell::INTERNAL;
	    }


	    LibGeoDecomp::Coord<2> coordinate(x, y);
	    if (localBoundingBox.inBounds(coordinate))
	    {
		Cell cell = localGrid->get(coordinate);
		

                // SET LISFLOOD CATCHMENT MODEL PARAMETERS
		cell.time_step = parameters.time_step;
		cell.DX = parameters.DX;
		cell.DY = parameters.DY;
		cell.edgeslope = parameters.edgeslope;
		cell.hflow_threshold = parameters.hflow_threshold;
		cell.mannings = parameters.mannings;
		cell.froude_limit = parameters.froude_limit;
		
		// SET GRID QUANTITIES
		cell.celltype = celltype;
		cell.celltype_double = static_cast<double>(celltype);
		
		// Optionally set some specific (synthetic) initial
		// conditions for testing / convenience - these are
		// not mutually exclusive

		// Set uniform inundation up to a certain elevation
		if(parameters.inundate_below_elevation)
		{
		    if(cell.elevation < parameters.init_water_level)
		    {
			cell.water_depth = parameters.init_water_level - cell.elevation;
		    }
		}
		
		// Initialise landscape with nonzero water depth above
		// a certain elevation
		if(parameters.inundate_above_elevation)
		{
		    if(cell.elevation > parameters.init_lowest_inundated_elevation)
		    {
			cell.water_depth = parameters.init_water_depth_above_elevation;
			cell.water_level = cell.elevation + cell.water_depth;
		    }
		}

		// Always set water level (elevation of water surface) 
		if(cell.water_depth > 0)
		{
		    cell.water_level = cell.elevation + cell.water_depth;
		}

		localGrid->set(coordinate, cell); 
	    }
	}
    }
}






