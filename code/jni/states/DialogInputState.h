#pragma once

namespace mage
{
	class DialogInputState : public InputState
	{
	public:
		static const int DEFAULT_LAYER;

		DialogInputState( GameState* owner );
		virtual ~DialogInputState();

	protected:
		virtual void OnEnter( const Dictionary& parameters );
		virtual void OnExit();

		Widget* mWidget;
	};
}
