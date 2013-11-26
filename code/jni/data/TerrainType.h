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
		int GetCoverBonus() const;

	protected:
		std::string mDisplayName;
		int mCoverBonus;

		friend class TerrainTypesTable;
	};


	inline int TerrainType::GetCoverBonus() const
	{
		return mCoverBonus;
	}
}
