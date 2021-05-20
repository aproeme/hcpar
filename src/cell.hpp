#ifndef HC_CELL_H
#define HC_CELL_H

//#include <libgeodecomp/communication/typemaps.h>
#include <libgeodecomp/misc/apitraits.h>
#include <libgeodecomp/storage/gridbase.h>


#define here neighborhood[LibGeoDecomp::FixedCoord<  0,  0 >()]
#define west neighborhood[LibGeoDecomp::FixedCoord< -1,  0 >()]
#define east neighborhood[LibGeoDecomp::FixedCoord<  1,  0 >()]
#define north neighborhood[LibGeoDecomp::FixedCoord< 0,  1 >()]
#define south neighborhood[LibGeoDecomp::FixedCoord< 0, -1 >()]


// forward declaration to resolve circular include dependency
class CatchmentParameters;


class Cell
{
    class API :
	public LibGeoDecomp::APITraits::HasFixedCoordsOnlyUpdate,
	public LibGeoDecomp::APITraits::HasStencil<LibGeoDecomp::Stencils::VonNeumann<2,1> >,
	public LibGeoDecomp::APITraits::HasCubeTopology<2>,
	public LibGeoDecomp::APITraits::HasCustomMPIDataType<Cell>,
	public LibGeoDecomp::APITraits::HasNanoSteps<4>
    {};
    
public:
    friend class Typemaps;

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
    double elevation = 0;
    double water_depth = 0;
    double water_level = 0;
    double qx = 0, qy = 0;
    double hflowx = 0, hflowy = 0;
    double hflow_threshold;
    double time_step;
    double DX, DY;
    double edgeslope;
    double no_data_value;
    double mannings;
    double froude_limit;
    static constexpr double gravity = 9.8;
    
    
    Cell()
	{}
        
    static void grid(LibGeoDecomp::GridBase<Cell, 2> *localGrid, LibGeoDecomp::Coord<2> globalDimensions, CatchmentParameters params);
    template<typename COORD_MAP> inline void update(const COORD_MAP& neighborhood, unsigned nanoStep);
 
    

#ifndef HC_DEBUG
    template<typename COORD_MAP> inline void initialiseGridValueUpdates(const COORD_MAP& neighborhood);
    template<typename COORD_MAP> inline void catchmentWaterInputs(const COORD_MAP& neighborhood);
    template<typename COORD_MAP> inline void flowRoute(const COORD_MAP& neighborhood);
    template<typename COORD_MAP> inline void flowRouteX(const COORD_MAP& neighborhood);
    template<typename COORD_MAP> inline void flowRouteY(const COORD_MAP& neighborhood);
    template<typename COORD_MAP> inline void updateQx(const COORD_MAP& neighborhood, double hflowx, double tempslope, double flow_time_step);
    template<typename COORD_MAP> inline void updateQy(const COORD_MAP& neighborhood, double hflowy, double tempslope, double flow_time_step);
    inline void updateQ(const double &q, double &q_new, double hflow, double tempslope, double flow_time_step);
    inline void froudeCheck(double &q, double hflow);
    template<typename COORD_MAP> inline void dischargeCheck(const COORD_MAP& neighborhood, double &q, double neighbour_water_depth, double Delta);
    template<typename COORD_MAP> inline void depthUpdate(const COORD_MAP& neighborhood);
    template<typename COORD_MAP> inline void updateWaterDepth(const COORD_MAP& neighborhood, double east_qx, double south_qy, double flow_time_step);
    inline double getFlowTimestep();
#endif
};


#ifdef HC_DEBUG
#include <debugupdate.tpp>
#else
#include <update.tpp>
#endif
#endif

