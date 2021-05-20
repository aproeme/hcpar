#ifndef HC_SIMULATION_H
#define HC_SIMULATION_H
#endif

#include <string>
#include <vector>

#include <gridquantities.hpp>
#include <cell.hpp>
#include <catchmentparameters.hpp>
#include <netcdfinitializer.hpp>
#include <selectmpidatatype.tpp>
#ifdef HC_DEBUG
#include <debugcellinitializer.hpp>
#endif

#include <libgeodecomp/communication/mpilayer.h>
#include <libgeodecomp/communication/typemaps.h>
#include <libgeodecomp/parallelization/simulator.h>
#include <libgeodecomp/parallelization/serialsimulator.h>
#include <libgeodecomp/parallelization/stripingsimulator.h>
#include <libgeodecomp/parallelization/hiparsimulator.h>
#include <libgeodecomp/geometry/partitions/recursivebisectionpartition.h>
#include <libgeodecomp/loadbalancer/noopbalancer.h>
#include <libgeodecomp/io/ppmwriter.h>
#include <libgeodecomp/io/tracingwriter.h>
#include <libgeodecomp/io/collectingwriter.h>
#include <libgeodecomp/io/parallelwriter.h>
#include <libgeodecomp/io/initializer.h>
#include <libgeodecomp/io/pnetcdfinitializer.h>
#include <libgeodecomp/io/pnetcdfwriter.h>


class Simulation
{
public:
    // Read parameters on each rank
    // (can replace with MPI_Bcast from rank 0 if overhead ever becomes noticeable)
    Simulation(string parameterFile);

    void gatherNetCDFSources();
    
    void prepareInitializer();
    
    void prepareSimulator();
    
    void addWriters();
    
    void run();
    
    CatchmentParameters parameters;
    vector<LibGeoDecomp::netCDFSource<Cell>> netCDFSources;
    LibGeoDecomp::Initializer<Cell> *initializer;
    LibGeoDecomp::SerialSimulator<Cell> *serialSimulator;
    LibGeoDecomp::DistributedSimulator<Cell> *parallelSimulator;
    vector<LibGeoDecomp::PnetCDFWriter<Cell>*> pnetCDFWriters;
};


  
