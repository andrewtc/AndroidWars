#pragma once

namespace mage
{
	class ProgressInputState : public DialogInputState
	{
	public:
		ProgressInputState( GameState* owner );
		~ProgressInputState();

	protected:
		void OnEnter( const Dictionary& parameters );
		void OnExit();
	};
}
