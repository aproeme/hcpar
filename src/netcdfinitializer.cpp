#include <netcdfinitializer.hpp>



NetCDFInitializer::NetCDFInitializer(const CatchmentParameters params)
    : LibGeoDecomp::PnetCDFInitializer<Cell>(params.dem_netcdf_file,
					     LibGeoDecomp::Selector<Cell>(&Cell::elevation, "elevation"),
					     params.dem_netcdf_variable,
					     params.no_of_iterations)
{
    
}

void NetCDFInitializer::grid(LibGeoDecomp::GridBase<Cell, 2> *localGrid)
{
    Cell::CellType celltype;
    LibGeoDecomp::CoordBox<2> localGridBoundingBox = localGrid->boundingBox();

    // Call grid() from base class LibGeoDecomp PnetCDFInitializer to initialize
    // grid values (elevation, etc.) from netCDF file(s)
    LibGeoDecomp::PnetCDFInitializer<Cell>::grid(localGrid);

    // Set cell types (edge, corner, etc.) for all cells in local grid
    for (int x=0; x<globalDimensions.x(); x++)
    {
	for (int y=0; y<globalDimensions.y(); y++)
	{
	    if (y == 0)
	    {
		if (x == 0) celltype = Cell::CORNER_NW;
		else if (x == globalDimensions.x()-1) celltype = Cell::CORNER_NE;
		else celltype = Cell::EDGE_NORTH;
	    }
	    else if (y == globalDimensions.y()-1)
	    {
		if (x == 0) celltype = Cell::CORNER_SW;
		else if (x == globalDimensions.x()-1) celltype = Cell::CORNER_SE;
		else celltype = Cell::EDGE_SOUTH;
	    }
	    else  
	    {
		if (x == 0) celltype = Cell::EDGE_WEST;
		else if (x == globalDimensions.x()-1) celltype = Cell::EDGE_EAST;
		else celltype = Cell::INTERNAL;
	    }
	}
    }
}


