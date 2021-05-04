// Overall update routine - this is what LibGeoDecomp calls each time step for each cell
template<typename COORD_MAP>
void Cell::update(const COORD_MAP& neighborhood, unsigned nanoStep)
{
    initialise_grid_value_updates(neighborhood);
    // set_global_timefactor()  
    
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
    water_depth = water_depth + 0.01;
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
  double hflow;
  double tempslope;
  double west_elevation_old;
  double west_water_depth_old;
  double local_time_factor = 1.0; //set_local_timefactor();

  switch (celltype){
  case Cell::INTERNAL:
  case Cell::EDGE_NORTH:
  case Cell::EDGE_SOUTH:
    west_elevation_old = west_old.elevation;
    west_water_depth_old = west_old.water_depth;
    tempslope = ((west_elevation_old + west_water_depth_old) - (here_old.elevation + here_old.water_depth)) / Cell::DX;
    break;
  case Cell::EDGE_WEST:
  case Cell::CORNER_NW:
  case Cell::CORNER_SW:
    west_elevation_old = Cell::no_data_value;
    west_water_depth_old = 0.0;
    tempslope = Cell::edgeslope;
    break;
  case Cell::EDGE_EAST:
  case Cell::CORNER_NE:
  case Cell::CORNER_SE:
    west_elevation_old = west_old.elevation;
    west_water_depth_old = west_old.water_depth;
    tempslope = Cell::edgeslope;
    break;
  default:
    std::cout << "\n\n WARNING: no x-direction flow route rule specified for cell type " << celltype << "\n\n";
    break;
  }


  if (here_old.water_depth > 0 || west_water_depth_old > 0)
    {
      hflow = std::max(here_old.elevation + here_old.water_depth, west_elevation_old + west_water_depth_old) - std::max(here_old.elevation, west_elevation_old);
      if (hflow > Cell::hflow_threshold)
	{
	  update_qx(neighborhood, hflow, tempslope, local_time_factor);
	  froude_check(qx, hflow);
	  discharge_check(neighborhood, qx, west_water_depth_old, Cell::DX);
	}
      else
	{
	  qx = 0.0;
	  // qxs = 0.0;
	}
    }




  // calc velocity now
  //if (qx > 0)
  //  {
  //	vel_dir[7] = qx / hflow;
  //  }
  // refactor: old code tries to update vel_dir belonging to neighbour sites - can't do this because neighborhood is passed as a CONST. Should make flow_route only modify its own vel_dir components, based on neighbouring qx qy (i.e. flip around the current logic of modifying neigbouring vel_dir based on local qx qy)
  // But need all cells to record their hflow (or precompute and store -qx/hflow)
  /*	if (qx < 0)
  	{
  	west_old.vel_dir[3] = (0 - qx) / hflow;
  	}

  	}*/

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
  double hflow;
  double tempslope;
  double north_elevation_old;
  double north_water_depth_old;
  double local_time_factor = 1.0; //set_local_timefactor();

  switch (celltype){
  case Cell::INTERNAL:
  case Cell::EDGE_WEST:
  case Cell::EDGE_EAST:
    north_elevation_old = north_old.elevation;
    north_water_depth_old = north_old.water_depth;
    tempslope = ((north_elevation_old + north_water_depth_old) - (here_old.elevation + here_old.water_depth)) / Cell::DY;
    break;
  case Cell::EDGE_NORTH:
  case Cell::CORNER_NW:
  case Cell::CORNER_NE:
    north_elevation_old = Cell::no_data_value;
    north_water_depth_old = 0.0;
    tempslope = Cell::edgeslope;
    break;
  case Cell::EDGE_SOUTH:
  case Cell::CORNER_SW:
  case Cell::CORNER_SE:
    north_elevation_old = north_old.elevation;
    north_water_depth_old = north_old.water_depth;
    tempslope = Cell::edgeslope;
    break;
  default:
    std::cout << "\n\n WARNING: no y-direction flow route rule specified for cell type " << static_cast<int>(celltype) << "\n\n";
    break;
  }


  if (here_old.water_depth > 0 || north_water_depth_old > 0)
    {
      hflow = std::max(here_old.elevation + here_old.water_depth, north_elevation_old + north_water_depth_old) - std::max(here_old.elevation, north_elevation_old);
      if (hflow > Cell::hflow_threshold)
	{
	  update_qy(neighborhood, hflow, tempslope, local_time_factor);
	  froude_check(qy, hflow);
	  discharge_check(neighborhood, qy, north_water_depth_old, Cell::DY);
	}
      else
	{
	  qy = 0.0;
	  // qys = 0.0;
	}
    }




  // calc velocity now
  //if (qy > 0)
  //  {
  //vel_dir[1] = qy / hflow;
  // }
  //refactor: old code tries to update vel_dir belonging to neighbour sites - can't do this because neighborhood is passed as a CONST. Should make flow_route only modify its own vel_dir components, based on neighbouring qx qy (i.e. flip around the current logic of modifying neigbouring vel_dir based on local qx qy)
  // But need all cells to record their hflow (or precompute and store -qx/hflow)

  /*		if (qx < 0)
  		{
  		north_old.vel_dir[5] = (0 - qy) / hflow;
  		}

  		}*/
}



template<typename COORD_MAP>
void Cell::update_qx(const COORD_MAP& neighborhood, double hflow, double tempslope, double local_time_factor)
{
  update_q(here_old.qx, qx, hflow, tempslope, local_time_factor);
}


template<typename COORD_MAP>
void Cell::update_qy(const COORD_MAP& neighborhood, double hflow, double tempslope, double local_time_factor)
{
  update_q(here_old.qy, qy, hflow, tempslope, local_time_factor);
}


void Cell::update_q(const double &q_old, double &q_new, double hflow, double tempslope, double local_time_factor)
{
  q_new = ((q_old - (Cell::gravity * hflow * local_time_factor * tempslope)) \
	   / (1.0 + Cell::gravity * hflow * local_time_factor * (Cell::mannings * Cell::mannings) \
	      * std::abs(q_old) / std::pow(hflow, (10.0 / 3.0))));
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
    double local_time_factor = 1.0; //set_local_timefactor();
    double criterion_magnitude = std::abs(q * local_time_factor / Delta);

  if (q > 0 && criterion_magnitude > (here_old.water_depth / 4.0))
    {
      q = ((here_old.water_depth * Delta) / 5.0) / local_time_factor;
    }
  else if (q < 0 && criterion_magnitude > (neighbour_water_depth / 4.0))
    {
      q = -((neighbour_water_depth * Delta) / 5.0) / local_time_factor;
    }
}
  


// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// DEPTH UPDATE
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
template<typename COORD_MAP>
void Cell::depth_update(const COORD_MAP& neighborhood)
{
  double east_qx_old;
  double south_qy_old;

  double local_time_factor = 1.0; //set_local_timefactor();

  switch (celltype){
  case Cell::INTERNAL:
  case Cell::EDGE_NORTH:
  case Cell::EDGE_WEST:
  case Cell::CORNER_NW:
    east_qx_old = east_old.qx;
    south_qy_old = south_old.qy;
    update_water_depth(neighborhood, east_qx_old, south_qy_old, local_time_factor);
    break;
  case Cell::EDGE_EAST:
  case Cell::CORNER_NE:
    east_qx_old = 0.0;
    south_qy_old = south_old.qy;
    update_water_depth(neighborhood, east_qx_old, south_qy_old, local_time_factor);
    break;
  case Cell::EDGE_SOUTH:
  case Cell::CORNER_SW:
    east_qx_old = east_old.qx;
    south_qy_old = 0.0;
    update_water_depth(neighborhood, east_qx_old, south_qy_old, local_time_factor);
    break;
  case Cell::CORNER_SE:
    east_qx_old = 0.0;
    south_qy_old = 0.0;
    update_water_depth(neighborhood, east_qx_old, south_qy_old, local_time_factor);
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
void Cell::update_water_depth(const COORD_MAP& neighborhood, double east_qx_old, double south_qy_old, double local_time_factor)
{
    // add to water_depth already updated with rainfall during this time step in order to accumulate updates
    water_depth = water_depth + local_time_factor * ( (east_qx_old - here_old.qx)/Cell::DX + (south_qy_old - here_old.qy)/Cell::DY );
}


