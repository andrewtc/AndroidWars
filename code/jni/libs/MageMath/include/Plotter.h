/*
 * Author      : Matthew Johnson
 * Date        : 3/May/2013
 * Description :
 *   Various plotting algorithms
 */
 
#pragma once

namespace mage
{

	// Bresenham algorithms
	// TFunction is of the form
	// plotPoint( int x, int y )
	// where x and y are the point that should be plotted
	template< typename TFunction >
	static void PlotLine( int x0, int y0, int x1, int y1, TFunction plotPoint );
	template< typename TFunction  >
	static void PlotLineAA( int x0, int y0, int x1, int y1, TFunction plotPoint );
	template< typename TFunction  >
	static void PlotThickLineAA( int x0, int y0, int x1, int y1, float width, TFunction plotPoint );
	template< typename TFunction  >
	static void PlotCircle( int x, int y, int r, TFunction plotPoint, bool fill=false );
	template< typename TFunction  >
	static void PlotEllipse( int x0, int y0, int x1, int y1, TFunction plotPoint, bool fill=false );
	template< typename TFunction  >
	static void PlotBezierSegment( int x0, int y0, int x1, int y1, int x2, int y2, TFunction plotPoint );
	template< typename TFunction  >
	static void PlotBezierSegmentAA( int x0, int y0, int x1, int y1, int x2, int y2, TFunction plotPoint );

#include "Plotter.hpp"
}