#pragma once

namespace mage
{
	/**
	 * Stores common information between all Units of a particular type.
	 */
	class UnitType : public Record
	{
	public:
		UnitType( const HashString& name );
		~UnitType();

	protected:
	};
}
