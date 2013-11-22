/*
 * Author      : Matthew Johnson
 * Date        : 17/Sep/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	class Spring;
	class PointMass;
	class SpringGrid
	{
	public:
		SpringGrid( const RectI& size, const Vec2f& spacing );
		~SpringGrid();

		void OnUpdate( float dt );
		void OnDraw();

		Vec2f ToVec2( const Vec3f& v ) const;

		void ApplyDirectionalForce( const Vec2f& force, const Vec2f& position, float radius );
		void ApplyDirectionalForce( const Vec3f& force, const Vec3f& position, float radius );

		void ApplyExplosionForce( float force, const Vec2f& position, float radius );
		void ApplyExplosionForce( float force, const Vec3f& position, float radius );

		void ApplyImplosionForce( float force, const Vec2f& position, float radius );
		void ApplyImplosionForce( float force, const Vec3f& position, float radius );

		Vec2i ScreenSize;

	private:
		Vec2f mSize;
		std::vector< Spring* > mSprings;
		std::vector< std::vector< PointMass* > > mPoints;
		std::vector< std::vector< PointMass* > > mPointsFixed;
	};

}