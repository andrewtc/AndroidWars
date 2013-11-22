#include "Quaternion.h"

using namespace mage;

template<> const Quatf Quatf::ZERO( 0.0f, 0.0f, 0.0f, 0.0f );
template<> const Quatf Quatf::IDENTITY( 1.0f, 0.0f, 0.0f, 0.0f );

template<> const Quatd Quatd::ZERO( 0.0, 0.0, 0.0, 0.0 );
template<> const Quatd Quatd::IDENTITY( 1.0, 0.0, 0.0, 0.0 );