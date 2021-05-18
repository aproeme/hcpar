#include <simulation.hpp>

Simulation::Simulation(string parameterFile) :
    parameters(CatchmentParameters(parameterFile))
{}



void Simulation::gatherNetCDFSources()
{
    for (GridQuantity quantity : parameters.inputNetCDFGridQuantities)
    {
	netCDFSources.push_back(LibGeoDecomp::netCDFSource<Cell> {
		parameters.inputNetCDFFileName[static_cast<int>(quantity)],
		parameters.inputNetCDFVariableName[static_cast<int>(quantity)],
		gridQuantitySelectors[static_cast<int>(quantity)]});
    }
}	



void Simulation::prepareInitializer()
{
    gatherNetCDFSources();
    
// Initialise grid (each rank initialises its own subgrid)
#ifdef HC_DEBUG
    initializer = new DebugCellInitializer(parameters.xmax, parameters.ymax, parameters.no_of_iterations);
#else
    initializer = new NetCDFInitializer(parameters, netCDFSources);
#endif
}


void Simulation::prepareSimulator()
{
    LibGeoDecomp::LoadBalancer *balancer;

    if(parameters.simulator == "serial")
    {
	serialSimulator = new LibGeoDecomp::SerialSimulator<Cell>(initializer);
    }
    else if(parameters.simulator == "striping")
    {
	balancer = LibGeoDecomp::MPILayer().rank()? 0 : new LibGeoDecomp::NoOpBalancer();
        parallelSimulator = new LibGeoDecomp::StripingSimulator<Cell>(
	    initializer,
	    balancer,
	    1);
    }
    else if(parameters.simulator == "hipar")
    {
	balancer = LibGeoDecomp::MPILayer().rank()? 0 : new LibGeoDecomp::NoOpBalancer();
	parallelSimulator = new LibGeoDecomp::HiParSimulator<Cell, LibGeoDecomp::RecursiveBisectionPartition<2> >(
	    initializer,
	    balancer,
	    1,
	    1);
    }
}




void Simulation::addWriters()
{
    if (parameters.simulator == "serial")
    {
	//serialSimulator->addWriter(
    }
    else
    {
	for (GridQuantity quantity : parameters.outputNetCDFGridQuantities)
	{
	    pnetCDFWriters.push_back(new LibGeoDecomp::PnetCDFWriter<Cell>(
					 initializer->gridDimensions(),
					 gridQuantitySelectors[static_cast<int>(quantity)],
					 gridQuantityString[static_cast<int>(quantity)],
					 parameters.outputNetCDFInterval[static_cast<int>(quantity)],
					 parameters.no_of_iterations));
	    
	    parallelSimulator->addWriter(pnetCDFWriters[static_cast<int>(quantity)]); 
	}

	if (LibGeoDecomp::MPILayer().rank() == 0)
	{
	    LibGeoDecomp::TracingWriter<Cell> *progressWriter = new LibGeoDecomp::TracingWriter<Cell>(parameters.progress_interval, parameters.no_of_iterations);
	    parallelSimulator->addWriter(progressWriter);
	}
    }
}



void Simulation::run()
{
    if( LibGeoDecomp::MPILayer().rank() == 0)
    {
	std::cout << "\nStarting simulation... \n";
    }

    if (parameters.simulator == "serial")
    {
	serialSimulator->run();
    }
    else
    {
	parallelSimulator->run();
	
	if(LibGeoDecomp::MPILayer().rank() == 0)
	{
	    std::cout << std::endl;
	}
    }
    

    
}









//
//  Example code to set up output to .ppm images 
//
/*
    //==================
    // SERIAL PPM OUTPUT
    //==================
    
    LibGeoDecomp::PPMWriter<Cell> *elevationPPMWriter = 0;
    elevationPPMWriter = new LibGeoDecomp::PPMWriter<Cell>(
    &Cell::elevation,
    0.0,
    255.0,
    "elevation/ppm/elevation",
    parameters.output_elevation_ppm_interval,
    LibGeoDecomp::Coord<2>(parameters.output_ppm_pixels_per_cell, parameters.output_ppm_pixels_per_cell));
    
    sim->addWriter(elevationPPMWriter);
    

    //====================
    // PARALLEl PPM OUTPUT
    //====================
    LibGeoDecomp::PPMWriter<Cell> *elevationPPMWriter = 0;

    if(LibGeoDecomp::MPILayer().rank() == 0)
	    {
		system("mkdir -p elevation/ppm");
		elevationPPMWriter = new LibGeoDecomp::PPMWriter<Cell>(
		    &Cell::elevation,
		    0.0,
		    255.0,
		    "elevation/ppm/elevation",
		    parameters.output_elevation_ppm_interval,
		    LibGeoDecomp::Coord<2>(parameters.output_ppm_pixels_per_cell, parameters.output_ppm_pixels_per_cell));
	    }

	    LibGeoDecomp::CollectingWriter<Cell> *elevationPPMCollectingWriter = new LibGeoDecomp::CollectingWriter<Cell>(elevationPPMWriter);

	    sim->addWriter(elevationPPMCollectingWriter);
*/





    

