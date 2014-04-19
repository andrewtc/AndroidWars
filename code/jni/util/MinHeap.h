#pragma once

namespace mage
{
	/**
	 * Fixed-size data structure optimized for finding the minimum element in a set.
	 */
	template< size_t capacity, typename key_t, typename value_t >
	class FixedSizeMinHeap
	{
	public:
		static const size_t CAPACITY = capacity;

		typedef value_t Value;
		typedef key_t Key;

		struct Pair
		{
			Pair();
			Pair( const Key& key, Value& value );
			~Pair();

			bool operator>( const Pair& other ) const;

			Key key;
			Value value;
		};

		typedef Pair* Node;
		typedef const Pair* ConstNode;

		FixedSizeMinHeap();
		~FixedSizeMinHeap();

		void insert( const Key& key, Value& value );
		void update( const Key& key, Value& value );
		Value popMinElement();
		Value peekMinElement() const;
		Node findNodeByValue( const Value& value );
		ConstNode findNodeByValue( const Value& value ) const;
		bool hasValue( const Value& value ) const;
		void clear();

		size_t getCapacity() const;
		size_t getSize() const;
		bool isEmpty() const;

	protected:
		Node getFirstNode();
		ConstNode getFirstNode() const;
		Node getLastNode();
		ConstNode getLastNode() const;
		Node getEndOfArray();
		ConstNode getEndOfArray() const;
		Node getNode( int index );
		ConstNode getNode( int index ) const;
		size_t getIndexOfNode( ConstNode node ) const;
		Node getParentOfNode( Node node );
		ConstNode getParentOfNode( ConstNode node ) const;
		Node getFirstChildOfNode( Node node );
		ConstNode getFirstChildOfNode( ConstNode node ) const;
		Node getSecondChildOfNode( Node node );
		ConstNode getSecondChildOfNode( ConstNode node ) const;

		void swap( Pair& first, Pair& second );
		void heapify( Node node );
		void bubbleUp( Node node );
		void bubbleDown( Node node );
		bool isValidNode( ConstNode node ) const;
		bool isValidHeap( ConstNode node ) const;

		size_t m_size;
		Pair m_swapPair;
		Pair m_pairs[ CAPACITY ];
	};
}

#include "MinHeap.inl"
