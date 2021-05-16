#include <cell.hpp>
#include <catchmentparameters.hpp>

void Cell::grid(LibGeoDecomp::GridBase<Cell, 2> *localGrid, LibGeoDecomp::Coord<2> globalDimensions, CatchmentParameters params)
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
		// Replace with more efficient bulk setting approach
		// using LibGeoDecomp::GridBase::saveMember() as also
		// used in e.g. LibGeoDecomp::PnetCDFInitializer
		Cell cell = localGrid->get(coordinate);
		cell.celltype = celltype;

		if(params.inundate_below_elevation)
		{
		    if(cell.elevation < params.init_water_surface_elevation)
		    {
			cell.water_depth = params.init_water_surface_elevation - cell.elevation;
			cell.water_surface_elevation = cell.elevation + cell.water_depth;
		    }
		}
		
		if(params.inundate_above_elevation)
		{
		    if(cell.elevation > params.init_lowest_inundated_elevation)
		    {
			cell.water_depth = params.init_water_depth_above_elevation;
			cell.water_surface_elevation = cell.elevation + cell.water_depth;
		    }
		}

		localGrid->set(coordinate, cell); 
	    }
	}
    }
}






