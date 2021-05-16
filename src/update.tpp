// Overall update routine - this is what LibGeoDecomp calls each time step for each cell
template<typename COORD_MAP>
void Cell::update(const COORD_MAP& neighborhood, unsigned nanoStep)
{
  initialise_grid_value_updates(neighborhood);
  
  // Add water to the catchment from rainfall input file
  catchment_waterinputs(neighborhood);
  
  // Distribute the water with the LISFLOOD Cellular Automaton algorithm
  flow_route_x(neighborhood);
  flow_route_y(neighborhood);
  
  // Calculate the new water depths in the catchment
  depth_update(neighborhood);
  
  // Water outputs from edges/catchment outlet 
  //water_flux_out(neighborhood);
}






// This function initialises new grid values based on their old values
// from previous time step as read through the neighborhood
// object. This is necessary so that multiple other functions can
// modify these values *in any order* by always reading from and
// adding to the new value.
template<typename COORD_MAP>
void Cell::initialise_grid_value_updates(const COORD_MAP& neighborhood)
{
  water_depth = here_old.water_depth;
}


// Add water to the catchment from rainfall input file
template<typename COORD_MAP>
void Cell::catchment_waterinputs(const COORD_MAP& neighborhood) 
{
    //water_depth = water_depth + 0.0001;
}



// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// THE WATER ROUTING ALGORITHM: LISFLOOD-FP
//
//           X direction
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
template<typename COORD_MAP>
void Cell::flow_route_x(const COORD_MAP& neighborhood)
{
  double tempslope;
  double west_elevation_old;
  double west_water_depth_old;
  double flow_timestep = get_flow_timestep();
  
  switch (celltype){
  case Cell::INTERNAL:   // excludes any corner or edge
  case Cell::EDGE_NORTH: // excludes Northern corners
  case Cell::EDGE_SOUTH: // excludes Southern corners
    west_elevation_old = west_old.elevation;
    west_water_depth_old = west_old.water_depth;
    tempslope = ((west_elevation_old + west_water_depth_old) - (here_old.elevation + here_old.water_depth)) / Cell::DX;
    break;
  case Cell::EDGE_WEST:
  case Cell::CORNER_NW: 
  case Cell::CORNER_SW:
    west_elevation_old = 0.0; // set to zero rather than NODATA value as per original HAIL-CAESAR code
    west_water_depth_old = 0.0; 
    tempslope = 0.0 - Cell::edgeslope; // corresponds to x == 1 in original HAIL-CAESAR code
    break;
  case Cell::EDGE_EAST:
  case Cell::CORNER_NE:
  case Cell::CORNER_SE:
    west_elevation_old = west_old.elevation; 
    west_water_depth_old = west_old.water_depth; 
    tempslope = Cell::edgeslope; // corresponds to x == imax in original HAIL-CAESAR code
    break;
  default:
    std::cout << "\n\n WARNING: no x-direction flow route rule specified for cell type " << celltype << "\n\n";
    break;
  }


  if (here_old.water_depth > 0 || west_water_depth_old > 0)  // still deal with west_old.elevation == NODATA
    {
      double hflow = std::max(here_old.elevation + here_old.water_depth, west_elevation_old + west_water_depth_old) - std::max(here_old.elevation, west_elevation_old);

      if (hflow > Cell::hflow_threshold)
	{
	  update_qx(neighborhood, hflow, tempslope, flow_timestep);
	  froude_check(qx, hflow);
	  discharge_check(neighborhood, qx, west_water_depth_old, Cell::DX);
	}
      else
	{
	  qx = 0.0;
	  // qxs = 0.0; 
	}
    }
}



// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// THE WATER ROUTING ALGORITHM: LISFLOOD-FP
//
//           Y direction
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
template<typename COORD_MAP>
void Cell::flow_route_y(const COORD_MAP& neighborhood)
{
  double tempslope;
  double south_elevation_old;
  double south_water_depth_old;
  double flow_timestep = get_flow_timestep();

  switch (celltype){
  case Cell::INTERNAL: // excludes any corner or edge
  case Cell::EDGE_WEST: // excludes Western corners
  case Cell::EDGE_EAST: // excludes Eastern corners
    south_elevation_old = south_old.elevation;
    south_water_depth_old = south_old.water_depth;
    tempslope = ((south_elevation_old + south_water_depth_old) - (here_old.elevation + here_old.water_depth)) / Cell::DY;
    break;
  case Cell::EDGE_SOUTH:
  case Cell::CORNER_SW:
  case Cell::CORNER_SE:
    south_elevation_old = 0.0; // set to zero rather than NODATA, as per original HAIL-CAESAR code
    south_water_depth_old = 0.0;
    tempslope = Cell::edgeslope; // corresponds to y == jmax in original HAIL-CAESAR code
    break;
  case Cell::EDGE_NORTH:
  case Cell::CORNER_NW:
  case Cell::CORNER_NE:
    south_elevation_old = south_old.elevation;
    south_water_depth_old = south_old.water_depth;
    tempslope = 0.0 - Cell::edgeslope; // corresponds to y == 1 in original HAIL-CAESAR code
    break;
  default:
    std::cout << "\n\n WARNING: no y-direction flow route rule specified for cell type " << static_cast<int>(celltype) << "\n\n";
    break;
  }


  if (here_old.water_depth > 0 || south_water_depth_old > 0) // still deal with south_old.elevation == NODATA
    {
      double hflow = std::max(here_old.elevation + here_old.water_depth, south_elevation_old + south_water_depth_old) - std::max(here_old.elevation, south_elevation_old);
      
      if (hflow > Cell::hflow_threshold)
	{
	  update_qy(neighborhood, hflow, tempslope, flow_timestep);
	  froude_check(qy, hflow);
	  discharge_check(neighborhood, qy, south_water_depth_old, Cell::DY);
	}
      else
	{
	  qy = 0.0;
	  // qys = 0.0;
	}
    }
}



template<typename COORD_MAP>
void Cell::update_qx(const COORD_MAP& neighborhood, const double hflow, const double tempslope, const double flow_timestep)
{
    std::cout << "update_qx" << std::endl;
    update_q(here_old.qx, qx, hflow, tempslope, flow_timestep);
}


template<typename COORD_MAP>
void Cell::update_qy(const COORD_MAP& neighborhood, const double hflow, const double tempslope, const double flow_timestep)
{
  update_q(here_old.qy, qy, hflow, tempslope, flow_timestep);
}


void Cell::update_q(const double &q_old, double &q_new, const double hflow, const double tempslope, const double flow_timestep)
{
  q_new = ((q_old - (Cell::gravity * hflow * flow_timestep * tempslope)) \
	   / (1.0 + Cell::gravity * hflow * flow_timestep * (Cell::mannings * Cell::mannings) \
	      * std::abs(q_old) / std::pow(hflow, (10.0 / 3.0))));

  std::cout << "q_new: " << q_new << std::endl;
}
    

// FROUDE NUMBER CHECK
// need to have these lines to stop too much water moving from
// one cell to another - resulting in negative discharges
// which causes a large instability to develop
// - only in steep catchments really
void Cell::froude_check(double &q, double hflow)
{
  if ((std::abs(q / hflow) / std::sqrt(Cell::gravity * hflow)) > Cell::froude_limit) // correctly reads newly calculated value of q, not here_old.q
    {
      q = std::copysign(hflow * (std::sqrt(Cell::gravity*hflow) * Cell::froude_limit), q);
    }
}


// DISCHARGE MAGNITUDE/TIMESTEP CHECKS
// If the discharge is too high for this timestep, scale back...
template<typename COORD_MAP>
void Cell::discharge_check(const COORD_MAP& neighborhood, double &q, double neighbour_water_depth, double Delta)
{
  double flow_timestep = get_flow_timestep();
  double criterion_magnitude = std::abs(q * flow_timestep / Delta);

  if (q > 0 && criterion_magnitude > (here_old.water_depth / 4.0))
    {
      q = ((here_old.water_depth * Delta) / 5.0) / flow_timestep;
    }
  else if (q < 0 && criterion_magnitude > (neighbour_water_depth / 4.0))
    {
      q = -((neighbour_water_depth * Delta) / 5.0) / flow_timestep;
    }
}
  

double Cell::get_flow_timestep()
{
    return Cell::time_step;
}


// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// DEPTH UPDATE
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
template<typename COORD_MAP>
void Cell::depth_update(const COORD_MAP& neighborhood)
{
  double east_qx_old;
  double north_qy_old;
  double flow_timestep = get_flow_timestep();
  
  switch (celltype)
  {
  case Cell::INTERNAL:
  case Cell::EDGE_SOUTH:
  case Cell::EDGE_WEST:
  case Cell::CORNER_SW:
      east_qx_old = east_old.qx;
      north_qy_old = north_old.qy;
      update_water_depth(neighborhood, east_qx_old, north_qy_old, flow_timestep);
      break;
  case Cell::EDGE_EAST:
  case Cell::CORNER_SE:
      east_qx_old = 0.0;
      north_qy_old = north_old.qy;
      update_water_depth(neighborhood, east_qx_old, north_qy_old, flow_timestep);
      break;
  case Cell::EDGE_NORTH:
  case Cell::CORNER_NW:
      east_qx_old = east_old.qx;
      north_qy_old = 0.0;
      update_water_depth(neighborhood, east_qx_old, north_qy_old, flow_timestep);
      break;
  case Cell::CORNER_NE:
      east_qx_old = 0.0;
      north_qy_old = 0.0;
      update_water_depth(neighborhood, east_qx_old, north_qy_old, flow_timestep);
      break;
  case Cell::NODATA:
      water_depth = 0.0;
      break;
  default:
      std::cout << "\n\n WARNING: no depth update rule specified for cell type " << Cell::CellType(int(celltype))<< "\n\n";
      break;
  }
  
  
  
  
}


template<typename COORD_MAP>
void Cell::update_water_depth(const COORD_MAP& neighborhood, double east_qx_old, double north_qy_old, double flow_timestep)
{
    // add to water_depth already updated with rainfall during this time step in order to accumulate updates
    water_depth = water_depth + flow_timestep * ( (east_qx_old - here_old.qx)/Cell::DX + (north_qy_old - here_old.qy)/Cell::DY );
    
    if(water_depth > 0)
    {
	water_surface_elevation = elevation + water_depth;
    }
}


