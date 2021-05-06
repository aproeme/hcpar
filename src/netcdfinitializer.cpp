#include <cell.hpp>
#include <netcdfinitializer.hpp>


NetCDFInitializer::NetCDFInitializer(const CatchmentParameters params)
    : LibGeoDecomp::PnetCDFInitializer<Cell>(params.dem_netcdf_file,
					     LibGeoDecomp::Selector<Cell>(&Cell::elevation, "elevation"),
					     params.dem_netcdf_variable,
					     params.no_of_iterations)
{}


void NetCDFInitializer::grid(LibGeoDecomp::GridBase<Cell, 2> *localGrid)
{
    //LibGeoDecomp::CoordBox<2> localGridBoundingBox = localGrid->boundingBox();
    
    // Call grid() from base class LibGeoDecomp PnetCDFInitializer to initialize
    // grid values (elevation, etc.) from netCDF file(s)
    LibGeoDecomp::PnetCDFInitializer<Cell>::grid(localGrid);
    
    // Call grid() from our Cell class to initialise cell types
    Cell::grid(localGrid, globalDimensions);
}


