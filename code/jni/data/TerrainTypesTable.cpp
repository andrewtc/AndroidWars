#include "androidwars.h"

using namespace mage;


MAGE_AUTO_GENERATE_TABLE_NAME( TerrainTypesTable, TerrainType );


TerrainTypesTable::TerrainTypesTable( Scenario* scenario )
	: Table( scenario )
{ }


TerrainTypesTable::~TerrainTypesTable() { }


void TerrainTypesTable::OnLoadRecordFromJSON( TerrainType* terrainType, const rapidjson::Value& object )
{
	// Read in all attributes.
	terrainType->mDisplayName = GetJSONStringValue( object, "animationSet", "" );
	terrainType->mAnimationSetName = Scenario::FormatAnimationName( GetJSONStringValue( object, "animationSet", "" ) );
	terrainType->mAnimationSetPath = Scenario::FormatAnimationPath( GetJSONStringValue( object, "animationSet", "" ) );
	terrainType->mIsCapturable = GetJSONBoolValue( object, "isCapturable", false );
	terrainType->mCoverBonus = GetJSONIntValue( object, "coverBonus", 0 );

	if( object.HasMember( "variations" ) )
	{
		// Get the array of variations.
		const rapidjson::Value& variations = object[ "variations" ];

		if( variations.IsArray() )
		{
			for( auto it = variations.Begin(); it != variations.End(); ++it )
			{
				// Load each variation from JSON.
				LoadVariationFromJSON( terrainType, *it );
			}
		}
		else
		{
			WarnFail( "Cannot load variations for %s from JSON because the \"variations\" property is not an array!", terrainType->ToString() );
		}
	}

	// Pre-load the animation set.
	terrainType->LoadAnimation();
}


void TerrainTypesTable::LoadVariationFromJSON( TerrainType* terrainType, const rapidjson::Value& object )
{
	assertion( terrainType, "Cannot load variation for null TerrainType!" );

	if( object.IsObject() )
	{
		// Load the animation name.
		HashString animationName = GetJSONStringValue( object, "animation", "" );
		DebugPrintf( "Loading variation \"%s\"...", animationName.GetCString() );

		if( !animationName.GetString().empty() )
		{
			if( object.HasMember( "conditions" ) )
			{
				// Get the array of conditions.
				const rapidjson::Value& conditions = object[ "conditions" ];

				if( conditions.IsArray() )
				{
					// Create a new Variation for the TerrainType.
					Variation* variation = terrainType->CreateVariation( animationName );

					for( auto it = conditions.Begin(); it != conditions.End(); ++it )
					{
						// Load each condition for the current variation.
						DebugPrintf( "Loading condition %d...", it - conditions.Begin() );
						LoadVariationConditionFromJSON( variation, *it );
					}
				}
				else
				{
					WarnFail( "Cannot load variation %d for %s from JSON because the \"conditions\" property is not an array!", terrainType->GetVariationCount(), terrainType->ToString() );
				}
			}
			else
			{
				WarnFail( "Cannot load variation %d for %s from JSON because no list of conditions was supplied!", terrainType->GetVariationCount(), terrainType->ToString() );
			}
		}
		else
		{
			WarnFail( "Cannot load variation %d for %s from JSON because no animation name was supplied!", terrainType->GetVariationCount(), terrainType->ToString() );
		}
	}
	else
	{
		WarnFail( "Cannot load variation %d for %s from JSON because the JSON value is not an object!", terrainType->GetVariationCount(), terrainType->ToString() );
	}
}


void TerrainTypesTable::LoadVariationConditionFromJSON( Variation* variation, const rapidjson::Value& object )
{
	assertion( variation, "Cannot load variation condition for null Variation!" );

	Variation::Condition* condition = nullptr;

	if( object.IsObject() )
	{
		for( auto it = object.MemberBegin(); it != object.MemberEnd(); ++it )
		{
			// Load the direction for which the list of conditions will apply.
			HashString directionName = it->name.GetString();
			PrimaryDirection direction = PrimaryDirection::GetDirectionByName( directionName );

			if( direction != PrimaryDirection::NONE )
			{
				// Make sure the value is an array.
				const rapidjson::Value& expressions = it->value;
				std::vector< HashString > expressionStrings;

				if( expressions.IsArray() )
				{
					// Load a list of all expression strings in the JSON array.
					for( auto it = expressions.Begin(); it != expressions.End(); ++it )
					{
						if( it->IsString() )
						{
							// Load the expression.
							expressionStrings.push_back( it->GetString() );
						}
						else
						{
							WarnFail( "Cannot load expression for condition %d from JSON because the expression is not a string!", variation->GetConditionCount() );
						}
					}
				}
				else if( expressions.IsString() )
				{
					// Load the expression as a string.
					expressionStrings.push_back( expressions.GetString() );
				}
				else
				{
					WarnFail( "Cannot load variation condition %d from JSON because the expression list is not an array or a string!", variation->GetConditionCount() );
				}

				if( !expressionStrings.empty() )
				{
					if( condition == nullptr )
					{
						// If a Variation Condition hasn't been created yet, create it.
						condition = variation->CreateCondition();
					}

					for( auto it2 = expressionStrings.begin(); it2 != expressionStrings.end(); ++it2 )
					{
						// Create a new expression for each successfully loaded string.
						DebugPrintf( "Adding expression %s = \"%s\"", direction.GetName().GetCString(), it2->GetCString() );
						condition->AddExpression( direction, *it2 );
					}
				}
				else
				{
					WarnFail( "Cannot load variation condition %d from JSON because no expressions were successfully loaded!", variation->GetConditionCount() );
				}
			}
			else
			{
				WarnFail( "Cannot load variation condition %d from JSON because an invalid direction \"%s\" was specified!", variation->GetConditionCount(), directionName.GetCString() );
			}
		}
	}
	else
	{
		WarnFail( "Cannot load variation condition %d from JSON because the JSON value is not an object!", variation->GetConditionCount() );
	}

	if( condition )
	{
		condition->DebugPrintExpressions();
	}
}

