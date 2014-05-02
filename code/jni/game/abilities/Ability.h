#pragma once


#define MAGE_DECLARE_ABILITY \
	public: \
		class Action; \
		static const HashString TYPE; \
		virtual const HashString& GetType() const \
		{ \
			return TYPE; \
		}

#define MAGE_IMPLEMENT_ABILITY( abilityClass, type ) \
	const HashString abilityClass::TYPE = type;

#define MAGE_IMPLEMENT_ACTION \
	public: \
		virtual const HashString& GetType() const \
		{ \
			return TYPE; \
		}


namespace mage
{
	/**
	 * Represents a type of action that can be performed by the player.
	 */
	class Ability
	{
	public:
		class Action;

		Ability( Map* map );
		virtual ~Ability();

	public:
		bool CanProcessAction( const Action* action ) const;
		virtual void ProcessAction( Action* action ) = 0;
		virtual const HashString& GetType() const = 0;
		Map* GetMap() const;

	private:
		Map* mMap;
	};


	/**
	 * Encapsulates all data for a single action performed by the player.
	 */
	class Ability::Action
	{
	public:
		Action() { }
		virtual ~Action() { }

		virtual void SaveToJSON( rapidjson::Document& document, rapidjson::Value& object ) const = 0;
		virtual void LoadFromJSON( const rapidjson::Value& object ) = 0;

		virtual const HashString& GetType() const = 0;
		bool IsType( const HashString& type ) const;
	};


	typedef std::vector< Ability::Action* > Actions;


	/**
	 * Mix-in class representing an Ability that targets a single Unit.
	 */
	class SingleTargetComponent
	{
	public:
		class Action;
	};


	/**
	 * Mix-in class representing an Action that targets a single Unit.
	 */
	class SingleTargetComponent::Action
	{
	public:
		Action();

	protected:
		void SaveToJSON( rapidjson::Document& document, rapidjson::Value& object ) const;
		void LoadFromJSON( const rapidjson::Value& object );

	public:
		int TargetID;
	};
}
