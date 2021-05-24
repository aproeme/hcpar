#include <cell.hpp>
#include <catchmentparameters.hpp>

// Definitions (initialisations) of static variables declared in
// cell.hpp. These need to live outside of the Cell class declaration
// in order for linker to find resulting symbols, and if put in header
// file itself will lead to redefinition errors due to repeated
// inclusion of the header, so naturally live here in source file
// instead.
double Cell::timestep = 0.0;
double Cell::DX = 0.0;
double Cell::DY = 0.0;
double Cell::no_data_value = 0.0;
double Cell::edgeslope = 0.0;
double Cell::hflowThreshold = 0.0;
double Cell::mannings = 0.0;
double Cell::froudeLimit = 0.0;
double Cell::waterDepthErosionThreshold = 0.0;
bool Cell::rain_in_high_places = false;
double Cell::rain_above_elevation = 0.0;
double Cell::waterOut = 0.0;
double Cell::courantNumber = 0.0;
const double Cell::gravity = 9.8;

void Cell::grid(LibGeoDecomp::GridBase<Cell, 2> *localGrid, const LibGeoDecomp::Coord<2> globalDimensions, const CatchmentParameters& parameters)
{
    // Set static LISFLOOD catchment model parameters
    Cell::timestep = parameters.timestep;
    Cell::DX = parameters.DX;
    Cell::DY = parameters.DY;
    Cell::edgeslope = parameters.edgeslope;
    Cell::hflowThreshold = parameters.hflowThreshold;
    Cell::courantNumber = parameters.courantNumber;
    Cell::mannings = parameters.mannings;
    Cell::froudeLimit = parameters.froudeLimit;
    Cell::waterDepthErosionThreshold = parameters.waterDepthErosionThreshold;
        
    // Set cell types (edge, corner, etc.) for all cells in local grid
    Cell::CellType celltype; 
    LibGeoDecomp::CoordBox<2> localBoundingBox = localGrid->boundingBox();
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
				
		// Set grid quantities
		cell.celltype = celltype;
		cell.celltype_double = static_cast<double>(celltype);
		cell.rainRate = numericalRainRate(parameters.physicalRainRate);
		

                // Optionally set some specific (synthetic) initial
		// conditions for testing / convenience - these are
		// not mutually exclusive

		// Set uniform inundation up to a certain elevation
		if(parameters.inundate_below_elevation)
		{
		    if(cell.elevation < parameters.init_waterLevel)
		    {
			cell.waterDepth = parameters.init_waterLevel - cell.elevation;
		    }
		}
		
		// Initialise landscape with nonzero water depth above
		// a certain elevation
		if(parameters.inundate_above_elevation)
		{
		    if(cell.elevation > parameters.init_lowest_inundated_elevation)
		    {
			cell.waterDepth = parameters.init_waterDepth_above_elevation;
			cell.waterLevel = cell.elevation + cell.waterDepth;
		    }
		}

		// Set rain in high places
		if(parameters.rain_in_high_places)
		{
		    cell.rain_in_high_places = true;
		    cell.rain_above_elevation = parameters.rain_above_elevation;
		}
		
		// Always set water level (elevation of water surface) 
		if(cell.waterDepth > 0)
		{
		    cell.waterLevel = cell.elevation + cell.waterDepth;
		}

		localGrid->set(coordinate, cell); 
	    }
	}
    }
}






