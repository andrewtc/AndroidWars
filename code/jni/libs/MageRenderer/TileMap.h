/*
 * Author      : Matthew Johnson
 * Date        : 17/Nov/2013
 * Description :
 *   Load and manage a Tiled map.
 */
 
#pragma once

namespace mage
{

	// A MapObject is any Tiled Object.
	// All MapObjects are stored and managed by the map.
	// When the map is destroyed (i.e. when loading a new map) all MapObjects will be destroyed.
	// To add extended functionality to MapObject you may set a NewMapObjectFn and return your subclass of MapObject.
	class MapObject
		: public Object
	{
	DECLARE_RTTI;
	public:
		MapObject( const std::string& name )
			: Object( name )
			, DebugColor( Color::GREY )
		{}
		virtual ~MapObject()
		{}

		// Override this to access custom property data.
		// Properties are stored "name" "value"
		virtual void OnLoadProperty( const std::string& name, const std::string& value ) {}

		// Called when the MapObject has finished loading all properties
		virtual void OnLoadFinished() {}

		// Override this to control drawing of the object.
		// By default draws an outlined rect with a red dot at the origin and a blue dot at the center if the Visible flag is true.
		// Camera is passed in case you want to cull objects or something.
		virtual void OnDraw( const Camera& camera ) const;

		// Override to get update ticks
		virtual void OnUpdate( float dt ) {}

		// Check if point is inside this object
		// point should be in same space as object
		bool Contains( const Vec2f& point ) const;

		bool Visible;
		Vec2f Position;
		RectF BoundingRect;
		Color DebugColor;
	};

	class TileMap
	{
	private:
		// Tileset
		struct TileSet
		{
			int TileSetIndex;
			uint32 FirstGid;
			Texture2D* TilesetSurface;
			int TileWidth, TileHeight;
			std::map< int, Dictionary > TileProperties;
		};
	public:
		// A tile on the map
		// Tiles can have the following properties
		//  - animLength : animation length in seconds
		//  - frames     : number of frames in the animation, this tile + frames
		//  - playing    : (default=true) true/false is animation playing
		class MapTile
		{
		public:
			friend class TileMap;
			MapTile();

		protected:
			int TileId;					// Tile ID used in tileset
			int TileIndex;				// Index into Map tile array
			//int TileSetIndex;			// Index into tileset array
			int TilePositionX;			// Position offset into tileset
			int TilePositionY;
			int TileAnimationOffsetX;	// Position offset for animation
			int TileAnimationOffsetY;
			int TileCollision;			// How Entities react to collision with this tile

		public:
			enum TileCollisionFlag
			{
				TC_NONE,
				TC_SOLID
			};

			void OnUpdate( float dt );

			void SetAnimLength( float time );
			void SetNumFrames( int n );
			void SetFrame( int frame );
			void PlayAnimation() { mAnimPlaying = true; }
			void StopAnimation() { mAnimPlaying = false; }

			const std::string& GetPropertyAsString( const HashString& propName ) const;

			bool operator==( const MapTile& tile ) const;
			bool operator!=( const MapTile& tile ) const;

		private:
			void RecomputeFrameTime();

			int mNumFrames;
			int mFrame;
			float mAnimLength;
			float mFrameTime;
			float now;
			TileSet* mTileset;		// The tileset owning this 
			bool mAnimPlaying;
			bool mAnimLooping;
		};

		typedef void(*MapPropertyFn)( const XmlReader::XmlReaderIterator& prop );
		typedef MapObject*(*NewMapObjectFn)( const XmlReader::XmlReaderIterator& objItr );

		static const int INVALID_TILE_ID = -1;

		TileMap();
		~TileMap();

		bool Load( const char* filename );
		void DestroyMap();
		void OnDraw( const Camera& camera );
		void OnUpdate( float dt );

		RectF GetMapBounds() const;

		MapTile& GetTile( int x, int y, unsigned int layerIndex=0U ) const;
		// Populates tiles array with all tiles of id tileId on the give layer
		uint32 GetTilesById( ArrayList< MapTile* >& tiles, uint32 tileId, uint32 layerIndex=0U ) const;

		// Set the TileID
		// This will cause the tiles graphical components to be re-evaluated
		// loadProperties determines if the property sheet for the new tile id is loaded or not
		// an id of 0 will make the tile invalid and thus not drawn
		void SetTileId( uint32 newId, MapTile& tile, bool loadProperties=true );

		// Add an object to the map
		// The object will now be managed by the map
		void AddMapObject( MapObject* obj, uint32 layerIndex=0U );
		// Remove the object from the map
		// If free == true, obj will be cleaned up
		void RemoveObject( MapObject*& obj, bool free=true );

		// Get the first MapObject* at the given position
		MapObject* GetFirstObjectAt( const Vec2f& position ) const;

		// Get all objects at given position
		// Uses the object type of the container to type check objects
		template< typename TObject >
		size_t GetAllObjectsAt( const Vec2f& position, ArrayList< TObject* >& objects, bool canBeDerived=false ) const
		{
			for ( auto itr = mObjects.begin(); itr != mObjects.end(); ++itr )
			{
				MapObject* obj = *itr;
				bool typematch = canBeDerived ? obj->IsDerived( TObject::TYPE ) : obj->IsExactly( TObject::TYPE );
				if ( typematch && obj->Contains( position ) )
				{
					objects.push_back( (TObject*) obj );
				}
			}
			return objects.size();
		}

		// Type matching version of GetFirstObjectAt()
		template< typename TObject >
		TObject* GetFirstObjectAtOfType( const Vec2f& position, bool canBeDerived=false ) const
		{
			MapObject* _ret = 0;
			for ( auto itr = mObjects.begin(); itr != mObjects.end(); ++itr )
			{
				MapObject* obj = *itr;
				bool typematch = canBeDerived ? obj->IsDerived( TObject::TYPE ) : obj->IsExactly( TObject::TYPE );
				if ( typematch && obj->Contains( position ) )
				{
					_ret = obj;
					break;
				}
			}
			return _ret;
		}

		void SetMapPropertyCB( MapPropertyFn fn );
		void SetNewMapObjectCB( NewMapObjectFn fn );

		inline int GetTileWidth() const           { return mTileWidth;          }
		inline int GetTileHeight() const          { return mTileHeight;         }
		inline int GetCollisionLayerIndex() const { return CollisionLayerIndex; }

		// Coord conversion functions
		Vec2f TileToWorld( int x, int y ) const;
		Vec2f TileToWorld( const Vec2i& pos ) const;
		Vec2i WorldToTile( float x, float y ) const;
		Vec2i WorldToTile( const Vec2f& pos ) const;

		// Iterate over all MapObjects in this map
		// TFunction: void fn( MapObject* obj );
		template< typename TFunction >
		void ForeachObject( TFunction fn )
		{
			for ( auto itr = mObjects.begin(); itr != mObjects.end(); ++itr )
				fn( *itr );
		}

		// Iterate over MapObjects of given type
		// Use the canBeDerived flag to get MapObjects derived from type as well
		// These derived objects will be passed into your callback as TObject*
		template< typename TObject >
		void ForeachObjectOfType( void( *fn )( TObject* ), bool canBeDerived=false )
		{
			for ( auto itr = mObjects.begin(); itr != mObjects.end(); ++itr )
			{
				MapObject* obj = *itr;
				if ( canBeDerived )
				{
					if ( obj->IsDerived( TObject::TYPE ) )
						fn( (TObject*) obj );
				}
				else
				{
					if ( obj->IsExactly( TObject::TYPE ) )
						fn( (TObject*) obj );
				}
			}
		}

		// Creates the default MapObject from data
		static MapObject* DefaultNewMapObjectFn( const XmlReader::XmlReaderIterator& objItr );
		// Use this to load the default fields of a MapObject
		static void LoadDefaultMapObjectFields( MapObject& obj, const XmlReader::XmlReaderIterator& objItr );
		static MapTile INVALID_TILE;
	private:
		// Layer index that is used by collision
		static int CollisionLayerIndex;

		// Type of map layer
		enum LayerType
		{
			LT_NONE,
			LT_TILE,
			LT_OBJ,
			LT_IMG
		};

		// Map layer base
		struct MapLayer
		{
			virtual ~MapLayer()
			{}

			LayerType Type;
			bool Visible;
			float Opacity;
		};

		// Map layer types
		struct TileLayer : public MapLayer
		{
			ArrayList< MapTile > Tiles;
		};

		struct ObjLayer : public MapLayer
		{
			virtual ~ObjLayer()
			{
				Objs.clear();
			}
			ArrayList< MapObject* > Objs;
		};

		struct ImgLayer : public MapLayer
		{
			Texture2D* ImageSurface;
			Vec2i ScrollOffset;
			Vec2i ScrollSpeed;
			int BackgroundMode;

			static const int BG_MODE_NONE        = 0;
			static const int BG_MODE_FILL_MAP    = 1;
			static const int BG_MODE_FILL_SCREEN = 2;
			static const int BG_MODE_TILE        = 3;
		};

		// Private helper functions

		// itr is <tileset>
		void LoadTileset( const XmlReader::XmlReaderIterator& itr );
		void LoadTilesetProperties( const XmlReader::XmlReaderIterator& itr, TileSet& tileset );
		// itr is <layer>
		void LoadTileLayer( const XmlReader::XmlReaderIterator& itr );
		// itr is <objectgroup>
		void LoadObjectGroup( const XmlReader::XmlReaderIterator& itr );
		// itr is <imagelayer>
		void LoadImageLayer( const XmlReader::XmlReaderIterator& itr );
		// itr is <image>
		Texture2D* LoadImage( const XmlReader::XmlReaderIterator& itr );
		// itr is <properties>
		void LoadProperties( Dictionary& props, const XmlReader::XmlReaderIterator& itr );
		void LoadProperties( MapObject* obj, const XmlReader::XmlReaderIterator& itr );
		// Load general layer info from a <layer> tag
		void LoadLayerBaseInfo( MapLayer* layer, const XmlReader::XmlReaderIterator& itr );

		// Get the tileset associated with this gid
		// The gid will be offset to be local to the returned TileSet
		TileSet* GetTileSetForGID( uint32& gid ) const;

		// Containers
		ArrayList< MapLayer* > mLayers;
		ArrayList< TileSet* > mTileSets;
		ArrayList< MapObject* > mObjects;

		int mMapWidth;
		int mMapHeight;
		int mTileWidth;
		int mTileHeight;

		std::string mMapDirectory;
		std::string mCurrentPath;

		// Callbacks
		MapPropertyFn mMapPropertyCB;
		NewMapObjectFn mNewMapObjectCB;

	};

	typedef TileMap::MapTile MapTile;

}
