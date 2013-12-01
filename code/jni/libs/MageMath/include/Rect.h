/*
 * Author      : Matthew Johnson
 * Date        : 18/May/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	template< typename TReal >
	class Rect
	{
	public:
		Rect();
		Rect( TReal left, TReal top, TReal right, TReal bottom );
		~Rect();

		inline TReal CenterX() const;
		inline TReal CenterY() const;
		inline TReal Height() const;
		inline TReal Width() const;

		// Casting
		template< typename TOther >
		operator Rect< TOther >()
		{
			return Rect< TOther >(
					static_cast< TOther >( Left ),
					static_cast< TOther >( Top ),
					static_cast< TOther >( Right ),
					static_cast< TOther >( Bottom )
					);
		}

		// Returns true if left < right && top < bottom
		inline bool IsValid() const;
		// Swap left/right and/or top/bottom as needed to make this rect valid
		void MakeValid() const;

		void Set( TReal left, TReal top, TReal right, TReal bottom );
		void Set( const Rect& r );

		bool Contains( TReal left, TReal top, TReal right, TReal bottom ) const;
		bool Contains( TReal x, TReal y ) const;
		bool Contains( const Rect& r ) const;

		// Boolean intersection
		// Returns true if rectangles intersect and sets this rect to the intersection
		bool Intersect( TReal left, TReal top, TReal right, TReal bottom );
		bool Intersect( const Rect& r );
		// If A and B intersect, set this rect to that intersection
		bool SetIntersect( const Rect& A, const Rect& B );

		bool TestIntersection( TReal left, TReal top, TReal right, TReal bottom ) const;
		bool TestIntersection( const Rect& r ) const;
		static bool TestIntersection( const Rect& A, const Rect& B );

		TReal Left;
		TReal Right;
		TReal Top;
		TReal Bottom;
	};

#include "Rect.hpp"

	typedef Rect< float > RectF;
	typedef Rect< int   > RectI;
}
