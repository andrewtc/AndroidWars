/*
 * Author      : Matthew Johnson
 * Date        : 4/Feb/2013
 * Description :
 *   Arbitrary Box. A Box is defined as:
 *     center
 *     edges (direction vectors, used for orientation)
 *     edge lengths
 */
 
#pragma once

#include "Vector3.h"

namespace mage
{

	// 3D Box
	template< typename TReal >
	class Box3
	{
	public:
		Box3();
		Box3( const Vector3< TReal >& center,
			const Vector3< TReal >* axes,
			const TReal* lengths );
		~Box3();

		// Get the vertex positions for this box
		void GetVerticies( Vector3< TReal > verts[8] ) const;

		Vector3< TReal > Center;
		Vector3< TReal > Axes[3];
		TReal Lengths[3];
	};


	//---------------------------------------
	// Implementation
	//---------------------------------------
	template< typename TReal >
	Box3< TReal >::Box3()
	{}
	//---------------------------------------
	template< typename TReal >
	Box3< TReal >::Box3( const Vector3< TReal >& center,
		const Vector3< TReal >* axes,
		const TReal* lengths )
		: Center( center )
	{
		Axes[0] = axes[0];
		Axes[1] = axes[1];
		Axes[2] = axes[2];

		Lengths[0] = lengths[0];
		Lengths[1] = lengths[1];
		Lengths[2] = lengths[2];
	}
	//---------------------------------------
	template< typename TReal >
	Box3< TReal >::~Box3()
	{}
	//---------------------------------------
	template< typename TReal >
	void Box3< TReal >::GetVerticies( Vector3< TReal > verts[8] ) const
	{
		Vector3< TReal > x = Lengths[0] * Axes[0];
		Vector3< TReal > y = Lengths[1] * Axes[1];
		Vector3< TReal > z = Lengths[2] * Axes[2];

		verts[0] = Center - x - y - z;
		verts[1] = Center + x - y - z;
		verts[2] = Center + x + y - z;
		verts[3] = Center - x + y - z;
		verts[4] = Center - x - y + z;
		verts[5] = Center + x - y + z;
		verts[6] = Center + x + y + z;
		verts[7] = Center - x + y + z;
	}
	//---------------------------------------

	typedef Box3< float  > Box3f;
	typedef Box3< double > Box3d;
}