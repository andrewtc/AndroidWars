#include "CoreLib.h"

using namespace mage;

MAGE_IMPLEMENT_RTTI( AnimationController, KeyframeController );

//---------------------------------------
KeyframeController::KeyframeController()
	: mLastPositionIndex( 0 )
	, mLastRotationIndex( 0 )
	, mLastPositionRotationIndex( 0 )
{}
//---------------------------------------
KeyframeController::~KeyframeController()
{}
//---------------------------------------
bool KeyframeController::OnUpdate( float currentTime )
{
	if ( !AnimationController::OnUpdate( currentTime ) )
	{
		return false;
	}

	// The controlled object must be of base type Node
	// in order to have position and rotation
	Node* node = (Node*) mObject;
	const float time = GetNormalTime( currentTime );
	float normTime = 0.0f;
	int i0 = 0, i1 = 0;
	Vec2f position;
	float rotation;

	// Pos
	if ( !PositionKeys.empty() )
	{
		GetKeyframe( time, PositionKeys, mLastPositionIndex, normTime, i0, i1 );
		position = GetInterpolatedPosition( normTime, i0, i1 );
		node->LocalTransform.SetTranslation( position );
	}

	// Rot
	if ( !RotationKeys.empty() )
	{
		GetKeyframe( time, RotationKeys, mLastRotationIndex, normTime, i0, i1 );
		rotation = GetInterpolatedRotation( normTime, i0, i1 );
		node->LocalTransform.SetRotation( rotation );
	}

	// PosRot
	if ( !PositionRotationKeys.empty() )
	{
		GetKeyframe( time, PositionRotationKeys, mLastPositionRotationIndex, normTime, i0, i1 );
		GetInterpolatedPositionRotation( position, rotation, normTime, i0, i1 );
		node->WorldTransform.SetTranslation( position );
		node->WorldTransform.SetRotation( rotation );
		node->WorldTransformIsCurrent = true;
		//node->LocalTransform.SetTranslation( position );
		//node->LocalTransform.SetRotation( rotation );
	}

	return true;
}
//---------------------------------------
void KeyframeController::InsertPositionRotationKey( const PositionRotationKey& key )
{
	// Insert key at back
	PositionRotationKeys.push_back( key );
	// Sort keys by time
	std::sort( PositionRotationKeys.begin(), PositionRotationKeys.end() );
	// Cached index is probably invalid - clear it
	mLastPositionRotationIndex = 0;
}
//---------------------------------------
void KeyframeController::RemovePositionRotationKey( float time )
{
	PositionRotationKeys.erase( PositionRotationKeys.begin() + mLastPositionRotationIndex );
	mLastPositionRotationIndex = 0;
}
//---------------------------------------
float KeyframeController::GetNextPositionRotationKeyTime()
{
	if ( PositionRotationKeys.empty() )
		return 0.0f;

	if ( ++mLastPositionRotationIndex >= PositionRotationKeys.size() )
		mLastPositionRotationIndex = 0;

	return PositionRotationKeys[ mLastPositionRotationIndex ].Time;
}
//---------------------------------------
float KeyframeController::GetPrevPositionRotationKeyTime()
{
	if ( PositionRotationKeys.empty() )
		return 0.0f;

	if ( --mLastPositionRotationIndex < 0 )
		mLastPositionRotationIndex = PositionRotationKeys.size() - 1;

	return PositionRotationKeys[ mLastPositionRotationIndex ].Time;
}
//---------------------------------------
void KeyframeController::Reset()
{
	mLastPositionIndex = 0;
	mLastRotationIndex = 0;
	mLastPositionRotationIndex = 0;
	mTime = 0;
}
//---------------------------------------
template< typename TKey >
void KeyframeController::GetKeyframe( float time, const ArrayList< TKey >& keys,
	int& lastIndex, float& normTime, int& i0, int& i1 )
{
	const int numKeys = (int) keys.size();

	if ( time <= keys[0].Time )
	{
		normTime = 0.0f;
		lastIndex = 0;
		i0 = 0;
		i1 = 1;
	}
	else
	{
		if ( time >= keys[ numKeys - 1 ].Time )
		{
			normTime = 0.0f;
			lastIndex = numKeys - 1;
			i0 = lastIndex;
			i1 = lastIndex;
		}
		else
		{
			int nextIndex;
			if ( time > keys[ lastIndex ].Time )
			{
				nextIndex = lastIndex + 1;
				while ( time >= keys[ nextIndex ].Time )
				{
					lastIndex = nextIndex++;
				}

				i0 = lastIndex;
				i1 = nextIndex;
				normTime = ( time - keys[ i0 ].Time ) / ( keys[ i1 ].Time - keys[ i0].Time );
			}
			else if ( time < keys[ lastIndex ].Time )
			{
				nextIndex = lastIndex - 1;
				while ( time <= keys[ nextIndex ].Time )
				{
					lastIndex = nextIndex--;
				}

				i0 = nextIndex;
				i1 = lastIndex;
				normTime = ( time - keys[ i0 ].Time ) / ( keys[ i1 ].Time - keys[ i0 ].Time );
			}
			else
			{
				normTime = 0.0f;
				i0 = lastIndex;
				i1 = lastIndex;
			}
		}
	}
}
//---------------------------------------
Vec2f KeyframeController::GetInterpolatedPosition( float time, int i0, int i1 ) const
{
	const Vec2f& P0 = PositionKeys[ i0 ].Position;
	const Vec2f& P1 = PositionKeys[ i1 ].Position;
	return P0 + time * ( P1 - P0 );
}
//---------------------------------------
float KeyframeController::GetInterpolatedRotation( float time, int i0, int i1 ) const
{
	const float A0 = RotationKeys[ i0 ].Rotation;
	const float A1 = RotationKeys[ i1 ].Rotation;
	return A0 + time * ( A1 - A0 );
}
//---------------------------------------
void KeyframeController::GetInterpolatedPositionRotation( Vec2f& pos, float& rot, float time, int i0, int i1 ) const
{
	const Vec2f& P0 = PositionRotationKeys[ i0 ].Position;
	const Vec2f& P1 = PositionRotationKeys[ i1 ].Position;
	const float A0 = PositionRotationKeys[ i0 ].Rotation;
	const float A1 = PositionRotationKeys[ i1 ].Rotation;

	pos = P0 + time * ( P1 - P0 );
	rot = A0 + time * ( A1 - A0 );
}
//---------------------------------------