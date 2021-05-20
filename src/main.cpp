#include <iostream>

#include <libgeodecomp/communication/typemaps.h>
#include <libgeodecomp/communication/mpilayer.h>

//#include <typemaps.h>
#include <simulation.hpp>
#include <debugcellinitializer.hpp>
#include <catchmentparameters.hpp>


int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    LibGeoDecomp::Typemaps::initializeMaps(); // initialize LibGeoDecomp default typemaps (this commits MPI types)
    Typemaps::initializeMaps(); // initialize custom typemaps for HAIL-CAESAR
    LibGeoDecomp::MPILayer().barrier();
    
#ifndef GIT_REVISION
#define GIT_REVISION "N/A"
#endif
    
    if(LibGeoDecomp::MPILayer().rank() == 0)
    {
	std::cout << std::endl;
	std::cout << "##################################" << std::endl;
	std::cout << "#  CATCHMENT HYDROGEOMORPHOLOGY  #" << std::endl;
	std::cout << "#        MODEL version ?.?       #" << std::endl;
	std::cout << "#          (HAIL-CAESAR)         #" << std::endl;
	std::cout << "##################################" << std::endl;
	std::cout << " Version: ?.?" << std::endl;
	std::cout << " at git commit number: " GIT_REVISION << std::endl;
	std::cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << std::endl;
      
	if (argc < 2)
	{
	    std::cout << "\n###################################################" << std::endl;
	    std::cout << "No parameter file supplied" << std::endl;
	    std::cout << "You must supply the path to a parameter file" << std::endl;
	    std::cout << "###################################################" << std::endl;
	  
	    exit(0);
	}
      
	if (argc > 2)
	{
	    std::cout << "Too many input arguments supplied (should be 1: path to parameter file)" << std::endl;
	    exit(0);
	}
    }
    
    //LibGeoDecomp::MPILayer().barrier();
    
    std::string parameterFile = argv[1];
    Simulation simulation(parameterFile);
    simulation.prepareInitializer();
    simulation.prepareSimulator();
    simulation.addWriters();
    simulation.run();

    MPI_Finalize();  
    return 0;
}
