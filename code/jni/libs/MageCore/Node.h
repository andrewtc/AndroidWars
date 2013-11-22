/*
 * Author      : Matthew Johnson
 * Date        : 13/Oct/2013
 * Description :
 *   Manages a parent children relationship of spatial objects.
 */
 
#pragma once

namespace mage
{

	class Node
		: public ControlledObject
	{
		DECLARE_RTTI;
	public:
		Node();
		Node( const std::string& name );
		virtual ~Node();

		size_t GetNumChildren() const		{ return mChildren.size(); }
		Node* GetParent() const				{ return mParent; }

		void AttachChild( Node* child );
		void DetachChild( Node* child );

		// Links this Nodes children to it's parent Node
		void RemoveFromHeirarchy();

		Node* GetChildAt( int i );

		// Put a position in worldspace into this nodes localspace
		Vec2f WorldToLocal( const Vec2f& worldPos );
		// Put a position in this nodes localspace into worldspace
		Vec2f LocalToWorld( const Vec2f& localPos );

		virtual void OnUpdate( float currentTime );
		virtual void OnDraw();

		Transform2D LocalTransform;
		Transform2D WorldTransform;
		bool WorldTransformIsCurrent;

		NodeVisualizer* Visualizer;
	private:
		Node* mParent;
		ArrayList< Node* > mChildren;
        typedef ArrayList< Node* >::iterator child_iterator;
	};

}