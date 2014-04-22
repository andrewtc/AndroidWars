#pragma once

namespace mage
{
	class BrushToolInputState : public DerivedInputState< EditorState >
	{
	public:
		void SetTileTemplate( const Tile& tile );

	private:
		BrushToolInputState( GameState* owner );
		~BrushToolInputState();

		virtual void OnEnter( const Dictionary& parameters );
		virtual void OnExit();

		virtual bool OnPointerDown( const Pointer& pointer );
		virtual bool OnPointerUp( const Pointer& pointer );
		virtual bool OnPointerMotion( const Pointer& activePointer, const PointersByID& pointersByID );

		Tile mTileTemplate;

		friend class GameState;
	};


	class PlaceToolInputState : public DerivedInputState< EditorState >
	{
	public:
		void SetSelectedUnitType( UnitType* unitType );
		void SetSelectedFaction( Faction* faction );

	private:
		PlaceToolInputState( GameState* owner );
		~PlaceToolInputState();

		virtual void OnDraw();

		virtual void OnEnter( const Dictionary& parameters );
		virtual void OnExit();

		virtual bool OnPointerDown( const Pointer& pointer );
		virtual bool OnPointerUp( const Pointer& pointer );
		virtual bool OnPointerMotion( const Pointer& activePointer, const PointersByID& pointersByID );

		void CreateUnitPlacementSprite( const Vec2f& position );
		void MoveUnitPlacementSprite( const Vec2f& position );
		void DestroyUnitPlacementSprite();
		Unit* CreateUnitAtScreenCoords( const Vec2f& screenCoords );

		UnitType* mSelectedUnitType;
		Faction* mSelectedFaction;
		Sprite* mUnitPlacementSprite;

		friend class GameState;
	};


	class EraserToolInputState : public DerivedInputState< EditorState >
	{
	private:
		EraserToolInputState( GameState* owner );
		~EraserToolInputState();

		virtual void OnEnter( const Dictionary& parameters );
		virtual void OnExit();

		virtual bool OnPointerDown( const Pointer& pointer );
		virtual bool OnPointerUp( const Pointer& pointer );
		virtual bool OnPointerMotion( const Pointer& activePointer, const PointersByID& pointersByID );

		void DestroyUnitAtScreenCoords( const Vec2f& screenCoords );

		friend class GameState;
	};
}
