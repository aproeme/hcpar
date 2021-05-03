#ifndef HC_CATCHMENTPARAMETERS_H
#define HC_CATCHMENTPARAMETERS_H

#include <string>

class CatchmentParameters
{
public: 
  CatchmentParameters(std::string parameter_filename);
  
  void read_parameters(std::string parameter_filename);
  
  // simulation parameters
  unsigned no_of_iterations = 100;
  std::string simulator = "hipar";
  bool debug = false;

    
  // for debugging only
  int xmax = 10;
  int ymax = 10;

    
  // input
  std::string dem_netcdf_file;

    
  // output
  bool elevation_ppm = false;
  bool elevation_bov = false;
  bool elevation_visit = false;
  bool elevation_netcdf = false;
  bool water_depth_ppm = false;
  bool water_depth_bov = false;
  bool water_depth_visit = false;
  bool water_depth_netcdf = false;
  int elevation_ppm_interval = 1;
  int elevation_bov_interval = 1; 
  int elevation_visit_interval = 1;
  int elevation_netcdf_interval = 1;
  int water_depth_ppm_interval = 1;
  int water_depth_bov_interval = 1;
  int water_depth_visit_interval = 1;
  int water_depth_netcdf_interval = 1; 
  int ppm_pixels_per_cell = 10;
  

  
  
};

#endif 
