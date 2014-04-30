#pragma once

namespace mage
{
	struct Action
	{
		Action() { }
		Action( const HashString& type ) :
			Type( type )
		{ }

		void LoadFromJSON( const rapidjson::Value& object );

		HashString Type;
		Dictionary Parameters;
		Dictionary Result;
	};

	typedef std::vector< Action > Actions;
}
