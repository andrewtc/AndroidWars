#pragma once

namespace mage
{
	/**
	 * Represents all non-changing aspects of a tile.
	 */
	class TerrainType : public Record
	{
	public:
		TerrainType( const HashString& name );
		virtual ~TerrainType();

		std::string GetDisplayName() const;

	protected:
		std::string mDisplayName;
	};
}
