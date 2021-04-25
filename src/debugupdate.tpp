#include <math.h>

template<typename COORD_MAP>
void Cell::update(const COORD_MAP& neighborhood, unsigned nanoStep)
{
    water_depth = here_old.water_depth + 1.0;
}




