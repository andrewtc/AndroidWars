#include "RendererLib.h"

using namespace mage;

const uint32 TMX_FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
const uint32 TMX_FLIPPED_VERTICALLY_FLAG   = 0x40000000;
const uint32 TMX_FLIPPED_DIAGONALLY_FLAG   = 0x20000000;

int TileMap::CollisionLayerIndex = -1;
MapTile TileMap::INVALID_TILE;

MAGE_IMPLEMENT_RTTI( Object, MapObject );

//---------------------------------------
// MapTile
//---------------------------------------
MapTile::MapTile()
	: TileId( -1 )
	, TileCollision( TC_NONE )
	, mAnimLength( 0.0f )
	, mFrameTime( 0.0f )
	, mNumFrames( 0 )
	, mFrame( 0 )
	, mAnimPlaying( false )
	, now( 0 )
{}
//---------------------------------------
void MapTile::OnUpdate( float dt )
{
	if ( mFrameTime != 0 && mNumFrames != 0 && mAnimPlaying )
	{
		now += dt;
		if ( now > mFrameTime )
		{
			SetFrame( ++mFrame );
			now = 0;
		}
	}
}
//---------------------------------------
void MapTile::SetAnimLength( float time )
{
	mAnimLength = time;
	RecomputeFrameTime();
}
//---------------------------------------
void MapTile::SetNumFrames( int n )
{
	mNumFrames = n;
	RecomputeFrameTime();
}
//---------------------------------------
void MapTile::RecomputeFrameTime()
{
	if ( mNumFrames > 0 )
	{
		mFrameTime = mAnimLength / mNumFrames;
	}
}
//---------------------------------------
void MapTile::SetFrame( int frame )
{
	if ( mFrame >= mNumFrames )
		mFrame = 0;
	TileAnimationOffsetX = mFrame * mTileset->TileWidth;
	// TODO maybe account for wrapping the tile animation to the next row
	/*if ( TilePositionX + TileAnimationOffsetX + mTileset->TileWidth >= mTileset->TilesetSurface->GetWidth() )
	{
		TileAnimationOffsetX = 0;
		TileAnimationOffsetY += mTileset->TileHeight;
	}*/
}
//---------------------------------------


//---------------------------------------
// Default MapObject drawing.
// Draws an outlined rect with a red dot at the origin and a blue dot at the center.
void MapObject::OnDraw( const Camera& camera ) const
{
	if ( !Visible ) return;

	const Vec2f& camPos = camera.GetPosition();
	const Vec2f& origin = Position- camPos;

	Color innerColor( DebugColor.a - 170, DebugColor.r, DebugColor.g, DebugColor.b );

	if ( camera.RectInViewport( BoundingRect ) )
	{
		// Outline
		DrawRectOutlined(
			origin.x, origin.y,
			BoundingRect.Width(), BoundingRect.Height(),
			innerColor, 2.0f, DebugColor );
		
		// Origin
		DrawRect( origin.x - 1, origin.y - 1, 2, 2, Color( 0xffff0000 ) );

		// Center
		DrawRect( origin.x + BoundingRect.CenterX() - 1,
			origin.y + BoundingRect.CenterY() - 1,
			2, 2,
			Color( 0xff0000ff ) );
	}
}
//---------------------------------------
bool MapObject::Contains( const Vec2f& point ) const
{
	Vec2f local = point - Position;
	return BoundingRect.Contains( local.x, local. y );
}
//---------------------------------------


//---------------------------------------
MapObject* TileMap::DefaultNewMapObjectFn( const XmlReader::XmlReaderIterator& objItr )
{
	std::string name = objItr.GetAttributeAsString( "name", "" );
	MapObject* obj = new MapObject( name );

	LoadDefaultMapObjectFields( *obj, objItr );
	
	return obj;
}
//---------------------------------------
void TileMap::LoadDefaultMapObjectFields( MapObject& obj, const XmlReader::XmlReaderIterator& objItr )
{
	int x = objItr.GetAttributeAsInt( "x" );
	int y = objItr.GetAttributeAsInt( "y" );
	int w = objItr.GetAttributeAsInt( "width", 0 );
	int h = objItr.GetAttributeAsInt( "height", 0 );
	bool visible = objItr.GetAttributeAsInt( "visible", 1 ) == 0 ? false : true;

	obj.Visible = visible;
	obj.Position.Set( x, y );
	obj.BoundingRect.Set( 0, 0, w, h );
}
//---------------------------------------


//---------------------------------------
TileMap::TileMap()
	: mMapPropertyCB( 0 )
	, mNewMapObjectCB( TileMap::DefaultNewMapObjectFn )
{}
//---------------------------------------
TileMap::~TileMap()
{
	DestroyMap();
}
//---------------------------------------
bool TileMap::Load( const char* filename )
{
	XmlReader reader( filename );
	XmlReader::XmlReaderIterator xmlRoot = reader.ReadRoot();

	// Ensure file exists before destroying old map
	if ( !xmlRoot.IsValid() )
	{
		return false;
	}

	// Clear old map data
	DestroyMap();

	mMapDirectory = std::string( filename ).substr( 0, std::string( filename ).find_last_of( "/" ) + 1 );

	// Map/Tile size
	xmlRoot.ValidateXMLAttributes( "width,height,tilewidth,tileheight","orientation,version" );
	mMapWidth   = xmlRoot.GetAttributeAsInt( "width" );
	mMapHeight  = xmlRoot.GetAttributeAsInt( "height" );
	mTileWidth  = xmlRoot.GetAttributeAsInt( "tilewidth" );
	mTileHeight = xmlRoot.GetAttributeAsInt( "tileheight" );

	// Custom properties
	if ( xmlRoot.NextChild( "properties" ).IsValid() )
	{
		for ( XmlReader::XmlReaderIterator propItr = xmlRoot.NextChild( "properties" ).NextChild( "property" );
			  propItr.IsValid(); propItr = propItr.NextSibling( "property" ) )
		{
			// Call user function for each property
			if ( mMapPropertyCB )
			{
				mMapPropertyCB( propItr );
			}
		}
	}

	// Load Map data
	for ( XmlReader::XmlReaderIterator itr = xmlRoot.NextChild();
		  itr.IsValid(); itr = itr.NextSibling() )
	{
		if ( itr.ElementNameEquals( "properties" ) )
		{
		}
		else if ( itr.ElementNameEquals( "tileset" ) )
		{
			LoadTileset( itr );
		}
		else if ( itr.ElementNameEquals( "layer" ) )
		{
			LoadTileLayer( itr );
		}
		else if ( itr.ElementNameEquals( "objectgroup" ) )
		{
			LoadObjectGroup( itr );
		}
		else if ( itr.ElementNameEquals( "imagelayer" ) )
		{
			LoadImageLayer( itr );
		}
		else
		{
			ConsolePrintf( CONSOLE_WARNING, "Found unknown tmx tag: %s\n", itr.ElementName() );
		}
	}

	// Signal the map is done loading
	EventManager::FireEvent( "OnMapLoaded" );

	return true;
}
//---------------------------------------
void TileMap::DestroyMap()
{
	// Clear tilesets
	DestroyVector( mTileSets );
	
	// Clear layers
	DestroyVector( mLayers );

	// Clear objects
	DestroyVector( mObjects );
}
//---------------------------------------
void TileMap::LoadTileset( const XmlReader::XmlReaderIterator& itr )
{
	TileSet* tileset = new TileSet;

	tileset->FirstGid = itr.GetAttributeAsInt( "firstgid" );
	tileset->TileWidth = itr.GetAttributeAsInt( "tilewidth", mTileWidth );
	tileset->TileHeight = itr.GetAttributeAsInt( "tileheight", mTileHeight );
	const char* source = itr.GetAttributeAsCString( "source", 0 );

	// Inline tileset
	if ( !source )
	{
		XmlReader::XmlReaderIterator imageTag = itr.NextChild( "image" );
		mCurrentPath = mMapDirectory;
		tileset->TilesetSurface = LoadImage( imageTag );

		LoadTilesetProperties( itr, *tileset );

	}
	// External tileset
	else
	{
		std::string tilesetPath = mMapDirectory;
		tilesetPath += std::string( source );
		std::string tilesetDirectory = tilesetPath.substr( 0, tilesetPath.find_last_of( "/" ) + 1 );
		tilesetPath = StringUtil::ExtractFilenameFromPath( tilesetPath );
		XmlReader tilesetReader( ( std::string( "tilesets/" ) + tilesetPath ).c_str() );
		XmlReader::XmlReaderIterator tilesetRoot = tilesetReader.ReadRoot();

		XmlReader::XmlReaderIterator imageTag = tilesetRoot.NextChild( "image" );
		mCurrentPath = tilesetDirectory;
		tileset->TilesetSurface = LoadImage( imageTag );

		LoadTilesetProperties( tilesetRoot, *tileset );
	}

	tileset->TileSetIndex = (int) mTileSets.size();
	mTileSets.push_back( tileset );
}
//---------------------------------------
void TileMap::LoadTilesetProperties( const XmlReader::XmlReaderIterator& itr, TileSet& tileset )
{
	for ( XmlReader::XmlReaderIterator tileItr = itr.NextChild( "tile" );
		tileItr.IsValid(); tileItr = tileItr.NextSibling( "tile" ) )
	{
		tileItr.ValidateXMLAttributes( "id","" );
		tileItr.ValidateXMLChildElemnts( "properties","" );

		int id = tileItr.GetAttributeAsInt( "id" );

		LoadProperties( tileset.TileProperties[ id ], tileItr.NextChild( "properties" ) );
	}
}
//---------------------------------------
void TileMap::LoadTileLayer( const XmlReader::XmlReaderIterator& itr )
{
	TileLayer* layer = new TileLayer;
	layer->Type = LT_TILE;

	LoadLayerBaseInfo( layer, itr );

	itr.ValidateXMLChildElemnts( "data","tile,properties" );
	XmlReader::XmlReaderIterator data = itr.NextChild( "data" );

	const char* encoding = data.GetAttributeAsCString( "encoding", 0 );
	const char* compression = data.GetAttributeAsCString( "compression", 0 );

	// Tile data is encoded
	if ( encoding )
	{
		// Base64 encoded
		if ( !strcmp( encoding, "base64" ) )
		{
			// Get encoded data
			std::string rawData = data.GetPCDataAsString();

			// Decode data
			std::vector< int > tiledata = base64_decode( rawData );

			// Tile data is compressed
			if ( compression )
			{
				if ( !strcmp( compression, "gzip" ) )
				{
					ConsolePrintf( CONSOLE_WARNING, "Warning: gzip compression not supported yet... no data loaded\n" );
					return;
				}
				else if ( !strcmp( compression, "zlib" ) )
				{
					ConsolePrintf( CONSOLE_WARNING, "Warning: zlib compression not supported yet... no data loaded\n" );
					return;
				}
				else
				{
					ConsolePrintf( CONSOLE_WARNING, "Warning: Unknown compression: %s\n", compression );
					return;
				}
			}

			int k = 0, size = (int) tiledata.size();
			for ( int j = 0; j < size; j += 4 )
			{
				MapTile tile;

				// Get global tile id
				unsigned int gid = tiledata[ j     ]       |
								   tiledata[ j + 1 ] << 8  |
								   tiledata[ j + 2 ] << 16 |
								   tiledata[ j + 3 ] << 24;

				// Read tile flags (unused)
				//bool flippedHorizontally = ( gid & TMX_FLIPPED_HORIZONTALLY_FLAG ) == 0 ? false : true;
				//bool flippedVertically   = ( gid & TMX_FLIPPED_VERTICALLY_FLAG   ) == 0 ? false : true;
				//bool flippedDiagonally   = ( gid & TMX_FLIPPED_DIAGONALLY_FLAG   ) == 0 ? false : true;

				// Clear flags
				gid &= ~( TMX_FLIPPED_HORIZONTALLY_FLAG |
					      TMX_FLIPPED_VERTICALLY_FLAG   |
					      TMX_FLIPPED_DIAGONALLY_FLAG );

				// Figure out which tileset this tile belongs to
				// and set its properties accordingly
				SetTileId( gid, tile );
				tile.TileIndex = k++;
				layer->Tiles.push_back( tile );
			}
		}
		// CSV encoded
		else if ( !strcmp( encoding, "csv" ) )
		{
			ConsolePrintf( CONSOLE_WARNING, "Warning: CSV encoding not supported yet... no data loaded\n" );
		}
		else
		{
			ConsolePrintf( CONSOLE_WARNING, "Warning: Unknown encoding: %s\n", encoding );
		}
	}
	// Raw XML tile data
	else
	{
		ConsolePrintf( CONSOLE_WARNING, "Warning: Raw tile data not supported yet... no data loaded\n" );
	}

	XmlReader::XmlReaderIterator propItr = itr.NextChild( "properties" );
	if ( propItr.IsValid() )
	{
		Dictionary layerProperties;
		LoadProperties( layerProperties, propItr );

		// The 'Collision' property specifies a layer is to be used for collision
		std::string _dummy_;
		if ( layerProperties.Get( "Collision", _dummy_ ) == Dictionary::DErr_SUCCESS )
		{
			CollisionLayerIndex = (int) mLayers.size();
		}
	}

	mLayers.push_back( layer );
}
//---------------------------------------
void TileMap::LoadObjectGroup( const XmlReader::XmlReaderIterator& itr )
{
	ObjLayer* layer = new ObjLayer;
	layer->Type = LT_OBJ;

	LoadLayerBaseInfo( layer, itr );

	for ( XmlReader::XmlReaderIterator objItr = itr.NextChild( "object" );
		objItr.IsValid(); objItr = objItr.NextSibling( "object" ) )
	{
		objItr.ValidateXMLAttributes( "x,y","name,width,height,type,visible" );

		MapObject* obj = mNewMapObjectCB( objItr );
		if ( obj )
		{
			// Load editor values for properties
			XmlReader::XmlReaderIterator propItr = objItr.NextChild( "properties" );
			if ( propItr.IsValid() )
			{
				LoadProperties( obj, propItr );
			}
			// Finished loading object
			obj->OnLoadFinished();
		}
		else
		{
			ConsolePrintf( CONSOLE_WARNING, "Failed to load object: '%s'\n", objItr.GetAttributeAsCString( "name" ) );
			continue;
		}

		layer->Objs.push_back( obj );
		mObjects.push_back( obj );
	}
	mLayers.push_back( layer );
}
//---------------------------------------
void TileMap::LoadImageLayer( const XmlReader::XmlReaderIterator& itr )
{
	ImgLayer* layer = new ImgLayer();
	layer->Type = LT_IMG;

	LoadLayerBaseInfo( layer, itr );

	XmlReader::XmlReaderIterator imageItr = itr.NextChild( "image" );

	if ( imageItr.IsValid() )
	{
		layer->ImageSurface = LoadImage( imageItr );

		// Custom properties
		if ( itr.NextChild( "properties" ).IsValid() )
		{
			for ( XmlReader::XmlReaderIterator propItr = itr.NextChild( "properties" ).NextChild( "property" );
				propItr.IsValid(); propItr = propItr.NextSibling( "property" ) )
			{
				std::string k = propItr.GetAttributeAsString( "name" );
				if ( k == "ScrollSpeed" )
					layer->ScrollSpeed = (Vec2i) propItr.GetAttributeAsVec2f( "value", Vec2f::ZERO );
			}
		}
	}
	else
	{
		layer->ImageSurface = NULL;
		ConsolePrintf( CONSOLE_WARNING, "Warning: Empty image layer - is this intentional?\n" );
	}

	mLayers.push_back( layer );
}
//---------------------------------------
Texture2D* TileMap::LoadImage( const XmlReader::XmlReaderIterator& itr )
{
	itr.ValidateXMLAttributes( "source","width,height,trans" );

	// Build image path
	const char* imgSource = itr.GetAttributeAsCString( "source" );
	// Remove ../ or ./ or / from beginning of path
	int s = 0;
	int len = strlen( imgSource );
	if ( len > 0 && imgSource[0] == '.' ) ++s;
	if ( len > 0 && imgSource[0] == '/' ) ++s;
	if ( len > 1 && imgSource[1] == '.' ) ++s;
	if ( len > 1 && imgSource[1] == '/' ) ++s;
	if ( len > 2 && imgSource[2] == '/' ) ++s;
	std::string sourcePath = imgSource + s;

	// Load image
	Texture2D* img = Texture2D::CreateTexture( sourcePath.c_str() );
	img->Load();
	if ( !img || !img->Load() )
	{
		ConsolePrintf( CONSOLE_WARNING, "Failed to load texture '%s'\n", sourcePath.c_str() );
		img = 0;
	}

	return img;
}
//---------------------------------------
void TileMap::LoadProperties( Dictionary& props, const XmlReader::XmlReaderIterator& itr )
{
	for ( XmlReader::XmlReaderIterator propItr = itr.NextChild( "property" );
		propItr.IsValid(); propItr = propItr.NextSibling( "property" ) )
	{
		std::string k = propItr.GetAttributeAsString( "name" );
		std::string v = propItr.GetAttributeAsString( "value" );
		props.Set( k, v );
	}
}
//---------------------------------------
void TileMap::LoadProperties( MapObject* obj, const XmlReader::XmlReaderIterator& itr )
{
	for ( XmlReader::XmlReaderIterator propItr = itr.NextChild( "property" );
		propItr.IsValid(); propItr = propItr.NextSibling( "property" ) )
	{
		std::string name  = propItr.GetAttributeAsString( "name" );
		std::string value = propItr.GetAttributeAsString( "value" );
		obj->OnLoadProperty( name, value );
	}
}
//---------------------------------------
void TileMap::LoadLayerBaseInfo( MapLayer* layer, const XmlReader::XmlReaderIterator& itr )
{
	// Validate layer tag
	itr.ValidateXMLAttributes( "name","x,y,width,height,opacity,visible" );

	// Read in attributes
	layer->Visible = itr.GetAttributeAsInt( "visible", 1 ) == 0 ? false : true;
	layer->Opacity = itr.GetAttributeAsFloat( "opacity", 1.0f );
}
//---------------------------------------
void TileMap::OnDraw( const Camera& camera )
{
	// Get visible range of tiles
	int cameraX = (int) camera.GetPosition().x;// - ( ((int) camera.GetViewWidth()) % mTileWidth );
	int cameraY = (int) camera.GetPosition().y;// - ( ((int) camera.GetViewHeight()) % mTileHeight );
	int startX = (int) ( camera.GetPosition().x / mTileWidth );
	int startY = (int) ( camera.GetPosition().y / mTileHeight );
	int endX = startX + camera.GetViewWidth() / mTileWidth;// - 1;
	int endY = startY + camera.GetViewHeight() / mTileHeight;// - 1;

	Mathi::ClampToRange( startX, 0, mMapWidth  - 1 );
	Mathi::ClampToRange( startY, 0, mMapHeight - 1 );
	Mathi::ClampToRange( endX,   0, mMapWidth  - 1 );
	Mathi::ClampToRange( endY,   0, mMapHeight - 1 );

	for ( auto layerItr = mLayers.begin(); layerItr != mLayers.end(); ++layerItr )
	{
		MapLayer* layer = *layerItr;

		// Skip invisible layers
		if ( !layer->Visible || layer->Opacity == 0.0f ) continue;

		// Tile layer
		if ( layer->Type == LT_TILE )
		{
			TileLayer* tileLayer = (TileLayer*) layer;

			// Loop and draw all visible tiles in the viewport
			for ( int y = startY; y <= endY; ++y )
			{
				for ( int x = startX; x <= endX; ++x )
				{
					int index = x + y * mMapWidth;

					// Draw tile if id is valid
					if ( tileLayer->Tiles[ index ].TileId >= 0 )
					{
						const MapTile& tile = tileLayer->Tiles[ index ];
						const TileSet* tileset = tile.mTileset; //mTileSets[ tile.TileSetIndex ];
						int tileHeightDiff = mTileHeight - tileset->TileHeight;

						DrawRect(
							tileset->TilesetSurface,
							x * mTileWidth - cameraX,
							y * mTileHeight - cameraY + tileHeightDiff,
							tile.TilePositionX + tile.TileAnimationOffsetX, tile.TilePositionY + tile.TileAnimationOffsetY, tileset->TileWidth, tileset->TileHeight
						);
					}
				}
			}
		}

		// Object layer
		else if ( layer->Type == LT_OBJ )
		{
			continue;
		}

		// Image layer
		else if ( layer->Type == LT_IMG )
		{
			ImgLayer* imgLayer = (ImgLayer*) layer;

			if ( imgLayer->ImageSurface )
			{
				DrawRect( imgLayer->ImageSurface,
					imgLayer->ScrollOffset.x, imgLayer->ScrollOffset.y );
				
				// @TODO some of this should go in update
				imgLayer->ScrollOffset += imgLayer->ScrollSpeed;

				const int width  = imgLayer->ImageSurface->GetWidth();
				const int height = imgLayer->ImageSurface->GetHeight();

				// Scroll left
				if ( imgLayer->ScrollSpeed.x < 0 )
				{
					if ( imgLayer->ScrollOffset.x + width < camera.GetViewWidth() )
					{
						DrawRect( imgLayer->ImageSurface,
							imgLayer->ScrollOffset.x + width, imgLayer->ScrollOffset.y );
					}

					if ( imgLayer->ScrollOffset.x + width < 0 )
					{
						imgLayer->ScrollOffset.x = 0;
					}
				}
				// Scroll right
				else if ( imgLayer->ScrollSpeed.x > 0 )
				{
					if ( imgLayer->ScrollOffset.x > 0 )
					{
						DrawRect( imgLayer->ImageSurface,
							imgLayer->ScrollOffset.x - width, imgLayer->ScrollOffset.y );
					}

					if ( imgLayer->ScrollOffset.x > camera.GetViewWidth() )
					{
						imgLayer->ScrollOffset.x = -width + camera.GetViewWidth();
					}
				}

				// Scroll up
				if ( imgLayer->ScrollSpeed.y < 0 )
				{
					if ( imgLayer->ScrollOffset.y + height < camera.GetViewHeight() )
					{
						DrawRect( imgLayer->ImageSurface,
							imgLayer->ScrollOffset.x, imgLayer->ScrollOffset.y + height );
					}

					if ( imgLayer->ScrollOffset.y + height < 0 )
					{
						imgLayer->ScrollOffset.y = 0;
					}
				}

				// Scroll down
				if ( imgLayer->ScrollSpeed.y > 0 )
				{
					if ( imgLayer->ScrollOffset.y > 0 )
					{
						DrawRect( imgLayer->ImageSurface,
							imgLayer->ScrollOffset.x, imgLayer->ScrollOffset.y - height );
					}

					if ( imgLayer->ScrollOffset.y > camera.GetViewHeight() )
					{
						imgLayer->ScrollOffset.y = -height + camera.GetViewHeight();
					}
				}
			}
		}
	}

	for ( auto itr = mObjects.begin(); itr != mObjects.end(); ++itr )
	{
		(*itr)->OnDraw( camera );
	}
}
//---------------------------------------
void TileMap::OnUpdate( float dt )
{
	for ( ArrayList< MapObject* >::const_iterator itr = mObjects.begin(); itr != mObjects.end(); ++itr )
	{
		(*itr)->OnUpdate( dt );
	}

	for ( auto layerItr = mLayers.begin(); layerItr != mLayers.end(); ++layerItr )
	{
		MapLayer* layer = *layerItr;

		// Skip invisible layers
		if ( !layer->Visible || layer->Opacity == 0.0f ) continue;


		// Tile layer
		if ( layer->Type == LT_TILE )
		{
			TileLayer* tileLayer = (TileLayer*) layer;

			// Loop and update all tiles
			for ( int y = 0; y <= mMapHeight-1; ++y )
			{
				for ( int x = 0; x <= mMapWidth-1; ++x )
				{
					int index = x + y * mMapWidth;

					// Check if id is valid
					if ( tileLayer->Tiles[ index ].TileId >= 0 )
					{
						MapTile& tile = tileLayer->Tiles[ index ];
						tile.OnUpdate( dt );
					}
				}
			}
		}
	}
}
//---------------------------------------
MapTile& TileMap::GetTile( int x, int y, unsigned int layerIndex ) const
{
	MapLayer* layer = mLayers[ layerIndex ];
	if ( layer->Type == LT_TILE )
	{
		TileLayer* tileLayer = (TileLayer*) layer;
		unsigned int index = (unsigned int) ( x + mMapWidth * y );
		if ( index < 0 || index >= tileLayer->Tiles.size() )
		{
			return INVALID_TILE;
		}
		return tileLayer->Tiles[ index ];
	}

	return INVALID_TILE;
}
//---------------------------------------
uint32 TileMap::GetTilesById( ArrayList< MapTile* >& tiles, uint32 tileId, uint32 layerIndex ) const
{
	if ( layerIndex < mLayers.size() )
	{
		MapLayer* layer = mLayers[ layerIndex ];
		if ( layer->Type == LT_TILE )
		{
			TileLayer* tileLayer = (TileLayer*) layer;
			// Loop and update all tiles
			for ( int y = 0; y <= mMapHeight-1; ++y )
			{
				for ( int x = 0; x <= mMapWidth-1; ++x )
				{
					int index = x + y * mMapWidth;

					// Check if id is valid
					if ( tileLayer->Tiles[ index ].TileId == tileId )
					{
						tiles.push_back( &tileLayer->Tiles[ index ] );
					}
				}
			}
		}
		else
		{
			WarnFail( "TileMap::GetTilesById() : layer type miss-match, expected TileLayer\n" );
		}
	}
	else
	{
		WarnFail( "TileMap::GetTilesById() : layerIndex %u/%u 'Out-of-bounds'\n", layerIndex, mLayers.size() );
	}
	return tiles.size();
}
//---------------------------------------
void TileMap::SetTileId( uint32 gid, MapTile& tile, bool loadProperties )
{
	// gid of 0 is invalid
	if ( gid > 0 )
	{
		TileSet* tileSet = GetTileSetForGID( gid );
		const int numXTiles = tileSet->TilesetSurface ? tileSet->TilesetSurface->GetWidth() / mTileWidth : 1;

		tile.TileId = gid;
		//tile.TileSetIndex = tilesetIndex;
		tile.TilePositionX = ( gid % numXTiles ) * tileSet->TileWidth;
		tile.TilePositionY = ( gid / numXTiles ) * tileSet->TileHeight;
		tile.TileAnimationOffsetX = 0;
		tile.TileAnimationOffsetY = 0;
		tile.mTileset = tileSet;

		// Load tile properties
		if ( loadProperties )
		{
			auto tileProps = tileSet->TileProperties.find( gid );
			if ( tileProps != tileSet->TileProperties.end() )
			{
				// Check for collision properties
				std::string collisionType;
				if ( tileProps->second.Get( "Collision", collisionType ) == Dictionary::DErr_SUCCESS )
				{
					if ( collisionType == "SOLID" )
					{
						tile.TileCollision = MapTile::TC_SOLID;
					}
				}

				// Check animation properties
				std::string animStr;
				if ( tileProps->second.Get( "animLength", animStr ) == Dictionary::DErr_SUCCESS )
				{
					float animTime = 0;
					StringUtil::StringToType( animStr, &animTime );
					tile.SetAnimLength( animTime );
				}
				if ( tileProps->second.Get( "frames", animStr ) == Dictionary::DErr_SUCCESS )
				{
					int numFrames = 0;
					StringUtil::StringToType( animStr, &numFrames );
					tile.SetNumFrames( numFrames );
				}
				if ( tileProps->second.Get( "playing", animStr ) == Dictionary::DErr_SUCCESS )
				{
					if ( StringUtil::StringiCmp( animStr, "true" ) )
						tile.PlayAnimation();
				}
				else
				{
					tile.PlayAnimation();
				}
			}
		}
	}
	else
	{
		// Tile is null and will not be draw
		tile.TileId = gid;
	}
}
//---------------------------------------
void TileMap::AddMapObject( MapObject* obj, uint32 layerIndex )
{
	if ( obj )
	{
		// TODO this should be a set or otherwise prevent duplicate insertion
		mObjects.push_back( obj );
	}
	else
	{
		WarnCrit( "Passed NULL MapObject to AddMapObject()\n" );
	}
}
//---------------------------------------
void TileMap::RemoveObject( MapObject*& obj, bool free )
{
	if ( obj )
	{
		mObjects.remove_elem( obj );
		if ( free )
		{
			delete obj;
			obj = 0;
		}
	}
}
//---------------------------------------
void TileMap::SetMapPropertyCB( MapPropertyFn fn )
{
	mMapPropertyCB = fn;
}
//---------------------------------------
void TileMap::SetNewMapObjectCB( NewMapObjectFn fn )
{
	if ( !fn )
		fn = &TileMap::DefaultNewMapObjectFn;
	mNewMapObjectCB = fn;
}
//---------------------------------------
RectF TileMap::GetMapBounds() const
{
	return RectF( 0, 0, mTileWidth * mMapWidth, mTileHeight * mMapHeight );
}
//---------------------------------------
TileMap::TileSet* TileMap::GetTileSetForGID( uint32& gid ) const
{
	int tilesetIndex = 0;
	for ( int i = (int) mTileSets.size() - 1; i >= 0; --i )
	{
		TileSet* tileset = mTileSets[i];

		if ( tileset->FirstGid <= gid )
		{
			tilesetIndex = tileset->TileSetIndex;
			// Fix gid to be local to this tileset
			gid = gid - tileset->FirstGid;
			break;
		}
	}

	return mTileSets[ tilesetIndex ];
}
//---------------------------------------
Vec2f TileMap::TileToWorld( int x, int y ) const
{
	return Vec2f( x * mTileWidth, y * mTileHeight );
}
//---------------------------------------
Vec2f TileMap::TileToWorld( const Vec2i& pos ) const
{
	return TileToWorld( pos.x, pos.y );
}
//---------------------------------------
Vec2i TileMap::WorldToTile( float x, float y ) const
{
	return Vec2i( (int) ( x / mTileWidth ),
				  (int) ( y / mTileHeight ) );
}
//---------------------------------------
Vec2i TileMap::WorldToTile( const Vec2f& pos ) const
{
	return WorldToTile( pos.x, pos.y );
}
//---------------------------------------
