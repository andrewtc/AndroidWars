#pragma once

namespace mage
{
	/**
	 * Stores common information between all Units of a particular type.
	 */
	class UnitType : public Record
	{
	public:
		static const char* const SPRITE_DIRECTORY;
		static const char* const ANIMATION_FILE_EXTENSION;

		static std::string FormatAnimationPath( const std::string& animationName );
		static std::string FormatAnimationName( const std::string& animationName );

		UnitType( const HashString& name );
		virtual ~UnitType();

		int GetMovementRange() const;
		int GetMovementCostAcrossTerrain( TerrainType* terrainType ) const;
		bool CanMoveAcrossTerrain( TerrainType* terrainType ) const;
		HashString GetAnimationSetName() const;
		MovementType* GetMovementType() const;

	protected:
		void LoadAnimation();

		int mMovementRange;
		HashString mMovementTypeName;
		HashString mAnimationSetName;		// Name of the animation set, Tank.anim
		std::string mAnimationSetPath;		// Path to the animation set, sprites/Tank.anim
		std::string mDisplayName;

		friend class UnitTypesTable;
	};


	inline int UnitType::GetMovementRange() const
	{
		return mMovementRange;
	}


	inline int UnitType::GetMovementCostAcrossTerrain( TerrainType* terrainType ) const
	{
		MovementType* movementType = GetMovementType();
		assertion( movementType, "No MovementType \"%s\" found for UnitType \"%s\"!", mMovementTypeName.GetString().c_str(), GetName().GetString().c_str() );
		return movementType->GetMovementCostAcrossTerrain( terrainType );
	}


	inline bool UnitType::CanMoveAcrossTerrain( TerrainType* terrainType ) const
	{
		return ( GetMovementCostAcrossTerrain( terrainType ) > -1 );
	}


	inline HashString UnitType::GetAnimationSetName() const
	{
		return mAnimationSetName;
	}
}
