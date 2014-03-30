#include "Vector2.h"

namespace mage
{

template<> const Vec2f Vec2f::ZERO = Vec2f();
template<> const Vec2f Vec2f::ONE( 1.0f );
template<> const Vec2f Vec2f::UNIT_X( 1.0f, 0.0f );
template<> const Vec2f Vec2f::UNIT_Y( 0.0f, 1.0f );

template<> const Vec2d Vec2d::ZERO = Vec2d();
template<> const Vec2d Vec2d::ONE( 1.0 );
template<> const Vec2d Vec2d::UNIT_X( 1.0, 0.0 );
template<> const Vec2d Vec2d::UNIT_Y( 0.0, 1.0 );

template<> const Vec2i Vec2i::ZERO = Vec2i();
template<> const Vec2i Vec2i::ONE( 1 );
template<> const Vec2i Vec2i::UNIT_X( 1, 0 );
template<> const Vec2i Vec2i::UNIT_Y( 0, 1 );

template<> const Vec2s Vec2s::ZERO = Vec2s();
template<> const Vec2s Vec2s::ONE( 1 );
template<> const Vec2s Vec2s::UNIT_X( 1, 0 );
template<> const Vec2s Vec2s::UNIT_Y( 0, 1 );

}
