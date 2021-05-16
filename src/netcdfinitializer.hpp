#ifndef HC_NETCDFINITIALIZER_H
#define HC_NETCDFINITIALIZER_H

#include <catchmentparameters.hpp>
#include <libgeodecomp/io/pnetcdfinitializer.h>

class NetCDFInitializer : public LibGeoDecomp::PnetCDFInitializer<Cell>
{
public:
    NetCDFInitializer(const CatchmentParameters params);
    
    void grid(LibGeoDecomp::GridBase<Cell, 2> *localGrid);
    
private:
    CatchmentParameters params;

};



#endif
