#ifndef HC_CATCHMENTPARAMETERS_H
#define HC_CATCHMENTPARAMETERS_H

//#include <string>

#include <cell.hpp>
#include <libgeodecomp/io/pnetcdfinitializer.h>

// forward declaration to resolve circular include dependency
class Cell;

class CatchmentParameters
{
public: 
    CatchmentParameters(std::string parameter_filename);
  
    void readParameters(std::string parameter_filename);
  
    void setBooleanParameter(bool& parameter, std::string value);

    void setIntegerParameter(int& parameter, std::string name, std::string value);

    void setUnsignedIntegerParameter(unsigned& parameter, std::string name, std::string value);

    void setStringParameter(std::string& parameter, std::string name, std::string value);

    void setDoubleParameter(double& parameter, std::string name, std::string value);

    void gatherNetCDFSources();
  
  
    // Numerical parameters
    std::string simulator = "hipar";
    unsigned no_of_iterations;
    double time_step;
    unsigned progress_interval = 1;
    bool debug = false; 
  
    // Input file information
    std::string input_dem_netcdf_file;
    std::string input_dem_netcdf_variable;
    bool input_dem_netcdf = false;
    std::string input_water_depth_netcdf_file;
    std::string input_water_depth_netcdf_variable;
    bool input_water_depth_netcdf = false;
    std::vector<LibGeoDecomp::netCDFSource<Cell>> netCDFSources;
    bool inundate_below_elevation = false;
    bool inundate_above_elevation = false;
    double init_water_surface_elevation;
    double init_water_depth_above_elevation;
    double init_lowest_inundated_elevation;
    double DX, DY; // should read these from netCDF metadata if available
    double no_data_value;
  
    // output
    bool output_elevation_ppm = false;
    bool output_elevation_bov = false;
    bool output_elevation_visit = false;
    bool output_elevation_netcdf = false;
    bool output_water_depth_ppm = false;
    bool output_water_depth_bov = false;
    bool output_water_depth_visit = false;
    bool output_water_depth_netcdf = false;
    bool output_water_surface_elevation_netcdf = false;
    int output_elevation_ppm_interval = 1;
    int output_elevation_bov_interval = 1; 
    int output_elevation_visit_interval = 1;
    int output_elevation_netcdf_interval = 1;
    int output_water_depth_ppm_interval = 1;
    int output_water_depth_bov_interval = 1;
    int output_water_depth_visit_interval = 1;
    int output_water_depth_netcdf_interval = 1;
    int output_water_surface_elevation_netcdf_interval = 1;
    int output_ppm_pixels_per_cell = 10;

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
