#include <math.h>

template<typename COORD_MAP>
void Cell::update(const COORD_MAP& neighborhood, unsigned nanoStep)
{
    water_depth = here.water_depth + 1.0;
}




