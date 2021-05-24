#include <debugcellinitializer.hpp>
#include <libgeodecomp/communication/mpilayer.h>

using namespace TNT;


DebugCellInitializer::DebugCellInitializer(int xmax,
				 int ymax,
				 unsigned no_of_iterations) :
  LibGeoDecomp::SimpleInitializer<Cell>(LibGeoDecomp::Coord<2>(xmax, ymax), no_of_iterations)
{
    elevation = Array2D<double> (xmax, ymax, 255.0);
    waterDepth = Array2D<double> (xmax, ymax, 255.0);
    
    for (int x=0; x<xmax; x++)
    {
	for (int y=0; y<ymax; y++)
	{
	    elevation[x][y] = (x+1)*(y+1);
	    waterDepth[x][y] = 10.0;
	}
    }
}


DebugCellInitializer::DebugCellInitializer(int xmax,
				 int ymax,
				 Array2D<double>& elevation_in,
				 Array2D<double>& waterDepth_in,
				 unsigned no_of_iterations) :
  LibGeoDecomp::SimpleInitializer<Cell>(LibGeoDecomp::Coord<2>(xmax, ymax), no_of_iterations)
{
  elevation = elevation_in;
  waterDepth = waterDepth_in;
}


void DebugCellInitializer::grid(LibGeoDecomp::GridBase<Cell, 2> *subgrid)
{
  Cell::CellType celltype;
  LibGeoDecomp::CoordBox<2> subgridBoundingBox = subgrid->boundingBox();

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
	  
	  // each rank only initialises its subgrid
	  if (subgridBoundingBox.inBounds(coordinate))
	    {
		Cell cell = subgrid->get(coordinate);
		cell.elevation = elevation[x][y];
		cell.waterDepth = waterDepth[x][y];
		subgrid->set(coordinate, cell); 
	    }
	}
    }
}



