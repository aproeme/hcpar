#ifndef HC_CATCHMENTPARAMETERS_H
#define HC_CATCHMENTPARAMETERS_H

#include <string>

class CatchmentParameters
{
public: 
  CatchmentParameters(std::string parameter_filename);
  
  void read_parameters(std::string parameter_filename);
  
  void setBoolean(bool& parameter, std::string value);

  void setInteger(int& parameter, std::string name, std::string value);

  void setUnsignedInteger(unsigned& parameter, std::string name, std::string value);

  void setString(std::string& parameter, std::string name, std::string value);

  void setDouble(double& parameter, std::string name, std::string value);

  
  // Numerical parameters
  std::string simulator = "hipar";
  unsigned no_of_iterations;
  double time_step;
  unsigned progress_interval = 1;
  bool debug = false; 
 
  // Input file information
  std::string dem_netcdf_file;
  std::string dem_netcdf_variable;
  double DX, DY; // should read these from netCDF metadata
  double no_data_value;
  
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

  // Hydrology
  double edgeslope;
  double courant_number; 
  double hflow_threshold;
  double mannings;
  double froude_limit;


  
  // for debugging only
  int xmax;
  int ymax;
  
};

#endif 
