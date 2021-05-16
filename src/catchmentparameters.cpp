#include <catchmentparameters.hpp>
#include <LSDParameterParser.hpp>
#include <libgeodecomp/communication/mpilayer.h>


CatchmentParameters::CatchmentParameters(std::string parameter_filename)
{
    readParameters(parameter_filename);
    gatherNetCDFSources();
}


void CatchmentParameters::readParameters(std::string parameter_filename)
{
    if(LibGeoDecomp::MPILayer().rank() == 0)
    {
	std::cout << "Reading simulation parameters from " << parameter_filename << "\n" << std::endl;
    }
  
    std::ifstream infile;
    infile.open(parameter_filename.c_str());
    std::string parameter, value, lower, lower_val;
    std::string bc;
  
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
	// Elevation Input Options
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// debug mode means no input elevation file is read
	// instead a grid of xmax by ymax is initialised with zero elevation
	// specify alternative grid dimensions with xmax & ymax in parameter file
	if (lower == "debug")
	{
	    setBooleanParameter(debug, value);
	}
	else if (lower == "xmax")
	{
	    setIntegerParameter(xmax, "xmax", value);
	}
	else if (lower == "ymax")
	{
	    setIntegerParameter(ymax, "ymax", value);
	}
	else if (lower == "input_dem_netcdf_file")
	{
	    input_dem_netcdf = true;
	    setStringParameter(input_dem_netcdf_file, "netCDF DEM input file", value);
	}
	else if (lower == "input_water_depth_netcdf_file")
	{
	    input_water_depth_netcdf = true;
	    setStringParameter(input_water_depth_netcdf_file, "netCDF water depth input file", value);
	}
	else if (lower == "input_dem_netcdf_variable")
	{
	    setStringParameter(input_dem_netcdf_variable, "netCDF variable name corresponding to elevation", value);
	}
	else if (lower == "input_water_depth_netcdf_variable")
	{
	    setStringParameter(input_water_depth_netcdf_variable, "netCDF variable corresponding to water depth", value);
	}
	else if (lower == "init_water_surface_elevation")
	{
	    inundate_below_elevation = true;
	    setDoubleParameter(init_water_surface_elevation, "Setting initial water surface level to", value);
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
      
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Hydrology
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=
	else if (lower == "slope_on_edge_cell")
	{
	    setDoubleParameter(edgeslope, "Slope on edge cells", value);
	}
	else if (lower == "hflow_threshold")
	{
	    setDoubleParameter(hflow_threshold, "Horizontal flow threshold", value);
	}
	else if (lower == "courant_number")
	{
	    setDoubleParameter(courant_number, "Courant number", value);
	}
	else if (lower == "mannings_n")
	{
	    setDoubleParameter(mannings, "Manning's n value", value);
	}
	else if (lower == "froude_num_limit")
	{
	    setDoubleParameter(froude_limit, "Froude number limit", value);
	}
      
      
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Numerical
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=
	else if (lower == "no_of_iterations")
	{
	    setUnsignedIntegerParameter(no_of_iterations, "Number of iterations", value);
	}
	else if (lower == "progress_interval")
	{
	    setUnsignedIntegerParameter(progress_interval, "Interval reporting progress to stdout", value);
	}
	else if (lower == "DX")
	{
	    setDoubleParameter(DX, "DX", value);
	}
	else if (lower == "DY")
	{
	    setDoubleParameter(DY, "DY", value);
	}
	else if (lower == "timestep")
	{
	    setDoubleParameter(time_step, "time step (in seconds)", value);
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
	else if (lower == "output_ppm_pixels_per_cell")
	{
	    setIntegerParameter(output_ppm_pixels_per_cell, "Pixels per grid cell in .ppm output image(s)", value);
	}
	else if (lower == "output_elevation_ppm")
	{
	    setBooleanParameter(output_elevation_ppm, value);
	}
	else if (lower == "output_water_depth_ppm")
	{
	    setBooleanParameter(output_water_depth_ppm, value);
	}
	else if (lower == "output_elevation_ppm_interval")
	{
	    setIntegerParameter(output_elevation_ppm_interval, "Interval writing .ppm output image for elevation", value);
	}
	else if (lower == "output_water_depth_ppm_interval")
	{
	    setIntegerParameter(output_water_depth_ppm_interval, "Interval writing .ppm output image for water depth", value);
	}
	else if (lower == "output_elevation_netcdf")
	{
	    setBooleanParameter(output_elevation_netcdf, value);
	}
	else if (lower == "output_water_depth_netcdf")
	{
	    setBooleanParameter(output_water_depth_netcdf, value);
	}
	else if (lower == "output_water_surface_elevation_netcdf")
	{
	    setBooleanParameter(output_water_surface_elevation_netcdf, value);
	}
	else if (lower == "output_elevation_netcdf_interval")
	{
	    setIntegerParameter(output_elevation_netcdf_interval, "Interval writing netCDF output for elevation", value);
	}
	else if (lower == "output_water_depth_netcdf_interval")
	{
	    setIntegerParameter(output_water_depth_netcdf_interval, "Interval writing netCDF output for water depth", value);
	}
	else if (lower == "output_water_surface_elevation_netcdf_interval")
	{
	    setIntegerParameter(output_water_surface_elevation_netcdf_interval, "Interval writing netCDF output for water surface elevation", value);
	}
    }
  
    if(LibGeoDecomp::MPILayer().rank() == 0)
    {
	std::cout << "No other parameters found, parameter ingestion complete." << std::endl;
    }
}



void CatchmentParameters::setBooleanParameter(bool& parameter, std::string value)
{
    parameter = (value == "yes") ? true : false;
}


void CatchmentParameters::setIntegerParameter(int& parameter, std::string name, std::string value)
{
    parameter = atoi(value.c_str());

    if(LibGeoDecomp::MPILayer().rank() == 0)
    {
	std::cout << name + ": " << parameter << std::endl;
    }
}


void CatchmentParameters::setUnsignedIntegerParameter(unsigned& parameter, std::string name, std::string value)
{
    parameter = atoi(value.c_str());

    if(LibGeoDecomp::MPILayer().rank() == 0)
    {
	std::cout << name + ": " << parameter << std::endl;
    }
}


void CatchmentParameters::setStringParameter(std::string& parameter, std::string name, std::string value)
{
    parameter = value;
  
    if(LibGeoDecomp::MPILayer().rank() == 0)
    {
	std::cout << name + ": " << parameter << std::endl;
    }  
}


void CatchmentParameters::setDoubleParameter(double& parameter, std::string name, std::string value)
{
    parameter = atof(value.c_str());
  
    if(LibGeoDecomp::MPILayer().rank() == 0)
    {
	std::cout << name + ": " << parameter << std::endl;
    }
}


void CatchmentParameters::gatherNetCDFSources()
{
    if(input_dem_netcdf)
    {
	LibGeoDecomp::Selector<Cell> dem_target_selector(&Cell::elevation, "elevation");
	netCDFSources.push_back(LibGeoDecomp::netCDFSource<Cell> {input_dem_netcdf_file, input_dem_netcdf_variable, dem_target_selector});
    }
  
    /*ostringstream debug;
      debug << "netCDFSources[0].file = " << netCDFSources[0].file << ", netCDFSources[0].variableName = " << netCDFSources[0].variableName << std::endl;
      std::cout << debug.str();*/
  
    if(input_water_depth_netcdf)
    {
	LibGeoDecomp::Selector<Cell> water_depth_target_selector(&Cell::water_depth, "water_depth");
	netCDFSources.push_back(LibGeoDecomp::netCDFSource<Cell> {input_water_depth_netcdf_file, input_water_depth_netcdf_variable, water_depth_target_selector});
    }
}
