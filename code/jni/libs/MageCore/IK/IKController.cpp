#include "CoreLib.h"

using namespace mage;

MAGE_IMPLEMENT_RTTI( Controller, IKController );

//---------------------------------------
IKController::IKController()
	: Iterations( 1 )
{}
//---------------------------------------
IKController::~IKController()
{}
//---------------------------------------
bool IKController::OnUpdate( float currentTime )
{
	Chain.OnUpdate( Iterations );
	return true;
}
//---------------------------------------