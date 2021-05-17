#ifndef HC_SELECTMPIDATATYPE_H
#define HC_SELECTMPIDATATYPE_H

#include <typemaps.h>

template<typename Cell>
class LibGeoDecomp::APITraits::SelectMPIDataType<Cell>  
{
public:
  static inline MPI_Datatype value()
  {
    return MPI_CELL;  // MPI_CELL is initialized by custom-generated Typemaps::initializeMaps() for HAIL-CAESAR Cell class
  }
};

#endif
