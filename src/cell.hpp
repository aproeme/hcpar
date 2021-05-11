#ifndef HC_CELL_H
#define HC_CELL_H

#include <libgeodecomp/misc/apitraits.h>
#include <libgeodecomp/storage/gridbase.h>

#define here_old neighborhood[LibGeoDecomp::FixedCoord<  0,  0 >()]
#define west_old neighborhood[LibGeoDecomp::FixedCoord< -1,  0 >()]
#define east_old neighborhood[LibGeoDecomp::FixedCoord<  1,  0 >()]
#define north_old neighborhood[LibGeoDecomp::FixedCoord< 0,  1 >()]
#define south_old neighborhood[LibGeoDecomp::FixedCoord< 0, -1 >()]

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
			 NODATA=9, GHOST_BORDER=10};
    
    CellType celltype;
    double elevation;
    double water_depth;
    double qx, qy;
    double time_step;
    double DX, DY;
    double edgeslope;
    double no_data_value;
    double hflow_threshold;
    double mannings;
    double froude_limit;
    double gravity = 9.8;
    
    Cell(CellType celltype_in = INTERNAL,
	 double elevation_in = 0.0,
	 double water_depth_in = 0.0,
	 double qx_in = 0.0,
	 double qy_in = 0.0,
	 double time_step = 1.0,
	 double DX = 1.0,
	 double DY = 1.0,
	 double edgeslope = 1.0,
	 double no_data_value=-9999,
	 double hflow_threshold=1.0,
	 double mannings = 1.0,
	 double froude_limit = 1.0) :
	celltype(celltype_in),
	elevation(elevation_in),
	water_depth(water_depth_in),
	qx(qx_in),
	qy(qy_in),
	time_step(time_step),
	DX(DX),
	DY(DY),
	edgeslope(edgeslope),
	no_data_value(no_data_value),
	hflow_threshold(hflow_threshold),
	mannings(mannings),
	froude_limit(froude_limit)
	{}
    
    
    static void grid(LibGeoDecomp::GridBase<Cell, 2> *localGrid, LibGeoDecomp::Coord<2> globalDimensions);
    // static because other classes (initializers) should be able to call Cell::grid() without instantiating a Cell
    
    template<typename COORD_MAP> inline void update(const COORD_MAP& neighborhood, unsigned nanoStep);
    
#ifndef HC_DEBUG
  template<typename COORD_MAP> inline void initialise_grid_value_updates(const COORD_MAP& neighborhood);
  template<typename COORD_MAP> inline void catchment_waterinputs(const COORD_MAP& neighborhood);
  template<typename COORD_MAP> inline void flow_route_x(const COORD_MAP& neighborhood);
  template<typename COORD_MAP> inline void flow_route_y(const COORD_MAP& neighborhood);
  template<typename COORD_MAP> inline void update_qx(const COORD_MAP& neighborhood, double hflow, double tempslope, double flow_time_step);
  template<typename COORD_MAP> inline void update_qy(const COORD_MAP& neighborhood, double hflow, double tempslope, double flow_time_step);
  inline void update_q(const double &q_old, double &q_new, double hflow, double tempslope, double flow_time_step);
  inline void froude_check(double &q, double hflow);
  template<typename COORD_MAP> inline void discharge_check(const COORD_MAP& neighborhood, double &q, double neighbour_water_depth, double Delta);
  template<typename COORD_MAP> inline void depth_update(const COORD_MAP& neighborhood);
  template<typename COORD_MAP> inline void update_water_depth(const COORD_MAP& neighborhood, double east_qx_old, double south_qy_old, double flow_time_step);
  inline double get_flow_timestep();
#endif
    
    
    
};


#ifdef HC_DEBUG
#include "debugupdate.tpp"
#else
#include "update.tpp"
#endif

#endif

