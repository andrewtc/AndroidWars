//---------------------------------------
template< typename PlotPoint >
void PlotLine( int x0, int y0, int x1, int y1, PlotPoint plotPoint )
{
	int dx =  std::abs( x1 - x0 ), sx = x0 < x1 ? 1 : -1;
	int dy = -std::abs( y1 - y0 ), sy = y0 < y1 ? 1 : -1;
	int err = dx + dy, e2;

	while ( true )
	{
		plotPoint( x0, y0 );
		if ( x0 == x1 && y0 == y1 ) break;
		e2 = 2 * err;
		if ( e2 >= dy ) { err += dy; x0 += sx; }
		if ( e2 <= dx ) { err += dx; y0 += sy; }
	}
}
//---------------------------------------
template< typename PlotPoint >
void PlotLineAA( int x0, int y0, int x1, int y1, PlotPoint plotPoint )
{
}
//---------------------------------------
template< typename PlotPoint >
void PlotThickLineAA( int x0, int y0, int x1, int y1, float width, PlotPoint plotPoint )
{
}
//---------------------------------------
template< typename PlotPoint >
void PlotCircle( int x, int y, int r, PlotPoint plotPoint, bool fill )
{
	for ( int d = fill ? 1 : r-1; d < r; ++d )
	{
		int ix = d, iy = 0;
		int dx = 1 - ( d << 1 );
		int dy = 0;
		int radialError = 0;

		while ( ix >= iy )
		{
			if ( fill )
			{
				PlotLine( ix + x, iy + y, -ix + x, iy + y, plotPoint );
				PlotLine( iy + x, ix + y, -iy + x, ix + y, plotPoint );
				PlotLine( -ix + x,-iy + y, ix + x,-iy + y, plotPoint );
				PlotLine( -iy + x,-ix + y, iy + x,-ix + y, plotPoint );
			}
			else
			{
				plotPoint(  ix + x, iy + y );
				plotPoint(  iy + x, ix + y );
				plotPoint( -ix + x, iy + y );
				plotPoint( -iy + x, ix + y );
				plotPoint( -ix + x,-iy + y );
				plotPoint( -iy + x,-ix + y );
				plotPoint(  ix + x,-iy + y );
				plotPoint(  iy + x,-ix + y );
			}
			++iy;
			radialError += dy;
			dy += 2;
			if ( ((radialError << 1 ) + dx) > 0 )
			{
				--ix;
				radialError += dx;
				dx += 2;
			}
		}
	}
}
//---------------------------------------
template< typename PlotPoint >
void PlotEllipse( int x0, int y0, int x1, int y1, PlotPoint plotPoint, bool fill )
{
}
//---------------------------------------
template< typename PlotPoint >
void PlotBezierSegment( int x0, int y0, int x1, int y1, int x2, int y2, PlotPoint plotPoint )
{
}
//---------------------------------------
template< typename PlotPoint >
void PlotBezierSegmentAA( int x0, int y0, int x1, int y1, int x2, int y2, PlotPoint plotPoint )
{
}
//---------------------------------------