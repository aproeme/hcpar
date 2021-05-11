#include <cell.hpp>

void Cell::grid(LibGeoDecomp::GridBase<Cell, 2> *localGrid, LibGeoDecomp::Coord<2> globalDimensions)
{
  CellType celltype; 
  LibGeoDecomp::CoordBox<2> localBoundingBox = localGrid->boundingBox();
  
    // Set cell types (edge, corner, etc.) for all cells in local grid
    for (int x=0; x<globalDimensions.x(); x++)
      {
	for (int y=0; y<globalDimensions.y(); y++)
	  {
	    if (y == 0)
	      {
		if (x == 0) celltype = Cell::CORNER_SW;
		else if (x == globalDimensions.x()-1) celltype = Cell::CORNER_SE;
		else celltype = Cell::EDGE_SOUTH;
	      }
	    else if (y == globalDimensions.y()-1)
	      {
		if (x == 0) celltype = Cell::CORNER_NW;
		else if (x == globalDimensions.x()-1) celltype = Cell::CORNER_NE;
		else celltype = Cell::EDGE_NORTH;
	      }
	    else  
	      {
		if (x == 0) celltype = Cell::EDGE_WEST;
		else if (x == globalDimensions.x()-1) celltype = Cell::EDGE_EAST;
		else celltype = Cell::INTERNAL;
	      }
	    
	    LibGeoDecomp::Coord<2> coordinate(x, y);
	    if (localBoundingBox.inBounds(coordinate))
	      {
		localGrid->set(coordinate, Cell(celltype)); 
	      }
	    
	  }
      }
}
