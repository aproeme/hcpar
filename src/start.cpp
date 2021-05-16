#include <libgeodecomp/communication/mpilayer.h>

#ifndef GIT_REVISION
#define GIT_REVISION "N/A"
#endif

void start(int argc, char *argv[])
{

  if(LibGeoDecomp::MPILayer().rank() == 0)
    {
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

  LibGeoDecomp::MPILayer().barrier();

}

