#ifndef HC_DEBUGCELLINITIALIZER_H
#define HC_DEBUGCELLINITIALIZER_H

#include <tnt.h>
#include <libgeodecomp/io/simpleinitializer.h>
#include <libgeodecomp/storage/gridbase.h>
#include <cell.hpp>

using namespace TNT;

class DebugCellInitializer : public LibGeoDecomp::SimpleInitializer<Cell>
{
public:
  // constructor
  DebugCellInitializer(int xmax, int ymax, unsigned no_of_iterations);

  DebugCellInitializer(int xmax, int ymax, Array2D<double>& elevation_in, Array2D<double>& water_depth_in, unsigned no_of_iterations);
    
  void grid(LibGeoDecomp::GridBase<Cell, 2> *subgrid);

private:
  Array2D<double> elevation;
  Array2D<double> water_depth;
};

#endif

