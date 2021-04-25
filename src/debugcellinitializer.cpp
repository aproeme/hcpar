#include <debugcellinitializer.hpp>
#include <libgeodecomp/communication/mpilayer.h>

using namespace TNT;


DebugCellInitializer::DebugCellInitializer(int xmax,
				 int ymax,
				 unsigned no_of_iterations) :
  LibGeoDecomp::SimpleInitializer<Cell>(LibGeoDecomp::Coord<2>(xmax, ymax), no_of_iterations)
{
    elevation = Array2D<double> (xmax, ymax, 255.0);
    water_depth = Array2D<double> (xmax, ymax, 255.0);
    
    for (int x=0; x<xmax; x++)
    {
	for (int y=0; y<ymax; y++)
	{
	    elevation[x][y] = (x+1)*(y+1);
	    water_depth[x][y] = 10.0;
	}
    }
}


DebugCellInitializer::DebugCellInitializer(int xmax,
				 int ymax,
				 Array2D<double>& elevation_in,
				 Array2D<double>& water_depth_in,
				 unsigned no_of_iterations) :
  LibGeoDecomp::SimpleInitializer<Cell>(LibGeoDecomp::Coord<2>(xmax, ymax), no_of_iterations)
{
  elevation = elevation_in;
  water_depth = water_depth_in;
}


void DebugCellInitializer::grid(LibGeoDecomp::GridBase<Cell, 2> *subgrid)
{
  Cell::CellType celltype;
  LibGeoDecomp::CoordBox<2> subgridBoundingBox = subgrid->boundingBox();
  std::string set_elevation = "";
  std::string check_set_elevation = "";

  for (int x=0; x<dimensions.x(); x++)
    {
      for (int y=0; y<dimensions.y(); y++)
	{
	  if (y == 0)
	    {
	      if (x == 0) celltype = Cell::CORNER_NW;
	      else if (x == dimensions.x()-1) celltype = Cell::CORNER_NE;
	      else celltype = Cell::EDGE_NORTH;
	    }
	  else if (y == dimensions.y()-1)
	    {
	      if (x == 0) celltype = Cell::CORNER_SW;
	      else if (x == dimensions.x()-1) celltype = Cell::CORNER_SE;
	      else celltype = Cell::EDGE_SOUTH;
	    }
	  else  
	    {
	      if (x == 0) celltype = Cell::EDGE_WEST;
	      else if (x == dimensions.x()-1) celltype = Cell::EDGE_EAST;
	      else celltype = Cell::INTERNAL;
	    }
	  
	  LibGeoDecomp::Coord<2> coordinate(x, y);
	  
	  // ensure each rank only initialises its subgrid
	  if (subgridBoundingBox.inBounds(coordinate))
	    {
		set_elevation += "rank " + std::to_string(LibGeoDecomp::MPILayer().rank()) + ": (x,y) = (" +  std::to_string(x) + "," + std::to_string(y) + "), elevation value to set = " + std::to_string(elevation[x][y]) + '\n';
		subgrid->set(coordinate, Cell(celltype, elevation[x][y], water_depth[x][y], 0.0, 0.0));
//		check_set_elevation += "rank " + std::to_string(LibGeoDecomp::MPILayer().rank()) + ": (x,y) = (" +  std::to_string(x) + "," + std::to_string(y) + "), check elevation value set = " + std::to_string(subgrid->get(coordinate).elevation) + '\n';
		
	    }
	}
    }
  std::cout << set_elevation;
//  std::cout << check_set_elevation;
}



