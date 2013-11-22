/*
 * Author      : Matthew Johnson
 * Date        : 11/Oct/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	template< typename _Ty,
		typename _Ax = std::allocator<_Ty> >
	class ArrayList
		: public std::vector< _Ty, _Ax >
	{
	public:

		// Removes the first occurrence of elem
		// Returns true if elem was removed
		bool remove_elem( const _Ty& elem )
		{
			typename std::vector< _Ty >::iterator i = std::find( this->begin(), this->end(), elem );
			bool _ret = i != this->end();
			if ( _ret )
				this->erase( i );
			return _ret;
		}

		// Remove all occurrences of elem
		// Returns number removed
		int remove_all_elem( const _Ty& elem )
		{
			int i = 0;
			this->erase(
				std::remove_if( this->begin(), this->end(), [&]( const _Ty& val ) -> bool
				{
					bool _ret = val == elem;
					if ( _ret ) ++i;
					return _ret;
				})
				, this->end()
			);
			return i;
		}

		void insert_at( size_t index, const _Ty& elem )
		{
			if ( index > this->size() )
			{
				WarnCrit( "ArrayList::insert_at( index, elem ) : index out-of-bounds!\n", "" );
			}
			else
			{
				typename std::vector< _Ty >::iterator i = this->begin() + index;
				this->insert( i, elem );
			}
		}
#ifdef ANDROID
        const _Ty* data() const
        {
            return &(*this)[0];
        }
        _Ty* data()
        {
            return &(*this)[0];
        }
#endif
	};

}