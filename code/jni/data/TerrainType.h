#pragma once

namespace mage
{
	/**
	 * Represents a set of conditions under which a TerrainType should play a specific animation.
	 */
	class Variation
	{
	public:
		/**
		 * Represents a set of expressions that must match in order for a TerrainType to use a Variation.
		 */
		class Condition
		{
		public:
			typedef std::vector< HashString > Expressions;
			typedef std::map< PrimaryDirection, Expressions > ExpressionsByDirection;

			void AddExpression( PrimaryDirection direction, const HashString& expression );
			const ExpressionsByDirection& GetExpressionsByDirection() const;
			bool HasExpression( PrimaryDirection direction, const HashString& expression ) const;
			void RemoveExpression( PrimaryDirection direction, const HashString& expression );
			void ClearExpressions();
			void DebugPrintExpressions() const;

		private:
			ExpressionsByDirection mExpressionsByDirection;
		};

		typedef std::vector< Condition* > Conditions;

		void SetAnimationName( const HashString& animationName );
		HashString GetAnimationName() const;

		Condition* CreateCondition();
		const Conditions& GetConditions() const;
		size_t GetConditionCount() const;
		bool HasCondition( const Condition* condition ) const;
		void DestroyCondition( Condition* condition );
		void DestroyAllConditions();

	private:
		Variation( const HashString& animationName );
		~Variation();

		HashString mAnimationName;
		Conditions mConditions;

		friend class TerrainType;
	};


	/**
	 * Represents all non-changing aspects of a tile.
	 */
	class TerrainType : public TerrainTypesTable::Record
	{
	public:
		static const HashString DEFAULT_ANIMATION_NAME;

		typedef std::vector< Variation* > Variations;

		TerrainType( const HashString& name );
		virtual ~TerrainType();

		Variation* CreateVariation( const HashString& animationName = "" );
		const Variations& GetVariations() const;
		size_t GetVariationCount() const;
		bool HasVariation( const Variation* variation ) const;
		void DestroyVariation( Variation* variation );
		void DestroyAllVariations();

		HashString GetAnimationSetName() const;
		std::string GetDisplayName() const;
		int GetIncome() const;
		int GetCoverBonus() const;
		bool IsCapturable() const;

	protected:
		void LoadAnimation();

		Variations mVariations;
		HashString mAnimationSetName;
		std::string mAnimationSetPath;
		std::string mDisplayName;
		int mIncome;
		int mCoverBonus;
		bool mIsCapturable;

		friend class TerrainTypesTable;
	};


	inline int TerrainType::GetCoverBonus() const
	{
		return mCoverBonus;
	}
}
