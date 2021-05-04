#ifndef HC_NETCDFINITIALIZER_H
#define HC_NETCDFINITIALIZER_H

#include <libgeodecomp/io/pnetcdfinitializer.h>
#include <cell.hpp>
#include <catchmentparameters.hpp>

class NetCDFInitializer : public LibGeoDecomp::PnetCDFInitializer<Cell>
{
public:
    NetCDFInitializer(const CatchmentParameters params);
    
    void grid(LibGeoDecomp::GridBase<Cell, 2> *localGrid);
    
    
};

#endif
