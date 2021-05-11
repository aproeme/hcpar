#include <catchmentparameters.hpp>
#include <LSDParameterParser.hpp>
#include <libgeodecomp/communication/mpilayer.h>


CatchmentParameters::CatchmentParameters(std::string parameter_filename)
{
  read_parameters(parameter_filename);
}


void CatchmentParameters::read_parameters(std::string parameter_filename)
{
  if(LibGeoDecomp::MPILayer().rank() == 0)
    {
      std::cout << "Reading the model parameters..." << std::endl;
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
	  setBoolean(debug, value);
	}
      else if (lower == "xmax")
	{
	  setInteger(xmax, "xmax", value);
	}
      else if (lower == "ymax")
	{
	  setInteger(ymax, "ymax", value);
	}
      
      // Specify elevation file to initialise from
      else if (lower == "dem_netcdf_file")
	{
	  setString(dem_netcdf_file, "DEM netCDF file", value);
	}
      // Specify name of variable in netCDF file that stores elevation data
      else if (lower == "dem_netcdf_variable")
	{
	  setString(dem_netcdf_variable, "DEM netCDF variable", value);
	}

      
      

      //=-=-=-=-=-=-=-=-=-=-=-=-=-=
      // Hydrology
      //=-=-=-=-=-=-=-=-=-=-=-=-=-=
      else if (lower == "slope_on_edge_cell")
	{
	  setDouble(edgeslope, "Slope on edge cells", value);
	}
      else if (lower == "hflow_threshold")
	{
	  setDouble(hflow_threshold, "Horizontal flow threshold", value);
	}
      else if (lower == "courant_number")
	{
	  setDouble(courant_number, "Courant number", value);
	}
      else if (lower == "mannings_n")
	{
	  setDouble(mannings, "Manning's n value", value);
	}
      else if (lower == "froude_num_limit")
	{
	  setDouble(froude_limit, "Froude number limit", value);
	}
      
      
      //=-=-=-=-=-=-=-=-=-=-=-=-=-=
      // Numerical
      //=-=-=-=-=-=-=-=-=-=-=-=-=-=
      else if (lower == "no_of_iterations")
	{
	  setUnsignedInteger(no_of_iterations, "Number of iterations", value);
	}
      else if (lower == "progress_interval")
	{
	  setUnsignedInteger(progress_interval, "Interval reporting progress to stdout", value);
	}
      else if (lower == "DX")
	{
	  setDouble(DX, "DX", value);
	}
      else if (lower == "DY")
	{
	  setDouble(DY, "DY", value);
	}
      else if (lower == "timestep")
	{
	  setDouble(time_step, "time step (in seconds)", value);
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
      else if (lower == "ppm_pixels_per_cell")
	{
	  setInteger(ppm_pixels_per_cell, "Pixels per grid cell in .ppm output image(s)", value);
	}
      else if (lower == "elevation_ppm")
	{
	  setBoolean(elevation_ppm, value);
	}
      else if (lower == "water_depth_ppm")
	{
	  setBoolean(water_depth_ppm, value);
	}
      else if (lower == "elevation_ppm_interval")
	{
	  setInteger(elevation_ppm_interval, "Interval writing .ppm output image for elevation", value);
	}
      else if (lower == "water_depth_ppm_interval")
	{
	  setInteger(water_depth_ppm_interval, "Interval writing .ppm output image for water depth", value);
	}
      else if (lower == "elevation_netcdf")
	{
	  setBoolean(elevation_netcdf, value);
	}
      else if (lower == "water_depth_netcdf")
	{
	  setBoolean(water_depth_netcdf, value);
	}
      else if (lower == "elevation_netcdf_interval")
	{
	  setInteger(elevation_netcdf_interval, "Interval writing netCDF output for elevation", value);
	}
      else if (lower == "water_depth_netcdf_interval")
	{
	  setInteger(water_depth_netcdf_interval, "Interval writing netCDF output for water_depth", value);
	}
    }
  
  if(LibGeoDecomp::MPILayer().rank() == 0)
    {
      std::cout << "No other parameters found, parameter ingestion complete." << std::endl;
    }
}


void CatchmentParameters::setBoolean(bool& parameter, std::string value)
{
  parameter = (value == "yes") ? true : false;
}


void CatchmentParameters::setInteger(int& parameter, std::string name, std::string value)
{
  parameter = atoi(value.c_str());

  if(LibGeoDecomp::MPILayer().rank() == 0)
    {
      std::cout << name + ": " << parameter << std::endl;
    }
}


void CatchmentParameters::setUnsignedInteger(unsigned& parameter, std::string name, std::string value)
{
  parameter = atoi(value.c_str());

  if(LibGeoDecomp::MPILayer().rank() == 0)
    {
      std::cout << name + ": " << parameter << std::endl;
    }
}


void CatchmentParameters::setString(std::string& parameter, std::string name, std::string value)
{
  parameter = value;
  
  if(LibGeoDecomp::MPILayer().rank() == 0)
    {
      std::cout << name + ": " << parameter << std::endl;
    }  
}


void CatchmentParameters::setDouble(double& parameter, std::string name, std::string value)
{
  parameter = atof(value.c_str());
  
  if(LibGeoDecomp::MPILayer().rank() == 0)
    {
      std::cout << name + ": " << parameter << std::endl;
    }
}
