#pragma once

namespace mage
{
	class SelectUnitInputState;
	class MoveUnitInputState;
	class SelectActionInputState;
	class SelectTargetInputState;

	/**
	 * Encapsulates all gameplay functionality.
	 */
	class GameplayState : public GameState
	{
	public:
		GameplayState();
		virtual ~GameplayState();

		Game* GetGame();
		const Game* GetGame() const;
		Map* GetMap();
		const Map* GetMap() const;
		MapView* GetMapView();
		const MapView* GetMapView() const;

		SelectUnitInputState* GetSelectUnitInputState() const;
		MoveUnitInputState* GetMoveUnitInputState() const;
		SelectActionInputState* GetSelectActionInputState() const;
		SelectTargetInputState* GetSelectTargetInputState() const;

		Widget* GetGameplayInterface() const;

	private:
		virtual void OnEnter( const Dictionary& parameters );
		virtual void OnUpdate( float elapsedTime );
		virtual void OnDraw();
		virtual void OnExit();

		virtual void OnScreenSizeChanged( int32 w, int32 h );
		virtual bool OnPointerDown( const Pointer& pointer );
		virtual bool OnPointerUp( const Pointer& pointer );
		virtual bool OnPointerMotion( const Pointer& activePointer, const PointersByID& pointersByID );

		bool mIsNetworkGame;
		Widget* mGameplayInterface;
		SelectUnitInputState* mSelectUnitInputState;
		MoveUnitInputState* mMoveUnitInputState;
		SelectActionInputState* mSelectActionInputState;
		SelectTargetInputState* mSelectTargetInputState;
		Scenario mScenario;
		Game mGame;
		Map mMap;
		MapView mMapView;
	};
}
