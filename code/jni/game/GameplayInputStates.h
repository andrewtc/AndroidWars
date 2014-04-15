#pragma once

namespace mage
{
	class SelectUnitInputState : public DerivedInputState< GameplayState >
	{
	public:
		void SetTileTemplate( const Tile& tile );

	private:
		SelectUnitInputState( GameState* owner );
		~SelectUnitInputState();

		virtual void OnEnter( const Dictionary& parameters );
		virtual void OnExit();

		virtual bool OnPointerDown( const Pointer& pointer );
		virtual bool OnPointerUp( const Pointer& pointer );
		virtual bool OnPointerMotion( const Pointer& activePointer, const PointersByID& pointersByID );

		friend class GameState;
	};
}
