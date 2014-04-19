#pragma once

namespace mage
{
	class ArrowSprite
	{
	public:
		static const char* const ANIMATION_SET_NAME;
		static const HashString DEFAULT_ANIMATION_NAME;

		ArrowSprite();
		~ArrowSprite();

		void Init( MapView* mapView );
		void Destroy();
		bool IsInitialized() const;

		void Update( float elapsedTime );
		void Draw( const Camera& camera );

		void SetPath( const Path& path );
		Path& GetPath();
		const Path& GetPath() const;

	protected:
		void InvalidateSegments();
		void RebuildSegments();
		Sprite* CreateSegment( const Vec2s& tilePos, PrimaryDirection startDirection, PrimaryDirection endDirection );
		void DestroyAllSegments();

		static size_t CreateSegmentAnimationID( PrimaryDirection startDirection, PrimaryDirection endDirection );
		void RegisterSegmentAnimation( const HashString& animationName, PrimaryDirection startDirection, PrimaryDirection endDirection );
		void RegisterSegmentAnimationBidirectional( const HashString& animationName, PrimaryDirection startDirection, PrimaryDirection endDirection );
		HashString GetSegmentAnimationName( PrimaryDirection startDirection, PrimaryDirection endDirection );

		bool mNeedsUpdate;
		MapView* mMapView;
		std::string mAnimationSetName;
		std::vector< Sprite* > mSegments;
		std::map< size_t, HashString > mArrowAnimationNamesByID;
		Path mPath;
	};
}
