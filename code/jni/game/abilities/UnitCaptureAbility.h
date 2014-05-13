#pragma once

namespace mage
{
	class UnitCaptureAbility : public UnitAbility
	{
		MAGE_DECLARE_ABILITY

	public:
		UnitCaptureAbility( Map* map );
		virtual ~UnitCaptureAbility();

		virtual void DetermineAvailableActions( const Unit* unit, const Path& movementPath, Actions& result ) const;

	private:
		virtual void ProcessUnitAction( UnitAbility::Action* unitAction );
	};


	class UnitCaptureAbility::Action : public UnitAbility::Action
	{
		MAGE_IMPLEMENT_ACTION

	public:
		Action();
		~Action();

		int CapturePointsRemoved;
		int CapturePointsRemaining;
	};
}
