#include "androidwars.h"

using namespace mage;


const char* const ArrowSprite::ANIMATION_SET_NAME = "Arrow";
const HashString ArrowSprite::DEFAULT_ANIMATION_NAME = "Idle";


ArrowSprite::ArrowSprite() :
	mMapView( nullptr ),
	mNeedsUpdate( true )
{ }


ArrowSprite::~ArrowSprite()
{
	if( IsInitialized() )
	{
		Destroy();
	}
}


void ArrowSprite::Init( MapView* mapView )
{
	assertion( !IsInitialized(), "Cannot initialize ArrowSprite that has already been initialized!" );

	// Keep track of the MapView.
	mMapView = mapView;

	// Force the segment Sprites to be recreated.
	mNeedsUpdate = true;

	// Load the arrow animation.
	std::string animationSetPath = Scenario::FormatAnimationPath( ANIMATION_SET_NAME );
	mAnimationSetName = Scenario::FormatAnimationName( ANIMATION_SET_NAME );
	SpriteManager::LoadSpriteAnimations( animationSetPath.c_str() );

	// Register arrow animations.
	RegisterSegmentAnimation( "EndEast",  PrimaryDirection::NONE, PrimaryDirection::EAST );
	RegisterSegmentAnimation( "EndNorth", PrimaryDirection::NONE, PrimaryDirection::NORTH );
	RegisterSegmentAnimation( "EndWest",  PrimaryDirection::NONE, PrimaryDirection::WEST );
	RegisterSegmentAnimation( "EndSouth", PrimaryDirection::NONE, PrimaryDirection::SOUTH );

	RegisterSegmentAnimationBidirectional( "EastWest", PrimaryDirection::EAST, PrimaryDirection::WEST );
	RegisterSegmentAnimationBidirectional( "NorthSouth", PrimaryDirection::NORTH, PrimaryDirection::SOUTH );

	RegisterSegmentAnimationBidirectional( "EastNorth", PrimaryDirection::EAST, PrimaryDirection::NORTH );
	RegisterSegmentAnimationBidirectional( "NorthWest", PrimaryDirection::NORTH, PrimaryDirection::WEST );
	RegisterSegmentAnimationBidirectional( "WestSouth", PrimaryDirection::WEST, PrimaryDirection::SOUTH );
	RegisterSegmentAnimationBidirectional( "EastSouth", PrimaryDirection::EAST, PrimaryDirection::SOUTH );

	RegisterSegmentAnimation( "HeadEast",  PrimaryDirection::WEST,  PrimaryDirection::NONE );
	RegisterSegmentAnimation( "HeadNorth", PrimaryDirection::SOUTH, PrimaryDirection::NONE );
	RegisterSegmentAnimation( "HeadWest",  PrimaryDirection::EAST,  PrimaryDirection::NONE );
	RegisterSegmentAnimation( "HeadSouth", PrimaryDirection::NORTH, PrimaryDirection::NONE );

	// Register path changed callback.
	mPath.OnChanged.AddCallback( this, &ArrowSprite::InvalidateSegments );
}


void ArrowSprite::Destroy()
{
	assertion( IsInitialized(), "Cannot destroy ArrowSprite that is not initialized!" );

	// Unregister animation set name.
	mAnimationSetName = "";

	// Unregister all arrow animation names.
	mArrowAnimationNamesByID.clear();

	// Clear the path and remove all callbacks.
	mPath.Clear();
	mPath.OnChanged.RemoveAllCallbacks();

	// Destroy all segments.
	DestroyAllSegments();

	// Unregister the MapView.
	mMapView = nullptr;
}


bool ArrowSprite::IsInitialized() const
{
	return ( mMapView != nullptr );
}


void ArrowSprite::Update( float elapsedTime )
{
	assertion( IsInitialized(), "Cannot update ArrowSprite that is not initialized!" );

	for( auto it = mSegments.begin(); it != mSegments.end(); ++it )
	{
		// Update all segments.
		Sprite* segment = *it;
		segment->OnUpdate( elapsedTime );
	}
}


void ArrowSprite::Draw( const Camera& camera )
{
	assertion( IsInitialized(), "Cannot draw ArrowSprite that is not initialized!" );

	if( mNeedsUpdate )
	{
		// Recreate segment sprites, as necessary.
		RebuildSegments();
		mNeedsUpdate = false;
	}

	for( auto it = mSegments.begin(); it != mSegments.end(); ++it )
	{
		// Draw all segments.
		Sprite* segment = *it;
		segment->OnDraw( camera );
	}
}


void ArrowSprite::SetPath( const Path& path )
{
	mPath = path;
}


Path& ArrowSprite::GetPath()
{
	return mPath;
}


const Path& ArrowSprite::GetPath() const
{
	return mPath;
}


size_t ArrowSprite::CreateSegmentAnimationID( PrimaryDirection startDirection, PrimaryDirection endDirection )
{
	return ( ( (size_t) startDirection.GetIndex() ) << CHAR_BIT ) | ( (size_t) endDirection.GetIndex() );
}


void ArrowSprite::RegisterSegmentAnimation( const HashString& animationName, PrimaryDirection startDirection, PrimaryDirection endDirection )
{
	// Create an ID from the start and end directions.
	size_t animationID = CreateSegmentAnimationID( startDirection, endDirection );

	// Make sure there isn't already an animation with the same ID.
	auto it = mArrowAnimationNamesByID.find( animationID );
	assertion( it == mArrowAnimationNamesByID.end(), "Cannot register arrow animation \"%s\" because a different animation (\"%s\") with the same start and end directions was already registered!", animationName.GetCString(), it->second.GetCString() );

	// Register the name under the animation ID.
	mArrowAnimationNamesByID[ animationID ] = animationName;
}


void ArrowSprite::RegisterSegmentAnimationBidirectional( const HashString& animationName, PrimaryDirection startDirection, PrimaryDirection endDirection )
{
	// Register arrow animation in both directions.
	RegisterSegmentAnimation( animationName, startDirection, endDirection );
	RegisterSegmentAnimation( animationName, endDirection, startDirection );
}


HashString ArrowSprite::GetSegmentAnimationName( PrimaryDirection startDirection, PrimaryDirection endDirection )
{
	HashString result = DEFAULT_ANIMATION_NAME;

	// Create an ID from the start and end directions.
	size_t animationID = CreateSegmentAnimationID( startDirection, endDirection );

	// Find the animation in the list (if it exists).
	auto it = mArrowAnimationNamesByID.find( animationID );

	if( it != mArrowAnimationNamesByID.end() )
	{
		// Return the name of the animation for the directions specified.
		result = it->second;
	}

	return result;
}


void ArrowSprite::InvalidateSegments()
{
	mNeedsUpdate = true;
}


void ArrowSprite::RebuildSegments()
{
	// Destroy all segments.
	DestroyAllSegments();

	// Start building the Path from the origin.
	PrimaryDirection previousDirection = PrimaryDirection::NONE;
	Vec2s tilePos = mPath.GetOrigin();
	Sprite* segment;

	for( size_t index = 0, length = mPath.GetLength(); index < length; ++index )
	{
		// Create a segment for each direction on the path.
		PrimaryDirection nextDirection = mPath.GetDirection( index );
		segment = CreateSegment( tilePos, previousDirection, nextDirection );
		mSegments.push_back( segment );

		// Advance the tile position.
		tilePos += nextDirection.GetOffset();

		// Keep track of the previous direction.
		previousDirection = nextDirection.GetOppositeDirection();
	}

	// Add the arrowhead segment and add it to the list of segments.
	segment = CreateSegment( tilePos, previousDirection, PrimaryDirection::NONE );
	mSegments.push_back( segment );
}


Sprite* ArrowSprite::CreateSegment( const Vec2s& tilePos, PrimaryDirection previousDirection, PrimaryDirection nextDirection )
{
	// Get the animation name.
	HashString animationName = GetSegmentAnimationName( previousDirection, nextDirection );

	// Get the position of the segment in the world.
	Vec2f worldCoords = mMapView->TileToWorldCoords( tilePos );

	// Create the segment and add it to the list of segments.
	Sprite* segment = SpriteManager::CreateSprite( mAnimationSetName, worldCoords, animationName );
	return segment;
}


void ArrowSprite::DestroyAllSegments()
{
	for( auto it = mSegments.begin(); it != mSegments.end(); ++it )
	{
		// Destroy all segment Sprites.
		Sprite* segment = *it;
		SpriteManager::DestroySprite( segment );
	}

	// Clear the list of segments.
	mSegments.clear();
}
