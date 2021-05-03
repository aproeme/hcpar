#ifndef HC_CELL_H
#define HC_CELL_H

#include <libgeodecomp/misc/apitraits.h>

/*
  #define here_old neighborhood[LibGeoDecomp::Coord<2>( 0, 0)]
  #define west_old neighborhood[LibGeoDecomp::Coord<2>(-1, 0)]
  #define east_old neighborhood[LibGeoDecomp::Coord<2>( 1, 0)]
  #define north_old neighborhood[LibGeoDecomp::Coord<2>( 0, -1)]
  #define south_old neighborhood[LibGeoDecomp::Coord<2>( 0,  1)]
*/

#define here_old neighborhood[LibGeoDecomp::FixedCoord<  0,  0 >()]
#define west_old neighborhood[LibGeoDecomp::FixedCoord< -1,  0 >()]
#define east_old neighborhood[LibGeoDecomp::FixedCoord<  1,  0 >()]
#define north_old neighborhood[LibGeoDecomp::FixedCoord< 0, -1 >()]
#define south_old neighborhood[LibGeoDecomp::FixedCoord< 0,  1 >()]

class Cell
{
    friend class Typemaps;
    
    class API :
	public LibGeoDecomp::APITraits::HasCustomMPIDataType<Cell>,
	public LibGeoDecomp::APITraits::HasFixedCoordsOnlyUpdate,
	public LibGeoDecomp::APITraits::HasStencil<LibGeoDecomp::Stencils::VonNeumann<2,1> >,
	public LibGeoDecomp::APITraits::HasCubeTopology<2>
    {};
    
    
public:
    enum CellType : int {INTERNAL=0,					\
			 EDGE_WEST=1, EDGE_NORTH=2, EDGE_EAST=3, EDGE_SOUTH=4, \
			 CORNER_NW=5, CORNER_NE=6, CORNER_SE=7, CORNER_SW=8, \
			 NODATA=9};
    
    CellType celltype;
    double elevation;
    double water_depth;
    double qx;
    double qy;
    
    // Constructor
    Cell(CellType celltype_in = INTERNAL,
	 double elevation_in = 0.0,
	 double water_depth_in = 0.0,
	 double qx_in = 0.0,
	 double qy_in = 0.0) :
	celltype(celltype_in),
	elevation(elevation_in),
	water_depth(water_depth_in),
	qx(qx_in),
	qy(qy_in)
	{}
    
    template<typename COORD_MAP> inline void update(const COORD_MAP& neighborhood, unsigned nanoStep);
};


#ifdef HC_DEBUG
#include "debugupdate.tpp"
#else
#include "update.tpp"
#endif

#endif

