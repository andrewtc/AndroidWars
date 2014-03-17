#pragma once

namespace mage
{
	class World
	{
	public:
		static const float TILE_WORLD_SCALE;

		World( Map* map );
		~World();

		void Update( float elapsedTime );
		void Draw( const Camera& camera );

	private:
		Sprite* CreateOrGetSprite( const HashString& animationSetName );

		Map* mMap;
		HashMap< Sprite* > mTileSpritesByName;
	};
}
