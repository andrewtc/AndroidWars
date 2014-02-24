#pragma once

namespace mage
{
	/**
	 * Represents a single state of the input within a GameState.
	 */
	class InputState
	{
	public:
		GameState* GetOwner() const;
		bool IsInitialized() const;

	protected:
		InputState( GameState* owner );
		virtual ~InputState();

		virtual void OnEnter( const Dictionary& parameters ) = 0;
		virtual void OnUpdate( float elapsedTime ) { };
		virtual void OnDraw() { };
		virtual void OnExit() = 0;

		virtual void OnScreenSizeChanged( int32 width, int32 height ) { }
		virtual bool OnPointerDown( float x, float y, size_t which ) { return false; }
		virtual bool OnPointerUp( float x, float y, size_t which ) { return false; }
		virtual bool OnPointerMotion( float x, float y, float dx, float dy, size_t which ) { return false; }

	private:
		void Enter( const Dictionary& parameters );
		void Update( float elapsedTime );
		void Draw();
		void Exit();

		GameState* mOwner;
		bool mIsInitialized;

		friend class GameState;
	};


	inline GameState* InputState::GetOwner() const
	{
		return mOwner;
	}


	inline bool InputState::IsInitialized() const
	{
		return mIsInitialized;
	}


	/**
	 * Defines an InputState that always has one type of GameState as its owner.
	 */
	template< class OwnerGameStateType >
	class DerivedInputState : public InputState
	{
	public:
		OwnerGameStateType* GetOwnerDerived() const;

	protected:
		DerivedInputState( GameState* owner );
		virtual ~DerivedInputState();
	};


	template< class OwnerGameStateType >
	DerivedInputState< OwnerGameStateType >::DerivedInputState( GameState* owner ) :
		InputState( owner )
	{
		// Make sure the owner of this object is derived from the proper GameState class.
		assertion( dynamic_cast< OwnerGameStateType* >( GetOwner() ) != nullptr, "Cannot create derived InputState because its owner is not derived from the correct GameState type!" );
	}


	template< class OwnerGameStateType >
	DerivedInputState< OwnerGameStateType >::~DerivedInputState() { }


	template< class OwnerGameStateType >
	OwnerGameStateType* DerivedInputState< OwnerGameStateType >::GetOwnerDerived() const
	{
		// Since we know the owner must be a subclass of OwnerGameStateType, we can do this.
		return reinterpret_cast< OwnerGameStateType* >( GetOwner() );
	}
}
