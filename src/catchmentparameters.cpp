#include <catchmentparameters.hpp>
#include <LSDParameterParser.hpp>
#include <libgeodecomp/communication/mpilayer.h>


CatchmentParameters::CatchmentParameters(string parameter_filename)
{
    inputNetCDFFileName = vector<string>(maxGridQuantities());
    inputNetCDFVariableName = vector<string>(maxGridQuantities());
    outputNetCDFInterval = vector<int>(maxGridQuantities());
    readParameters(parameter_filename);
}


void CatchmentParameters::readParameters(string parameter_filename)
{
    if(LibGeoDecomp::MPILayer().rank() == 0)
    {
	std::cout << "Reading simulation parameters from " << parameter_filename << "\n" << std::endl;
    }
  
    std::ifstream infile;
    infile.open(parameter_filename.c_str());
    string parameter, value, lower, lower_val;
    string bc;
  
    // now ingest parameters
    while (infile.good())
    {
	parse_line(infile, parameter, value);
	lower = parameter;
	if (parameter == "NULL")
	    continue;
	for (unsigned int i=0; i<parameter.length(); ++i)
	{
	    lower[i] = std::tolower(parameter[i]);  // converts to lowercase
	}
	
	// get rid of control characters
	value = RemoveControlCharactersFromEndOfString(value);


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Input Options
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=
	if (lower == "xmax") // not used when initialising grid from a netCDF file
	{
	    xmax = atoi(value.c_str());
	    notifyUser("xmax", value);
	}
	else if (lower == "ymax") // not used when initialising grid from a netCDF file
	{
	    ymax = atoi(value.c_str());
	    notifyUser("ymax", value);
	}
	
	// NetCDF Filenames
	else if (lower == "input_dem_netcdf_file")
	{
	    notifyUser("  netCDF DEM input file", value);
	    inputNetCDFGridQuantities.push_back(GridQuantity::elevation);
	    inputNetCDFFileName[GridQuantity::elevation] = value;
	}
	else if (lower == "input_water_depth_netcdf_file")
	{
	    notifyUser("  netCDF water depth input file", value);
	    inputNetCDFGridQuantities.push_back(GridQuantity::water_depth);
	    inputNetCDFFileName[GridQuantity::water_depth] = value;
	}
	// NetCDF Variables
	else if (lower == "input_dem_netcdf_variable")
	{
	    notifyUser("  netCDF variable name corresponding to elevation", value);
	    inputNetCDFVariableName[GridQuantity::elevation] = value;
	}
	else if (lower == "input_water_depth_netcdf_variable")
	{
	    notifyUser("  netCDF variable corresponding to water depth", value);
	    inputNetCDFVariableName[GridQuantity::water_depth] = value;
	}

	// Grid initialisation options - 
	// Setting up different initial conditions, 
	// mainly for testing and development
	// and for seeing how the algorithm operates
	// under controlled conditions
	else if (lower == "init_water_level")
	{
	    inundate_below_elevation = true;
	    setDoubleParameter(init_water_level, "Setting initial water surface level to", value);
	}
	else if (lower == "init_water_depth_above_elevation")
	{
	    inundate_above_elevation = true;
	    setDoubleParameter(init_water_depth_above_elevation, "Setting water depth above minimum elevation to", value);
	}
	else if (lower == "init_lowest_inundated_elevation")
	{
	    inundate_above_elevation = true;
	    setDoubleParameter(init_lowest_inundated_elevation, "Minimum elevation to inundate", value);
	}

	
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// LISFLOOD Hydrology Parameters
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	else if (lower == "slope_on_edge_cell")
	{
	    setDoubleParameter(edgeslope, "  Slope on edge cells", value);
	}
	else if (lower == "hflow_threshold")
	{
	    setDoubleParameter(hflow_threshold, "  Horizontal flow threshold", value);
	}
	else if (lower == "courant_number")
	{
	    setDoubleParameter(courant_number, "  Courant number", value);
	}
	else if (lower == "mannings_n")
	{
	    setDoubleParameter(mannings, "  Manning's n value", value);
	}
	else if (lower == "froude_num_limit")
	{
	    setDoubleParameter(froude_limit, "  Froude number limit", value);
	}
      
      
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Numerical
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=
	else if (lower == "no_of_iterations")
	{
	    setUnsignedIntegerParameter(no_of_iterations, "  Number of iterations", value);
	}
	else if (lower == "progress_interval")
	{
	    setUnsignedIntegerParameter(progress_interval, "  Interval reporting progress to stdout", value);
	}
	else if (lower == "DX")
	{
	    setDoubleParameter(DX, "  DX", value);
	}
	else if (lower == "DY")
	{
	    setDoubleParameter(DY, "  DY", value);
	}
	else if (lower == "timestep")
	{
	    setDoubleParameter(time_step, "  time step (in seconds)", value);
	}
      
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// LibGeoDecomp Options
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=
	else if (lower == "simulator")
	{
	    simulator = value;
	}
      
      
      
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Output Options
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// NetCDF Outputs
	else if (lower == "output_elevation")
	{
	    if (value == "netcdf")
	    {
		outputNetCDFGridQuantities.push_back(GridQuantity::elevation);
	    }
	}
	else if (lower == "output_water_depth")
	{
	    if (value == "netcdf")
	    {
		outputNetCDFGridQuantities.push_back(GridQuantity::water_depth);
	    }
	}
	else if (lower == "output_water_level")
	{
	    if(value == "netcdf")
	    {
		outputNetCDFGridQuantities.push_back(GridQuantity::water_level);
	    }
	}
	else if (lower == "output_qx")
	{
	    if(value == "netcdf")
	    {
		outputNetCDFGridQuantities.push_back(GridQuantity::qx);
	    }
	}
	else if (lower == "output_qy")
	{
	    if(value == "netcdf")
	    {
		outputNetCDFGridQuantities.push_back(GridQuantity::qy);
	    }
	}
	else if (lower == "output_hflowx")
	{
	    if(value == "netcdf")
	    {
		outputNetCDFGridQuantities.push_back(GridQuantity::hflowx);
	    }
	}
	else if (lower == "output_hflowy")
	{
	    if(value == "netcdf")
	    {
		outputNetCDFGridQuantities.push_back(GridQuantity::hflowy);
	    }
	}
	else if (lower == "output_celltype")
	{
	    if(value == "netcdf")
	    {
		outputNetCDFGridQuantities.push_back(GridQuantity::celltype_double);
	    }
	}

	
	// NetCDF Output Intervals - how often to write to file
	else if (lower == "output_interval_elevation")
	{
	    notifyUser("  Interval writing output for elevation", value);
	    outputNetCDFInterval[GridQuantity::elevation] = atoi(value.c_str());
	}
	else if (lower == "output_interval_water_depth")
	{
	    notifyUser("  Interval writing output for water depth", value);
	    outputNetCDFInterval[GridQuantity::water_depth] = atoi(value.c_str());
	}
	else if (lower == "output_interval_water_level")
	{
	    notifyUser("  Interval writing output for water level", value);
	    outputNetCDFInterval[GridQuantity::water_level] = atoi(value.c_str());
	}
	else if (lower == "output_interval_qx")
	{
	    notifyUser("  Interval writing output for qx", value);
	    outputNetCDFInterval[GridQuantity::qx] = atoi(value.c_str());
	}
	else if (lower == "output_interval_qy")
	{
	    notifyUser("  Interval writing output for qy", value);
	    outputNetCDFInterval[GridQuantity::qy] = atoi(value.c_str());
	}
	else if (lower == "output_interval_hflowx")
	{
	    notifyUser("  Interval writing output for hflowx", value);
	    outputNetCDFInterval[GridQuantity::hflowx] = atoi(value.c_str());
	}
	else if (lower == "output_interval_hflowy")
	{
	    notifyUser("  Interval writing output for hflowy", value);
	    outputNetCDFInterval[GridQuantity::hflowy] = atoi(value.c_str());
	} 
	else if (lower == "output_interval_celltype")
	{
	    outputNetCDFInterval[GridQuantity::celltype_double] = atoi(value.c_str());
	}
    }

    
    if(LibGeoDecomp::MPILayer().rank() == 0)
    {
	std::cout << "No other parameters found, parameter ingestion complete." << std::endl;
    }
}


void CatchmentParameters::notifyUser(string notification, string value)
{
   if(LibGeoDecomp::MPILayer().rank() == 0)
    {
	std::cout << notification + ": " << value << std::endl;
    }
}



void CatchmentParameters::setUnsignedIntegerParameter(unsigned& parameter, string name, string value)
{
    parameter = atoi(value.c_str());

    if(LibGeoDecomp::MPILayer().rank() == 0)
    {
	std::cout << name + ": " << parameter << std::endl;
    }
}



void CatchmentParameters::setDoubleParameter(double& parameter, string name, string value)
{
    parameter = atof(value.c_str());
  
    if(LibGeoDecomp::MPILayer().rank() == 0)
    {
	std::cout << name + ": " << parameter << std::endl;
    }
}


