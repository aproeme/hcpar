#ifndef HC_CELL_H
#define HC_CELL_H

#include <libgeodecomp/misc/apitraits.h>
#include <libgeodecomp/storage/gridbase.h>

#define here_old neighborhood[LibGeoDecomp::FixedCoord<  0,  0 >()]
#define west_old neighborhood[LibGeoDecomp::FixedCoord< -1,  0 >()]
#define east_old neighborhood[LibGeoDecomp::FixedCoord<  1,  0 >()]
#define north_old neighborhood[LibGeoDecomp::FixedCoord< 0,  1 >()]
#define south_old neighborhood[LibGeoDecomp::FixedCoord< 0, -1 >()]


// forward declaration to resolve circular include dependency
class CatchmentParameters;

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
    enum CellType : int {
	INTERNAL=0,
	EDGE_WEST=1,
	EDGE_NORTH=2,
	EDGE_EAST=3,
	EDGE_SOUTH=4, 
	CORNER_NW=5,
	CORNER_NE=6,
	CORNER_SE=7,
	CORNER_SW=8,
	NODATA=9,
    };


    CellType celltype;
    double celltype_double;
    double elevation;
    double water_depth;
    double water_level;
    double qx, qy;
    double hflow;
    double hflow_threshold;
    double time_step;
    double DX, DY;
    double edgeslope;
    double no_data_value;
    double mannings;
    double froude_limit;
    double gravity = 9.8;

    
    Cell()
	{}
    

    
    Cell(double time_step,
	 double DX,
	 double DY,
	 double edgeslope,
	 double no_data_value,
	 double hflow_threshold,
	 double mannings,
	 double froude_limit,
	 CellType celltype = INTERNAL,
	 double celltype_double = 0.0,
	 double elevation = 0.0,
	 double water_depth = 0.0,
	 double water_level = 0.0,
	 double qx = 0.0,
	 double qy = 0.0) :
	time_step(time_step),
	DX(DX),
	DY(DY),
	edgeslope(edgeslope),
	no_data_value(no_data_value),
	hflow_threshold(hflow_threshold),
	mannings(mannings),
	froude_limit(froude_limit),
	celltype(celltype),
	celltype_double(celltype_double),
	elevation(elevation),
	water_depth(water_depth),
	water_level(water_level),
	qx(qx),
	qy(qy)
	{}
    

    static void grid(LibGeoDecomp::GridBase<Cell, 2> *localGrid, LibGeoDecomp::Coord<2> globalDimensions, CatchmentParameters params);
    
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

