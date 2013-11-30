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

		UnitType( const HashString& name );
		virtual ~UnitType();

		HashString GetAnimationSetName() const;

	protected:
		void LoadAnimation();

		HashString mAnimationSetName;
		std::string mDisplayName;

		friend class UnitTypesTable;
	};


	inline HashString UnitType::GetAnimationSetName() const
	{
		return mAnimationSetName;
	}
}
