#include "Matrix3.h"

using namespace mage;

template<> const Matrix3f Matrix3f::ZERO( false );
template<> const Matrix3f Matrix3f::IDENTITY( true );

template<> const Matrix3d Matrix3d::ZERO( false );
template<> const Matrix3d Matrix3d::IDENTITY( true );