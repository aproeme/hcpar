#include <netcdfinitializer.hpp>

NetCDFInitializer::NetCDFInitializer(const CatchmentParameters params) :
    LibGeoDecomp::PnetCDFInitializer<Cell>(params.netCDFSources, params.no_of_iterations),
    params(params)
{}

void NetCDFInitializer::grid(LibGeoDecomp::GridBase<Cell, 2> *localGrid)
{
    // Call grid() from LibGeoDecomp::PnetCDFInitializer base class
    // to initialize grid values (elevation, etc.) from netCDF file(s)
    LibGeoDecomp::PnetCDFInitializer<Cell>::grid(localGrid);
    
    // Call grid() from Cell class to initialize celltypes.
    // Relies on fact that globalDimensions is already set by
    // LibGeoDecomp::PnetCDFInitializer
    Cell::grid(localGrid, globalDimensions, params);
        
}

