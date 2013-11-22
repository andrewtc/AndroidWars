//---------------------------------------
template< typename TReal >
Rect< TReal >::Rect()
	: Left( 0 )
	, Right( 0 )
	, Top( 0 )
	, Bottom( 0 )
{}
//---------------------------------------
template< typename TReal >
Rect< TReal >::Rect( TReal left, TReal top, TReal right, TReal bottom )
	: Left( left )
	, Right( right )
	, Top( top )
	, Bottom( bottom )
{}
//---------------------------------------
template< typename TReal >
Rect< TReal >::~Rect()
{}
//---------------------------------------
template< typename TReal >
inline TReal Rect< TReal >::CenterX() const
{
	return (TReal) ( ( Right - Left ) / 2.0 );
}
//---------------------------------------
template< typename TReal >
inline TReal Rect< TReal >::CenterY() const
{
	return (TReal) ( ( Bottom - Top ) / 2.0 );
}
//---------------------------------------
template< typename TReal >
inline TReal Rect< TReal >::Height() const
{
	return (TReal) ( Bottom - Top );
}
//---------------------------------------
template< typename TReal >
inline TReal Rect< TReal >::Width() const
{
	return (TReal) ( Right - Left );
}
//---------------------------------------
template< typename TReal >
inline bool Rect< TReal >::IsValid() const
{
	return Left < Right && Top < Bottom;
}
//---------------------------------------
template< typename TReal >
void Rect< TReal >::MakeValid() const
{
	if ( Left < Right )
	{
		TReal tmp = Left;
		Left = Right;
		Right = tmp;
	}

	if ( Top < Bottom )
	{
		TReal tmp = Top;
		Top = Bottom;
		Bottom = tmp;
	}
}
//---------------------------------------
template< typename TReal >
void Rect< TReal >::Set( TReal left, TReal top, TReal right, TReal bottom )
{
	Left   = left;
	Right  = right;
	Top    = top;
	Bottom = bottom;
}
//---------------------------------------
template< typename TReal >
void Rect< TReal >::Set( const Rect& r )
{
	Left   = r.Left;
	Right  = r.Right;
	Top    = r.Top;
	Bottom = r.Bottom;
}
//---------------------------------------
template< typename TReal >
bool Rect< TReal >::Contains( TReal left, TReal top, TReal right, TReal bottom ) const
{
	return left >= Left && right <= Right && top >= Top && bottom <= Bottom;
}
//---------------------------------------
template< typename TReal >
bool Rect< TReal >::Contains( TReal x, TReal y ) const
{
	return Contains( x, y, x, y );
}
//---------------------------------------
template< typename TReal >
bool Rect< TReal >::Contains( const Rect& r ) const
{
	return Contains( r.Left, r.Top, r.Right, r.Bottom );
}
//---------------------------------------
template< typename TReal >
bool Rect< TReal >::Intersect( TReal left, TReal top, TReal right, TReal bottom )
{
	bool intersects = TestIntersection( left, top, right, bottom );

	if ( intersects )
	{
		Set( 
			Math< TReal >::Max( Left,   left   ),
			Math< TReal >::Max( Top,    top    ),
			Math< TReal >::Min( Right,  right  ),
			Math< TReal >::Min( Bottom, bottom )
		);
	}

	return intersects;
}
//---------------------------------------
template< typename TReal >
bool Rect< TReal >::Intersect( const Rect& r )
{
	return Intersect( r.Left, r.Top, r.Right, r.Bottom );
}
//---------------------------------------
template< typename TReal >
bool Rect< TReal >::SetIntersect( const Rect& A, const Rect& B )
{
	bool intersects = TestIntersection( A, B );

	if ( intersects )
	{
		Set( 
			Math< TReal >::Max( A.Left,   B.Left   ),
			Math< TReal >::Max( A.Top,    B.Top    ),
			Math< TReal >::Min( A.Right,  B.Right  ),
			Math< TReal >::Min( A.Bottom, B.Bottom )
		);
	}

	return intersects;
}
//---------------------------------------
template< typename TReal >
bool Rect< TReal >::TestIntersection( TReal left, TReal top, TReal right, TReal bottom ) const
{
	return Left <= right && Right >= left && Top <= bottom && Bottom >= top;
}
//---------------------------------------
template< typename TReal >
bool Rect< TReal >::TestIntersection( const Rect& r ) const
{
	return TestIntersection( r.Left, r.Top, r.Right, r.Bottom );
}
//---------------------------------------
template< typename TReal >
bool Rect< TReal >::TestIntersection( const Rect& A, const Rect& B )
{
	return A.TestIntersection( B );
}
//---------------------------------------