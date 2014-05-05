#pragma once

namespace mage
{
	class UnitMoveMapAnimation : public UnitMapAnimation
	{
	protected:
		UnitMoveMapAnimation( UnitSprite* unitSprite, const Path& movementPath, float movementSpeed = DEFAULT_MOVEMENT_SPEED );
		virtual ~UnitMoveMapAnimation();

	public:
		static const float DEFAULT_MOVEMENT_SPEED;

	protected:
		virtual void OnStart();
		virtual void OnUpdate( float elapsedTime );
		virtual void OnFinish();

	private:
		float mMovementSpeed;
		float mInterpolationSpeed;
		float mPercentageComplete;
		Path mMovementPath;

		friend class MapView;
	};
}
