#ifndef HC_GRIDQUANTITIES_H
#define HC_GRIDQUANTITIES_H
#include <vector>
#include <string>

#include <cell.hpp>

#include <libgeodecomp/storage/selector.h>

// Used to simplify and generalise simulation setup
// Combine these in unified way with Cell class to keep everything in one place?


// If modifying GridQuantity enum, always update Max to be equal to the highest-valued actual grid variable
// See https://stackoverflow.com/questions/2102582/how-can-i-count-the-items-in-an-enum
// celltype_double is for diagnosis
enum GridQuantity : int {
    elevation=0,
    water_depth=1,
    water_level=2,
    qx=3,
    qy=4,
    hflow=5,
    celltype_double=6,
    Max=celltype_double
};


static const int maxGridQuantities()
{
    return static_cast<int>(GridQuantity::Max) + 1;
}


static const std::vector<std::string> gridQuantityString = { "elevation", "water_depth", "water_level", "qx", "qy", "hflow", "celltype" };


// Selectors need to be provided to LibGeoDecomp, within which
// they are used internally to access and modify grid variables
static const std::vector<LibGeoDecomp::Selector<Cell>> gridQuantitySelectors = {
    LibGeoDecomp::Selector<Cell>(&Cell::elevation, "elevation"),
    LibGeoDecomp::Selector<Cell>(&Cell::water_depth, "water_depth"),
    LibGeoDecomp::Selector<Cell>(&Cell::water_level, "water_level"),
    LibGeoDecomp::Selector<Cell>(&Cell::qx, "qx"),
    LibGeoDecomp::Selector<Cell>(&Cell::qy, "qy"),
    LibGeoDecomp::Selector<Cell>(&Cell::hflow, "hflow"),
    LibGeoDecomp::Selector<Cell>(&Cell::celltype_double, "celltype")
};


static const LibGeoDecomp::Selector<Cell> gridQuantitySelector(GridQuantity quantity)
{
    return gridQuantitySelectors[static_cast<int>(quantity)];
}




#endif
