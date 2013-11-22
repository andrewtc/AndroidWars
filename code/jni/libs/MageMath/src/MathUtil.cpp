#include "MathUtil.h"

//using namespace mage;
namespace mage
{

//template<> unsigned long Mathi::Seed;
//template<> unsigned long Mathf::Seed;
//template<> unsigned long Mathd::Seed;

template<> const float Math< float >::EPSILON    = FLT_EPSILON;
template<> const float Math< float >::MAX_REAL   = FLT_MAX;
template<> const float Math< float >::ZERO_TOLERANCE = 1e-06f;
template<> const float Math< float >::PI         = static_cast< float >( 4.0 * atan( 1.0 ) );
template<> const float Math< float >::TWO_PI     = 2.0f * Mathf::PI;
template<> const float Math< float >::HALF_PI    = 0.5f * Mathf::PI;
template<> const float Math< float >::INV_PI     = 1.0f / Mathf::PI;
template<> const float Math< float >::DEG_TO_RAD = Mathf::PI / 180.0f;
template<> const float Math< float >::RAD_TO_DEG = 180.0f / Mathf::PI;

template<> const double Math< double >::EPSILON    = DBL_EPSILON;
template<> const double Math< double >::MAX_REAL   = DBL_MAX;
template<> const double Math< double >::ZERO_TOLERANCE = 1e-08;
template<> const double Math< double >::PI         = 4.0 * atan( 1.0 );
template<> const double Math< double >::TWO_PI     = 2.0 * Mathd::PI;
template<> const double Math< double >::HALF_PI    = 0.5 * Mathd::PI;
template<> const double Math< double >::INV_PI     = 1.0 / Mathf::PI;
template<> const double Math< double >::DEG_TO_RAD = Mathd::PI / 180.0;
template<> const double Math< double >::RAD_TO_DEG = 180.0 / Mathd::PI;

template<> const int Math< int >::EPSILON    = 0;
template<> const int Math< int >::MAX_REAL   = INT_MAX;
template<> const int Math< int >::ZERO_TOLERANCE = 0;
template<> const int Math< int >::PI         = 3;
template<> const int Math< int >::TWO_PI     = 6;
template<> const int Math< int >::HALF_PI    = 1;
template<> const int Math< int >::INV_PI     = 0;
template<> const int Math< int >::DEG_TO_RAD = 0;
template<> const int Math< int >::RAD_TO_DEG = 0;

}