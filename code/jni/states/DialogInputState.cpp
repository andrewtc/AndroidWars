#include "androidwars.h"

using namespace mage;

const int DialogInputState::DEFAULT_LAYER = 10000;


DialogInputState::DialogInputState( GameState* owner ) :
	InputState( owner ),
	mWidget( nullptr )
{ }


DialogInputState::~DialogInputState() { }


void DialogInputState::OnEnter( const Dictionary& parameters )
{
	DebugPrintf( "Entering DialogInputState" );

	// Get the index of the dialog.
	int drawLayer = parameters.Get( "drawLayer", DEFAULT_LAYER );

	// Get the name of the Widget to create.
	std::string title = parameters.Get( "title", std::string() );

	// Get the name of the Widget to create.
	HashString widgetName = parameters.Get( "widgetName", std::string() );

	// Get the name of the template to load.
	HashString templateToLoad = parameters.Get( "template", std::string() );

	if( !templateToLoad.GetString().empty() )
	{
		// Load the Widget from the template.
		mWidget = gWidgetManager->CreateWidgetFromTemplate( templateToLoad, widgetName );

		if( mWidget )
		{
			// Center the Widget.
			mWidget->SetOffset( Vec2f( gWindowWidth, gWindowHeight ) * 0.5f );
			mWidget->SetHorizontalAlignment( Widget::HORIZONTAL_ALIGN_CENTER );
			mWidget->SetVerticalAlignment( Widget::VERTICAL_ALIGN_CENTER );

			// Set the layer of the Widget.
			mWidget->SetDrawLayer( drawLayer );

			// Add the Widget to the root.
			gWidgetManager->GetRootWidget()->AddChild( mWidget );
		}
		else
		{
			WarnFail( "Could not create Widget from template \"%s\" for DialogInputState!", templateToLoad.GetCString() );
		}
	}
	else
	{
		WarnFail( "No template was specified for DialogInputState!" );
	}
}


void DialogInputState::OnExit()
{
	DebugPrintf( "Exiting DialogInputState" );

	if( mWidget )
	{
		// Remove the widget from the root.
		gWidgetManager->GetRootWidget()->RemoveChild( mWidget );

		// Destroy the Widget.
		gWidgetManager->DestroyWidget( mWidget );
		mWidget = nullptr;
	}
}
