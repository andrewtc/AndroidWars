#include "Matrix4.h"

using namespace mage;

template<> const Matrix4f Matrix4f::ZERO( true );
template<> const Matrix4f Matrix4f::IDENTITY( false );

template<> const Matrix4d Matrix4d::ZERO( true );
template<> const Matrix4d Matrix4d::IDENTITY( false );
