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
      std::cout << "Initialising the model parameters..." << std::endl;
    }
  
  std::ifstream infile;
  // Open the parameter file
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
	  debug = (value == "yes") ? true : false;
	}
      else if (lower == "xmax")
	{
	  xmax = atoi(value.c_str());
	  if(LibGeoDecomp::MPILayer().rank() == 0)
	    {
	      std::cout << "xmax: " << xmax  << std::endl;
	    }
	}
      else if (lower == "ymax")
	{
	  ymax = atoi(value.c_str());
	  if(LibGeoDecomp::MPILayer().rank() == 0)
	    {
	      std::cout << "ymax: " << ymax  << std::endl;
	    }
	}

      // Specify elevation file to initialise from
      else if (lower == "dem_netcdf_file")
	{
	  dem_netcdf_file = value;
	  dem_netcdf_file = RemoveControlCharactersFromEndOfString(dem_netcdf_file);
	  if(LibGeoDecomp::MPILayer().rank() == 0)
	    {
	      std::cout << "DEM NetCDF file: " << dem_netcdf_file << std::endl;
	    }
	}
      // Specify name of variable in netCDF file that stores elevation data
      else if (lower == "dem_netcdf_variable")
	{
	  dem_netcdf_variable = value;
	  dem_netcdf_variable = RemoveControlCharactersFromEndOfString(dem_netcdf_variable);
	  if(LibGeoDecomp::MPILayer().rank() == 0)
	    {
	      std::cout << "DEM NetCDF variable: " << dem_netcdf_variable << std::endl;
	    }
	}


      //=-=-=-=-=-=-=-=-=-=-=-=-=-=
      // Numerical
      //=-=-=-=-=-=-=-=-=-=-=-=-=-=
      else if (lower == "no_of_iterations")
	{
	  no_of_iterations = atoi(value.c_str());
	  if(LibGeoDecomp::MPILayer().rank() == 0)
	    {
	      std::cout << "no of iterations: " << no_of_iterations << std::endl;
	    }
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
	  ppm_pixels_per_cell = atoi(value.c_str());
	  
      	  if(LibGeoDecomp::MPILayer().rank() == 0)
	    {
	      std::cout << "Pixels per cell (used in PPM output): " <<  ppm_pixels_per_cell << std::endl;
	    }
	}
	  
      else if (lower == "elevation_ppm")
	{
	  elevation_ppm = (value == "yes") ? true : false;
	}
      else if (lower == "water_depth_ppm")
	{
	  water_depth_ppm = (value == "yes") ? true : false;
	}
      else if (lower == "elevation_ppm_interval")
	{
	  elevation_ppm_interval = atoi(value.c_str());
	}
      else if (lower == "water_depth_ppm_interval")
	{
	  water_depth_ppm_interval = atoi(value.c_str());
	}
      else if (lower == "water_depth_bov")
	{
	  water_depth_bov = (value == "yes") ? true : false;
	}
      else if (lower == "water_depth_bov_interval")
	{
	  water_depth_bov_interval = atoi(value.c_str());
	}
      else if (lower == "water_depth_visit")
	{
	  water_depth_visit = (value == "yes") ? true : false;
	}
      else if (lower == "water_depth_visit_interval")
	{
	  water_depth_visit_interval = atoi(value.c_str());
	}
      else if (lower == "elevation_netcdf")
	{
	  elevation_netcdf = (value == "yes") ? true : false;
	}
      else if (lower == "water_depth_netcdf")
	{
	  water_depth_netcdf = (value == "yes") ? true : false;
	}
      else if (lower == "elevation_netcdf_interval")
	{
	  elevation_netcdf_interval = atoi(value.c_str());
	  if(LibGeoDecomp::MPILayer().rank() == 0)
	  {
	      std::cout << "elevation_netcdf_interval: " << elevation_netcdf_interval  << std::endl;
	  }
	}
      else if (lower == "water_depth_netcdf_interval")
	{
	  water_depth_netcdf_interval = atoi(value.c_str());
	}
    }

  if(LibGeoDecomp::MPILayer().rank() == 0)
    {
      std::cout << "No other parameters found, parameter ingestion complete."
		<< std::endl;
    }
}

