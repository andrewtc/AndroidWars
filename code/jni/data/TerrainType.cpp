#include "androidwars.h"

using namespace mage;

const HashString TerrainType::DEFAULT_ANIMATION_NAME = "Idle";


void Variation::Condition::AddExpression( PrimaryDirection direction, const HashString& expression )
{
	if( !HasExpression( direction, expression ) )
	{
		// Add the expression to the list of expressions.
		mExpressionsByDirection[ direction ].push_back( expression );
	}
	else
	{
		WarnFail( "Expression %s = \"%s\" already found!", direction.GetName().GetCString(), expression.GetCString() );
	}
}


const Variation::Condition::ExpressionsByDirection& Variation::Condition::GetExpressionsByDirection() const
{
	return mExpressionsByDirection;
}


bool Variation::Condition::HasExpression( PrimaryDirection direction, const HashString& expression ) const
{
	bool result = false;

	// Find the entry for the supplied tile offset (if any).
	auto it = mExpressionsByDirection.find( direction );

	if( it != mExpressionsByDirection.end() )
	{
		// If the entry was found, find the expression in the list.
		const Expressions& expressions = it->second;
		auto it2 = expressions.begin();

		for( ; it2 != expressions.end(); ++it2 )
		{
			if( *it2 == expression )
			{
				// If the expression was found, break.
				break;
			}
		}

		if( it2 != expressions.end() )
		{
			// Remove the expression.
			result = true;
		}
	}

	return result;
}


void Variation::Condition::RemoveExpression( PrimaryDirection direction, const HashString& expression )
{
	// Find the entry for the supplied tile offset (if any).
	auto it = mExpressionsByDirection.find( direction );

	if( it != mExpressionsByDirection.end() )
	{
		// If the entry was found, find the expression in the list.
		Expressions& expressions = it->second;
		auto it2 = expressions.begin();

		for( ; it2 != expressions.end(); ++it2 )
		{
			if( *it2 == expression )
			{
				// If the expression was found, break;
				break;
			}
		}

		if( it2 != expressions.end() )
		{
			// Remove the expression.
			expressions.erase( it2 );
		}
	}
}


void Variation::Condition::ClearExpressions()
{
	// Remove all expressions.
	mExpressionsByDirection.clear();
}


void Variation::Condition::DebugPrintExpressions() const
{
	DebugPrintf( "CONDITION:" );

	for( auto it = mExpressionsByDirection.begin(); it != mExpressionsByDirection.end(); ++it )
	{
		// Print a heading for the direction.
		DebugPrintf( "  %s:", it->first.GetName().GetCString() );

		const Expressions& expressions = it->second;

		for( auto it2 = expressions.begin(); it2 != expressions.end(); ++it2 )
		{
			// Print each expression.
			DebugPrintf( "    \"%s\"", it2->GetCString() );
		}
	}
}


Variation::Variation( const HashString& animationName ) :
	mAnimationName( animationName )
{ }


Variation::~Variation()
{
	// Destroy all Conditions.
	DestroyAllConditions();
}


void Variation::SetAnimationName( const HashString& animationName )
{
	mAnimationName = animationName;
}


HashString Variation::GetAnimationName() const
{
	return mAnimationName;
}


Variation::Condition* Variation::CreateCondition()
{
	// Create a new Condition.
	Condition* condition = new Condition();

	// Add the Condition to the list of conditions.
	mConditions.push_back( condition );

	// Return the new Condition.
	return condition;
}


const Variation::Conditions& Variation::GetConditions() const
{
	return mConditions;
}


size_t Variation::GetConditionCount() const
{
	return mConditions.size();
}


bool Variation::HasCondition( const Condition* condition ) const
{
	auto it = mConditions.begin();

	for( ; it != mConditions.end(); ++it )
	{
		if( *it == condition )
		{
			break;
		}
	}

	return ( it != mConditions.end() );
}


void Variation::DestroyCondition( Condition* condition )
{
	auto it = mConditions.begin();

	for( ; it != mConditions.end(); ++it )
	{
		if( *it == condition )
		{
			// Find the Condition in the list.
			break;
		}
	}

	if( it != mConditions.end() )
	{
		// Destroy the Condition.
		mConditions.erase( it );
	}
}


void Variation::DestroyAllConditions()
{
	for( auto it = mConditions.begin(); it != mConditions.end(); ++it )
	{
		// Destroy all Conditions.
		delete *it;
	}

	// Clear the list of Conditions.
	mConditions.clear();
}


TerrainType::TerrainType( const HashString& name ) :
	Record( name ),
	mIncome( 0 ),
	mCoverBonus( 0 ),
	mIsCapturable( false )
{ }


TerrainType::~TerrainType()
{
	// Destroy all Variations.
	DestroyAllVariations();
}


Variation* TerrainType::CreateVariation( const HashString& animationName )
{
	// Create a new Variation.
	Variation* variation = new Variation( animationName );

	// Add the new Variation to the list.
	mVariations.push_back( variation );

	// Return the new Variation.
	return variation;
}


const TerrainType::Variations& TerrainType::GetVariations() const
{
	return mVariations;
}


size_t TerrainType::GetVariationCount() const
{
	return mVariations.size();
}


bool TerrainType::HasVariation( const Variation* variation ) const
{
	bool result = false;
	auto it = mVariations.begin();

	for( ; it != mVariations.end(); ++it )
	{
		if( *it == variation )
		{
			break;
		}
	}

	return ( it != mVariations.end() );
}


void TerrainType::DestroyVariation( Variation* variation )
{
	auto it = mVariations.begin();

	for( ; it != mVariations.end(); ++it )
	{
		if( *it == variation )
		{
			break;
		}
	}

	assertion( it != mVariations.end(), "Cannot destroy Variation that is not part of %s!", ToString() );
	mVariations.erase( it );
}


void TerrainType::DestroyAllVariations()
{
	for( auto it = mVariations.begin(); it != mVariations.end(); ++it )
	{
		// Destroy all Variations.
		delete *it;
	}

	// Clear the list of Variations.
	mVariations.clear();
}


void TerrainType::LoadAnimation()
{
	// Load the animation.
	SpriteManager::LoadSpriteAnimations( mAnimationSetPath.c_str() );
}


HashString TerrainType::GetAnimationSetName() const
{
	return mAnimationSetName;
}


std::string TerrainType::GetDisplayName() const
{
	std::string result = mDisplayName;

	if( result.empty() )
	{
		// If the display name is empty, return the name string.
		result = GetName().GetString();
	}

	return mDisplayName;
}


int TerrainType::GetIncome() const
{
	return mIncome;
}


bool TerrainType::IsCapturable() const
{
	return mIsCapturable;
}
