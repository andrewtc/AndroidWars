#include "Vector4.h"

namespace mage
{

	template<> const Vec4f Vec4f::ZERO = Vec4f();
	template<> const Vec4f Vec4f::ONE( 1.0f );
	template<> const Vec4f Vec4f::UNIT_X( 1.0f, 0.0f, 0.0f, 0.0f );
	template<> const Vec4f Vec4f::UNIT_Y( 0.0f, 1.0f, 0.0f, 0.0f );
	template<> const Vec4f Vec4f::UNIT_Z( 0.0f, 0.0f, 1.0f, 0.0f );
	template<> const Vec4f Vec4f::UNIT_W( 0.0f, 0.0f, 0.0f, 1.0f );

	template<> const Vec4d Vec4d::ZERO = Vec4d();
	template<> const Vec4d Vec4d::ONE( 1.0 );
	template<> const Vec4d Vec4d::UNIT_X( 1.0, 0.0, 0.0, 0.0 );
	template<> const Vec4d Vec4d::UNIT_Y( 0.0, 1.0, 0.0, 0.0 );
	template<> const Vec4d Vec4d::UNIT_Z( 0.0, 0.0, 1.0, 0.0 );
	template<> const Vec4d Vec4d::UNIT_W( 0.0, 0.0, 0.0, 1.0 );

}
