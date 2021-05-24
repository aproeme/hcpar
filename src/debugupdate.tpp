#include <math.h>

template<typename COORD_MAP>
void Cell::update(const COORD_MAP& neighborhood, unsigned nanoStep)
{
    waterDepth = here.waterDepth + 1.0;
}




