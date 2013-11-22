#include "RendererLib.h"

using namespace mage;

//---------------------------------------
SpringGrid::SpringGrid( const RectI& size, const Vec2f& spacing )
{
	int numCol = (int) ( size.Width() / spacing.x  ) + 1;
	int numRow = (int) ( size.Height() / spacing.y ) + 1;

	mPoints.resize( numCol );
	mPointsFixed.resize( numCol );
	for ( int i = 0; i < numCol; ++i )
	{
		mPoints[i].resize( numRow );
		mPointsFixed[i].resize( numRow );
	}

	// Make masses
	int c = 0, r = 0;
	for ( float y = size.Top; y <= size.Bottom; y += spacing.y )
	{
		for ( float x = size.Left; x <= size.Right; x += spacing.x )
		{
			mPoints[c][r] = new PointMass( Vec3f( x, y, 0 ), 1 );
			mPointsFixed[c][r] = new PointMass( Vec3f( x, y, 0 ), 0 );
			++c;
		}
		++r;
		c = 0;
	}

	// Make springs
	for ( int y = 0; y < numRow; ++y )
	{
		for ( int x = 0; x < numCol; ++x )
		{
			if ( x == 0 || y == 0 || x == numCol - 1 || y == numRow - 1 )
				mSprings.push_back( new Spring( mPointsFixed[x][y], mPoints[x][y], 0.1f, 0.1f ) );
			else if ( x % 3 == 0 && y % 3 == 0 )
				mSprings.push_back( new Spring( mPointsFixed[x][y], mPoints[x][y], 0.002f, 0.02f ) );

			const float k = 0.28f;
			const float damping = 0.06f;

			if ( x != 0 )
				mSprings.push_back( new Spring( mPoints[x-1][y], mPoints[x][y], k, damping ) );
			if ( y != 0 )
				mSprings.push_back( new Spring( mPoints[x][y-1], mPoints[x][y], k, damping ) );
		}
	}
}
//---------------------------------------
SpringGrid::~SpringGrid()
{
	for ( int i = 0; i < mPoints.size(); ++i )
	{
		for ( int j = 0; j < mPoints[0].size(); ++j )
		{
			delete mPoints[i][j];
			delete mPointsFixed[i][j];
		}
	}
	for ( int i = 0; i < mSprings.size(); ++i )
		delete mSprings[i];
}
//---------------------------------------
void SpringGrid::OnUpdate( float dt )
{
	for ( auto itr = mSprings.begin(); itr != mSprings.end(); ++itr )
		(*itr)->OnUpdate( dt );

	for ( int i = 0; i < mPoints.size(); ++i )
	{
		for ( int j = 0; j < mPoints[0].size(); ++j )
		{
			mPoints[i][j]->OnUpdate( dt );
		}
	}
}
//---------------------------------------
void SpringGrid::OnDraw()
{
	int w = mPoints.size();
	int h = mPoints[0].size();
	Color color = Color::DARK_BLUE;

	for ( int y = 1; y < h; ++y )
	{
		for ( int x = 1; x < w; ++x )
		{
			Vec2f left, up;
			Vec2f p = ToVec2( mPoints[x][y]->Position );
			if ( x != 1 )
			{
				left = ToVec2( mPoints[x-1][y]->Position );
				float thickness = y % 3 == 1 ? 3 : 1;

				int clampedx = Mathi::Min( x + 1, w - 1 );
				Vec2f mid = Vec2f::CatmullRom( ToVec2( mPoints[x-2][y]->Position ), left, p, ToVec2( mPoints[clampedx][y]->Position ), 0.5f );

				if ( ((left + p) / 2 - mid).LengthSqr() > 1 )
				{
					DrawLine( left, mid, thickness, color );
					DrawLine( mid, p, thickness, color );
				}
				else
					DrawLine( left, p, thickness, color );
			}
			if ( y != 1 )
			{
				up = ToVec2( mPoints[x][y-1]->Position );
				float thickness = x % 3 == 1 ? 3 : 1;

				int clampedy = Mathi::Min( y + 1, h - 1 );
				Vec2f mid = Vec2f::CatmullRom( ToVec2( mPoints[x][y-2]->Position ), up, p, ToVec2( mPoints[x][clampedy]->Position ), 0.5f );

				if ( ((up + p) / 2 - mid).LengthSqr() > 1 )
				{
					DrawLine( up, mid, thickness, color );
					DrawLine( mid, p, thickness, color );
				}
				else
					DrawLine( up, p, thickness, color );
			}

			if ( x != 1 && y != 1 )
			{
				Vec2f upleft( ToVec2( mPoints[x-1][y-1]->Position ) );
				DrawLine( 0.5f * ( upleft + up ), 0.5f * ( left + p ), 1.0f, color );
				DrawLine( 0.5f * ( upleft + left ), 0.5f * ( up + p ), 1.0f, color );
			}
		}
	}
}
//---------------------------------------
Vec2f SpringGrid::ToVec2( const Vec3f& v ) const
{
	const float factor = ( v.z + 2000 ) / 2000;
	return ( Vec2f( v.x, v.y ) - ScreenSize / 2.0f ) * factor + ScreenSize / 2.0f;
}
//---------------------------------------
void SpringGrid::ApplyDirectionalForce( const Vec2f& force, const Vec2f& position, float radius )
{
	ApplyDirectionalForce( Vec3f( force.x, force.y, 0 ), Vec3f( position.x, position.y, 0 ), radius );
}
//---------------------------------------
void SpringGrid::ApplyDirectionalForce( const Vec3f& force, const Vec3f& position, float radius )
{
	for ( int i = 0; i < mPoints.size(); ++i )
	{
		for ( int j = 0; j < mPoints[0].size(); ++j )
		{
			float d = ( mPoints[i][j]->Position - position ).LengthSqr();
			if ( d < radius * radius )
				mPoints[i][j]->ApplyForce( 10 * force / ( 10 + d ) );
		}
	}
}
//---------------------------------------
void SpringGrid::ApplyExplosionForce( float force, const Vec2f& position, float radius )
{
	ApplyExplosionForce( force, Vec3f( position.x, position.y, 0 ), radius );
}
//---------------------------------------
void SpringGrid::ApplyExplosionForce( float force, const Vec3f& position, float radius )
{
	for ( int i = 0; i < mPoints.size(); ++i )
	{
		for ( int j = 0; j < mPoints[0].size(); ++j )
		{
			Vec3f dp = mPoints[i][j]->Position - position;
			float d = dp.LengthSqr();
			if ( d < radius * radius )
			{
				mPoints[i][j]->ApplyForce( 100 * force * dp / ( 10000 + d ) );
				mPoints[i][j]->IncreaseDamping( 0.6f );
			}
		}
	}
}
//---------------------------------------
void SpringGrid::ApplyImplosionForce( float force, const Vec2f& position, float radius )
{
	ApplyImplosionForce( force, Vec3f( position.x, position.y, 0 ), radius );
}
//---------------------------------------
void SpringGrid::ApplyImplosionForce( float force, const Vec3f& position, float radius )
{
	for ( int i = 0; i < mPoints.size(); ++i )
	{
		for ( int j = 0; j < mPoints[0].size(); ++j )
		{
			Vec3f dp = position - mPoints[i][j]->Position;
			float d = dp.LengthSqr();
			if ( d < radius * radius )
			{
				mPoints[i][j]->ApplyForce( 10 * force * dp / ( 100 + d ) );
				mPoints[i][j]->IncreaseDamping( 0.6f );
			}
		}
	}
}
//---------------------------------------