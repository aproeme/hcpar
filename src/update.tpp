template<typename COORD_MAP>
void Cell::update(const COORD_MAP& neighborhood, unsigned nanoStep)
{
    water_depth = here_old.water_depth + 0.01*(west_old.water_depth + north_old.water_depth);
}




