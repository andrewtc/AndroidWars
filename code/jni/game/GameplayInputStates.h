#pragma once

namespace mage
{
	class SelectUnitInputState : public DerivedInputState< GameplayState >
	{
		SelectUnitInputState( GameState* owner );
		~SelectUnitInputState();

		virtual void OnEnter( const Dictionary& parameters );
		virtual void OnExit();

		virtual bool OnPointerDown( const Pointer& pointer );
		virtual bool OnPointerUp( const Pointer& pointer );
		virtual bool OnPointerMotion( const Pointer& activePointer, const PointersByID& pointersByID );

		friend class GameState;
	};


	class MoveUnitInputState : public DerivedInputState< GameplayState >
	{
		MoveUnitInputState( GameState* owner );
		~MoveUnitInputState();

		virtual void OnEnter( const Dictionary& parameters );
		virtual void OnExit();

		virtual bool OnPointerDown( const Pointer& pointer );
		virtual bool OnPointerUp( const Pointer& pointer );
		virtual bool OnPointerMotion( const Pointer& activePointer, const PointersByID& pointersByID );

		bool mAllowMovement;
		Vec2s mLastPointerTilePos;

		friend class GameState;
	};


	class SelectActionInputState : public DerivedInputState< GameplayState >
	{
		SelectActionInputState( GameState* owner );
		~SelectActionInputState();

		virtual void OnEnter( const Dictionary& parameters );
		virtual void OnExit();

		void RebuildActionsMenu();
		Button* CreateActionButton( WidgetTemplate& widgetTemplate, const std::string& label );

		void OnActionButtonPressed( const HashString& actionType );
		void OnCancelButtonPressed();

		ListLayout* mActionMenu;

		friend class GameState;
	};


	class SelectTargetInputState : public DerivedInputState< GameplayState >
	{
		SelectTargetInputState( GameState* owner );
		~SelectTargetInputState();

		virtual void OnEnter( const Dictionary& parameters );
		virtual void OnExit();

		virtual bool OnPointerDown( const Pointer& pointer );

		Unit* GetTargetForAction( const Ability::Action* action ) const;
		UnitSprite* GetTargetUnitSpriteForAction( const Ability::Action* action ) const;
		bool IsValidTarget( const Unit* unit ) const;

		void OnConfirmButtonPressed();
		void OnCancelButtonPressed();

		ListLayout* mSelectionMenu;
		HashString mActionType;
		Actions mValidActions;

		friend class GameState;
	};


	class PerformActionInputState : public DerivedInputState< GameplayState >
	{
		PerformActionInputState( GameState* owner );
		~PerformActionInputState();

		virtual void OnEnter( const Dictionary& parameters );
		virtual void OnExit();

		void OnAnimationFinished();

		Ability::Action* mAction;
		MapAnimation* mMapAnimation;

		friend class GameState;
	};
}
