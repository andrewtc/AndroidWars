#include "Matrix2.h"

using namespace mage;

template<> const Matrix2f Matrix2f::ZERO;
template<> const Matrix2f Matrix2f::IDENTITY( 0, 1, 0, 1 );

template<> const Matrix2d Matrix2d::ZERO;
template<> const Matrix2d Matrix2d::IDENTITY( 0, 1, 0, 1 );