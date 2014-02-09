#include "androidwars.h"

using namespace mage;


MainMenuState::MainMenuState() :
	GameState(), mWidget( nullptr )
{
	DebugPrintf( "MainMenuState created!" );
}


MainMenuState::~MainMenuState()
{
	DebugPrintf( "MainMenuState destroyed!" );
}


void MainMenuState::OnEnter( const Dictionary& parameters )
{
	DebugPrintf( "MainMenuState entered!" );

	mWidget = Widget::LoadWidget( "ui/main_menu.xml" );
	mWidget->Show();
}


void MainMenuState::OnUpdate( float elapsedTime )
{

}


void MainMenuState::OnDraw()
{

}


void MainMenuState::OnExit()
{
	DebugPrintf( "MainMenuState exited!" );

	mWidget->Hide();
	Widget::DestroyWidget( mWidget );
	mWidget = nullptr;
}
