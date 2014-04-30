#pragma once

#define MAGE_DECLARE_ABILITY \
	public: \
		static const HashString NAME; \
		static const TargetType TARGET_TYPE; \
		virtual const HashString& GetName() const \
		{ \
			return NAME; \
		} \
		virtual TargetType GetTargetType() const \
		{ \
			return TARGET_TYPE; \
		}


namespace mage
{
	class Unit;


	class UnitAbility
	{
	public:
		enum TargetType
		{
			TARGET_TYPE_NONE,
			TARGET_TYPE_SINGLE
		};

		UnitAbility() { }
		virtual ~UnitAbility() { }

		virtual void DetermineAvailableActions( const Unit* unit, const Path& movementPath, Actions& result ) const = 0;
		virtual void ProcessAction( Action& action ) = 0;
		virtual const HashString& GetName() const = 0;
		virtual TargetType GetTargetType() const = 0;

	protected:
		bool ProcessMove( Unit* unit, const Path& path );
	};


	class UnitWaitAbility : public UnitAbility
	{
		MAGE_DECLARE_ABILITY

	public:
		UnitWaitAbility();
		virtual ~UnitWaitAbility();

		virtual void DetermineAvailableActions( const Unit* unit, const Path& movementPath, Actions& result ) const;
		virtual void ProcessAction( Action& action );
	};


	class UnitAttackAbility : public UnitAbility
	{
		MAGE_DECLARE_ABILITY

	public:
		UnitAttackAbility();
		virtual ~UnitAttackAbility();

		virtual void DetermineAvailableActions( const Unit* unit, const Path& movementPath, Actions& result ) const;
		virtual void ProcessAction( Action& action );
	};
}
