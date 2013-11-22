/*
 * Author      : Matthew Johnson
 * Date        : 17/Nov/2013
 * Description :
 *   Load and manage a Tiled map.
 */
 
#pragma once

namespace mage
{

	struct MapTile
	{
		MapTile()
			: TileId( -1 )
			, TileCollision( TC_NONE )
		{}

		int TileId;			// Tile ID used in tileset
		int TileIndex;		// Index into Map tile array
		int TileSetIndex;	// Index into tileset array
		int TilePositionX;	// Position in Map space
		int TilePositionY;
		int TileCollision;	// How Entities react to collision with this tile

		enum TileCollisionFlag
		{
			TC_NONE,
			TC_SOLID
		};
	};

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
		virtual void OnLoadProperty( const XmlReader::XmlReaderIterator& propItr ) {}

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
	public:
		typedef void(*MapPropertyFn)( const XmlReader::XmlReaderIterator& prop );
		typedef MapObject*(*NewMapObjectFn)( const XmlReader::XmlReaderIterator& objItr );

		TileMap();
		~TileMap();

		bool Load( const char* filename );
		void DestroyMap();
		void OnDraw( const Camera& camera );
		void OnUpdate( float dt );

		RectI GetMapBounds() const;

		MapTile& GetTile( int x, int y, unsigned int layerIndex=0U ) const;

		void SetMapPropertyCB( MapPropertyFn fn );
		void SetNewMapObjectCB( NewMapObjectFn fn );

		inline int GetTileWidth() const           { return mTileWidth;          }
		inline int GetTileHeight() const          { return mTileHeight;         }
		inline int GetCollisionLayerIndex() const { return CollisionLayerIndex; }

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
		};

		// Tileset
		struct TileSet
		{
			int TileSetIndex;
			uint32 FirstGid;
			Texture2D* TilesetSurface;
			std::map< int, Dictionary > TileProperties;
		};

		// Private helper functions

		// itr is <tileset>
		void LoadTileset( const XmlReader::XmlReaderIterator& itr );
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

}