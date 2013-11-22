/*
 * Author      : Matthew Johnson
 * Date        : 16/Oct/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	class Stroke
	{
	public:
		Stroke();
		~Stroke();

		void AddPoint( const Vec2f& point );
		void ClearPoints();
		void OnDraw( bool drawApprox=false );

		void GetControl( int i, Vec2f& p, float& t ) const;
		size_t GetNumControls() const	{ return mControlPoints.size(); }

		void ComputeCurve();

	private:
		void Subdivide( const ArrayList< Vec2f >& inCurve, ArrayList< Vec2f >& outCurve );

		ArrayList< Vec2f > mControlPoints;
		ArrayList< float > mControlTimes;
		ArrayList< Vec2f > mCurvePoints;
		bool mIsCurrent;
		uint32 mLevels;
		float mLength;
	};

}