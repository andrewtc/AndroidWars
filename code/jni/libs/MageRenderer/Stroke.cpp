#include "RendererLib.h"

using namespace mage;

//---------------------------------------
Stroke::Stroke()
	: mIsCurrent( false )
	, mLevels( 3U )
{}
//---------------------------------------
Stroke::~Stroke()
{}
//---------------------------------------
void Stroke::AddPoint( const Vec2f& point )
{
	if ( mCurvePoints.empty() || ( mCurvePoints.back() != point ) )
	{
		mCurvePoints.push_back( point );
		mIsCurrent = false;
	}
}
//---------------------------------------
void Stroke::ClearPoints()
{
	mIsCurrent = false;
	mCurvePoints.clear();
	mControlPoints.clear();
	mControlTimes.clear();
}
//---------------------------------------
void Stroke::OnDraw( bool drawApprox )
{
	/*if ( !mIsCurrent )
	{
		ComputeCurve();
		mIsCurrent = true;
	}*/

	DrawPolyLine( (float*)&mCurvePoints[0], mCurvePoints.size(), 2.0f, Color::BLUE, false );
	if ( drawApprox )
	{
		DrawPoints( (float*)&mControlPoints[0], mControlPoints.size(), 4.0f, Color::RED );
		DrawPolyLine( (float*)&mControlPoints[0], mControlPoints.size(), 1.0f, Color::ORANGE, false );
	}
}
//---------------------------------------
void Stroke::GetControl( int i, Vec2f& p, float& t ) const
{
	p = mControlPoints[i];
	t = mControlTimes[i];
}
//---------------------------------------
void Stroke::ComputeCurve()
{
	if ( mIsCurrent )
		return;

	Vec2f p0, p1, p2;
	ArrayList< float > lengths;
	float totalLength = 0;

	mLength = 0;

	if ( mCurvePoints.size() == 0 )
		return;

	mControlPoints.push_back( mCurvePoints[0] );

	for ( size_t i = 1; i < mCurvePoints.size() - 1; ++i )
	{
		p0 = mCurvePoints[ i - 1 ];
		p1 = mCurvePoints[ i     ];
		p2 = mCurvePoints[ i + 1 ];
		
		float a = std::acosf( ( p2 - p1 ).Normalize() * ( p1 - p0 ).Normalize() );

		if ( a > Mathf::DegreesToRadians( 5 ) )
		{
			mControlPoints.push_back( p2 );
		}
	}

	mControlPoints.push_back( mCurvePoints.back() );

	if ( mControlPoints.size() == 1 )
		return;

	if ( mControlPoints.size() == 2 )
	{
		p0 = mControlPoints[ 0 ];
		p1 = mControlPoints[ 1 ];

		float d = p0.GetEuclidianDistanceTo( p1 );
		mLength += d;
		lengths.push_back( d );
	}
	else
	{
		for ( size_t i = 1; i < mControlPoints.size() - 1; ++i )
		{
			p0 = mControlPoints[ i - 1 ];
			p1 = mControlPoints[ i ];

			float d = p0.GetEuclidianDistanceTo( p1 );
			mLength += d;
			lengths.push_back( d );
		
		}
	}

	for ( size_t i = 0; i < mControlPoints.size(); ++i )
	{
		float d = lengths[i];
		float t = totalLength / mLength;

		totalLength += d;

		mControlTimes.push_back( t );
	}

	/*
	ArrayList< Vec2f > tmp;

	Subdivide( mControlPoints, mCurvePoints );

	for ( uint32 i = 0; i < mLevels / 2; ++i )
	{
		Subdivide( mCurvePoints, tmp );
		Subdivide( tmp, mCurvePoints );
	}*/
}
//---------------------------------------
void Stroke::Subdivide( const ArrayList< Vec2f >& inCurve, ArrayList< Vec2f >& outCurve )
{
	outCurve.clear();

	// No input
	if ( inCurve.size() == 0 )
	{
		return;
	}

	Vec2f p0, p1, p2;

	p0 = inCurve[0];
	outCurve.push_back( p0 );

	// Single point
	if ( inCurve.size() == 1 )
	{
		return;
	}

	p1 = inCurve[1];

	// Line segment
	if ( inCurve.size() == 2 )
	{
		outCurve.push_back( p1 );
		return;
	}

	// Polyline
	outCurve.push_back( ( p0 + p1 ) / 2.0f );

	for ( size_t i = 1; i < inCurve.size() - 1; ++i )
	{
		p0 = inCurve[ i - 1 ];
		p1 = inCurve[ i     ];
		p2 = inCurve[ i + 1 ];

		outCurve.push_back( ( p0 + 6.0f * p1 + p2 ) / 8.0f );
		outCurve.push_back( ( p1 + p2 ) / 2.0f );
	}
	outCurve.push_back( p2 );
}
//---------------------------------------