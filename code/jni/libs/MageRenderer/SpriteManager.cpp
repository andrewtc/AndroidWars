#include "RendererLib.h"

namespace mage
{
namespace SpriteManager
{
SpriteComponent* LoadSpriteDefinitionDirectory( SpriteComponent* parent,
		const XmlReader::XmlReaderIterator& itr );

HashMap< SpriteDefinition* > mSpriteDefinitions;
HashMap< SpriteAnimationSet* > mSpriteAnimationSets;
ArrayList< Sprite* > mSprites;

bool LoadSpriteDefinition( const char* filename )
{
	/* Definition file layout
	<img name"image.png" w="" h="" >
		<definitions>
			<dir name="" >
				<spr name="id" x="" y="" w="" h="" />
				...
			</dir>
			...
		</definitions>
	<img>
	*/

	SpriteDefinition* sprDef = new SpriteDefinition();

	sprDef->Name = StringUtil::ExtractFilenameFromPath( filename );

	XmlReader reader( filename );
	XmlReader::XmlReaderIterator root = reader.ReadRoot();

	root.ValidateXMLAttributes( "name,w,h","" );
	root.ValidateXMLChildElemnts( "definitions","" );

	// Load spritesheet image
	char* buffer=0;
	unsigned int size;
	std::string spriteSheetName = std::string( filename ).substr( 0, std::string( filename ).find_last_of( "/" ) + 1 )
		+ root.GetAttributeAsString( "name" );
	sprDef->SpriteSheet = Texture2D::CreateTexture( spriteSheetName.c_str() );
	sprDef->SpriteSheet->Load();
	
	// Loop through the definitions
	for ( XmlReader::XmlReaderIterator defItr = root.NextChild( "definitions" ).NextChild( "dir" );
		  defItr.IsValid(); defItr = defItr.NextSibling( "dir" ) )
	{
		sprDef->RootComponent = LoadSpriteDefinitionDirectory( nullptr, defItr );
	}

	mSpriteDefinitions[ sprDef->Name ] = sprDef;

	//DebugPrintSpriteDefinition( sprDef );

	return true;
}
//---------------------------------------
SpriteComponent* LoadSpriteDefinitionDirectory( SpriteComponent* parent,
	const XmlReader::XmlReaderIterator& defItr )
{
	defItr.ValidateXMLAttributes( "name","" );
	defItr.ValidateXMLChildElemnts( "","spr,dir" );

	SpriteComponent* sprComp = new SpriteComponent();

	sprComp->Name = defItr.GetAttributeAsString( "name" );

	// Root of sprite tree is simply a slash
	if ( sprComp->Name == "/" ) sprComp->Name = "root";

	// Get the clipping rectangles
	for ( XmlReader::XmlReaderIterator sprItr = defItr.NextChild( "spr" );
		  sprItr.IsValid(); sprItr = sprItr.NextSibling( "spr" ) )
	{
		sprItr.ValidateXMLAttributes( "name,x,y,w,h","" );
		sprItr.ValidateXMLChildElemnts( "","" );

		std::string sprName = sprItr.GetAttributeAsString( "name" );
		int x = sprItr.GetAttributeAsInt( "x" );
		int y = sprItr.GetAttributeAsInt( "y" );
		int w = sprItr.GetAttributeAsInt( "w" );
		int h = sprItr.GetAttributeAsInt( "h" );

		sprComp->SpriteClips[ sprName ] = RectI( x, y, x + w, y + h );
	}

	if ( parent )
	{
		parent->ChildComponents[ sprComp->Name ] = sprComp;
	}

	// Get child definitions
	for ( XmlReader::XmlReaderIterator defCItr = defItr.NextChild( "dir" );
	      defCItr.IsValid(); defCItr = defCItr.NextSibling( "dir" ) )
	{
		LoadSpriteDefinitionDirectory( sprComp, defCItr );
	}

	return sprComp;
}
//---------------------------------------
bool LoadSpriteAnimations( const char* filename )
{
	/* Animation file
	<animations spriteSheet="img.png" ver="1.2">
		<anim name="" loops="">
			<cell index="0" delay="">
				<spr name="path_to_sprite_component" x="" y="" z=""/>
				...
			</cell>
			...
		</anm>
		...
	</animations>
	*/

	SpriteAnimationSet* animSet = new SpriteAnimationSet();

	animSet->Name = StringUtil::ExtractFilenameFromPath( filename );

	// Already loaded
	if ( mSpriteAnimationSets.find( animSet->Name ) != mSpriteAnimationSets.end() )
	{
		delete animSet;
		return true;
	}

	XmlReader reader( filename );
	XmlReader::XmlReaderIterator root = reader.ReadRoot();

	if ( !root.IsValid() )
	{
		ConsolePrintf( CONSOLE_ERROR, "Failed to load %s\n", filename );
		delete animSet;
		return false;
	}

	root.ValidateXMLAttributes( "spriteSheet,ver","" );
	root.ValidateXMLChildElemnts( "anim","" );

	// Get the name of the sprite definition this animation set is for
	std::string definitionName = root.GetAttributeAsString( "spriteSheet" );
	HashMap< SpriteDefinition* >::iterator foundDef = mSpriteDefinitions.find( definitionName );
	if ( foundDef != mSpriteDefinitions.end() )
	{
		animSet->MyDefinition = foundDef->second;
	}
	else
	{
		// Definition not loaded - try to load it
		std::string basePath = StringUtil::StripFilenameFromPath( filename );
		std::string sprdefPath = basePath + definitionName;
		if ( LoadSpriteDefinition( sprdefPath.c_str() ) )
		{
			animSet->MyDefinition = mSpriteDefinitions[ definitionName ];
		}
		// Failure
		else
		{
			assertion( false, "Failed to link animation '%s' to sprite '%s'\nSprite file not found.\n",
				animSet->Name.GetString().c_str(), definitionName.c_str() );
			return false;
		}
	}

	// Loop through the animations
	for ( XmlReader::XmlReaderIterator animItr = root.NextChild( "anim" );
		animItr.IsValid(); animItr = animItr.NextSibling( "anim" ) )
	{
		animItr.ValidateXMLAttributes( "name,loops","" );
		animItr.ValidateXMLChildElemnts( "cell","" );

		SpriteAnimation sprAnim;

		sprAnim.Name = animItr.GetAttributeAsString( "name" );
		sprAnim.LoopType = animItr.GetAttributeAsInt( "loops" );

		// Loop through all the frames
		for ( XmlReader::XmlReaderIterator cellItr = animItr.NextChild( "cell" );
			cellItr.IsValid(); cellItr = cellItr.NextSibling( "cell" ) )
		{
			cellItr.ValidateXMLAttributes( "index,delay","" );
			cellItr.ValidateXMLChildElemnts( "spr","" );

			SpriteAnimationFrame animFrame;

			animFrame.Index = cellItr.GetAttributeAsInt( "index" );
			animFrame.Delay = cellItr.GetAttributeAsInt( "delay" );

			// Get the frame components
			for ( XmlReader::XmlReaderIterator sprItr = cellItr.NextChild( "spr" );
				sprItr.IsValid(); sprItr = sprItr.NextSibling( "spr" ) )
			{
				sprItr.ValidateXMLAttributes( "name,x,y,z","" );
				sprItr.ValidateXMLChildElemnts( "","" );

				SpriteAnimationComponent* animComp = new SpriteAnimationComponent();

				std::string componentName = sprItr.GetAttributeAsString( "name" );

				animComp->FrameOffsetX = sprItr.GetAttributeAsInt( "x" );
				animComp->FrameOffsetY = sprItr.GetAttributeAsInt( "y" );
				animComp->FrameZOrder  = sprItr.GetAttributeAsInt( "z" );

				// Get the component this animation references
				std::string clipName = StringUtil::ExtractFilenameFromPath( componentName );
				std::string clipPath = componentName.substr( 1, componentName.find_last_of( "/" )-1 );
				animComp->SprComponent = 
					animSet->MyDefinition->
					GetComponentFromPath( clipPath );
				animComp->SprComponentClipName = clipName;

				animFrame.SprAnimComponent = animComp;

				// @TODO Load all components
				break;
			}

			sprAnim.Frames.push_back( animFrame );
		}

		sprAnim.FrameCount = (int) sprAnim.Frames.size();
		animSet->Animations[ sprAnim.Name ] = sprAnim;
	}

	mSpriteAnimationSets[ animSet->Name ] = animSet;

	//DebugPrintSpriteAnimationSet( animSet );

	return true;
}
//---------------------------------------
Sprite* CreateSprite( const HashString& animName, const Vec2f& position,
	const HashString& initialAnimName )
{
	SpriteAnimationSet*& anim = mSpriteAnimationSets[ animName ];
	if ( !anim )
	{
		WarnFail( "No such sprite animation '%s'. Did you forget to load it first?\n", animName.GetString().c_str() );
		return 0;
	}
	Sprite* sprite = new Sprite( *anim, initialAnimName );

	sprite->Position = position;

	mSprites.push_back( sprite );

	return sprite;
}
//---------------------------------------
void OnUpdate( float dt )
{
	for ( ArrayList< Sprite* >::iterator itr = mSprites.begin(); itr != mSprites.end(); ++itr )
	{
		(*itr)->OnUpdate( dt );
	}
}
//---------------------------------------
void OnDraw( const Camera& camera )
{
	for ( ArrayList< Sprite* >::const_iterator itr = mSprites.begin(); itr != mSprites.end(); ++itr )
	{
		(*itr)->OnDraw( camera );
	}
}
//---------------------------------------
void DestroyAllSprites()
{
	DestroyVector( mSprites );
}
//---------------------------------------
void DestorySprite( Sprite*& sprite )
{
	ArrayList< Sprite* >::iterator foundItr = std::find( mSprites.begin(), mSprites.end(), sprite );
	if ( foundItr != mSprites.end() )
	{
		mSprites.erase(
			foundItr, foundItr + 1 );
		Delete0( sprite );
	}
}
//---------------------------------------
}
}
