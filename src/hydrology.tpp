#ifndef HC_HYDROLOGY_H
#define HC_HYDROLOGY_H


// Add water to the catchment 
template<typename COORD_MAP>
void Cell::catchmentWaterInputs(const COORD_MAP& neighborhood) 
{
    // uniform persistent rainfall
    if(rain_in_high_places)
    {
	if(here.elevation >= rain_above_elevation)
	{
	    waterDepth = here.waterDepth + rainRate;
	}
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
    double west_waterDepth;
    double flowTimestep = getFlowTimestep();
    
    switch (celltype)
    {
    case INTERNAL:   // excludes any corner or edge
    case EDGE_NORTH: // excludes Northern corners
    case EDGE_SOUTH: // excludes Southern corners
    {
	west_elevation = west.elevation; 
	west_waterDepth = west.waterDepth; 
	tempslope = ((west_elevation + west_waterDepth) - (here.elevation + here.waterDepth)) / DX;
	break;
    }
    case EDGE_WEST:
    case CORNER_NW: 
    case CORNER_SW:
    {
	west_elevation = 0.0; // set to zero rather than NODATA value as per original HAIL-CAESAR code
	west_waterDepth = 0.0; 
	tempslope = 0.0 - edgeslope; // corresponds to x == 1 in original HAIL-CAESAR code
	break;
    }
    case EDGE_EAST:
    case CORNER_NE:
    case CORNER_SE:
    {
	west_elevation = west.elevation; 
	west_waterDepth = west.waterDepth; 
	tempslope = edgeslope; // corresponds to x == imax in original HAIL-CAESAR code
	break;
    }
    default:
	std::cout << "\n\n WARNING: no x-direction flow route rule specified for cell type " << static_cast<int>(celltype) << "\n\n";
	break;
    }
    
    
    
    if (here.waterDepth > 0 || west_waterDepth > 0)  // still deal with west.elevation == NODATA
    {
	hflowX = std::max(here.elevation + here.waterDepth, west_elevation + west_waterDepth) - std::max(here.elevation, west_elevation);
	
	
	if (hflowX > hflowThreshold)
	{
	    updateQx(neighborhood, hflowX, tempslope, flowTimestep);
	    froudeCheck(qX, hflowX);
	    dischargeCheck(neighborhood, qX, west_waterDepth, DX);
	}
	else
	{
	    qX = 0.0;
	    // qXs = 0.0; 
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
    double south_waterDepth;
    double flowTimestep = getFlowTimestep();

    switch (celltype)
    {
    case INTERNAL: // excludes any corner or edge
    case EDGE_WEST: // excludes Western corners
    case EDGE_EAST: // excludes Eastern corners
    {
	south_elevation = south.elevation;
	south_waterDepth = south.waterDepth;
	tempslope = ((south_elevation + south_waterDepth) - (here.elevation + here.waterDepth)) / DY;
	break;
    }
    case EDGE_SOUTH:
    case CORNER_SW:
    case CORNER_SE:
    {
	south_elevation = 0.0; // set to zero rather than NODATA, as per original HAIL-CAESAR code
	south_waterDepth = 0.0;
	tempslope = edgeslope; // corresponds to y == jmax in original HAIL-CAESAR code
	break;
    }
    case EDGE_NORTH:
    case CORNER_NW:
    case CORNER_NE:
    {
	south_elevation = south.elevation;
	south_waterDepth = south.waterDepth;
	tempslope = 0.0 - edgeslope; // corresponds to y == 1 in original HAIL-CAESAR code
	break;
    }
    default:
	std::cout << "\n\n WARNING: no y-direction flow route rule specified for cell type " << static_cast<int>(celltype) << "\n\n";
	break;
    }

    
    if (here.waterDepth > 0 || south_waterDepth > 0) // still deal with south.elevation == NODATA
    {
	hflowY = std::max(here.elevation + here.waterDepth, south_elevation + south_waterDepth) - std::max(here.elevation, south_elevation);
    
	if (hflowY > hflowThreshold)
	{
	    updateQy(neighborhood, hflowY, tempslope, flowTimestep);
	    froudeCheck(qY, hflowY);
	    dischargeCheck(neighborhood, qY, south_waterDepth, DY);
	}
	else
	{
	    qY = 0.0;
	    // qYs = 0.0;
	}
    }
}


template<typename COORD_MAP>
void Cell::updateQx(const COORD_MAP& neighborhood, const double hflowX, const double tempslope, const double flowTimestep)
{
    updateQ(here.qX, qX, hflowX, tempslope, flowTimestep);
}


template<typename COORD_MAP>
void Cell::updateQy(const COORD_MAP& neighborhood, const double hflowY, const double tempslope, const double flowTimestep)
{
    updateQ(here.qY, qY, hflowY, tempslope, flowTimestep);
}


void Cell::updateQ(const double &q, double &q_new, const double hflow, const double tempslope, const double flowTimestep)
{
    q_new = ((q - (gravity * hflow * flowTimestep * tempslope)) / (1.0 + gravity * hflow * flowTimestep * (mannings * mannings) * std::abs(q) / std::pow(hflow, (10.0 / 3.0))));

}
    

// FROUDE NUMBER CHECK
// need to have these lines to stop too much water moving from
// one cell to another - resulting in negative discharges
// which causes a large instability to develop
// - only in steep catchments really
void Cell::froudeCheck(double &q, const double hflow)
{
    if ((std::abs(q / hflow) / std::sqrt(gravity * hflow)) > froudeLimit) // correctly reads newly calculated value of q, not here.q
    {
	q = std::copysign(hflow * (std::sqrt(gravity*hflow) * froudeLimit), q);
    }
}


// DISCHARGE MAGNITUDE/TIMESTEP CHECKS
// If the discharge is too high for this timestep, scale back...
template<typename COORD_MAP>
void Cell::dischargeCheck(const COORD_MAP& neighborhood, double &q, double neighbour_waterDepth, double Delta)
{
    double flowTimestep = getFlowTimestep();
    double criterion_magnitude = std::abs(q * flowTimestep / Delta);

    if (q > 0 && criterion_magnitude > (here.waterDepth / 4.0))
    {
	q = ((here.waterDepth * Delta) / 5.0) / flowTimestep;
    }
    else if (q < 0 && criterion_magnitude > (neighbour_waterDepth / 4.0))
    {
	q = -((neighbour_waterDepth * Delta) / 5.0) / flowTimestep;
    }
}




double Cell::getFlowTimestep()
{
    // Start with the maximum time step currently set
    double flowTimestep = timestep;
    
    // Scale it back to the CFL condition if it exceeds CFL.
    /*if (flowTimestep > CFLCondition())
    {
	flowTimestep = CFLCondition();
	}*/
    
    return flowTimestep;
}


// Apply the Courant-Friedrichs-Lewy condition
// To do: compute maxdepth so it can be used here,
// which requires comparison across cells
double Cell::CFLCondition()
{
    double maxdepth = waterDepth; // REPLACE WITH ACTUAL MAXDEPTH

    return courantNumber * (DX / std::sqrt(gravity * (maxdepth)));
}



// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// DEPTH UPDATE
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
template<typename COORD_MAP>
void Cell::depthUpdate(const COORD_MAP& neighborhood)
{
    double east_qX;
    double north_qY;
    double flowTimestep = getFlowTimestep();
  
    switch (celltype)
    {
    case INTERNAL:
    case EDGE_SOUTH:
    case EDGE_WEST:
    case CORNER_SW:
    {
	east_qX = east.qX;
	north_qY = north.qY;
	updateWaterDepth(neighborhood, east_qX, north_qY, flowTimestep);
	break;
    }
    case EDGE_EAST:
    case CORNER_SE:
    {
	east_qX = 0.0;
	north_qY = north.qY;
	updateWaterDepth(neighborhood, east_qX, north_qY, flowTimestep);
	break;
    }
    case EDGE_NORTH:
    case CORNER_NW:
    {
	east_qX = east.qX;
	north_qY = 0.0;
	updateWaterDepth(neighborhood, east_qX, north_qY, flowTimestep);
	break;
    }
    case CORNER_NE:
    {
	east_qX = 0.0;
	north_qY = 0.0;
	updateWaterDepth(neighborhood, east_qX, north_qY, flowTimestep);
	break;
    }
    case NODATA:
    {
	waterDepth = 0.0;
	break;
    }
    default:
	std::cout << "\n\n WARNING: no depth update rule specified for cell type " << static_cast<int>(celltype) << "\n\n";
	break;
    }
}


template<typename COORD_MAP>
void Cell::updateWaterDepth(const COORD_MAP& neighborhood, const double east_qX, const double north_qY, const double flowTimestep)
{

    waterDepth = here.waterDepth + flowTimestep * ( (east_qX - here.qX)/DX + (north_qY - here.qY)/DY );
    
    if (waterDepth > 0)
    {
	waterLevel = here.elevation + waterDepth; 
    }
}






// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// WATER FLUXES OUT OF CATCHMENT
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Calculate the water coming out and zero any water depths at the edges
// This will actually set it to the minimum water depth
// This must be done so that water can still move sediment to the edge of the catchment
// and hence remove it from the catchment. (otherwise you would get sediment build
// up around the edges.
template<typename COORD_MAP>
void Cell::waterFluxOut(const COORD_MAP& neighborhood)
{
    double flowTimestep = timestep;
    
    // To do: sum up total water out
    switch(celltype)
    {
    case EDGE_EAST:
    case EDGE_WEST:
    case EDGE_NORTH:
    case EDGE_SOUTH:
    case CORNER_SE:
    case CORNER_SW:
    case CORNER_NE:
    case CORNER_NW:
    {
	if (here.waterDepth > waterDepthErosionThreshold)
	{
	    waterDepth = waterDepthErosionThreshold;
	    waterOut = ((waterDepth - waterDepthErosionThreshold)*DX*DY)/flowTimestep;
	}
	break;
    }
    }
    
    
}


// Convert physical rain rate (mm/hour)
//     to numerical rain rate (metres/timestep)
double Cell::numericalRainRate(const double physicalRainRate)
{
    return timestep*(physicalRainRate/3600000.0);
}


#endif
