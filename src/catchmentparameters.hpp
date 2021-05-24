#ifndef HC_CATCHMENTPARAMETERS_H
#define HC_CATCHMENTPARAMETERS_H

#include <string>
#include <vector>

#include <gridquantities.hpp>
#include <cell.hpp>

#include <libgeodecomp/io/pnetcdfinitializer.h>

// forward declaration to resolve circular include dependency
class Cell;

class CatchmentParameters
{
public: 
    CatchmentParameters(string parameter_filename);
    
    void readParameters(string parameter_filename);

    void notifyUser(string notification, string value);
    
    void setUnsignedIntegerParameter(unsigned& parameter, string name, string value);
    
    void setDoubleParameter(double& parameter, string name, string value);
    

    // Numerical parameters
    string simulator;
    unsigned no_of_iterations;
    double timestep;
    unsigned progress_interval=1;
    
    // Inputs
    vector<GridQuantity> inputNetCDFGridQuantities;
    vector<string> inputNetCDFFileName;
    vector<string> inputNetCDFVariableName;
    bool inundate_below_elevation = false;
    double init_waterLevel;
    bool inundate_above_elevation = false;
    double init_lowest_inundated_elevation;
    double init_waterDepth_above_elevation;
    bool rain_in_high_places = false;
    double physicalRainRate; // (mm/hour)
    double rain_above_elevation; // metres of elevation above which to rain
    double DX, DY; // should read these from netCDF metadata if available
    double no_data_value;
    
    // Outputs
    vector<GridQuantity> outputNetCDFGridQuantities;
    vector<int> outputNetCDFInterval;
    
    // Hydrology
    double edgeslope;
    double courantNumber; 
    double hflowThreshold;
    double mannings;
    double froudeLimit;
    double waterDepthErosionThreshold;
    
    // for testing & development
    int xmax;
    int ymax;
  
};

#endif 
