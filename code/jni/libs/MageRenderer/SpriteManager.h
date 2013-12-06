/*
 * Author      : Matthew Johnson
 * Date        : 18/Nov/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{
	//---------------------------------------
	// Manages all sprite resources
	// This is a namespace that looks like a static class
	namespace SpriteManager
	{
	//private:
	//	SpriteManager();
	//	~SpriteManager();
	//public:
		// Load sprite definition file
		bool LoadSpriteDefinition( const char* filename, bool linearFilter=true );
		// Load animation info
		bool LoadSpriteAnimations( const char* filename, bool linearFilter=true );

		// Update all Sprite animations
		void OnUpdate( float dt );
		// Render all sprites
		void OnDraw( const Camera& camera );

		// Create a new Sprite using the specified animation resource
		Sprite* CreateSprite( const HashString& animName, const Vec2f& position,
			const HashString& initialAnimName="Idle" );

		// Delete all sprite resources
		void DestroyAllSprites();
		// Delete a specific sprite
		void DestorySprite( Sprite*& sprite );

	//private:
	//	SpriteComponent* LoadSpriteDefinitionDirectory( SpriteComponent* parent,
	//		const XmlReader::XmlReaderIterator& itr );
	//
	//	HashMap< SpriteDefinition* > mSpriteDefinitions;
	//	HashMap< SpriteAnimationSet* > mSpriteAnimationSets;
	//	ArrayList< Sprite* > mSprites;
	};

}
