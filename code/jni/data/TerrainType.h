#pragma once

namespace mage
{
	/**
	 * Represents all non-changing aspects of a tile.
	 */
	class TerrainType
	{
	public:
		TerrainType( const std::string& name );
		~TerrainType();

	protected:
		int mNameHash;
		std::string mName;
	};
}
