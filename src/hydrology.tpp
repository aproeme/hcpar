#ifndef HC_HYDROLOGY_H
#define HC_HYDROLOGY_H

// Add water to the catchment from rainfall input file
template<typename COORD_MAP>
void Cell::catchmentWaterInputs(const COORD_MAP& neighborhood) 
{
    // rain in high places
    if(here.elevation > 240)
    {
	water_depth = here.water_depth + 0.0001;
    }
}



template<typename COORD_MAP>
void Cell::flowRoute(const COORD_MAP& neighborhood)
{
    flowRouteX(neighborhood);
    flowRouteY(neighborhood);
}



// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// THE WATER ROUTING ALGORITHM: LISFLOOD-FP
//
//           X direction
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
template<typename COORD_MAP>
void Cell::flowRouteX(const COORD_MAP& neighborhood)
{
  double tempslope;
  double west_elevation;
  double west_water_depth;
  double flow_timestep = getFlowTimestep();
  
  switch (celltype){
  case INTERNAL:   // excludes any corner or edge
  case EDGE_NORTH: // excludes Northern corners
  case EDGE_SOUTH: // excludes Southern corners
    west_elevation = west.elevation; 
    west_water_depth = west.water_depth; 
    tempslope = ((west_elevation + west_water_depth) - (here.elevation + here.water_depth)) / DX;
    break;
  case EDGE_WEST:
  case CORNER_NW: 
  case CORNER_SW:
    west_elevation = 0.0; // set to zero rather than NODATA value as per original HAIL-CAESAR code
    west_water_depth = 0.0; 
    tempslope = 0.0 - edgeslope; // corresponds to x == 1 in original HAIL-CAESAR code
    break;
  case EDGE_EAST:
  case CORNER_NE:
  case CORNER_SE:
    west_elevation = west.elevation; 
    west_water_depth = west.water_depth; 
    tempslope = edgeslope; // corresponds to x == imax in original HAIL-CAESAR code
    break;
  default:
      std::cout << "\n\n WARNING: no x-direction flow route rule specified for cell type " << static_cast<int>(celltype) << "\n\n";
    break;
  }
  
  
  if (here.water_depth > 0 || west_water_depth > 0)  // still deal with west.elevation == NODATA
    {
	hflowx = std::max(here.elevation + here.water_depth, west_elevation + west_water_depth) - std::max(here.elevation, west_elevation);

      if (hflowx > hflow_threshold)
	{
	  updateQx(neighborhood, hflowx, tempslope, flow_timestep);
	  froudeCheck(qx, hflowx);
	  dischargeCheck(neighborhood, qx, west_water_depth, DX);
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
void Cell::flowRouteY(const COORD_MAP& neighborhood)
{
  double tempslope;
  double south_elevation;
  double south_water_depth;
  double flow_timestep = getFlowTimestep();

  switch (celltype){
  case INTERNAL: // excludes any corner or edge
  case EDGE_WEST: // excludes Western corners
  case EDGE_EAST: // excludes Eastern corners
    south_elevation = south.elevation;
    south_water_depth = south.water_depth;
    tempslope = ((south_elevation + south_water_depth) - (here.elevation + here.water_depth)) / DY;
    break;
  case EDGE_SOUTH:
  case CORNER_SW:
  case CORNER_SE:
    south_elevation = 0.0; // set to zero rather than NODATA, as per original HAIL-CAESAR code
    south_water_depth = 0.0;
    tempslope = edgeslope; // corresponds to y == jmax in original HAIL-CAESAR code
    break;
  case EDGE_NORTH:
  case CORNER_NW:
  case CORNER_NE:
    south_elevation = south.elevation;
    south_water_depth = south.water_depth;
    tempslope = 0.0 - edgeslope; // corresponds to y == 1 in original HAIL-CAESAR code
    break;
  default:
    std::cout << "\n\n WARNING: no y-direction flow route rule specified for cell type " << static_cast<int>(celltype) << "\n\n";
    break;
  }

  
  if (here.water_depth > 0 || south_water_depth > 0) // still deal with south.elevation == NODATA
    {
      hflowy = std::max(here.elevation + here.water_depth, south_elevation + south_water_depth) - std::max(here.elevation, south_elevation);
      
      if (hflowy > hflow_threshold)
	{
          updateQy(neighborhood, hflowy, tempslope, flow_timestep);
	  froudeCheck(qy, hflowy);
	  dischargeCheck(neighborhood, qy, south_water_depth, DY);
	}
      else
	{
	  qy = 0.0;
	  // qys = 0.0;
	}
    }
}



template<typename COORD_MAP>
void Cell::updateQx(const COORD_MAP& neighborhood, const double hflowx, const double tempslope, const double flow_timestep)
{
    updateQ(here.qx, qx, hflowx, tempslope, flow_timestep);
}


template<typename COORD_MAP>
void Cell::updateQy(const COORD_MAP& neighborhood, const double hflowy, const double tempslope, const double flow_timestep)
{
  updateQ(here.qy, qy, hflowy, tempslope, flow_timestep);
}


void Cell::updateQ(const double &q, double &q_new, const double hflow, const double tempslope, const double flow_timestep)
{
  q_new = ((q - (gravity * hflow * flow_timestep * tempslope)) \
	   / (1.0 + gravity * hflow * flow_timestep * (mannings * mannings) \
	      * std::abs(q) / std::pow(hflow, (10.0 / 3.0))));
}
    

// FROUDE NUMBER CHECK
// need to have these lines to stop too much water moving from
// one cell to another - resulting in negative discharges
// which causes a large instability to develop
// - only in steep catchments really
void Cell::froudeCheck(double &q, const double hflow)
{
  if ((std::abs(q / hflow) / std::sqrt(gravity * hflow)) > froude_limit) // correctly reads newly calculated value of q, not here.q
    {
      q = std::copysign(hflow * (std::sqrt(gravity*hflow) * froude_limit), q);
    }
}


// DISCHARGE MAGNITUDE/TIMESTEP CHECKS
// If the discharge is too high for this timestep, scale back...
template<typename COORD_MAP>
void Cell::dischargeCheck(const COORD_MAP& neighborhood, double &q, double neighbour_water_depth, double Delta)
{
  double flow_timestep = getFlowTimestep();
  double criterion_magnitude = std::abs(q * flow_timestep / Delta);

  if (q > 0 && criterion_magnitude > (here.water_depth / 4.0))
    {
      q = ((here.water_depth * Delta) / 5.0) / flow_timestep;
    }
  else if (q < 0 && criterion_magnitude > (neighbour_water_depth / 4.0))
    {
      q = -((neighbour_water_depth * Delta) / 5.0) / flow_timestep;
    }
}
  

double Cell::getFlowTimestep()
{
    return time_step;
}


// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// DEPTH UPDATE
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
template<typename COORD_MAP>
void Cell::depthUpdate(const COORD_MAP& neighborhood)
{
  double east_qx;
  double north_qy;
  double flow_timestep = getFlowTimestep();
  
  switch (celltype)
  {
  case INTERNAL:
  case EDGE_SOUTH:
  case EDGE_WEST:
  case CORNER_SW:
      east_qx = east.qx;
      north_qy = north.qy;
      updateWaterDepth(neighborhood, east_qx, north_qy, flow_timestep);
      break;
  case EDGE_EAST:
  case CORNER_SE:
      east_qx = 0.0;
      north_qy = north.qy;
      updateWaterDepth(neighborhood, east_qx, north_qy, flow_timestep);
      break;
  case EDGE_NORTH:
  case CORNER_NW:
      east_qx = east.qx;
      north_qy = 0.0;
      updateWaterDepth(neighborhood, east_qx, north_qy, flow_timestep);
      break;
  case CORNER_NE:
      east_qx = 0.0;
      north_qy = 0.0;
      updateWaterDepth(neighborhood, east_qx, north_qy, flow_timestep);
      break;
  case NODATA:
      water_depth = 0.0;
      break;
  default:
      std::cout << "\n\n WARNING: no depth update rule specified for cell type " << static_cast<int>(celltype) << "\n\n";
      break;
  }
}


template<typename COORD_MAP>
void Cell::updateWaterDepth(const COORD_MAP& neighborhood, const double east_qx, const double north_qy, const double flow_timestep)
{
    water_depth = here.water_depth + flow_timestep * ( (east_qx - here.qx)/DX + (north_qy - here.qy)/DY );
    
    if(water_depth > 0)
    {
	water_level = here.elevation + water_depth; 
    }
}


#endif
