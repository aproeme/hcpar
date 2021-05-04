#include <iostream>
#include <tnt.h>
#include <libgeodecomp/communication/typemaps.h>
#include <libgeodecomp/communication/mpilayer.h>
#include <libgeodecomp/parallelization/simulator.h>
#include <libgeodecomp/parallelization/serialsimulator.h>
#include <libgeodecomp/parallelization/stripingsimulator.h>
#include <libgeodecomp/parallelization/hiparsimulator.h>
#include <libgeodecomp/geometry/partitions/recursivebisectionpartition.h>
#include <libgeodecomp/loadbalancer/noopbalancer.h>
#include <libgeodecomp/io/ppmwriter.h>
#include <libgeodecomp/io/tracingwriter.h>
#include <libgeodecomp/io/collectingwriter.h>
#include <libgeodecomp/io/bovwriter.h>
#include <libgeodecomp/io/parallelwriter.h>
#include <libgeodecomp/io/pnetcdfwriter.h>
#include <libgeodecomp/io/pnetcdfinitializer.h>
#include <typemaps.h>
#include <start.hpp>
#include <cell.hpp>
#include <selectmpidatatype.tpp>
#include <debugcellinitializer.hpp>
#include <netcdfinitializer.hpp>
#include <catchmentparameters.hpp>


int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
  
    LibGeoDecomp::Typemaps::initializeMaps(); // initialize LibGeoDecomp default typemaps (this commits MPI types)
    Typemaps::initializeMaps(); // initialize custom typemaps for HAIL-CAESAR
    LibGeoDecomp::MPILayer().barrier();
  
    start(argc, argv);   // Print info to stdout and validate input 
    std::string parameter_filename(argv[1]);
    
    // Read model parameters on each rank (can replace with MPI_Bcast from rank 0 should overhead ever become noticeable)
    CatchmentParameters parameters = CatchmentParameters(parameter_filename);
    
    //LibGeoDecomp::SimpleInitializer<Cell> *initialiser = 0;
    LibGeoDecomp::Initializer<Cell> *initialiser = 0;

    
    if(parameters.debug)
    {
	// Initialise grid (each rank initialises its own subgrid)
	initialiser = new DebugCellInitializer(parameters.xmax, parameters.ymax, parameters.no_of_iterations);
    }
    else
    {
	initialiser = new NetCDFInitializer(parameters);
    }



    
    if(parameters.simulator == "serial")
    {
	LibGeoDecomp::SerialSimulator<Cell> *sim = new LibGeoDecomp::SerialSimulator<Cell>(initialiser);
      
	// Set up visualisation outputs  
	LibGeoDecomp::PPMWriter<Cell> *elevationPPMWriter = 0;
	LibGeoDecomp::PPMWriter<Cell> *water_depthPPMWriter = 0;
	if(parameters.elevation_ppm)
	{
	    system("mkdir -p elevation/ppm");
	    elevationPPMWriter = new LibGeoDecomp::PPMWriter<Cell>(
		&Cell::elevation,
		0.0,
		255.0,
		"elevation/ppm/elevation",
		parameters.elevation_ppm_interval,
		LibGeoDecomp::Coord<2>(parameters.ppm_pixels_per_cell, parameters.ppm_pixels_per_cell));

	    sim->addWriter(elevationPPMWriter);
	}
	if(parameters.water_depth_ppm)
	{
	    system("mkdir -p water_depth/ppm");
	    water_depthPPMWriter = new LibGeoDecomp::PPMWriter<Cell>(
		&Cell::water_depth,
		0.0,
		1.0,
		"water_depth/ppm/water_depth",
		parameters.water_depth_ppm_interval,
		LibGeoDecomp::Coord<2>(parameters.ppm_pixels_per_cell, parameters.ppm_pixels_per_cell));

	    sim->addWriter(water_depthPPMWriter);
	}
	// Write out simulation progress
	if (LibGeoDecomp::MPILayer().rank() == 0)
	{
	    sim->addWriter(new LibGeoDecomp::TracingWriter<Cell>(1, parameters.no_of_iterations));
	}
      
	if( LibGeoDecomp::MPILayer().rank() == 0){ std::cout << "\nStarting parallel simulation... \n\n"; }
	sim->run();
	LibGeoDecomp::MPILayer().barrier();
    }
    else // Parallel
    {
	LibGeoDecomp::DistributedSimulator<Cell> *sim = 0;
      
	// Choose LibGeoDecomp Simulator type
	if(parameters.simulator == "striping")
	{
	    sim = new LibGeoDecomp::StripingSimulator<Cell>(
		initialiser,
		LibGeoDecomp::MPILayer().rank()? 0 : new LibGeoDecomp::NoOpBalancer(), 1);
	}
	else if(parameters.simulator == "hipar")
	{
	    sim = new LibGeoDecomp::HiParSimulator<Cell, LibGeoDecomp::RecursiveBisectionPartition<2> >(
		initialiser,
		LibGeoDecomp::MPILayer().rank() ? 0 : new LibGeoDecomp::NoOpBalancer(), 1, 1);
	}
	
	// Set up output to netCDF format
	if(parameters.elevation_netcdf)
	{
	    LibGeoDecomp::PnetCDFWriter<Cell> *elevationPnetCDFWriter = new LibGeoDecomp::PnetCDFWriter<Cell>(
		initialiser->gridDimensions(),
		LibGeoDecomp::Selector<Cell>(&Cell::elevation, "elevation"),
		"elevation",
		parameters.elevation_netcdf_interval,
	        parameters.no_of_iterations);
		
	    sim->addWriter(elevationPnetCDFWriter);
	}
	if(parameters.water_depth_netcdf)
	{
	    LibGeoDecomp::PnetCDFWriter<Cell> *waterDepthPnetCDFWriter = new LibGeoDecomp::PnetCDFWriter<Cell>(
		initialiser->gridDimensions(),
		LibGeoDecomp::Selector<Cell>(&Cell::water_depth, "water_depth"),
		"water_depth",
		parameters.water_depth_netcdf_interval,
		parameters.no_of_iterations);
	    
	    sim->addWriter(waterDepthPnetCDFWriter);
	}
            
	// Set up output to ppm
	if(parameters.elevation_ppm)
	{
	    LibGeoDecomp::PPMWriter<Cell> *elevationPPMWriter = 0;
	    if(LibGeoDecomp::MPILayer().rank() == 0)
	    {
		system("mkdir -p elevation/ppm");
		elevationPPMWriter = new LibGeoDecomp::PPMWriter<Cell>(
		    &Cell::elevation,
		    0.0,
		    255.0,
		    "elevation/ppm/elevation",
		    parameters.elevation_ppm_interval,
		    LibGeoDecomp::Coord<2>(parameters.ppm_pixels_per_cell, parameters.ppm_pixels_per_cell));
	    }
	    LibGeoDecomp::CollectingWriter<Cell> *elevationPPMCollectingWriter = new LibGeoDecomp::CollectingWriter<Cell>(elevationPPMWriter);

	    sim->addWriter(elevationPPMCollectingWriter);
	}
	if(parameters.water_depth_ppm)
	{
	    LibGeoDecomp::PPMWriter<Cell> *water_depthPPMWriter = 0;
	    if(LibGeoDecomp::MPILayer().rank() == 0)
	    {
		system("mkdir -p water_depth/ppm");
		water_depthPPMWriter = new LibGeoDecomp::PPMWriter<Cell>(
		    &Cell::water_depth,
		    0.0,
		    1.0,
		    "water_depth/ppm/water_depth",
		    parameters.water_depth_ppm_interval,
		    LibGeoDecomp::Coord<2>(parameters.ppm_pixels_per_cell,
					   parameters.ppm_pixels_per_cell));
	    }
	    LibGeoDecomp::CollectingWriter<Cell> *water_depthPPMCollectingWriter = new LibGeoDecomp::CollectingWriter<Cell>(water_depthPPMWriter);
	    sim->addWriter(water_depthPPMCollectingWriter);
	}
	if(parameters.water_depth_bov)
	{
	    system("mkdir -p water_depth/bov");
	    sim->addWriter(new LibGeoDecomp::BOVWriter<Cell>(
			       LibGeoDecomp::Selector<Cell>(&Cell::water_depth, "water_depth"),
			       "water_depth/bov/water_depth",
			       parameters.water_depth_bov_interval));
	}
      
// Write out simulation progress
	if (LibGeoDecomp::MPILayer().rank() == 0)
	{
	    sim->addWriter(new LibGeoDecomp::TracingWriter<Cell>(1, parameters.no_of_iterations));
	}
      
	if( LibGeoDecomp::MPILayer().rank() == 0)
	{
	    std::cout << "\nStarting parallel simulation... \n\n";
	}

	sim->run();
	LibGeoDecomp::MPILayer().barrier();
    }
  
    MPI_Finalize();  
    return 0;
}

