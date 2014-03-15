#pragma once

namespace mage
{
	/**
	 * Represents all non-changing aspects of a tile.
	 */
	class TerrainType : public TerrainTypesTable::Record
	{
	public:
		TerrainType( const HashString& name );
		virtual ~TerrainType();

		std::string GetDisplayName() const;
		int GetCoverBonus() const;
		bool IsCapturable() const;

	protected:
		void LoadAnimation();

		HashString mAnimationSetName;
		std::string mAnimationSetPath;
		std::string mDisplayName;
		int mCoverBonus;
		bool mIsCapturable;

		friend class TerrainTypesTable;
	};


	inline int TerrainType::GetCoverBonus() const
	{
		return mCoverBonus;
	}
}
